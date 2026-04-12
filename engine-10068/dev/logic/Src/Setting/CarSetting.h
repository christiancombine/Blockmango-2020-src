#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class CarSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static CarSetting* getCarSetting(int carId);

		static bool m_sbIsCarFree;
		int					type;
		int					carId;
		int					moneyType;
		int					price;
		int					speedSound;
		int					normalSound;
		int					extraSound;
		int					maxPassengers;
		float               passengersDamage;

		float				traction;				// 最大牵引力(N)
		float				reverseTraction;		// 倒车最大牵引力(N)
		float				brakingForce;			// 制动力(N)
		float				mass;					// 质量(kg) 
		float				coefAirDrag;			// 空气阻力系数(N*s^2/m^2)
		float				coefRollingFriction;	// 滚动摩擦系数
		float				minTurningRadius;		// 最大转弯半径(m)
		float				maxCentripetalForce;	// 最大向心力(N)
		float				corneringStiffness;		// 轮胎侧偏刚度(1/deg)
		float				maxCoefLateralFriction;	// 轮胎最大侧向摩擦系数(1)
		float				coefRestitution;		// 碰撞恢复系数(1)
		float				minBounceSpeed;			// 最小反弹速度(m/s)

		String				name;
		String				model;
		String				texture;
	private:
		static std::map<int, CarSetting*> m_carSettingMap;
	};
}
