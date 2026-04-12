#ifndef __GUI_WINDOW_MOVE_ANIMATOR_HEADER__
#define __GUI_WINDOW_MOVE_ANIMATOR_HEADER__

#include "Core.h"
#include "UI/GUIWindow.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUISystem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWindowMoveAnimator
	{
	public:
		using FinishCallback = std::function<void(GUIWindow*)>;

	private:
		static constexpr ui64 DURATION = 300;

		struct MotionInfo
		{
			GUIWindow* target;
			UVector2 srcPos;
			UVector2 dstPos;
			ui64 startTime;
		};

		ui64 m_currentTime = 0;
		deque<MotionInfo>::type m_movingList;
		vector<GUIWindow*>::type m_finishList;
		FinishCallback m_finishCallback;

		static void scalePositionFromRoot(UVector2& pos, float scale)
		{
			pos.d_x.d_scale *= scale;
			pos.d_x.d_offset *= scale;
			pos.d_y.d_scale *= scale;
			pos.d_y.d_offset *= scale;
		}
		
	public:
		void startMove(GUIWindow* target, const UVector2& to)
		{
			m_movingList.push_back({ target, target->GetPosition(), to, m_currentTime });
		}

		void onFinish(FinishCallback callback)
		{
			m_finishCallback = callback;
		}

		void update(ui64 nTimeElapse);
	};


	class ItemExchangeAnimator
	{
	private:
		vector<GuiItemStack*>::type m_guiItemStackPool;
		GUIWindowMoveAnimator m_animator;

		GuiItemStack* createGuiItemStack(GuiSlotTable* hotbar, i32 itemId, i32 meta, const UVector2& pos);
		void releaseGuiItemStack(GuiItemStack* guiItemStack)
		{
			GUISystem::Instance()->GetRootWindow()->RemoveChildWindow(guiItemStack);
			m_guiItemStackPool.push_back(guiItemStack);
		}

	public:
		ItemExchangeAnimator();
		~ItemExchangeAnimator();
		void startAnimation(GuiSlotTable* hotbar, i32 itemId, i32 meta, const UVector2& pos, const UVector2& targetPos);
		void update(ui64 nTimeElapse)
		{
			m_animator.update(nTimeElapse);
		}
	};
}

#endif // !__GUI_WINDOW_MOVE_ANIMATOR_HEADER__
