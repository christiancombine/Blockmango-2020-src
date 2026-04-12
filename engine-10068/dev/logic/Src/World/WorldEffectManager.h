#ifndef __WORLD_EFFECT_MANAGER_HEADER__
#define __WORLD_EFFECT_MANAGER_HEADER__

#include "Core.h"
#include "Scene/SceneNode.h"
#include "Effect/EffectSystem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class SimpleEffect
	{
	public:
		String mEffectName = "";
		String mTemplateName = "";
		Vector3 mPosition;
		float mYaw = 0;
		float mPitch = 0;
		int mDuration = -1;
		float mScaling = 1.0F;
		SceneNode* mNode = NULL;
		EffectSystem* mEffect = NULL;
		bool mAutoScale = false;
		bool mEnableLocalPos = false;

	public:
		SimpleEffect(String effectName, String templateName, Vector3 position, float yaw, float pitch, int duration, SceneNode* node, EffectSystem* effect)
			: mEffectName(effectName)
			, mTemplateName(templateName)
			, mPosition(position)
			, mYaw(yaw)
			, mPitch(pitch)
			, mDuration(duration)
			, mNode(node)
			, mEffect(effect)
		{}
		
		void setExtenalParam(const Vector3& beginPos, const Vector3& endPos);
		void setSpecialParam(float yaw_1, float yaw_2);
		void setParabolicParam(const Vector3& point_1, const Vector3& point_2, const Vector3& point_3);
		void setViewSpace(bool isViewSpace);
		void setEnableLocalPos(bool enableLocalPos) { mEnableLocalPos = enableLocalPos; }

	};

	class WorldEffectManager : public Singleton<WorldEffectManager>, public ObjectAlloc
	{
	private:
		SceneNode* mEffectNode = NULL;
		std::map<String, SimpleEffect*> mSimpleEffectMap;

	public:
		WorldEffectManager();
		~WorldEffectManager();

		SceneNode* getEffectNode() { return mEffectNode; }
		SimpleEffect* addSimpleEffect(String templateName, Vector3 position, float yaw, int duration, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f);
		void removeSimpleEffect(String effectName);
		void renderSimpleEffects(ui32 frameTime, Vector3 pos);

	private:
		void destoryAllSimpleEffect();
		void removeExpiredSimpleEffect();
		String getNextEffectName();
	};

}

#endif 
