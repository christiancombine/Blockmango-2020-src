#include "GUIWindowMoveAnimator.h"
#include "UI/GUIWindowManager.h"


namespace BLOCKMAN
{
	void GUIWindowMoveAnimator::update(ui64 nTimeElapse)
	{
		m_currentTime += nTimeElapse;

		if (m_finishCallback)
		{
			for (auto pWindow : m_finishList)
			{
				m_finishCallback(pWindow);
			}
		}
		m_finishList.clear();

		while (!m_movingList.empty())
		{
			const auto& info = m_movingList.front();
			if (m_currentTime < info.startTime + DURATION)
			{
				break;
			}
			info.target->SetPosition(info.dstPos);
			m_finishList.push_back(info.target);
			m_movingList.pop_front();
		}

		for (const auto& info : m_movingList)
		{
			auto pastTime = m_currentTime - info.startTime;
			LordAssert(m_currentTime < info.startTime + DURATION);
			auto newPos = info.srcPos;
			scalePositionFromRoot(newPos, static_cast<float>(DURATION - pastTime) / DURATION);
			auto temp = info.dstPos;
			scalePositionFromRoot(temp, static_cast<float>(pastTime) / DURATION);
			newPos += temp;
			info.target->SetPosition(newPos);
		}
	}

	//----------------------------------------------------------------------------------------------

	GuiItemStack * ItemExchangeAnimator::createGuiItemStack(GuiSlotTable* hotbar, i32 itemId, i32 meta, const UVector2& pos)
	{
		GuiItemStack* newGuiItemStack;
		if (m_guiItemStackPool.empty())
		{
			static size_t count = 0;
			newGuiItemStack = hotbar->cloneGuiItemStack(StringUtil::Format("BlockFortMovingGuiItemStack-%u", count++).c_str());
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(itemId, 1, meta);
			newGuiItemStack->setItemStack(itemStack);
		}
		else
		{
			newGuiItemStack = m_guiItemStackPool.back();
			m_guiItemStackPool.pop_back();
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(itemId, 1, meta);
			newGuiItemStack->setItemStack(itemStack);
		}
		newGuiItemStack->setNumberVisibility(GuiItemStack::NumberVisibility::NEVER_VISIBLE);
		float w = newGuiItemStack->GetRenderArea().getWidth() / 2;
		float h = newGuiItemStack->GetRenderArea().getHeight() / 2;
		auto newPos = pos - UVector2({ 0, w }, { 0 , h });
		newGuiItemStack->SetPosition(newPos);
		newGuiItemStack->SetHorizontalAlignment(HA_LEFT);
		newGuiItemStack->SetVerticalAlignment(VA_TOP);
		GUISystem::Instance()->GetRootWindow()->AddChildWindow(newGuiItemStack);
		return newGuiItemStack;
	}

	ItemExchangeAnimator::ItemExchangeAnimator()
	{
		m_animator.onFinish([this](GUIWindow* window)
		{
			if (auto guiItemStack = dynamic_cast<GuiItemStack*>(window))
			{
				releaseGuiItemStack(guiItemStack);
			}
			else
			{
				LordLogError("Animated GUIWindow is not a GuiItemStack");
			}
		});
	}

	ItemExchangeAnimator::~ItemExchangeAnimator()
	{
		for (auto guiItemStack : m_guiItemStackPool)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(guiItemStack);
		}
	}

	void ItemExchangeAnimator::startAnimation(GuiSlotTable* hotbar, i32 itemId, i32 meta, const UVector2& pos, const UVector2& targetPos)
	{
		auto movingGuiItemStack = createGuiItemStack(hotbar, itemId, meta, pos);
		float w = movingGuiItemStack->GetRenderArea().getWidth() / 2;
		float h = movingGuiItemStack->GetRenderArea().getHeight() / 2;
		auto newPos = targetPos - UVector2({ 0, w }, { 0 , h });
		m_animator.startMove(movingGuiItemStack, newPos);
	}
}
