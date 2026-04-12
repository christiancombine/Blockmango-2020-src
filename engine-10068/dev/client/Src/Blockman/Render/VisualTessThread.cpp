#include "VisualTessThread.h"
#include "RenderGlobal.h"
#include "cWorld/WorldClient.h"
#include "cChunk/ChunkClient.h"
#include "Chunk/ChunkService.h"
#include "cWorld/Blockman.h"
#include "Util/CPUTimer.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#include <string.h>
#include <unistd.h>
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <sys/prctl.h>
#endif
#endif

namespace BLOCKMAN
{
	//------------------------------------------------------------------------------
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
DWORD THREAD_CALLBACK TessThread::ThreadProc(void* t)
{
	TessThread* pThread = reinterpret_cast<TessThread*>(t);
	SetThreadName(pThread->GetThreadName());
	pThread->Run();
	return 0;
}
ui32 g_MainThreadId = ::GetCurrentThreadId();
#else
void* TessThread::ThreadProc(void* pParam)
{
	TessThread* pThread = reinterpret_cast<TessThread*>(pParam);
	pThread->Run();
	return 0;
}
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
ui32 g_MainThreadId = ui32(pthread_self());
#else
ui32 g_MainThreadId = pthread_mach_thread_np(pthread_self());
#endif
#endif
//------------------------------------------------------------------------------
ui32 TessThread::GetMainThreadId()
{
	return g_MainThreadId;
}

//------------------------------------------------------------------------------
ui32 TessThread::GetCurrentThreadId()
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	return ::GetCurrentThreadId();
#elif LORD_PLATFORM == LORD_PLATFORM_ANDROID
	return ui32(pthread_self());
#else
	return pthread_mach_thread_np(pthread_self());
#endif
}

//------------------------------------------------------------------------------
TessThread::TessThread()
	: m_priority(Normal)
	, m_stackSize(0)
	, m_stopEvent(true)
{
	memset(&m_hThread, 0, sizeof(m_hThread));
}

//------------------------------------------------------------------------------
TessThread::~TessThread()
{
	if (this->IsRunning())
	{
		// force to exit
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		TerminateThread(m_hThread, 0);
#else
		pthread_kill(m_hThread, 9);
#endif
	}
}

//------------------------------------------------------------------------------
void
TessThread::SetPriority(Priority p)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	int nPriority = THREAD_PRIORITY_NORMAL;

	if (p == Low)
		nPriority = THREAD_PRIORITY_BELOW_NORMAL;
	else if (p == Normal)
		nPriority = THREAD_PRIORITY_NORMAL;
	else if (p == High)
		nPriority = THREAD_PRIORITY_ABOVE_NORMAL;

	::SetThreadPriority(m_hThread, nPriority);
#endif
	m_priority = p;
}

//------------------------------------------------------------------------------
void TessThread::Start()
{
	assert(!this->IsRunning());
	m_stopEvent.Reset();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	m_hThread = ::CreateThread(0, m_stackSize, ThreadProc, this, CREATE_SUSPENDED, nullptr);
	assert(m_hThread);
	// apply thread priority
	SetPriority(m_priority);
	ResumeThread((HANDLE)m_hThread);
#else
	if (m_hThread != 0) return;

	pthread_attr_t attr;
	sched_param param;
	pthread_attr_init(&attr);

	switch (m_priority)
	{
	case Low:
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int min_priority = sched_get_priority_min(SCHED_RR);
		param.sched_priority = min_priority;
		pthread_attr_setschedparam(&attr, &param);
	}
	break;
	case High:
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int max_priority = sched_get_priority_max(SCHED_RR);
		param.sched_priority = max_priority;
		pthread_attr_setschedparam(&attr, &param);
	}
	break;
	case Normal:
		break;
	}
	pthread_t nVal;
	pthread_create(&nVal, &attr, ThreadProc, (void *)this);
	pthread_attr_destroy(&attr);
	if (nVal == 0)
	{
		return;
	}
	m_hThread = nVal;
#endif
}

//------------------------------------------------------------------------------
bool
TessThread::IsRunning() const
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	if (nullptr != m_hThread)
	{
		DWORD exitCode = 0;
		if (GetExitCodeThread(m_hThread, &exitCode))
		{
			if (STILL_ACTIVE == exitCode)
			{
				return true;
			}
		}
	}
#else
	if (0 != m_hThread)
	{
		return !m_stopEvent.IsSignalled();
	}
#endif
	return false;
}

