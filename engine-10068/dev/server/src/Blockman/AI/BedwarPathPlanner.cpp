#include "AI/PathPlanner.h"
#include "Blockman/World/ServerWorld.h"
#include "Server.h"
#include <queue>
#include "sparsehash/dense_hash_map"

namespace BLOCKMAN
{
	using namespace LORD;
	using BlockPos = Vector3i;

	struct Node
	{
		Node* parent = nullptr;
		BlockPos pos;
		float f = std::numeric_limits<float>::infinity();
		float g = std::numeric_limits<float>::infinity();
		bool closed = false;
	};

	struct ExpandedChild
	{
		Node* node;
		float cost;
	};

	class NodeManager
	{
	public:
		class NodeArray : public std::vector<Node>
		{
		public:
			NodeArray() { reserve(1024); }
			bool full() const { return size() == 1024; }
		};

		struct BlockPosHash
		{
			unsigned operator()(const BlockPos& pos) const
			{
				unsigned value = (pos.y << 20) | (pos.z << 10) | pos.x;
				value ^= value >> 16;
				value *= UINT32_C(0x7feb352d);
				value ^= value >> 15;
				value *= UINT32_C(0x846ca68b);
				value ^= value >> 16;
				return value;
			}
		};

		template<typename T>
		class BlockPosHashMap : public google::dense_hash_map<BlockPos, T, BlockPosHash>
		{
		public:
			BlockPosHashMap()
			{
				this->set_empty_key({ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() });
				this->set_deleted_key({ std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min() });
			}
		};

	public:
		NodeManager()
		{
			nodeArrayList.push_back({});
		}

		Node* getNode(const BlockPos& pos)
		{
			auto&& node = posToNode[pos];
			if (!node)
			{
				node = newNode();
				node->pos = pos;
			}
			return node;
		}

		Node* newNode()
		{
			auto&& tail = nodeArrayList.back();
			tail.push_back({});
			auto ret = &tail.back();
			if (tail.full())
				nodeArrayList.push_back({});
			return ret;
		}

		void clear()
		{
			*this = {};
		}

	private:
		std::vector<NodeArray> nodeArrayList;
		BlockPosHashMap<Node*> posToNode;
	};

	class BlockIdCache
	{
	public:
		struct ColumnPos
		{
			short x, z;

			friend bool operator==(const ColumnPos& lhs, const ColumnPos& rhs)
			{
				return memcmp(&lhs, &rhs, sizeof(ColumnPos)) == 0;
			}
		};

		struct ColumnPosHash
		{
			unsigned operator()(const ColumnPos& pos) const
			{
				unsigned value = (pos.z << 16) | pos.x;
				value ^= value >> 16;
				value *= UINT32_C(0x7feb352d);
				value ^= value >> 15;
				value *= UINT32_C(0x846ca68b);
				value ^= value >> 16;
				return value;
			}
		};

		template<typename T>
		class ColumnPosHashMap : public google::dense_hash_map<ColumnPos, T, ColumnPosHash>
		{
		public:
			ColumnPosHashMap()
			{
				this->set_empty_key({ std::numeric_limits<short>::max(), std::numeric_limits<short>::max() });
				this->set_deleted_key({ std::numeric_limits<short>::min(), std::numeric_limits<short>::min() });
			}
		};

	public:
		BlockIdCache()
			: world(Server::Instance()->getWorld())
		{
		}

		~BlockIdCache()
		{
			clear();
		}

		int getBlockId(const BlockPos& pos)
		{
			if (pos.y >= 256 || pos.y < 0)
			{
				return 0;
			}

			if (pos.x >= std::numeric_limits<short>::max() || pos.x <= std::numeric_limits<short>::min()||
				pos.z >= std::numeric_limits<short>::max() || pos.z <= std::numeric_limits<short>::min())
			{
				LordLogError("Invalid position for path finder: %d, %d, %d", pos.x, pos.y, pos.z);
				Log::printCallStack();
				Log::printLuaStack();
				return 0;
			}

			auto&& idArray = posToId[{(short)pos.x, (short)pos.z}];
			if (!idArray)
			{
				idArray = (uint16_t*) malloc(sizeof(uint16_t) * 256);
				memset(idArray, 0, sizeof(uint16_t) * 256);
			}
			auto&& id = idArray[pos.y];
			if (id == 0)
			{
				id = world->getBlockId(pos) + 1;
			}
			return id - 1;
		}

		void clear()
		{
			for (auto&& pair : posToId)
			{
				free(((uint16_t*)(pair.second)));
			}
			posToId.clear();
		}

