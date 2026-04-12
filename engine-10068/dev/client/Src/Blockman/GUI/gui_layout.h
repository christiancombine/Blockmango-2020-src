#pragma once

#ifndef __LAYOUT_HEADER__
#define __LAYOUT_HEADER__

#include "Core.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIWindow.h"
#include "Util/LanguageManager.h"
#include "Util/SubscriptionGuard.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/GuideSystemSetting.h"
#include "Audio/SoundDef.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_layout : public ObjectAlloc
	{
	private:
		static bool rootGuiExists;

	public:
		gui_layout();
		gui_layout(const String& jsonName) : m_jsonName(jsonName) {}
		virtual ~gui_layout();

		GUIWindow* getRootWindow() { return  m_rootWindow; }
		bool isLoaded() const { return m_rootWindow != nullptr; }
		bool isAttached() const { return m_container != nullptr; }
		bool isShown() const { return m_rootWindow && m_rootWindow->IsVisible(); }

		void load();
		void unload();
		void attachTo(gui_layout* parent, GUIWindow * container, bool visible = true);
		void detach();
		void show();
		void hide();
		void update(ui32 nTimeElapse);
		virtual bool isForceUpdate() { return false; }
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onStart() {}
		virtual void onLoad() {}
		virtual void onUnload() {}
		virtual void onUpdate(ui32 nTimeElapse) {}
		virtual void checkBackgroundMusic() {}
		virtual bool isNeedLoad() { return true; }
		virtual bool isIgnoreLock() { return false; }
		virtual UVector2 getItemPosition() { return UVector2(UDim(0, 0), UDim(0, 0)); }
		virtual void onGetCommonData(String key, String data) {}

		void addLayout(GUIWindow * container, gui_layout* layout)
		{
			layout->attachTo(this, container, false);
		}
		void removeLayout(gui_layout * layout);
		GUIString getString(String key) {
			return LanguageManager::Instance()->getString(key).c_str();
		}

		void setText(const String & viewName, const String& key);
		void setText(GUIWindow * window, const String& key);
		bool checkGameType(ClientGameType gameType);
		bool checkGameType(vector<ClientGameType>::type games);

		template<typename Class = GUIWindow>
		Class* getWindow(const GUIString& name);
		String getName() { return m_jsonName; }

	protected:
		void removeAllEventsOfWindow(const GUIString& name)
		{
			if (auto window = getWindow(name))
			{
				window->removeAllEvents();
			}
		}

		void playSoundByType(SoundType soundType);
		void stopOrCloseBackgroundMusic();
		
	protected:
		SubscriptionGuard m_subscriptionGuard;
		const bool m_isRootLayout = false;
		String m_jsonName;
		gui_layout* m_parentLayout = nullptr;
		GUIWindow* m_rootWindow = nullptr;
		GUIWindow* m_container = nullptr;
		list<gui_layout*>::type m_childLayouts;

		template<typename T>
		class DataSetter
		{
		private:
			T* m_target;
			T m_value;

		public:
			DataSetter(T& target, const T& value) : m_target(&target), m_value(value) {}

			template<typename... Args>
			bool operator()(Args...)
			{
				*m_target = m_value;
				return true;
			}
		};

		template<typename T>
		static DataSetter<T> makeDataSetter(T& mem, T val)
		{
			return { mem, val };
		}

	private:
		const static GUIWindowType GWT_ROOT_WINDOW;
		class RootGuiWindow : public GUIWindow
		{
			using GUIWindow::GUIWindow;
		public:
			void Update(ui32 nTimeElapse) override;
		};

		void loadRootWindow();

	};

	template<typename Class>
	Class * gui_layout::getWindow(const GUIString & name)
	{
		auto ret = GUIWindowManager::Instance()->GetGUIWindow(name);
		auto window = ret;
		while (window && window != m_rootWindow)
		{
			window = window->GetParent();
		}
		return window ? dynamic_cast<Class*>(ret) : nullptr;
	}

}


#endif
