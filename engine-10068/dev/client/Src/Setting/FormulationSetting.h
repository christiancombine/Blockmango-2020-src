#pragma once
#ifndef __FORMULATION_SETTING_HEADER__
#define __FORMULATION_SETTING_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct FormulationItem
	{
		int id = 0;
		int ItemId = 0;
		int Count = 0;
		String Name = "";
		String Icon = "";
		String Materials = "";
	};

	struct MaterialItem
	{
		int id = 0;
		int ItemId = 0;
		String Icon = "";
		int Count = 0;
	};

	class FormulationSetting
	{
	private:
		static map<int, FormulationItem*>::type m_formulation_map;
		static map<int, MaterialItem*>::type m_material_map;

	public:
		static void	loadSetting();
		static void unloadSetting();
		static FormulationItem* findFormulationById(int id);
		static MaterialItem* findMaterialById(int id);
		static map<int, FormulationItem*>::type getFormulationMap() { return m_formulation_map; }

	private:
		static void loadFormulationSetting();
		static void loadMaterialSetting();
	};
}

#endif 
