#include "CPUTimer.h"
#include <algorithm>

using namespace std::chrono;

namespace LORD
{

	using PrintLine = void (*)(std::ostream&, unsigned, unsigned, RecordSection*);
	static void printToStream(std::ostream& os, RecordSection* root, unsigned frameCount, PrintLine printLine)
	{
		if (frameCount == 0)
			return;

		std::vector<unsigned> indexStack;
		constexpr size_t kInitialStackSize = 20;
		indexStack.reserve(kInitialStackSize);

		// Depth-first Search
		printLine(os, frameCount, 0, root);
		RecordSection* curSection = root;
		indexStack.push_back(0);
		while (!indexStack.empty())
		{
			auto index = indexStack.back();
			auto size = curSection->getInnerSectionCount();
			if (index >= size)
			{
				indexStack.pop_back();
				curSection = curSection->getOuterSection();
				continue;
			}
			curSection = curSection->getInnerSection(index);
			printLine(os, frameCount, indexStack.size(), curSection);
			++indexStack.back();
			indexStack.push_back(0);
		}
	}

	static void printRecordSectionStatistics(std::ostream& os, unsigned maxHistoryIndex, unsigned indent, RecordSection* section)
	{
		const auto& history = section->getStatisticsHitory();
		RecordSection::Duration totalDuration{};
		RecordSection::Duration maxDuration{};
		unsigned frameCount = 0;
		unsigned totalEnterCount = 0;
		for (unsigned i = 0; i < history.size() && i < maxHistoryIndex; ++i)
		{
			const auto& statistics = history[i];
			if (statistics.count < 0)  // this record is invalidated, skip it
				continue;
			totalDuration += statistics.totalTime;
			maxDuration = (std::max)(statistics.totalTime, maxDuration);
			totalEnterCount += statistics.count;
			frameCount++;
		}
		float averageTimeInMilliseconds = duration_cast<duration<float, std::milli>>(totalDuration).count() / frameCount;
		float maxTimeInMilliseconds = duration_cast<duration<float, std::milli>>(maxDuration).count();
		float averageEnterCount = totalEnterCount / (float)frameCount;

		os << std::endl;
		for (unsigned i = 0; i < indent; ++i)
		{
			os << "  ";
		}
		os << section->getName() << ": " << averageEnterCount << ", " << averageTimeInMilliseconds << ", " << maxTimeInMilliseconds;
	}

	void RecordSection::printResultToStream(std::ostream& os, unsigned frameCount)
	{
		printToStream(os, this, frameCount, printRecordSectionStatistics);
	}

	static void printSwitchTestResultOfOneRecordSection(std::ostream& os, unsigned maxHistoryIndex, unsigned indent, RecordSection* section)
	{
		const auto& history = section->getStatisticsHitory();
		float averageTime[2] = {};
		float maxTime[2] = {};
		for (int i = 0; i < 2; ++i)
		{
			RecordSection::Duration totalDuration{};
			RecordSection::Duration maxDuration{};
			unsigned frameCount = 0;
			for (unsigned j = i; j < history.size() && j < maxHistoryIndex; j += 2)
			{
				const auto& statistics = history[j];
				if (statistics.count < 0)  // this record is invalidated, skip it
					continue;
				totalDuration += statistics.totalTime;
				maxDuration = (std::max)(statistics.totalTime, maxDuration);
				frameCount++;
			}
			averageTime[i] = duration_cast<duration<float, std::milli>>(totalDuration).count() / frameCount;
			maxTime[i] = duration_cast<duration<float, std::milli>>(maxDuration).count();
		}

		os << std::endl;
		for (unsigned i = 0; i < indent; ++i)
		{
			os << "  ";
		}
		os << section->getName() << ": " << averageTime[0] / averageTime[1] << ", " << maxTime[0] / maxTime[1];
	}

	void RecordSection::printSwitchTestResultToStream(std::ostream& os, unsigned frameCount)
	{
		printToStream(os, this, frameCount, printSwitchTestResultOfOneRecordSection);
	}

	static void printRecordSeriesOfOneRecordSection(std::ostream& os, unsigned maxHistoryIndex, unsigned indent, RecordSection* section)
	{
		const auto& history = section->getStatisticsHitory();
		os << std::endl;
		for (unsigned i = 0; i < indent; ++i)
		{
			os << "  ";
		}
		os << section->getName() << ": ";
		for (size_t i = 0; i < history.size() && i < maxHistoryIndex; ++i)
		{
			const auto& statistics = history[i];
			if (statistics.count < 0)
				os << "nan, ";
			else
				os << duration_cast<duration<float, std::milli>>(statistics.totalTime).count() << ", ";
		}
	}

	void RecordSection::printRecordSeries(std::ostream& os, unsigned frameCount)
	{
		printToStream(os, this, frameCount, printRecordSeriesOfOneRecordSection);
	}

	bool CPUTimer::enabled = false;
	bool CPUTimer::enabledInThisFrame = false;
	std::vector<CPUTimer::Record> CPUTimer::records;
	RecordSection CPUTimer::rootRecordSection;
	std::unordered_set<std::string> CPUTimer::internedString;
	static RingBuffer<FrameStatistics, kFrameHistorySize> frameStatisticsHistory;

	void CPUTimer::SetCPUTimerCount(int count)
	{
		frameStatisticsHistory[0].cpuTimerCount = count;
	}

	void CPUTimer::Stop()
	{
		if (!enabledInThisFrame)
			return;
		records.push_back({ nullptr, Clock::now() });
	}

	void CPUTimer::StartNewFrame(Time startTime)
	{
		bool enabledInLastFrame = enabledInThisFrame;
		enabledInThisFrame = enabled;
		if (!enabledInLastFrame)
			return;
		rootRecordSection.newFrame();

		std::vector<Time> startTimeStack;
		constexpr size_t kInitialStackSize = 20;
		startTimeStack.reserve(kInitialStackSize);
		RecordSection* curSection = &rootRecordSection;
		for (const auto& record : records)
		{
			if (record.name)
			{
				curSection = curSection->getInnerSection(GetInternedString(record.name));
				startTimeStack.push_back(record.time);
			}
			else
			{
				auto&& statistics = curSection->getLastFrameStatistics();
				statistics.totalTime += record.time - startTimeStack.back();
				statistics.count++;
				curSection = curSection->getOuterSection();
				startTimeStack.pop_back();
			}
		}
		LordAssert(startTimeStack.empty());

		SetCPUTimerCount(records.size());
		rootRecordSection.getLastFrameStatistics().count = 1;
		rootRecordSection.getLastFrameStatistics().totalTime = records.back().time - records.front().time;
		records.clear();

		records.push_back({ "CPUTimer::StartNewFrame", startTime });
		records.push_back({ nullptr, Clock::now() });
	}

	void CPUTimer::Reset()
	{
		records.clear();
	}

	void CPUTimer::PrintResults(std::ostream& os, unsigned frameCount)
	{
		rootRecordSection.printResultToStream(os, frameCount);
	}

	void CPUTimer::PrintRecordSeries(std::ostream& os, unsigned frameCount)
	{
		rootRecordSection.printRecordSeries(os, frameCount);
	}

	bool CPUTimer::IsEnabled()
	{
		return enabled;
	}

	void CPUTimer::SetEnabled(bool enabled)
	{
		CPUTimer::enabled = enabled;
	}

	void CPUTimer::DoStart(const char* name)
	{
		records.push_back({ name, Clock::now() });
	}
}