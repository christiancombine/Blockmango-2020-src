#pragma once
#include <memory>
#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiActorWindow;

	class GuiEffectViewOfObtainment : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		constexpr static char* JSON_NAME = "EffectViewOfObtainment.json";
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_effectImage = nullptr;
		GUIStaticImage* m_iconImage = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
		bool m_playIconEffect = false;
		float m_currScale = 0.2f;
		float m_targetScale_1 = 1.2f;
		float m_targetScale_2 = 0.8f;
		float m_targetScale_3 = 1.0f;
		int m_currIconEffectStage = 0;

		bool m_playActorEffect = false;
		float m_currTime = 0.0f;
		float m_timeOfActorEffectStage0 = 1.6f;
		float m_timeOfActorEffectStage1 = 3.2f;
		int m_currActorEffectStage = 0;

	public:
		GuiEffectViewOfObtainment(const GUIWindowType& nType, const GUIString& strName);
		~GuiEffectViewOfObtainment() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetImage(const GUIString& strImageName) { SetImage(strImageName, "effect_view_of_obtainment_icon_1.effect"); }
		void SetImage(const GUIString& strImageName, const GUIString& strEffectName);
		
		void SetActor(const GUIString& strActorName) { SetActor(strActorName, "idle", 0.0f, "effect_view_of_obtainment_actor_1.effect"); }
		void SetActor(const GUIString& strActorName, const GUIString& strSkillName) { SetActor(strActorName, strSkillName, 0.0f, "effect_view_of_obtainment_actor_1.effect"); }
		void SetActor(const GUIString& strActorName, const GUIString& strSkillName, float Rotate) { SetActor(strActorName, strSkillName, Rotate, "effect_view_of_obtainment_actor_1.effect"); }
		void SetActor(const GUIString& strActorName, const GUIString& strSkillName, float Rotate, const GUIString& strEffectName);
		
		int SetSkillName(const GUIString& strSkillName);
		void UseBodyPart(const String& masterName, const String &slaveName);

		void SetActorPos(float scaleX, float offsetX, float scaleY, float offsetY);
		bool canClose() { return !m_playIconEffect && !m_playActorEffect; }
	};
}