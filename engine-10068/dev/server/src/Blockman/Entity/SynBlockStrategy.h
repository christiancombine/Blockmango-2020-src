#pragma once

#ifndef __SYN_BLOCK_STRATEGY_H__
#define __SYN_BLOCK_STRATEGY_H__

#include <vector>
#include <map>
#include <unordered_map>
#include "BM_TypeDef.h"
#include "Network/NetworkDefine.h"

namespace BLOCKMAN
{
class SyncBlockStrategy : public ObjectAlloc
{
public:
	using BlockInfo = NETWORK_DEFINE::S2CPacketUpdateBlockInfo;

	virtual void init(const Vector3& playPosition, std::vector<BlockPos>& blocks) = 0;
	virtual void add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta) = 0;
	virtual void move(const Vector3& playPosition) = 0;
	virtual void sync(ui64 raknetID) = 0;
	virtual bool checkSync() { return false; }
	virtual ~SyncBlockStrategy() {}

	virtual float getDistance(const Vector3& pos, const Vector3& playPosition)
	{
		Vector3 distance = playPosition - pos;
		return distance.len();
	}

	int getblocknum() { return blockNum; }
	int blockNum = 0;

protected:
	const int onceSyncBlockNum = 1000;
	const int onceSyncBlockPeriod = 1000; // ms
	int currentSyncBlockNum = 0;
	float currentWaitSyncTime = 0;
};

class SyncBlockStrategySection : public SyncBlockStrategy
{
public:
	void init(const Vector3& playPosition, std::vector<BlockPos>& blocks) override;
	void add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta) override;
	void move(const Vector3& playPosition) override;
	void sync(ui64 raknetID) override;
	bool checkSync() { return isNeedSync; }
	inline int aPosToRPos(int a);
	inline int rPosToaPos(int r);
	~SyncBlockStrategySection() { mNearBySection.clear(); }

private:
	struct BlockPosInSection
	{
		i8 relativeX = 0;
		i8 relativeY = 0;
		i8 relativeZ = 0;

		bool operator== (const BlockPosInSection& b) const
		{
			return relativeX == b.relativeX && relativeY == b.relativeY && relativeZ == b.relativeZ;
		}

		bool operator< (const BlockPosInSection& b) const
		{
			if (relativeX < b.relativeX) return true;
			if (relativeX > b.relativeX) return false;
			if (relativeY < b.relativeY) return true;
			if (relativeY > b.relativeY) return false;
			if (relativeZ < b.relativeZ) return true;
			return false;
		}
	};

	struct BlockInfoInSection
	{
		ui16 id = 0;
		ui8 meta = 0;
	};

	struct SectionInfo
	{
		i16 absoluteX = 0;
		ui8 absoluteY = 0;
		i16 absoluteZ = 0;

		bool operator== (const SectionInfo& s) const
		{
			return absoluteX == s.absoluteX && absoluteY == s.absoluteY && absoluteZ == s.absoluteZ;
		}

		bool operator< (const SectionInfo& s) const
		{
			if (absoluteX < s.absoluteX) return true;
			if (absoluteX > s.absoluteX) return false;
			if (absoluteY < s.absoluteY) return true;
			if (absoluteY > s.absoluteY) return false;
			if (absoluteZ < s.absoluteZ) return true;
			return false;
		}
	};

	struct HashFuncBlockPos
	{
		std::size_t operator()(const BlockPosInSection &b) const
		{
			using std::size_t;
			using std::hash;

			return ((hash<int>()(b.relativeX)
				^ (hash<int>()(b.relativeY) << 1)) >> 1)
				^ (hash<int>()(b.relativeZ) << 1);
		}
	};

	struct EqualKeyBlockPos
	{
		bool operator () (const BlockPosInSection &lhs, const BlockPosInSection &rhs) const
		{
			return lhs.relativeX == rhs.relativeX
				&& lhs.relativeY == rhs.relativeY
				&& lhs.relativeZ == rhs.relativeZ;
		}
	};

	// using BlockInfoInSectionMap = std::map<BlockPosInSection, BlockInfoInSection>;
	using BlockInfoInSectionMap = std::unordered_map<BlockPosInSection, BlockInfoInSection, HashFuncBlockPos, EqualKeyBlockPos>;

	struct BlockInfoInSectionAll
	{
		bool isNeedSync = false;
		int distance = 0;
		BlockInfoInSectionMap blockInfoInSectionMap;
	};

	struct HashFuncSection
	{
		std::size_t operator()(const SectionInfo &s) const
		{
			using std::size_t;
			using std::hash;

			return ((hash<int>()(s.absoluteX)
				^ (hash<int>()(s.absoluteY) << 1)) >> 1)
				^ (hash<int>()(s.absoluteZ) << 1);
		}
	};

	struct EqualKeySection
	{
		bool operator () (const SectionInfo &lhs, const SectionInfo &rhs) const
		{
			return lhs.absoluteX == rhs.absoluteX
				&& lhs.absoluteY == rhs.absoluteY
				&& lhs.absoluteZ == rhs.absoluteZ;
		}
	};

	// using SectionInfoMap = std::map<SectionInfo, BlockInfoInSectionAll >;
	using SectionInfoMap = std::unordered_map<SectionInfo, BlockInfoInSectionAll, HashFuncSection, EqualKeySection>;

	SectionInfoMap mNearBySection;
	SectionInfo mLastSection;
	bool isNeedSync = false;
};

class SyncBlockStrategyVector : public SyncBlockStrategy
{
public:
	void init(const Vector3& playPosition, std::vector<BlockPos>& blocks) override;
	void add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta) override;
	void move(const Vector3& playPosition) override;
	void sync(ui64 raknetID) override;
	~SyncBlockStrategyVector() { m_updateBlocks.clear(); }

private:
	struct UpdateBlockItem
	{
		BlockPos pos = Vector3i::ZERO;
		int id = 0;
		int meta = 0;
		float distance = 0.0;

		UpdateBlockItem() = default;
		UpdateBlockItem(Vector3i pos, int id, int meta, float dis)
			: pos(pos), id(id), meta(meta), distance(dis) {}

		bool operator < (const UpdateBlockItem& ubi) const
		{
			return this->distance < ubi.distance;
		}
	};
	using UpdateBlocks = std::vector<UpdateBlockItem>;
	UpdateBlocks m_updateBlocks;

	const int moveUpdateDistanceCd = 10;
	// means distance square root of ( _x * _x + _y * _y + _z * _z)
	const int needChangeDistance = 5;
	int m_moveNextUpdateDistanceTick = 0;
};

class SyncBlockContext : public ObjectAlloc
{
public:
	SyncBlockContext() = default;
	SyncBlockContext(SyncBlockStrategy *s) : mStrategy(s) {}
	~SyncBlockContext() { LordSafeDelete(mStrategy); }

	void init(const Vector3& playPosition, std::vector<BlockPos>& blocks) { if (mStrategy == nullptr) return; mStrategy->init(playPosition, blocks); }
	void add(const Vector3& playPosition, BlockPos& block, int blockId, int meta) { if (mStrategy == nullptr) return; mStrategy->add(playPosition, block, blockId, meta); }
	void move(const Vector3& playPosition) { if (mStrategy == nullptr) return; mStrategy->move(playPosition); }
	void sync(ui64 raknetID) { if (mStrategy == nullptr) return; mStrategy->sync(raknetID); }
	bool checkSync();
	int getblockNum() { if (mStrategy == nullptr) return 0;  return mStrategy->getblocknum(); }

private:
	SyncBlockStrategy *mStrategy = nullptr;
};

}


#endif