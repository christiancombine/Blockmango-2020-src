#ifndef __SCHEMATIC_TASK_QUEUE_HEADER__
#define __SCHEMATIC_TASK_QUEUE_HEADER__

#include "BM_Container_def.h"
#include "Network/NetworkDefine.h"

namespace BLOCKMAN
{
	class World;
	class SchematicModel;

	class SchematicTask
	{
	public:
		ui64 placeRakssid{ 0 };
		String schematicName{ "" };
		BlockPos startPos = Vector3i::ZERO;
		bool reversalX{ false };
		bool reversalZ{ false };
		bool coverOldBlock{ true };
		int placeBlockId{ 0 };
		int placeOffset{ 0 };
		int extraParam{ 0 };

		SchematicModel* schematic = NULL;
		i16 processX = 0;
		i16 processY = 0;
		i16 processZ = 0;
		LORD::map<PACKET_DETAIL::Vector3i, bool>::type placeCoverResult;
		std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo> subRecordBlocks;
		bool isCreate = true;
	};

	class SchematicTaskQueue : public ObjectAlloc
	{
	public:
		SchematicTaskQueue(World* world);
		~SchematicTaskQueue();

	private:
		World* m_world = NULL;
		list<SchematicTask>::type m_queue;

	public:
		void onUpdate();
		void createSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
			bool reversalX = false, bool reversalZ = false, bool coverOldBlock = true, 
			int placeBlockId = 0, int placeOffset = 0, int extraParam = 0);
		void destroySchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
			bool reversalX = false, bool reversalZ = false, int placeOffset = 0, int extraParam = 0);
	private:
		bool initTaskSchematic(SchematicTask& task);
		void updateCreateSchematic(SchematicTask& task, int& changeBlockCount);
		void updateDestroySchematic(SchematicTask& task, int& changeBlockCount);
		void finishSchematicTask(SchematicTask& task);
		void clearQueue();
	};
}

#endif