	private:
		ColumnPosHashMap<uint16_t*> posToId;
		ServerWorld* world;
	};

	class BedwarWorldGraph
	{
	public:
		BedwarWorldGraph()
		{
			world = Server::Instance()->getWorld();
		}
		bool isSceneBlock(const BlockPos& pos, int id);
		bool checkBlockCanMove(const BlockPos& pos);
		bool isNotStandBlock(int id);
		bool isAirBlock(int id);
		bool canMovePos(const BlockPos& blockPos, const BlockPos& fromPos);
		std::vector<ExpandedChild> expand(Node* node);

		Node* getNode(const BlockPos& pos)
		{
			return nodeManager.getNode(pos);
		}

		void reset()
		{
			nodeManager.clear();
			blockIdCache.clear();
		}

	private:
		NodeManager nodeManager;
		ServerWorld* world;
		BlockIdCache blockIdCache;
	};

	class BedwarPathPlanner : public IPathPlanner
	{
	public:
		struct NodePtrCompare
		{
			bool operator()(const Node* lhs, const Node* rhs) const
			{
				return lhs->f > rhs->f;
			}
		};

	private:
		virtual bool findPath(const LORD::Vector3& from, const LORD::Vector3& to, int expandLimit) override;
		virtual LORD::Vector3 getPosInPath(int index) const override { return path[index]->pos; }
		virtual int getPathSize() const override { return path.size(); }

		float heuristic(Node* node);
		void reset();

	private:
		std::priority_queue<Node*, std::vector<Node*>, NodePtrCompare> open;
		Node* source = nullptr;
		Node* target = nullptr;
		std::vector<Node*> path;
		BedwarWorldGraph graph;
	};
	REGISTER_PATH_PLANNER(BedwarPathPlanner);

