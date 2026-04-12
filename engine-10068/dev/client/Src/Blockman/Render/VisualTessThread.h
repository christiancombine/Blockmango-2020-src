/********************************************************************
filename: 	VisualTessThread.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-23
*********************************************************************/
#ifndef __VISUAL_TESS_THREAD_HEADER__
#define __VISUAL_TESS_THREAD_HEADER__

#include "BM_TypeDef.h"

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#include <pthread.h>
#endif

namespace BLOCKMAN
{
	
class RenderGlobal;
class WorldClient;

#define THREAD_CALLBACK __stdcall
class TessThread
{
public:
	enum Priority
	{
		Low,
		Normal,
		High,
	};
public:
	TessThread();
	virtual ~TessThread();
	void SetPriority(Priority p);
	Priority GetPriority() const;
	void SetStackSize(size_t s);
	size_t GetStackSize() const;

	virtual void Start();
	virtual void Stop();
	bool IsRunning() const;
	bool IsStopRequested() const;

public:
	/// 设置所在线程名(方便VS调试)
	static void SetThreadName(const char* name);

	/// return thread handle
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	void* GetThreadHandle() const {
		return m_hThread;
	}
#else
	pthread_t GetThreadHandle() const {
		return m_hThread;
	}
#endif

	static unsigned int GetMainThreadId();
	static unsigned int GetCurrentThreadId();
	static bool IsMainThread();

	static void Sleep(int ms);

protected:
	virtual void Run() = 0;
	virtual const char* GetThreadName();
protected:

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	void*		  m_hThread;
	static unsigned long THREAD_CALLBACK ThreadProc(void* t);
#else
	pthread_t	  m_hThread;
	static void* ThreadProc(void* t);
#endif
	Priority m_priority;
	size_t m_stackSize;
protected:
	WaitEvent m_stopEvent;
};

//------------------------------------------------------------------------------
inline bool TessThread::IsMainThread()
{
	return TessThread::GetMainThreadId() == TessThread::GetCurrentThreadId();
}

//------------------------------------------------------------------------------
inline bool TessThread::IsStopRequested() const
{
	return m_stopEvent.IsSignalled();
}

class VisualTessThread : public TessThread
{
public:
	VisualTessThread();
	~VisualTessThread();

public:
	void activeStage1(RenderGlobal* gr, WorldClient* world);
	void activeStage2();
	bool WaitForFinish(unsigned int ms);
	ui64 GetRuntime();
	virtual void Stop();
	bool IsActived() const { return m_actived; }

protected:
	virtual void Run();

private:
	ui64 m_timeBegin;
	ui64 m_timeEnd;

	RenderGlobal* m_renderGlobal;
	WorldClient* m_world;

private:
	WaitEvent m_stage1;
	WaitEvent m_stage2;
	WaitEvent m_finishEvent;
	bool m_actived = false;
};

/*
typedef vector<ChunkClient*>::type ChunkClientArr;
typedef vector<Vector2i>::type ChunkIDArr;

class WorldGenerateThread : public TessThread
{
public:
	WorldGenerateThread();
	~WorldGenerateThread();

	int getNewlyGeneratedChunks(ChunkClientArr& _outChunks); //returns count of newly generated chunks

	void setWorld(WorldClient* pWorld) { m_pWorld = pWorld); };

	void requestChunks(const ChunkIDArr& chunkIds);

	void cancelChunk(const Vector2i& chunkId);
	ChunkIDArr cancelChunks(const ChunkIDArr& chunkIds);

	void recycleChunk(ChunkClient* chunk);
	void recycleChunks(const ChunkClientArr& chunks);

	void releaseAllResources();

	int getRemainingChunkCount();

	void GenerateChunk(ChunkClient* newChunk);
};
*/
}

#endif