#pragma once
#include "Core.h"

namespace BLOCKMAN
{
	class Entity;

	class TeleportPositionConverter
	{
	public:
		static LORD::Vector3i ConvertToRelativeAndUpdateAnchor(int entityId, uint64_t raknetSessionId, const LORD::Vector3& pos);
		static LORD::Vector3 ConvertToAbsoluteAndUpdateAnchor(int entityId, const LORD::Vector3i& pos);
		static void ClearServerSideAnchors(uint64_t raknetSessionId);
		static void ClearClientSideAnchors();

	private:
		static constexpr float PRECISION = 128;
		LORD::Vector3i anchorPos = LORD::Vector3i::ZERO;

	private:
		void updateAnchorPos(const LORD::Vector3i& newPos);
		LORD::Vector3i convertToRelative(const LORD::Vector3& pos) const;
		LORD::Vector3 convertToAbsolute(const LORD::Vector3i& pos) const;
	};
}