	enum BlockID
	{
		AIR = 0,
		STONE = 1,
		GRASS = 2,
		DIRT = 3,
		COBBLESTONE = 4,
		PLANKS = 5,
		SAPLING = 6,
		BEDROCK = 7,
		WATER_MOVING = 8,
		WATER_STILL = 9,
		LAVA_MOVING = 10,
		LAVA_STILL = 11,
		SAND = 12,
		GRAVEL = 13,
		ORE_GOLD = 14,
		ORE_IRON = 15,
		ORE_COAL = 16,
		WOOD = 17,
		LEAVES = 18,
		SPONGE = 19,
		GLASS = 20,
		ORE_LAPIS = 21,
		BLOCK_LAPIS = 22,
		DISPENSER = 23,
		SAND_STONE = 24,
		MUSIC = 25,
		BED = 26,
		RAIL_POWERED = 27,
		RAIL_DETECTOR = 28,
		PISTON_STICKY_BASE = 29,
		WEB = 30,
		TALL_GRASS = 31,
		DEAD_BUSH = 32,
		PISTON_BASE = 33,
		PISTON_EXTENSION = 34,
		CLOTH = 35,
		PISTON_MOVING = 36,
		PLANT_YELLOW = 37,
		PLANT_RED = 38,
		MUSHROOM_BROWN = 39,
		MUSHROOM_RED = 40,
		BLOCK_GOLD = 41,
		BLOCK_IRON = 42,
		STONE_DOUBLE_SLAB = 43,
		STONE_SINGLE_SLAB = 44,
		BRICK = 45,
		TNT = 46,
		BOOK_SHELF = 47,
		COBBLESTONE_MOSSY = 48,
		OBSIDIAN = 49,
		TORCH_WOOD = 50,
		FIRE = 51,
		MOB_SPAWNER = 52,
		STAIRS_WOOD_OAK = 53,
		CHEST = 54,
		REDSTONE_WIRE = 55,
		ORE_DIAMOND = 56,
		BLOCK_DIAMOND = 57,
		WORK_BENCH = 58,
		CROPS = 59,
		TILLED_FIELD = 60,
		FURNACE_IDLE = 61,
		FURNACE_BURNING = 62,
		SIGN_POST = 63,
		DOOR_WOOD = 64,
		LADDER = 65,
		RAIL = 66,
		STAIRS_COBBLESTONE = 67,
		SIGN_WALL = 68,
		LEVER = 69,
		PRESSURE_PLATE_STONE = 70,
		DOOR_IRON = 71,
		PRESSURE_PLATE_PLANKS = 72,
		ORE_REDSTONE = 73,
		ORE_READSTONE_GLOWING = 74,
		TORCH_REDSTONE_IDLE = 75,
		TORCH_REDSTONE_ACTIVE = 76,
		STONE_BUTTON = 77,
		SNOW = 78,
		ICE = 79,
		BLOCK_SNOW = 80,
		CACTUS = 81,
		BLOCK_CLAY = 82,
		REED = 83,
		JUKEBOX = 84,
		FENCE = 85,
		PUMPKIN = 86,
		NETHERRACK = 87,
		SLOW_SAND = 88,
		GLOW_STONE = 89,
		PORTAL = 90,
		PUMPKIN_LANTERN = 91,
		CAKE = 92,
		REDSTONE_REPEATER_IDLE = 93,
		REDSTONE_REPEATER_ACTIVE = 94,
		STAIND_GLASS = 95,
		TRAPDOOR = 96,
		SILVERFISH = 97,
		STONE_BRICK = 98,
		MUSHROOM_CAPBROWN = 99,
		MUSHROOM_CAP_RED = 100,
		FENCE_IRON = 101,
		THIN_GLASS = 102,
		MELON = 103,
		PUMPKIN_STEM = 104,
		MELON_STEM = 105,
		VINE = 106,
		FENCE_GATE = 107,
		STAIRS_BRICK = 108,
		STAIRS_STONE_BRICK = 109,
		MYCELIUM = 110,
		WATERLILY = 111,
		NETHER_BRICK = 112,
		NETHER_FENCE = 113,
		STAIRS_NETHER_BRICK = 114,
		NETHER_STALK = 115,
		ENCHANTMENT_TABLE = 116,
		BREWING_STAND = 117,
		CAULDRON = 118,
		END_PORTAL = 119,
		END_PORTAL_FRAME = 120,
		WHITE_STONE = 121,
		DRAGON_EGG = 122,
		REDSTONE_LAMP_IDLE = 123,
		REDSTONE_LAMP_ACTIVE = 124,
		WOOD_DOUBLE_SLAB = 125,
		WOOD_SINGLE_SLAB = 126,
		COCOA_PLANT = 127,
		STAIRS_SAND_STONE = 128,
		ORE_EMERALD = 129,
		ENDER_CHEST = 130,
		TRIP_WIRE_SOURCE = 131,
		TRIP_WIRE = 132,
		BLOCK_EMERALD = 133,
		STAIRS_WOOD_SPRUCE = 134,
		STAIRS_WOOD_BIRCH = 135,
		STAIRS_WOOD_JUNGLE = 136,
		COMMAND_BLOCK = 137,
		BEACON = 138,
		COBBLESTONE_WALL = 139,
		FLOWER_POT = 140,
		CARROT = 141,
		POTATO = 142,
		WOODEN_BUTTON = 143,
		SKULL = 144,
		ANVIL = 145,
		CHEST_TRAPPED = 146,
		PRESSURE_PLATE_GOLD = 147,
		PRESSURE_PLATE_IRON = 148,
		REDSTONE_COMPARATOR_IDLE = 149,
		REDSTONE_COMPARATOR_ACTIVE = 150,
		DAYLIGHT_SENSOR = 151,
		BLOCK_REDSTONE = 152,
		ORE_NETHER_QUARTZ = 153,
		HOPPER_BLOCK = 154,
		BLOCK_NETHER_QUARTZ = 155,
		STAIRS_NETHER_QUARTZ = 156,
		RAIL_ACTIVATOR = 157,
		DROPPER = 158,
		CLAY_HARDENED_STAINED = 159,
		STAIND_GLASS_PANE = 160,

		WOOD2 = 162,
		STAIRS_WOOD_DARKOAK = 164,
		SLIME = 165,

		HAY_BLOCK = 170,
		WOOL_CARPET = 171,
		CLAY_HARDENED = 172,
		BLOCK_COAL = 173,
		PACKED_ICE = 174,
		RAIL_RECEIVE = 175,
		WHITE_FENCE_GATE = 184,
		WHITE_WOOD_DOOR = 194,
		MAGMA = 213,
		ALLIUM = 1351,
		BLUE_ORCHID = 1352,
		HOUSTONIA = 1353,
		OXEYE_DAISY = 1354,
		PAEONIA = 1355,
		TULIP_PINK = 1356,
		TULIP_RED = 1357,
		TULIP_WHITE = 1358
	};

	const static std::set<int> kAirBlockIds = {
		AIR,
		TALL_GRASS
	};

	const static std::set<int> kNotStandIds = {
		WATERLILY,
		FENCE,
		WATER_MOVING,
		WATER_STILL,
		PLANT_RED,
		PLANT_YELLOW,
		COBBLESTONE_WALL,
		STAIND_GLASS_PANE
	};

