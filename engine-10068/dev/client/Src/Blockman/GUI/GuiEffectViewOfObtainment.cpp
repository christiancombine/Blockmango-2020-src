#include "GuiEffectViewOfObtainment.h"
#include "Render/TextureAtlas.h"
#include "GUI/GuiDef.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GuiActorWindow.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/ImageManager.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	GuiEffectViewOfObtainment::GuiEffectViewOfObtainment(const GUIWindowType& nType, const GUIString& strName)
		: BaseClass(nType, strName)
		, m_window(GUIWindowManager::Instance()->LoadWindowFromJSON(JSON_NAME))
	{
		renameComponents(m_window);
		m_effectImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_iconImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_actorWindow = dynamic_cast<GuiActorWindow*>(m_window->GetChildByIndex(2));
		if (Root::Instance()->isEditorMode())
		{
			AddWindowToDrawList(*m_window);
		}
		else
		{
			AddChildWindow(m_window);
		}
	}

	void GuiEffectViewOfObtainment::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		if (Root::Instance()->isEditorMode())
		{
			m_window->SetPosition(getAbsolutePositionFromRoot());
		}

		if (!m_effectImage || !m_iconImage || !m_actorWindow)
		{
			return;
		}

		if (m_playIconEffect)
		{
			int deltaTime = Root::Instance()->getFrameTime();
			if (m_currIconEffectStage == 0)
			{
				m_currScale += 7.5f * deltaTime * 0.001f;
				if (m_currScale >= m_targetScale_1)
				{
					m_currScale = m_targetScale_1;
					m_currIconEffectStage = 1;
				}
			}
			else if (m_currIconEffectStage == 1)
			{
				m_currScale -= 3.0f * deltaTime * 0.001f;
				if (m_currScale <= m_targetScale_2)
				{
					m_currScale = m_targetScale_2;
					m_currIconEffectStage = 2;
				}
			}
			else if (m_currIconEffectStage == 2)
			{
				m_currScale += 0.75f * deltaTime * 0.001f;
				if (m_currScale >= m_targetScale_3)
				{
					m_currScale = m_targetScale_3;
					m_currIconEffectStage = 3;
				}
			}
			else if (m_currIconEffectStage == 3)
			{
				m_playIconEffect = false;
			}
			else
			{
				m_currScale = 1.0f;
				m_playIconEffect = false;
			}

			m_iconImage->SetScale(Vector3(m_currScale, m_currScale, 1.0f));
		}

		if (m_playActorEffect)
		{
			float deltaTime = (float)Root::Instance()->getFrameTime();
			if (m_currActorEffectStage == 0)
			{
				m_currTime += deltaTime * 0.001f;
				if (m_currTime >= m_timeOfActorEffectStage0)
				{
					m_actorWindow->SetVisible(true);
					m_actorWindow->PlaySkill("rotate");
					m_currTime = m_timeOfActorEffectStage0;
					m_currActorEffectStage = 1;
				}
			}
			else if (m_currActorEffectStage == 1)
			{
				m_currTime += deltaTime * 0.001f;
				if (m_currTime >= m_timeOfActorEffectStage1)
				{
					m_actorWindow->PlaySkill("idle");
					m_currTime = m_timeOfActorEffectStage1;
					m_currActorEffectStage = 2;
				}
			}
			else if (m_currActorEffectStage == 2)
			{
				m_playActorEffect = false;
			}
			else
			{
				m_playActorEffect = false;
			}
		}

		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiEffectViewOfObtainment::Destroy()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_effectImage = nullptr;
		m_iconImage = nullptr;
		m_actorWindow = nullptr;
		BaseClass::Destroy();
	}

	void GuiEffectViewOfObtainment::SetImage(const GUIString& strImageName, const GUIString& strEffectName)
	{
		if (!m_effectImage || !m_iconImage || !m_actorWindow)
		{
			return;
		}
		m_effectImage->SetEffectName(strEffectName);
		m_iconImage->SetImage(strImageName);
		m_iconImage->SetVisible(true);
		m_actorWindow->SetVisible(false);
		m_playIconEffect = true;
		m_currIconEffectStage = 0;
		m_currScale = 0.2f;
	}

	void GuiEffectViewOfObtainment::SetActor(const GUIString& strActorName, const GUIString& strSkillName, float Rotate, const GUIString& strEffectName)
	{
		if (!m_effectImage || !m_iconImage || !m_actorWindow)
		{
			return;
		}
		m_effectImage->SetEffectName(strEffectName);
		m_actorWindow->SetActor(strActorName, strSkillName, Rotate);
		m_iconImage->SetVisible(false);
		m_actorWindow->SetVisible(false);
		m_playActorEffect = true;
		m_currActorEffectStage = 0;
		m_currTime = 0.0f;
	}

	int GuiEffectViewOfObtainment::SetSkillName(const GUIString& strSkillName)
	{
		if (m_actorWindow)
		{
			m_actorWindow->SetSkillName(strSkillName);
		}
		return 0;
	}

	void GuiEffectViewOfObtainment::UseBodyPart(const String& masterName, const String& slaveName)
	{
		if (m_actorWindow)
		{
			m_actorWindow->UseBodyPart(masterName, slaveName);
		}
	}

	void GuiEffectViewOfObtainment::SetActorPos(float scaleX, float offsetX, float scaleY, float offsetY)
	{
		if (m_actorWindow)
		{
			m_actorWindow->SetPosition(UVector2(UDim(scaleX, offsetX), UDim(scaleY, offsetY)));
		}
	}
}
