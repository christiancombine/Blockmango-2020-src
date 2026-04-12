#pragma once
#ifndef __GUI_MINI_MAP_HEADER__
#define __GUI_MINI_MAP_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticImage.h"
#include "Util/SubscriptionGuard.h"
#include <map>


using namespace LORD;

namespace BLOCKMAN
{
	class gui_miniMap : public  gui_layout
	{
	public:
		struct Point
		{
			float x = 0;
			float y = 0;
			float z = 0;
			int time = 0;
		};
		gui_miniMap();
		virtual ~gui_miniMap();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onMapSizeChange(const EventArgs & events);
		bool setAirDropPosition(Vector3 position) {
			this->m_airDropPosition = position; 
			return true;
		};
		bool onAttackPlayerChange(ui64 platformUserId);
		bool onAttackedPlayerChange(ui64 platformUserId);
		bool onDiePlayerChange(ui64 platformUserId, float x, float y, float z);
	private:
		void loadMiniMapConfig();
		void renderPlayerLocation(ui64  platformUserId, int teamId, Vector3&  playerPosition, float playerYaw);
		void renderSafetyZoneRange();
		void renderAircraftAirLine();
		void renderAirDropPosition();
		float positionXRatio(float inWorldPositionX);
		float positionZRatio(float inWorldPositionZ);
		void renderCreaturePosition();
		void renderAttackPlayerPostion(ui32 nTimeElapse);
		void renderAttackedPlayerPostion(ui32 nTimeElapse);
		void renderDiePlayerPostion(ui32 nTimeElapse);
		bool isDiePlayer(ui64  platformUserId);
	private:
		SubscriptionGuard m_subscriptionGuard;
		bool m_configIsLoad = false;
		bool m_isPanoramicMap = false;
		float m_miniMapWidth = 200.f;
		float m_miniMapHeight = 200.f;

		float m_panoramaMapWidth = 400.f;
		float m_panoramaMapHeight = 400.f;
		float m_localMapWidth = 200.f;
		float m_localMapHeight = 200.f;
		float CV_PI = 3.14;
		int m_mapWidth = 512;
		int m_mapHeight = 512;
		String m_imagesetName = "";
		Vector3 m_airDropPosition = Vector3(0, 0, 0);
		Vector3 m_worldOriginPosition = Vector3(0.f, 0.f, 0.f);
		Vector3 m_safetyZoneStartPosition = Vector3(0, 0, 0);
		Vector3 m_safetyZoneEndPosition = Vector3(0, 0, 0);
		Vector3 m_nextSafetyZoneStartPosition = Vector3(0, 0, 0);
		Vector3 m_nextSafetyZoneEndPosition = Vector3(0, 0, 0);

		//Vector3 m_worldOriginPosition = Vector3(1505.f, 31.f, -815.f);
		//Vector3 m_safetyZoneStartPosition = Vector3(1505.f, 31.f, -815.f);
		//Vector3 m_safetyZoneEndPosition = Vector3(2017.f, 31.f, -303.f);
		//Vector3 m_nextSafetyZoneStartPosition = Vector3(1605.f, 31.f, -715.f);
		//Vector3 m_nextSafetyZoneEndPosition = Vector3(1917.f, 31.f, -403.f);

		map<i64, GUIWindow*>::type m_teamMemberLocation;
		map<int,String>::type m_creature;
		map<ui64, int>::type m_attackPlayer;
		map<ui64, int>::type m_attackedPlayer;
		map<ui64, Point>::type m_diePlayer;
		int m_stayTime = 10000;
		GUIImage* m_mapImager = nullptr;
		GUIStaticImage* m_airLineView = nullptr;
		GUIStaticImage* m_airDropIcon = nullptr;
		GUIStaticImage* m_safetyZone = nullptr;
		GUIStaticImage* m_nextSafetyZone = nullptr;
		GUIStaticImage* m_aircraftLocation = nullptr;
		GUIStaticImage* m_centerLine = nullptr;
	};

}
#endif