	const static std::set<int> kCanBreakMap = {
		CLOTH,
		WHITE_STONE,
		STAIND_GLASS,
		GLASS,
		PLANKS,
		OBSIDIAN,
		SLIME
	};

	bool BedwarWorldGraph::isSceneBlock(const BlockPos& pos, int id)
	{
		if (id == AIR || id == BED /* bed */)
			return false;
		return !world->isInChangeRecord(pos);
	}

	bool BedwarWorldGraph::checkBlockCanMove(const BlockPos& pos)
	{
		int blockId = blockIdCache.getBlockId(pos);
		// 检查方块能否移动（破坏），如果可以返回true
		if (kAirBlockIds.find(blockId) != kAirBlockIds.end())
			return true;
		if (isSceneBlock(pos, blockId))
			return false;

		return blockId > 256 // custom block
			|| blockId == AIR
			|| blockId == TALL_GRASS
			|| blockId == WATER_MOVING
			|| blockId == WATER_STILL
			|| blockId == WATERLILY
			|| blockId == BED
			|| kCanBreakMap.find(blockId) != kCanBreakMap.end();
	}

	bool BedwarWorldGraph::isNotStandBlock(int id)
	{
		return kNotStandIds.find(id) != kNotStandIds.end();
	}

	bool BedwarWorldGraph::isAirBlock(int id)
	{
		return kAirBlockIds.find(id) != kAirBlockIds.end();
	}

	bool BedwarWorldGraph::canMovePos(const BlockPos& blockPos, const BlockPos& fromPos)
	{
		auto floorPos = Vector3i(blockPos.x, blockPos.y - 1, blockPos.z);
		auto headPos = Vector3i(blockPos.x, blockPos.y + 1, blockPos.z);
		auto floorId = blockIdCache.getBlockId(floorPos);
		if (isNotStandBlock(floorId))
			return false;
		if (blockPos.y > fromPos.y)
		{
			for (int y = fromPos.y + 3; y <= blockPos.y + 2; ++y)
			{
				if (!checkBlockCanMove(Vector3i(fromPos.x, y, fromPos.z)))
					return false;
			}
		}
		else if (blockPos.y < fromPos.y)
		{
			for (int y = blockPos.y + 3; y <= fromPos.y + 2; ++y)
			{
				if (!checkBlockCanMove(Vector3i(blockPos.x, y, blockPos.z)))
					return false;
			}
		}
		return checkBlockCanMove(blockPos) && checkBlockCanMove(headPos);
	}

