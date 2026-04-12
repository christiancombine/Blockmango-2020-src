#include "CarSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, CarSetting*> CarSetting::m_carSettingMap = {};
	bool CarSetting::m_sbIsCarFree = false;

	bool CarSetting::loadSetting(bool bIsClient)
	{
		CarSetting* pSetting = NULL;
		m_carSettingMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (bIsClient)
		{
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/car.csv")).c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/car.csv")).c_str();
#else
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "car.csv")).c_str();
#endif
		}

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open car setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new CarSetting();
			pReader->GetInt("CarId", pSetting->carId);
			pReader->GetInt("Type", pSetting->type);
			pReader->GetInt("MoneyType", pSetting->moneyType);
			pReader->GetInt("Price", pSetting->price);
			pReader->GetInt("SpeedSound", pSetting->speedSound);
			pReader->GetInt("NormalSound", pSetting->normalSound);
			pReader->GetInt("ExtraSound", pSetting->extraSound);
			pReader->GetInt("Passengers", pSetting->maxPassengers);
			pReader->GetFloat("PassengersDamage", pSetting->passengersDamage);

			pReader->GetFloat("Traction", pSetting->traction, 5000);
			pReader->GetFloat("ReverseTraction", pSetting->reverseTraction, 1000);
			pReader->GetFloat("BrakingForce", pSetting->brakingForce, 20000);
			pReader->GetFloat("Mass", pSetting->mass, 1500);
			pReader->GetFloat("CoefAirDrag", pSetting->coefAirDrag, 4.5);
			pReader->GetFloat("CoefRollingFriction", pSetting->coefRollingFriction, 0.03);
			pReader->GetFloat("MinTurningRadius", pSetting->minTurningRadius, 7);
			pReader->GetFloat("MaxCentripetalForce", pSetting->maxCentripetalForce, 35000);
			pReader->GetFloat("CorneringStiffness", pSetting->corneringStiffness, 0.04);
			pReader->GetFloat("MaxCoefLateralFriction", pSetting->maxCoefLateralFriction, 2);
			pReader->GetFloat("CoefRestitution", pSetting->coefRestitution, 0.5);
			pReader->GetFloat("MinBounceSpeed", pSetting->minBounceSpeed, 5);

			pReader->GetString("Name", pSetting->name);
			pReader->GetString("Model", pSetting->model);
			pReader->GetString("Texture", pSetting->texture);

			m_carSettingMap[pSetting->carId] = pSetting;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void CarSetting::unloadSetting()
	{
		for (auto iter = m_carSettingMap.begin(); iter != m_carSettingMap.end(); iter++)
		{
			CarSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_carSettingMap.clear();
	}

	CarSetting * CarSetting::getCarSetting(int carId)
	{
		auto iter = m_carSettingMap.find(carId);
		if (iter != m_carSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}
