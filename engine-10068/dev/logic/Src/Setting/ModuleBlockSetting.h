#pragma once

#include <string>
#include <unordered_map>
#include "Util/MacroUtil.h"
#include "Util/CSVReader.h"

namespace BLOCKMAN
{
	enum RotationType
	{
		RotationType_None,
		RotationType_0,
		RotationType_90,
		RotationType_180,
		RotationType_270,
		RotationType_Max,
	};

	struct ModuleBlockData
	{
		int _id{ 0 }; // id
		std::string _moduleName; // module file name
		std::string _moduleIcon; // module icon
		int _costBlockNum{ 0 }; // cost block num
		std::vector<std::vector<int>> vecOffsetPos;
		std::vector<std::vector<int>> vecRotation;

		ModuleBlockData()
		{
			vecOffsetPos.resize(int(RotationType::RotationType_Max));
			vecRotation.resize(int(RotationType::RotationType_Max));
		}
	};
	using ModuleBlockDataMap = std::unordered_map<int, ModuleBlockData>;

	class ModuleBlockSetting
	{
	public:
		static bool loadSetting(bool isClient);
		static void unloadSetting();

		static const ModuleBlockData* getModuleBlockData(int id);
		static const ModuleBlockData* getModuleBlockData(std::string fileName);

	private:
		static void parseOffset(const std::shared_ptr<LORD::CsvReader>& pReader, const std::string& name, ModuleBlockData& data);

		static ModuleBlockDataMap m_ModuleBlockDataMap;
	};
}