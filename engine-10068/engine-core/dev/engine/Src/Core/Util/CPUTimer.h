#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <unordered_set>
#include "Core.h"
//#include "Util/RingBuffer.h"
//#include "Debug/RecordSection.h"

#define PERFORMANCE_PROFILE_ENABLE

#ifdef PERFORMANCE_PROFILE_ENABLE
#  define TIMER_JOIN(a, b) a##b
#  define TIMER_JOIN_MACRO(a, b) TIMER_JOIN(a, b)
#  define PROFILE_CPU(name) LORD::CPUTimer::AutoRecorder TIMER_JOIN_MACRO(__cpu_auto_recorder, __LINE__)(name);
#else
#  define PROFILE_CPU(name)  
#endif

namespace LORD
{
	constexpr static unsigned kFrameHistorySize = 300;

	template<typename T, size_t length>
	class RingBuffer
	{
	public:
		template<typename U>
		void add(U&& element)
		{
			m_buffer[m_tail] = std::forward<U>(element);
			m_tail = (m_tail + 1) % length;
			m_size = (std::min)(m_size + 1, length);
		}

		void addUninitialized()
		{
			m_tail = (m_tail + 1) % length;
			m_size = (std::min)(m_size + 1, length);
		}

		T& operator[](size_t index)
		{
			LordAssert(index < m_size);
			return m_buffer[(m_tail + length - 1 - index) % length];
		}

		const T& operator[](size_t index) const
		{
			LordAssert(index < m_size);
			return m_buffer[(m_tail + length - 1 - index) % length];
		}

		size_t size() const
		{
			return m_size;
		}

		void clear()
		{
			m_size = 0;
		}

	private:
		std::array<T, length> m_buffer;
		size_t m_tail = 0;
		size_t m_size = 0;
	};

	class RecordSection
	{
	public:
		using Duration = std::chrono::steady_clock::duration;

		struct StatisticsInOneFrame
		{
			Duration totalTime{};
			int count = 0;
		};

		using StatisticsHistory = RingBuffer<StatisticsInOneFrame, kFrameHistorySize>;

	private:
		const char* name = "";
		StatisticsHistory statisticsHistory;
		std::vector<std::unique_ptr<RecordSection>> innerSections;
		RecordSection* outerSection = nullptr;

	public:
		RecordSection() = default;
		RecordSection(const RecordSection&) = delete;
		RecordSection(RecordSection&&) = delete;
		RecordSection& operator=(const RecordSection&) = delete;
		RecordSection& operator=(RecordSection&&) = delete;

		void newFrame()
		{
			statisticsHistory.add(StatisticsInOneFrame{});
			for (auto&& inner : innerSections)
			{
				inner->newFrame();
			}
		}

		void invalidateLastFrame()
		{
			statisticsHistory[0].count = -1;
			for (auto&& inner : innerSections)
			{
				inner->invalidateLastFrame();
			}
		}

		const char* getName() const
		{
			return name;
		}

		StatisticsInOneFrame& getLastFrameStatistics()
		{
			return statisticsHistory[0];
		}

		const StatisticsHistory& getStatisticsHitory() const
		{
			return statisticsHistory;
		}

		RecordSection* getInnerSection(const char* name)
		{
			for (auto&& inner : innerSections)
			{
				if (inner->name == name)
					return inner.get();
			}
			return newInnerSection(name);
		}

		RecordSection* getInnerSection(size_t index)
		{
			return innerSections[index].get();
		}

		size_t getInnerSectionCount() const
		{
			return innerSections.size();
		}

		RecordSection* newInnerSection(const char* name)
		{
			innerSections.push_back(std::make_unique<RecordSection>());
			auto newNode = innerSections.back().get();
			newNode->name = name;
			newNode->statisticsHistory.add(StatisticsInOneFrame{});
			newNode->outerSection = this;
			return newNode;
		}

		RecordSection* getOuterSection() const
		{
			return outerSection;
		}

		void Reset()
		{
			statisticsHistory.clear();
			innerSections.clear();
		}

		void printResultToStream(std::ostream& os, unsigned frameCount);
		void printSwitchTestResultToStream(std::ostream& os, unsigned frameCount);
		void printRecordSeries(std::ostream& os, unsigned frameCount);
	};

	struct FrameStatistics
	{
		using Clock = std::chrono::steady_clock;
		using Time = Clock::time_point;
		using Duration = Clock::duration;

		Time startTime{};
		Duration frameTime{};
		int cpuTimerCount = 0;
		int gpuTimerCount = 0;
	};

	class CPUTimer
	{
	public:
		using Clock = std::chrono::steady_clock;
		using Time = Clock::time_point;
		using Duration = Clock::duration;

		struct Record
		{
			const char* name;
			Time time;
		};

		class AutoRecorder
		{
		public:
			template<typename T>
			AutoRecorder(const T& name) { CPUTimer::Start(name); }
			~AutoRecorder() { CPUTimer::Stop(); }
		};

	public:
		// only accept string literal
		template<size_t size>
		static void Start(const char(&name)[size])
		{
			if (!enabledInThisFrame)
				return;
			DoStart(name);
		}
		template<typename Traits, typename Alloc>
		static void Start(const std::basic_string<char, Traits, Alloc>& name)
		{
			if (!enabledInThisFrame)
				return;
			return DoStart(GetInternedString(name.c_str()));
		}
		static void SetCPUTimerCount(int count);
		static void Stop();
		static void StartNewFrame(Time startTime = Clock::now());
		static void Reset();
		static void PrintResults(std::ostream& os, unsigned frameCount);
		static void PrintRecordSeries(std::ostream& os, unsigned frameCount);
		static bool IsEnabled();
		static void SetEnabled(bool enabled);
		static void StartForLua(char* name)
		{
			if (!enabledInThisFrame)
				return;
			return DoStart(GetInternedString(name));
		}

	private:
		static void DoStart(const char* name);
		template<typename T>
		static const char* GetInternedString(const T& str)
		{
			auto result = internedString.insert(str);
			return result.first->c_str();
		}

	private:
		static bool enabled;
		static bool enabledInThisFrame;
		static std::vector<Record> records;
		static RecordSection rootRecordSection;
		static std::unordered_set<std::string> internedString;
	};
}