//------------------------------------------------------------------------------
void
TessThread::Stop()
{
	//  assert(this->IsRunning());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	assert(m_hThread != nullptr);
#else
	assert(m_hThread != 0);
#endif
	m_stopEvent.Signal();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// wait for the thread to terminate
	WaitForSingleObject((HANDLE)m_hThread, INFINITE);
	CloseHandle((HANDLE)m_hThread);
	m_hThread = nullptr;
#else
	if (m_hThread != 0)
	{
		bool ret = pthread_join(m_hThread, nullptr);
		m_hThread = 0;
		if (0 == ret)
		{
			return;
		}
		else
		{
			pthread_kill(m_hThread, 9);
		}
	}
#endif
}

//------------------------------------------------------------------------------
void
TessThread::SetThreadName(const char* name)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// update the Windows thread name so that it shows up correctly
	// in the Debugger
	struct THREADNAME_INFO
	{
		DWORD dwType;     // must be 0x1000
		LPCSTR szName;    // pointer to name (in user address space)
		DWORD dwThreadID; // thread ID (-1 = caller thread)
		DWORD dwFlags;    // reserved for future use, must be zero
	};

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = ::GetCurrentThreadId();
	info.dwFlags = 0;
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#elif LORD_PLATFORM == LORD_PLATFORM_ANDROID
	//prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
#else
	pthread_setname_np(name);
#endif
}
//------------------------------------------------------------------------------
const char*
TessThread::GetThreadName()
{
	return "MayThread";
}

void TessThread::Sleep(int ms)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	::Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}


VisualTessThread::VisualTessThread()
	: m_stage1(true)
	, m_stage2(true)
	, m_finishEvent(true)
	, m_timeBegin(0)
	, m_timeEnd(0)
	, m_renderGlobal(nullptr)
	, m_world(nullptr)
	, m_actived(false)
{
	SetPriority(Normal);
}

VisualTessThread::~VisualTessThread()
{
}

void VisualTessThread::activeStage1(RenderGlobal* gr, WorldClient* world)
{
	m_actived = true;
	m_renderGlobal = gr;
	m_world = world;
	m_stage1.Signal();
}

void VisualTessThread::activeStage2()
{
	m_stage2.Signal();
}

bool VisualTessThread::WaitForFinish(unsigned int ms)
{
	PROFILE_CPU("VisualTessThread::WaitForFinish");
	bool ret = m_finishEvent.Wait(ms);
	m_finishEvent.Reset();
	return ret;
}

ui64 VisualTessThread::GetRuntime()
{
	return m_timeEnd - m_timeBegin;
}

void VisualTessThread::Stop()
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	assert(m_hThread != nullptr);
#else
	assert(m_hThread != 0);
#endif
	m_stopEvent.Signal();
	m_stage1.Signal();
	m_stage2.Signal();

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// wait for the thread to terminate
	WaitForSingleObject((HANDLE)m_hThread, INFINITE);
	CloseHandle((HANDLE)m_hThread);
	m_hThread = nullptr;
#else
	if (m_hThread != 0)
	{
		bool ret = pthread_join(m_hThread, nullptr);
		m_hThread = 0;
		if (0 == ret)
		{
			return;
		}
		else
		{
			pthread_kill(m_hThread, 9);
		}
	}
#endif
}

void VisualTessThread::Run()
{
	SetThreadName("VisualTessThread");

	while (true)
	{
		if (m_stopEvent.Wait(1))
		{
			break;
		}

		Blockman* bm = Blockman::Instance();
		try
		{
			if (m_stage1.Wait(0xFFFFFFFF))
			{
				m_stage1.Reset();
				bm->m_visibiBegin = m_timeBegin = LORD::Time::Instance()->getMicroseconds();
				if (m_world)
					m_world->applyRebuildVisibility();
				if (m_renderGlobal)
				{
					bm->m_findVisBegin = LORD::Time::Instance()->getMicroseconds();
					m_renderGlobal->setupTerrain(1.f);
					bm->m_lightingBegin = LORD::Time::Instance()->getMicroseconds();
					m_renderGlobal->updateSectionLighting();
				}

				Blockman::Instance()->m_lightingEnd = LORD::Time::Instance()->getMicroseconds();
				if (m_stage2.Wait(0xFFFFFFFF))
				{
					Blockman::Instance()->m_meshBegin = LORD::Time::Instance()->getMicroseconds();
					m_stage2.Reset();
					if (m_renderGlobal)
					{
						m_renderGlobal->updateSectionMesh();
					}
				}
				m_timeEnd = LORD::Time::Instance()->getMicroseconds();
				Blockman::Instance()->m_waitActive = Blockman::Instance()->m_meshGenEnd;
				Blockman::Instance()->m_meshGenEnd = m_timeEnd;
				m_finishEvent.Signal();
			}
		}
		catch (...)
		{
			LordLogError("Error in VisualTessThread");
		}
	}
}

}// namespace BLOCKMAN
