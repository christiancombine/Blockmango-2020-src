#include "TeleportPositionConverter.h"
#include "Entity/Entity.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct EntitySessionPair
	{
		int entityId;
		uint64_t raknetSessionId;
	};
	struct EntitySessionPairHash
	{
		std::size_t operator()(const EntitySessionPair& pair) const
		{
			return std::hash<int>{}(pair.entityId) ^ std::hash<uint64_t>{}(pair.raknetSessionId);
		}
	};
	struct EntitySessionPairEqual
	{
		bool operator()(const EntitySessionPair& lhs, const EntitySessionPair& rhs) const
		{
			return lhs.entityId == rhs.entityId && lhs.raknetSessionId == rhs.raknetSessionId;
		}
	};
	static unordered_map<EntitySessionPair, TeleportPositionConverter, EntitySessionPairHash, EntitySessionPairEqual>::type _serverSideConverters;
	static unordered_map<int, TeleportPositionConverter>::type _clientSideConverters;

	Vector3i TeleportPositionConverter::ConvertToRelativeAndUpdateAnchor(int entityId, uint64_t raknetSessionId, const Vector3& pos)
	{
		auto&& converter = _serverSideConverters[{entityId, raknetSessionId}];
		auto ret = converter.convertToRelative(pos);
		converter.updateAnchorPos(ret + converter.anchorPos);
		return ret;
	}

	LORD::Vector3 TeleportPositionConverter::ConvertToAbsoluteAndUpdateAnchor(int entityId, const Vector3i& pos)
	{
		auto&& converter = _clientSideConverters[entityId];
		auto ret = converter.convertToAbsolute(pos);
		converter.updateAnchorPos(pos + converter.anchorPos);
		return ret;
	}

	void TeleportPositionConverter::ClearServerSideAnchors(uint64_t raknetSessionId)
	{
		for (auto it = _serverSideConverters.begin(); it != _serverSideConverters.end(); )
		{
			if (it->first.raknetSessionId == raknetSessionId)
			{
				it = _serverSideConverters.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void TeleportPositionConverter::ClearClientSideAnchors()
	{
		_clientSideConverters.clear();
	}

	void TeleportPositionConverter::updateAnchorPos(const LORD::Vector3i& newPos)
	{
		constexpr float ratio = 0.25;
		Vector3 floatCenterPos = anchorPos;
		Vector3 floatNewPos = newPos;
		anchorPos = (ratio * floatCenterPos + (1 - ratio) * floatNewPos).getRound();
	}

	Vector3i TeleportPositionConverter::convertToRelative(const Vector3& pos) const
	{
		auto fixedPointNumberPos = (pos * PRECISION).getRound();
		return fixedPointNumberPos - anchorPos;
	}

	Vector3 TeleportPositionConverter::convertToAbsolute(const Vector3i& pos) const
	{
		return Vector3(pos + anchorPos) / PRECISION;
	}
}