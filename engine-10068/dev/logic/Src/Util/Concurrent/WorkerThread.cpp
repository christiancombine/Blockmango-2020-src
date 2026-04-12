#include "WorkerThread.h"

namespace BLOCKMAN
{
	namespace WORKER_THREAD
	{
		void WorkerThread::mainLoop(StopFlag shouldStop)
		{
			while (!shouldStop)
			{
				WorkerTaskPtr task;
				if (m_taskQueue.wait_and_pop(task, 1000))
				{
					//LordLogInfo("[Worker thread] wait_and_pop size:[%d]", m_taskQueue.size());
					if (task->run())
					{
						m_finishedTaskCount.fetch_add(1);
					}
					else
					{
						//LordLogInfo("[Worker thread] run false");
					}
				}
			}
		}

		void WorkerThread::addTaskImpl(const WorkerTaskPtr& task)
		{
			if (m_status != Status::RUNNING)
			{
				LordLogError("[Worker thread] Task is added to stopped WorkerThread");
			}
			if (m_isDebug)
			{
				task->run();
			}
			else
			{
				// NOTE: Still, task might be added to stopped WorkerThread
				m_taskQueue.push(task);
			}
		}

		void WorkerThread::start()
		{
			if (m_isDebug)
			{
				return;
			}
			Status expected = Status::READY;
			if (m_status.compare_exchange_strong(expected, Status::RUNNING))
			{
				m_thread.start();
				LordLogInfo("[Worker thread] started: %s[%d]", m_thread.getName().c_str(), m_thread.getNativeId());
			}
			else
			{
				throw std::logic_error("[Worker thread] is started more than once");
			}
		}

		void WorkerThread::stopAsync()
		{
			if (m_isDebug)
			{
				return;
			}
			Status expected = Status::RUNNING;
			if (m_status.compare_exchange_strong(expected, Status::STOPPED))
			{
				cancelAllTasks();
				m_thread.stopAsync();
				LordLogInfo("[Worker thread] stopAsync: %s[%d]", m_thread.getName().c_str(), m_thread.getNativeId());
			}
			else
			{
				LordLogWarning("[Worker thread] is stopped more than once. Thread name: %s", m_thread.getName().c_str());
			}
		}

		void WorkerThread::stopSync()
		{
			if (m_isDebug)
			{
				return;
			}
			Status expected = Status::RUNNING;
			if (m_status.compare_exchange_strong(expected, Status::STOPPED))
			{
				cancelAllTasks();
				m_thread.stopSync();
				LordLogInfo("[Worker thread] stopSync: %s[%d]", m_thread.getName().c_str(), m_thread.getNativeId());
			}
			else
			{
				LordLogWarning("[Worker thread] is stopped more than once. Thread name: %s", m_thread.getName().c_str());
			}
		}

		void WorkerThread::cancelAllTasks()
		{
			LordLogInfo("[Worker thread] cancelAllTasks");
			decltype(m_taskQueue) queue;
			queue.swapContent(m_taskQueue);
			WorkerTaskPtr task;
			while (queue.try_pop(task))
			{
				task->cancel();
			}
		}

		WorkerThread& createAndStartThread()
		{
			LordLogInfo("[Worker thread] createAndStartThread");
			struct WorkerThreadDeleter
			{
				void operator()(WorkerThread* t) const
				{
					t->stopSync();
					delete t;
				}
			};
			// construction of static local variable is thread-safe
			static std::unique_ptr<WorkerThread, WorkerThreadDeleter> workerThread(new WorkerThread("DefaultWorkerThread"));
			workerThread->start();
			return *workerThread;
		}

		WorkerThread& WorkerThread::getDefault()
		{
			// construction of static local variable is thread-safe
			static WorkerThread& defaultWorkerThread = createAndStartThread();
			return defaultWorkerThread;
		}
	}
}
