#pragma once
#ifndef __DESIGNATION_SETTING_HEADER__
#define  __DESIGNATION_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct DesignationItem
	{
		int Id = 0;
		int Type = 0;
		int Condition = 0;
		int Weight = 0;
		String Name = "";
		int Status = 1;
		String ConditionName = "";
	};

	struct DesignationTypeItem
	{
		int Id = 0;
		int Type = 0;
		int Weight = 0;
		String Name = "";
	};
	enum DesignationStatus
	{
		NOT_HAS= 1,
		HAS = 2,
		USE = 3
	};

	struct DesignationTotal
	{
		int num = 0;
	};
	class DesignationSetting
	{
	private:
		static map<int, DesignationItem*>::type m_designation_map;
		static map<int, DesignationTypeItem*>::type m_designation_type_map;
		static map<int, DesignationTotal*>::type m_designation_total_map;
	public:
		static void	loadSetting();
		static void unloadSetting();
		static DesignationItem* findDesignationById(int id);
		static DesignationTypeItem* findDesignationTypeById(int id);
		static DesignationTotal* findDesignationTotalById(int id);
		static map<int, DesignationItem*>::type getDesignationMap() { return m_designation_map; }
		static map<int, DesignationTypeItem*>::type getDesignationTypeMap() { return m_designation_type_map; }
		static map<int, DesignationTotal*>::type getDesignationTotalMap() { return m_designation_total_map; };
		static void parseUpdateDesignationData(const String & data);
		static void parseUpdateDesignationTypeData(const String & data);
	private:
		static void loadDesignationSetting();
		static void loadDesignationTypeSetting();
	};
}

#endif 


