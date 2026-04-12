
#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#include "TypeDef.h"
#include "Math/IntMath.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;

namespace NETWORK_DEFINE 
{
	class S2CPacketUpdateBlockInfo{
	public:
		i16 x, y, z;
		ui16 blockMetaAndId;

		S2CPacketUpdateBlockInfo() {}
		S2CPacketUpdateBlockInfo(const Vector3i& pos, int blockId, int blockMeta = 0)
			: x((i16)pos.x)
			, y((i16)pos.y)
			, z((i16)pos.z)
			, blockMetaAndId(blockMeta << 12 | blockId & 0xFFF)
		{
		}
		DEFINE_NETWORK_SERIALIZER(x, y, z, blockMetaAndId);
		ui16 getBlockId() const { return blockMetaAndId & 0xFFF; }
		ui8 getBlockMeta() const { return (blockMetaAndId >> 12) & 15; }
	};

	class UpdateBlockInfo {
	public:
		ui8 xz;
		ui16 idAndMeta;

		UpdateBlockInfo() {}
		UpdateBlockInfo(int xz, int idAndMeta)
			:xz(ui8(xz))
			, idAndMeta(idAndMeta)
		{
		}
		DEFINE_NETWORK_SERIALIZER(xz, idAndMeta);
		ui16 getId() const { return idAndMeta & 0xFFF; }
		ui8 getMeta() const { return (idAndMeta >> 12) & 15; }

		ui8 getX() const { return (xz >> 4) & 0xF; }
		ui8 getZ() const { return xz & 0xf; }
	};

	class TempBlockInfo {
	public:
		ui8 y;
		ui16 nums;
		vector<UpdateBlockInfo>::type infos;

		vector<ui64>::type posList;
		vector<ui64>::type  idMetas;

		TempBlockInfo() {}
		TempBlockInfo(ui8 y)
			: y(y)
		{
		}

		void save() {
			int posIndex = 0;
			int idMetaIndex = 0;
			ui64 pos = 0;
			ui64 idMeta = 0;
			nums = ui16(infos.size());
			int i = 0;
			for (UpdateBlockInfo info : infos)
			{
				i++;
				pos = ((ui64(info.xz) << posIndex) | pos);

				if (posIndex == 56 || i == infos.size())
				{
					posIndex = 0;
					posList.push_back(pos);
					pos = 0;
				}
				else
				{
					posIndex += 8;
				}

				idMeta =((ui64(info.idAndMeta) << idMetaIndex) | idMeta);
				if (idMetaIndex == 48 || i == infos.size())
				{
					idMetaIndex = 0;
					idMetas.push_back(idMeta);
					idMeta = 0;
				}
				else
				{
					idMetaIndex += 16;
				}
			}
		}

		void load() 
		{
			vector<ui16>::type idAndMetas;
			infos.clear();
			for (auto idMeta : idMetas)
			{
				idAndMetas.push_back(ui16((idMeta >> 00) & 0xFFFF));
				idAndMetas.push_back(ui16((idMeta >> 16) & 0xFFFF));
				idAndMetas.push_back(ui16((idMeta >> 32) & 0xFFFF));
				idAndMetas.push_back(ui16((idMeta >> 48) & 0xFFFF));
			}

			vector<ui8>::type xzList;
			for (auto xz : posList)
			{
				xzList.push_back(ui8((xz >> 0) & 0xFF));
				xzList.push_back(ui8((xz >> 8) & 0xFF));
				xzList.push_back(ui8((xz >>16) & 0xFF));
				xzList.push_back(ui8((xz >>24) & 0xFF));
				xzList.push_back(ui8((xz >>32) & 0xFF));
				xzList.push_back(ui8((xz >>40) & 0xFF));
				xzList.push_back(ui8((xz >>48) & 0xFF));
				xzList.push_back(ui8((xz >>56) & 0xFF));
			}

			for (ui16 i = 0; i < nums; i++)
			{
				infos.push_back(UpdateBlockInfo(xzList[i], idAndMetas[i]));
			}
		}

		//DEFINE_NETWORK_SERIALIZER(y, nums, posList, idMetas);
		DEFINE_NETWORK_SERIALIZER(y, infos);
	};

	class TempChunk {
	public:
		i16 chunkX, chunkZ;
		vector<TempBlockInfo>::type blocks;

		TempChunk() {}
		TempChunk(i16 chunkX, i16 chunkZ)
			: chunkX(chunkX)
			, chunkZ(chunkZ)
		{}

		DEFINE_NETWORK_SERIALIZER(chunkX, chunkZ, blocks);
	};

	enum PacketPlayerActionType : ui8 {
		START_SNEAK,
		STOP_SNEAK,
		START_PULL_BOW,
		STOP_PULL_BOW,
		START_SPRINTING,
		STOP_SPRINTING,
		START_ON_FIRE,
		STOP_ON_FIRE
	};

	enum C2SPacketUseItemUseType : ui8 {
		ITEM_USE_TYPE_EASTABLE,
		ITEM_USE_TYPE_BUCKET,
		ITEM_USE_TYPE_COMMON,
	};

	enum PacketAnimateType : ui8 {
		ANIMATE_TYPE_ATTACK,
		ANIMATE_TYPE_BASE_ACTION,
		ANIMATE_TYPE_UPPER_ACTION
	};

	enum class PacketLoginResult
	{
		emSucc = 0,
		emErrTokenWrong = 1,
		emErrBeKicked = 2,
		emErrGetUserAttrFail = 3,
		emErrWaitRoleAttrFail = 4,
		emErrServerQuitting = 5
	};

}

namespace PACKET_DETAIL
{
	using Vector3 = std::array<real32, 3>;
	using Vector3i = std::array<i32, 3>;
}

#endif