	std::vector<ExpandedChild> BedwarWorldGraph::expand(Node* node)
	{
		std::vector<ExpandedChild> ret;
		auto addPoint = [&](const BlockPos& point, const BlockPos& tPos, float extraG) -> bool
		{
			if (!canMovePos(tPos, point))
				return false;
			Node* tPoint = getNode(tPos);
			if (tPoint->closed)
				return false;
			float cost = 0;
			if (tPos.x != point.x && tPos.z != point.z)
			{
				// 斜着走 默认耗费 1.4
				cost = 1.4;
				if (tPos.y > point.y) // 斜着往上的路线舍弃
					return false;
			}
			else
			{
				// 水平走默认耗费 1
				cost = 1;
			}
			// 额外耗费,根据y值得判断
			cost += extraG;
			ret.push_back({ tPoint, cost });
			return true;
		};

		auto point = node->pos;

		const static Vector3i offsets[] = {
			Vector3i::NEG_UNIT_X,
			Vector3i::NEG_UNIT_Z,
			Vector3i::UNIT_Z,
			Vector3i::UNIT_X
		};
		int yMin = point.y;
		int yMax = point.y + 1;
		for (const auto& offset : offsets)
		{
			std::vector<int> hasBlock;
			auto nextPos = point + offset;
			for (nextPos.y = yMin; nextPos.y <= yMax; nextPos.y++)
			{
				int blockId = blockIdCache.getBlockId(nextPos);
				if (!isAirBlock(blockId))
					hasBlock.push_back(nextPos.y);
			}
			if (hasBlock.empty())
			{
				for (nextPos.y = point.y - 1; nextPos.y >= point.y - 10; --nextPos.y)
				{
					if (!isAirBlock(blockIdCache.getBlockId(nextPos)))
					{
						hasBlock.push_back(nextPos.y);
						break;
					}
				}
				for (nextPos.y = point.y + 2; nextPos.y <= point.y + 10; ++nextPos.y)
				{
					if (!isAirBlock(blockIdCache.getBlockId(nextPos)))
					{
						hasBlock.push_back(nextPos.y);
					}
				}
			}
			if (hasBlock.empty())
			{
				// 证明y检测范围内没有方块,选择自己搭路
				if (nextPos.x == point.x || nextPos.z == point.z)  // always true
					addPoint(point, Vector3i(nextPos.x, point.y, nextPos.z), 0.8); // 向斜方向的不允许搭方块 需要搭方块的情况下 移动
			}
			else
			{
				nextPos.y = hasBlock.back();
				hasBlock.pop_back();
				auto topBlock = nextPos;
				while (!isAirBlock(blockIdCache.getBlockId(topBlock.getPosY()))
					|| !isAirBlock(blockIdCache.getBlockId(topBlock.getPosY2())))
				{
					topBlock.y++;
				}
				if (topBlock.y >= point.y)
				{
					if (topBlock.y - point.y < 20)
					{
						hasBlock.push_back(topBlock.y);
						hasBlock.push_back(topBlock.y + 3);
					}
				}
				else
				{
					hasBlock.push_back(topBlock.y);
					hasBlock.push_back(point.y + 2);
				}
				// 找出一个能直走并且站上去的方块
				int addPointCount = 0;
				bool isFootBlockHalf = blockIdCache.getBlockId(point.getNegY()) == BlockID::BED;
				for (int index = 0; index < (int) hasBlock.size() - 1; ++index)
				{
					int y = hasBlock[index];
					int toY = y + 1;
					int nextY = hasBlock[index + 1];
					if (nextY - y > 2 && nextY > toY + 1)
					{
						// 水平方向直走不会被挡住
						float extraG = 0;
						if (abs(toY - point.y) == 1)
							extraG = 0.2f;
						else if (toY > point.y)
							extraG = 0.2f + (toY - point.y - 1);
						else if (toY < point.y)
							extraG = 0.2f + (point.y - toY - 1);
						if (toY > point.y && isFootBlockHalf)
							; // 如果脚下为搬砖,并且需要跳起来搭方块,即放弃路线
						else if (addPoint(point, Vector3i(nextPos.x, toY, nextPos.z), extraG))
							addPointCount++;
					}
				}
				// 破坏正前方的方块并且
				if (addPointCount == 0)
				{
					float extraG = 0;
					int toY = point.y;
					while (toY <= yMax - 1)
					{
						if (checkBlockCanMove(Vector3i(nextPos.x, toY, nextPos.z)) &&
							checkBlockCanMove(Vector3i(nextPos.x, toY + 1, nextPos.z)))
						{
							extraG = extraG + 1.5f;
							addPoint(point, Vector3i(nextPos.x, toY, nextPos.z), extraG);
							break;
						}
						toY = toY + 1;
						extraG = extraG + 1.2f;
					}
				}
			}
		}
		return ret;
	}

	bool BedwarPathPlanner::findPath(const LORD::Vector3& from, const LORD::Vector3& to, int expandLimit)
	{
		auto fromPos = from.getFloor();
		auto toPos = to.getFloor();
		if (source && source->pos == fromPos &&
			target && target->pos == toPos)
			return true;
		reset();
		source = graph.getNode(fromPos);
		target = graph.getNode(toPos);

		source->g = 0;
		source->f = heuristic(source);
		open.push(source);
		Node* closestPoint = nullptr;
		int expandNumLeft = expandLimit;
		if (expandNumLeft == 0)
		{
			expandNumLeft = std::numeric_limits<int>::max();
		}
		while (!open.empty())
		{
			auto current = open.top();
			open.pop();
			if (current->closed)
				continue;
			if (current->pos.x == target->pos.x && current->pos.z == target->pos.z)
			{
				closestPoint = current;
				break;
			}
			current->closed = true;
			for (auto child : graph.expand(current))
			{
				auto newG = current->g + child.cost;
				auto neighbor = child.node;
				if (neighbor->g <= newG)
					continue;
				neighbor->g = newG;
				neighbor->parent = current;
				neighbor->f = newG + heuristic(neighbor);
				open.push(neighbor);
			}
			if (--expandNumLeft == 0)
			{
				closestPoint = open.top();
				break;
			}
		}
		if (!closestPoint)
			return false;
		auto current = closestPoint;
		path.push_back(target);
		while (current->parent)
		{
			current = current->parent;
			path.push_back(current);
		}
		std::reverse(path.begin(), path.end());
		return true;
	}

	float BedwarPathPlanner::heuristic(Node* node)
	{
		auto diff = target->pos - node->pos;
		diff.abs();
		return static_cast<float>(diff.x + diff.y + diff.z);
	}

	void BedwarPathPlanner::reset()
	{
		open = {};
		path.clear();
		graph.reset();
	}
}

