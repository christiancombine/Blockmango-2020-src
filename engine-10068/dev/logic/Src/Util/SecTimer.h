#pragma once
#include "Core.h"
#include "Object/Singleton.h"
#include "TypeDef.h"
#include <functional>
#include "Object/Root.h"
#include "Util/CPUTimer.h"
using namespace LORD;

struct SecTimer
{
	using TickCallBack = std::function<void(int)>;

	struct Timer
	{
		int id{};
		ui32 lastUpdateTime{};
		ui32 ticks{};
		bool hasStart{};
		bool hasStop{};
		bool executeOnce{ false }; //是否执行一次
		bool hasExecute{ false };
		int period{};
		TickCallBack tickCallBack;
	};
	
	static int createTimer(TickCallBack tickCallback)
	{
		return createTimerWithPeriod(tickCallback, 1000);
	}

	static int createTimerWithPeriod(TickCallBack tickCallback, int period)
	{
		auto timer = LORD::make_shared<Timer>();
		timer->id = m_idacc++;
		timer->lastUpdateTime = 0;
		timer->ticks = 0;
		timer->hasStart = false;
		timer->hasStop = false;
		timer->tickCallBack = tickCallback;
		timer->period = period;
		m_TimerList[timer->id] = timer;
		return timer->id;
	}

	static void startTimer(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
		{
			timer->second->hasStart = true;
			timer->second->lastUpdateTime = Root::Instance()->getCurrentTime();
		}
	}

	static void startTimerOnce(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
		{
			timer->second->hasStart = true;
			timer->second->executeOnce = true;
			timer->second->lastUpdateTime = Root::Instance()->getCurrentTime();
		}
	}

	static void stopTimer(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
			timer->second->hasStop = true;
	}

	static void removeTimer(int id)
	{
		auto timer = m_TimerList.find(id);
		if (timer != m_TimerList.end())
		{
			m_TimerList.erase(timer);
		}
	}

	static void removeExpiredTimer()
	{
		for (auto timer = m_TimerList.begin(); timer != m_TimerList.end();)
		{
			if (timer->second->executeOnce && timer->second->hasExecute)
			{
				timer = m_TimerList.erase(timer);
			}
			else
			{
				++timer;
			}
		}
	}

	static void update()
	{
		if (m_TimerList.empty())
		{
			return;
		}

		PROFILE_CPU("SecTimer::update");
		auto currentTick = Root::Instance()->getCurrentTime();
		for (auto timer = m_TimerList.begin();timer != m_TimerList.end();++timer)
		{
			if (timer->second->hasStart && !timer->second->hasStop)
			{
				if (currentTick >= timer->second->lastUpdateTime + timer->second->period)
				{
					timer->second->ticks += 1;
					timer->second->tickCallBack(timer->second->ticks);
					timer->second->hasExecute = true;
					timer->second->lastUpdateTime = timer->second->lastUpdateTime + timer->second->period;
				}
			}
		}

		removeExpiredTimer();
	}

	// not referenced
	static void uninitialize()
	{
		m_TimerList.clear();
	}
	
private:
	static ui16 m_idacc;
	static map<int, std::shared_ptr<Timer>>::type m_TimerList;
};

