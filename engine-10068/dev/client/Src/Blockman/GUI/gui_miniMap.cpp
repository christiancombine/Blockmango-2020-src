#include "gui_miniMap.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIImagesetManager.h"
#include "Resource/ResourceManager.h"
#include "cWorld/Blockman.h"
#include "World/World.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Object/Root.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityCreature.h"
#include  "Render/RenderGlobal.h"


/** 
	���ļ������漰�� start position ��Ϊ��Ļ���Ϸ���һ���� ��end position ��Ϊ��Ļ���·���һ����
*/

namespace BLOCKMAN
{

	gui_miniMap::gui_miniMap() : gui_layout("MiniMap.json")
	{
	}

	gui_miniMap::~gui_miniMap()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_miniMap::onLoad()
	{
		loadMiniMapConfig();
		m_airDropIcon = getWindow<GUIStaticImage>("MiniMap-Air-Drop");
		m_aircraftLocation = getWindow<GUIStaticImage>("MiniMap-Aircraft-Location");
		m_safetyZone = getWindow<GUIStaticImage>("MiniMap-SafetyZone");
		m_nextSafetyZone = getWindow<GUIStaticImage>("MiniMap-NextSafetyZone");
		m_airLineView = getWindow<GUIStaticImage>("MiniMap-AirLine");
		m_centerLine = getWindow<GUIStaticImage>("MiniMap-CenterLine");

		getWindow("MiniMap-Map")->SetClipChild(true);
		getWindow<GUIStaticImage>("MiniMap-Map")->SetImage(m_imagesetName.c_str());
		getWindow("MiniMap-Map")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_miniMap::onMapSizeChange, this, std::placeholders::_1));
		m_airDropIcon->SetArea(UDim(0.0f, -8.0f), UDim(0.0f, -20.f), UDim(0, 16.f), UDim(0, 20.f));
		m_subscriptionGuard.add(ChickenAirDropPosition::subscribe(std::bind(&gui_miniMap::setAirDropPosition, this, std::placeholders::_1)));

		m_subscriptionGuard.add(WalkingDeadAttackPlayerEvent::subscribe(std::bind(&gui_miniMap::onAttackPlayerChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(WalkingDeadAttackedPlayerEvent::subscribe(std::bind(&gui_miniMap::onAttackedPlayerChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(WalkingDeadDiePlayerEvent::subscribe(std::bind(&gui_miniMap::onDiePlayerChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_miniMap::onUpdate(ui32 nTimeElapse)
	{
		getWindow("MiniMap")->SetSize({ { 0, m_miniMapWidth }, { 0, m_miniMapHeight } });
		EntityPlayers players = Blockman::Instance()->m_pWorld->getPlayers();
		for (auto item : players)
		{
			auto player = item.second;
			renderPlayerLocation(player->getPlatformUserId(), player->getTeamId(), player->position, player->getYaw());
		}
		renderSafetyZoneRange();
		renderAircraftAirLine();
		renderAirDropPosition();
		if (checkGameType(ClientGameType::WalkingDead))
		{
			renderCreaturePosition();
			renderAttackedPlayerPostion(nTimeElapse);
			renderAttackPlayerPostion(nTimeElapse);
			renderDiePlayerPostion(nTimeElapse);
		}
	}

	bool gui_miniMap::onMapSizeChange(const EventArgs & events)
	{
		m_isPanoramicMap = !m_isPanoramicMap;
		if (m_isPanoramicMap)
		{
			getWindow("MiniMap")->SetHorizontalAlignment(HA_CENTRE);
		}
		else
		{
			getWindow("MiniMap")->SetHorizontalAlignment(HA_RIGHT);
		}
		m_miniMapWidth = m_isPanoramicMap ? m_panoramaMapWidth : m_localMapWidth;
		m_miniMapHeight = m_isPanoramicMap ? m_panoramaMapHeight : m_localMapHeight;
		return true;
	}

	void gui_miniMap::loadMiniMapConfig()
	{
		if (m_configIsLoad)
		{
			return;
		}
		m_configIsLoad = true;

		String path = "";
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Blockman::Instance()->m_worldName, "minimap");
#else
		path = PathUtil::ConcatPath(Blockman::Instance()->m_worldName, "minimap");
#endif
		ResourceGroupManager::Instance()->addResourceLocation(path.c_str(), "FileSystem", "General");
		String miniMapConfigPath = Root::Instance()->getMapPath() + Blockman::Instance()->m_worldName + "/minimap/minimapConfig.json";
		if (!PathUtil::IsFileExist(miniMapConfigPath))
		{
			LordLogError("miniMapConfigPath path [ %s ] does not exist, close mini map", miniMapConfigPath.c_str());
			RootGuiLayout::Instance()->showMiniMap(false);
			m_miniMapWidth = 0.f;
			m_miniMapHeight = 0.f;
			return;
		}
		DataStream* stream = ResourceGroupManager::Instance()->openResource("minimapConfig.json");
		if (stream == NULL)
		{
			LordLogError("miniMapConfigPath path [ %s ] does not exist, close mini map", miniMapConfigPath.c_str());
			RootGuiLayout::Instance()->showMiniMap(false);
			m_miniMapWidth = 0.f;
			m_miniMapHeight = 0.f;
			return;
		}

		m_miniMapWidth = 200.f;
		m_miniMapHeight = 200.f;

		String str = "";
		while (!stream->eof())
		{
			str += stream->getLine();
		}
		stream->close();
		LordDelete(stream);

		rapidjson::Document doc;
		doc.Parse(str.c_str());

		if (doc.HasParseError()) {
			LordLogError("_parseMiniMapConfigPath fail, parse json fail, [%s]", str.c_str());
			RootGuiLayout::Instance()->showMiniMap(false);
			return;
		}

		if (!doc.HasMember("width") || !doc.HasMember("height") || !doc.HasMember("originPositionX") ||
			!doc.HasMember("originPositionY") || !doc.HasMember("originPositionZ") || !doc.HasMember("imagesetName"))
		{
			LordLogError("_parseMiniMapConfigPath fail, parse json fail, [%s]", str.c_str());
			RootGuiLayout::Instance()->showMiniMap(false);
			return;
		}

		String imageJsonName = doc.FindMember("imageJsonName")->value.GetString();
		m_imagesetName = doc.FindMember("imagesetName")->value.GetString();
		m_mapWidth = doc.FindMember("width")->value.GetInt();
		m_mapHeight = doc.FindMember("height")->value.GetInt();
		float x = doc.FindMember("originPositionX")->value.GetFloat();
		float y = doc.FindMember("originPositionY")->value.GetFloat();
		float z = doc.FindMember("originPositionZ")->value.GetFloat();

		if (doc.HasMember("panoramaMapWidth") && doc.HasMember("panoramaMapHeight"))
		{
			m_panoramaMapWidth = doc.FindMember("panoramaMapWidth")->value.GetFloat();
			m_panoramaMapHeight = doc.FindMember("panoramaMapHeight")->value.GetFloat();
		}

		if (doc.HasMember("localMapWidth") && doc.HasMember("localMapHeight"))
		{
			m_localMapWidth = doc.FindMember("localMapWidth")->value.GetFloat();
			m_localMapHeight = doc.FindMember("localMapHeight")->value.GetFloat();
		}

		m_worldOriginPosition = Vector3(x, y, z);
		RootGuiLayout::Instance()->showMiniMap(true);
		LordLogInfo("miniMapConfigPath info [%s]", str.c_str());
		GUIImageset* r = GUIImagesetManager::Instance()->CreateFromFile(imageJsonName.c_str());
		r->load();
		m_mapImager = r->DefineImage("clip_minimap", ERect(0.f, 0.f, 0.f, 0.f));
		m_imagesetName = StringUtil::Format("set:%s image:clip_minimap", imageJsonName.c_str());

		m_miniMapWidth = m_localMapWidth;
		m_miniMapHeight = m_localMapHeight;
	}

	void gui_miniMap::renderPlayerLocation(ui64  platformUserId, int teamId, Vector3 & playerVector, float playerYaw)
	{
		auto me = Blockman::Instance()->m_pPlayer;
		if (platformUserId == 0 || platformUserId != me->getPlatformUserId() && (teamId != me->getTeamId() || teamId == 0))
		{
			return;
		}

		if (!m_teamMemberLocation[platformUserId])
		{
			String iconName = StringUtil::Format("MiniMap-Locale-%llu", platformUserId);
			GUIStaticImage* icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
			icon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 10.f), UDim(0, 10.f));
			icon->SetImage("set:chicken_icon.json image:chichen_palyer_location_icon");
			if (checkGameType(ClientGameType::WalkingDead))
			{
				icon->SetImage("set:walkingDeadMiniMap.json image:player");
			}
			icon->SetTouchable(false);
			m_teamMemberLocation[platformUserId] = icon;
			getWindow("MiniMap")->AddChildWindow(icon);
		}

		//Mini map shows player location.
		m_teamMemberLocation[platformUserId]->SetRotate(playerYaw);

		float playerPositionX = positionXRatio(playerVector.x);
		float playerPositionZ = positionZRatio(playerVector.z);
		if (m_isPanoramicMap)
		{
			m_teamMemberLocation[platformUserId]->SetArea(UDim(playerPositionX, -14.f), UDim(playerPositionZ, -14.f), UDim(0, 28.f), UDim(0, 28.f));
			if (m_mapImager)
			{
				m_mapImager->SetUV(ERect(0.f, 0.f, 1.f, 1.f));
			}
		}
		else
		{
			m_teamMemberLocation[platformUserId]->SetArea(UDim(0.5f, -14.f), UDim(0.5f, -14.f), UDim(0, 28.f), UDim(0, 28.f));
			float mapX = playerPositionX;
			float mapY = playerPositionZ;
			if (m_mapImager)
			{
				m_mapImager->SetUV(ERect(mapX - 0.1f, mapY - 0.1f, mapX + 0.1f, mapY + 0.1f));
			}
		}
	}

	void gui_miniMap::renderSafetyZoneRange()
	{
		if (!Blockman::Instance()->m_globalRender->isStart())
		{
			m_safetyZone->SetVisible(false);
			m_nextSafetyZone->SetVisible(false);
			m_centerLine->SetVisible(false);
			return;
		}

		m_safetyZone->SetVisible(true);
		m_nextSafetyZone->SetVisible(true);
		m_centerLine->SetVisible(true);

		m_safetyZoneStartPosition = Blockman::Instance()->m_globalRender->getPoisonCircleStartPosition();
		m_safetyZoneEndPosition = Blockman::Instance()->m_globalRender->getPoisonCircleEndPosition();
		m_nextSafetyZoneStartPosition = Blockman::Instance()->m_globalRender->getSafeCircleStartPosition();
		m_nextSafetyZoneEndPosition = Blockman::Instance()->m_globalRender->getSafeCircleEndPosition();

		float safetyZoneStartX = positionXRatio(m_safetyZoneStartPosition.x);
		float safetyZoneStartZ = positionZRatio(m_safetyZoneStartPosition.z);

		float safetyZoneEndX = positionXRatio(m_safetyZoneEndPosition.x);
		float safetyZoneEndZ = positionZRatio(m_safetyZoneEndPosition.z);

		float nextSafetyZoneStartX = positionXRatio(m_nextSafetyZoneStartPosition.x);
		float nextSafetyZoneStartZ = positionZRatio(m_nextSafetyZoneStartPosition.z);

		float nextSafetyZoneEndX = positionXRatio(m_nextSafetyZoneEndPosition.x);
		float nextSafetyZoneEndZ = positionZRatio(m_nextSafetyZoneEndPosition.z);

		float nextSafetyZoneMidpointX = (nextSafetyZoneStartX + nextSafetyZoneEndX) / 2;
		float nextSafetyZoneMidpointZ = (nextSafetyZoneStartZ + nextSafetyZoneEndZ) / 2;

		auto playerVector = Blockman::Instance()->m_pPlayer->position;
		float playerPositionX = positionXRatio(playerVector.x);
		float playerPositionZ = positionZRatio(playerVector.z);

		m_centerLine->SetVisible(!(playerPositionX > nextSafetyZoneStartX && playerPositionX < nextSafetyZoneEndX && playerPositionZ > nextSafetyZoneStartZ && playerPositionZ < nextSafetyZoneEndZ));

		if (m_isPanoramicMap)
		{
			m_safetyZone->SetArea(UDim(safetyZoneStartX, 0.f), UDim(safetyZoneStartZ, 0.f), UDim(safetyZoneEndX - safetyZoneStartX, 0.f), UDim(safetyZoneEndZ - safetyZoneStartZ, 0));
			m_nextSafetyZone->SetArea(UDim(nextSafetyZoneStartX, 0.f), UDim(nextSafetyZoneStartZ, 0), UDim(nextSafetyZoneEndX - nextSafetyZoneStartX, 0), UDim(nextSafetyZoneEndZ - nextSafetyZoneStartZ, 0));

			float forPlayerMidpointX = (nextSafetyZoneMidpointX + playerPositionX) / 2;
			float forPlayerMidpointZ = (nextSafetyZoneMidpointZ + playerPositionZ) / 2;

			float centerLineDx = Math::Abs(nextSafetyZoneMidpointX - playerPositionX);
			float centerLineDz = Math::Abs(nextSafetyZoneMidpointZ - playerPositionZ);
			float distances = Math::Sqrt(centerLineDx*centerLineDx + centerLineDz * centerLineDz);
			float centerLineYaw = Math::ATan((playerPositionX - nextSafetyZoneMidpointX) / (playerPositionZ - nextSafetyZoneMidpointZ)) * 180.f / Math::PI;
			m_centerLine->SetRotate(-centerLineYaw);
			m_centerLine->SetArea(UDim(forPlayerMidpointX, -0.5f), UDim(forPlayerMidpointZ - (distances / 2), 0), UDim(0, 1.f), UDim(distances, 0));
		}
		else
		{
			//������Ϊ���ģ���ȡȫ����ͼ��1/5Ϊ�ֲ���ͼ
			//����ֲ���ͼ���λ��
			float mapStartX = playerPositionX - 0.1f;
			float mapStartZ = playerPositionZ - 0.1f;

			//����ֲ���ͼ�յ�λ��
			float mapEndX = playerPositionX + 0.1f;
			float mapEndZ = playerPositionZ + 0.1f;

			//���㵱ǰ��ȫ�����λ��
			float safetyZoneX = (safetyZoneStartX - mapStartX) / 0.2f;
			float safetyZoneZ = (safetyZoneStartZ - mapStartZ) / 0.2f;

			//���㵱ǰ��ȫ�����
			float safetyZoneW = (safetyZoneEndX - safetyZoneStartX) / 0.2f;
			float safetyZoneH = (safetyZoneEndZ - safetyZoneStartZ) / 0.2f;

			m_safetyZone->SetArea(UDim(safetyZoneX, 0.f), UDim(safetyZoneZ, 0), UDim(safetyZoneW, 0), UDim(safetyZoneH, 0));

			float nextSafetyZoneX = (nextSafetyZoneStartX - mapStartX) / 0.2f;
			float nextSafetyZoneZ = (nextSafetyZoneStartZ - mapStartZ) / 0.2f;

			float nextSafetyZoneW = (nextSafetyZoneEndX - nextSafetyZoneStartX) / 0.2f;
			float nextSafetyZoneH = (nextSafetyZoneEndZ - nextSafetyZoneStartZ) / 0.2f;

			m_nextSafetyZone->SetArea(UDim(nextSafetyZoneX, 0.f), UDim(nextSafetyZoneZ, 0), UDim(nextSafetyZoneW, 0), UDim(nextSafetyZoneH, 0));

			//������һ����ȫ����Ծֲ���ͼ��λ��
			nextSafetyZoneMidpointX = (nextSafetyZoneMidpointX - mapStartX) / 0.2f;
			nextSafetyZoneMidpointZ = (nextSafetyZoneMidpointZ - mapStartZ) / 0.2f;


			float minimapMidpointX = 0.5f;
			float minimapMidpointZ = 0.5f;

			float forMinimapMidpointX = (nextSafetyZoneMidpointX + minimapMidpointX) / 2;
			float forMinimapMidpointZ = (nextSafetyZoneMidpointZ + minimapMidpointZ) / 2;

			//���������ľ���
			float centerLineDx = Math::Abs(nextSafetyZoneMidpointX - minimapMidpointX);
			float centerLineDz = Math::Abs(nextSafetyZoneMidpointZ - minimapMidpointZ);
			float distances = Math::Sqrt(centerLineDx*centerLineDx + centerLineDz * centerLineDz);

			//������ת�Ƕ�
			float centerLineYaw = Math::ATan((minimapMidpointX - nextSafetyZoneMidpointX) / (minimapMidpointZ - nextSafetyZoneMidpointZ)) * 180.f / Math::PI;
			m_centerLine->SetRotate(-centerLineYaw);
			m_centerLine->SetArea(UDim(forMinimapMidpointX, -0.5f), UDim(forMinimapMidpointZ - distances / 2, 0), UDim(0, 1.f), UDim(distances, 0));
		}
	}

	void gui_miniMap::renderAircraftAirLine()
	{
		auto aircraft = Blockman::Instance()->getWorld()->getAircraft();
		if (aircraft && m_isPanoramicMap && Blockman::Instance()->m_pPlayer->isOnAircraft())
		{
			m_airLineView->SetVisible(true);
			m_aircraftLocation->SetVisible(true);
			m_aircraftLocation->SetRotate(aircraft->rotationYaw - 45.f);
			float aircraftPositionX = positionXRatio(aircraft->position.x);
			float aircraftPositionZ = positionZRatio(aircraft->position.z);
			m_aircraftLocation->SetArea(UDim(aircraftPositionX, -25.f), UDim(aircraftPositionZ, -25.f), UDim(0, 50.f), UDim(0, 50.f));
			Vector3 airLineStart = aircraft->getAirLineStartPosition();
			Vector3 airLineEnd = aircraft->getAirLineEndPosition();

			float dx = airLineEnd.x - airLineStart.x;
			float dz = airLineEnd.z - airLineStart.z;
			float distances = Math::Sqrt(dx*dx + dz * dz);

			Vector3 midpoint = (airLineStart + airLineEnd) / 2;

			float midpointX = positionXRatio(midpoint.x);
			float midpointZ = positionZRatio(midpoint.z);
			m_airLineView->SetArea(UDim(midpointX, -0.5f), UDim(midpointZ, -distances / 2), UDim(0, 1.f), UDim(0, distances));
			m_airLineView->SetRotate(aircraft->rotationYaw);

			for (auto view : m_teamMemberLocation)
			{
				view.second->SetVisible(false);
			}
		}
		else
		{
			m_aircraftLocation->SetVisible(false);
			m_airLineView->SetVisible(false);
			for (auto view : m_teamMemberLocation)
			{
				view.second->SetVisible(true);
			}
		}
	}

	void gui_miniMap::renderAirDropPosition()
	{
		if (m_airDropPosition.x == 0 && m_airDropPosition.y == 0 && m_airDropPosition.z == 0)
		{
			m_airDropIcon->SetVisible(false);
			return;
		}
		m_airDropIcon->SetVisible(true);
		float airDropPositionX = positionXRatio(m_airDropPosition.x);
		float airDropPositionZ = positionZRatio(m_airDropPosition.z);
		if (m_isPanoramicMap)
		{
			m_airDropIcon->SetArea(UDim(airDropPositionX, -15.f), UDim(airDropPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
		}
		else
		{
			auto playerVector = Blockman::Instance()->m_pPlayer->position;
			float mapStartPositionX = positionXRatio(playerVector.x) - 0.1f;
			float mapStartPositionZ = positionZRatio(playerVector.z) - 0.1f;

			airDropPositionX = (airDropPositionX - mapStartPositionX) / 0.2f;
			airDropPositionZ = (airDropPositionZ - mapStartPositionZ) / 0.2f;

			m_airDropIcon->SetArea(UDim(airDropPositionX, -15.f), UDim(airDropPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
		}

		auto world = Blockman::Instance()->m_pWorld;
		Vector3 pos = Vector3(m_airDropPosition.x + (world->m_Rand.nextFloat() - 0.5f) * 2, m_airDropPosition.y + world->m_Rand.nextFloat() * 20, m_airDropPosition.z + (world->m_Rand.nextFloat() - 0.5f) * 2);
		world->spawnParticle("mobSpell", pos, Vector3(0, 0, 0.9f));
	}

	float gui_miniMap::positionXRatio(float inWorldPositionX)
	{
		return  (inWorldPositionX - m_worldOriginPosition.x) / m_mapWidth;
	}

	float gui_miniMap::positionZRatio(float inWorldPositionZ)
	{
		return (inWorldPositionZ - m_worldOriginPosition.z) / m_mapHeight;
	}

	/*--------------WalkingDead  Start-------------------*/
	void gui_miniMap::renderCreaturePosition()
	{
		auto EntityMap = Blockman::Instance()->m_pWorld->getLoadedEntityMap();
		int playerEntityId = Blockman::Instance()->m_pPlayer->entityId;
		for (auto it = EntityMap->begin(); it != EntityMap->end(); it++)
		{
			EntityCreature* pAI = dynamic_cast<EntityCreature*>(it->second);
			if (pAI)
			{
				String iconName = StringUtil::Format("MiniMap-Creature-%d", pAI->entityId);
				map<int, String>::iterator creatureItem = m_creature.find(pAI->entityId);
				GUIStaticImage* creatureIcon = nullptr;
				if (pAI->getCurrentTargetEntityId() == playerEntityId)
				{
					if (creatureItem == m_creature.end())
					{
						m_creature[pAI->entityId] = iconName;
						creatureIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
						creatureIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 10.f), UDim(0, 10.f));
						creatureIcon->SetImage("set:walkingDeadMiniMap.json image:creaturePosition");
						creatureIcon->SetTouchable(false);
						getWindow("MiniMap")->AddChildWindow(creatureIcon);
					}
					int indx = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
					creatureIcon = dynamic_cast<GUIStaticImage*>(getWindow("MiniMap")->GetChildByIndex(indx));
					if (creatureIcon)
					{
						float pAIPositionX = positionXRatio(pAI->position.x);
						float pAIPositionZ = positionZRatio(pAI->position.z);
						if (m_isPanoramicMap)
						{
							creatureIcon->SetArea(UDim(pAIPositionX, -15.f), UDim(pAIPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
						}
						else
						{
							auto playerVector = Blockman::Instance()->m_pPlayer->position;
							float mapStartPositionX = positionXRatio(playerVector.x) - 0.1f;
							float mapStartPositionZ = positionZRatio(playerVector.z) - 0.1f;

							pAIPositionX = (pAIPositionX - mapStartPositionX) / 0.2f;
							pAIPositionZ = (pAIPositionZ - mapStartPositionZ) / 0.2f;

							creatureIcon->SetArea(UDim(pAIPositionX, -15.f), UDim(pAIPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
						}
					}
				}
				else if (creatureItem != m_creature.end() && pAI->getCurrentTargetEntityId() != playerEntityId)
				{
					GUIWindowManager::Instance()->DestroyGUIWindow(creatureItem->second.c_str());
					getWindow("MiniMap")->RemoveChildWindow((GUIString)(creatureItem->second.c_str()));
					m_creature.erase(pAI->entityId);
				}
			}
		}
		if (!m_creature.empty())
		{
			//m_creature[1115] = StringUtil::Format("MiniMap-Creature-%d", 1115);;

			for (auto it = m_creature.begin(); it != m_creature.end(); )
			{
				Entity * pCreature =Blockman::Instance()->m_pWorld->getEntity(it->first);
				if (pCreature == nullptr)
				{
					GUIWindowManager::Instance()->DestroyGUIWindow(it->second.c_str());
					getWindow("MiniMap")->RemoveChildWindow((GUIString)(it->second.c_str()));
					m_creature.erase(it++);
				}
				else
				{
					++it;
				}

			}
		}
	}

	void gui_miniMap::renderAttackPlayerPostion(ui32 nTimeElapse)
	{
		for (auto it = m_attackPlayer.begin(); it != m_attackPlayer.end(); it++)
		{
			if (isDiePlayer(it->first))
			{
				it->second = m_stayTime;
			}

			it->second += nTimeElapse;
			String iconName = StringUtil::Format("MiniMap-attackPlayer-%llu", it->first);
			int index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
			GUIStaticImage* attackPlayerIcon = nullptr;
			if (it->second >= m_stayTime && index != -1)
			{
				GUIWindowManager::Instance()->DestroyGUIWindow(iconName.c_str());
				getWindow("MiniMap")->RemoveChildWindow((GUIString)(iconName.c_str()));
			}
			else if (it->second < m_stayTime)
			{
				if (index == -1)
				{
					attackPlayerIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
					attackPlayerIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 10.f), UDim(0, 10.f));
					attackPlayerIcon->SetImage("set:walkingDeadMiniMap.json image:attack");
					attackPlayerIcon->SetTouchable(false);
					getWindow("MiniMap")->AddChildWindow(attackPlayerIcon);
				}
				index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
				attackPlayerIcon = dynamic_cast<GUIStaticImage*>(getWindow("MiniMap")->GetChildByIndex(index));
				if (attackPlayerIcon)
				{
					auto world = BLOCKMAN::Blockman::Instance()->m_pWorld;
					auto players = world->getPlayers();
					for (auto player : players)
					{
						if (player.second->getPlatformUserId() == it->first)
						{
							float attackPlayerPositionX = positionXRatio(player.second->position.x);
							float attackPlayerPositionZ = positionZRatio(player.second->position.z);
							if (m_isPanoramicMap)
							{
								attackPlayerIcon->SetArea(UDim(attackPlayerPositionX, -25.f), UDim(attackPlayerPositionZ, -25.f), UDim(0, 50.f), UDim(0, 50.f));
							}
							else
							{
								auto playerVector = Blockman::Instance()->m_pPlayer->position;
								float mapStartPositionX = positionXRatio(playerVector.x) - 0.1f;
								float mapStartPositionZ = positionZRatio(playerVector.z) - 0.1f;

								attackPlayerPositionX = (attackPlayerPositionX - mapStartPositionX) / 0.2f;
								attackPlayerPositionZ = (attackPlayerPositionZ - mapStartPositionZ) / 0.2f;

								attackPlayerIcon->SetArea(UDim(attackPlayerPositionX, -25.f), UDim(attackPlayerPositionZ, -25.f), UDim(0, 50.f), UDim(0, 50.f));
							}
						}
					}

				}
			}

		}

	}
	bool gui_miniMap::onAttackPlayerChange(ui64 platformUserId)
	{
		auto it = m_attackPlayer.find(platformUserId);
		if (it == m_attackPlayer.end())
		{
			m_attackPlayer[platformUserId] = 0;
		}
		else
		{
			it->second = 0;
		}
		return true;
	}

	void gui_miniMap::renderAttackedPlayerPostion(ui32 nTimeElapse)
	{
		for (auto it = m_attackedPlayer.begin(); it != m_attackedPlayer.end(); it++)
		{

			auto item = m_attackPlayer.find(it->first);
			if (item != m_attackPlayer.end() && item->second < m_stayTime)
			{
				it->second = m_stayTime;
			}
			if (isDiePlayer(it->first))
			{
				it->second = m_stayTime;
			}

			it->second += nTimeElapse;
			String iconName = StringUtil::Format("MiniMap-attackedPlayer-%llu", it->first);
			int index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
			GUIStaticImage* attackedPlayerIcon = nullptr;
			if (it->second >= m_stayTime && index != -1)
			{
				GUIWindowManager::Instance()->DestroyGUIWindow(iconName.c_str());
				getWindow("MiniMap")->RemoveChildWindow((GUIString)(iconName.c_str()));
			}
			else if (it->second < m_stayTime)
			{
				if (index == -1)
				{
					attackedPlayerIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
					attackedPlayerIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 10.f), UDim(0, 10.f));
					attackedPlayerIcon->SetImage("set:walkingDeadMiniMap.json image:attacked");
					attackedPlayerIcon->SetTouchable(false);
					getWindow("MiniMap")->AddChildWindow(attackedPlayerIcon);
				}
				index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
				attackedPlayerIcon = dynamic_cast<GUIStaticImage*>(getWindow("MiniMap")->GetChildByIndex(index));
				if (attackedPlayerIcon)
				{
					auto world = BLOCKMAN::Blockman::Instance()->m_pWorld;
					auto players = world->getPlayers();
					for (auto player : players)
					{
						if (player.second->getPlatformUserId() == it->first)
						{
							float attackedPlayerPositionX = positionXRatio(player.second->position.x);
							float attackedPlayerPositionZ = positionZRatio(player.second->position.z);
							if (m_isPanoramicMap)
							{
								attackedPlayerIcon->SetArea(UDim(attackedPlayerPositionX, -25.f), UDim(attackedPlayerPositionZ, -25.f), UDim(0, 50.f), UDim(0, 50.f));
							}
							else
							{
								auto playerVector = Blockman::Instance()->m_pPlayer->position;
								float mapStartPositionX = positionXRatio(playerVector.x) - 0.1f;
								float mapStartPositionZ = positionZRatio(playerVector.z) - 0.1f;

								attackedPlayerPositionX = (attackedPlayerPositionX - mapStartPositionX) / 0.2f;
								attackedPlayerPositionZ = (attackedPlayerPositionZ - mapStartPositionZ) / 0.2f;

								attackedPlayerIcon->SetArea(UDim(attackedPlayerPositionX, -25.f), UDim(attackedPlayerPositionZ, -25.f), UDim(0, 50.f), UDim(0, 50.f));
							}
						}
					}

				}
			}

		}
	}
	bool gui_miniMap::onAttackedPlayerChange(ui64 platformUserId)
	{
		auto it = m_attackedPlayer.find(platformUserId);
		if (it == m_attackedPlayer.end())
		{
			m_attackedPlayer[platformUserId] = 0;
		}
		else
		{
			it->second = 0;
		}
		return true;
	}

	void gui_miniMap::renderDiePlayerPostion(ui32 nTimeElapse)
	{
		for (auto it = m_diePlayer.begin(); it != m_diePlayer.end(); it++)
		{
			it->second.time += nTimeElapse;
			String iconName = StringUtil::Format("MiniMap-diePlayer-%llu", it->first);
			int index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
			GUIStaticImage* diePlayerIcon = nullptr;
			if (it->second.time >= m_stayTime && index != -1)
			{
				GUIWindowManager::Instance()->DestroyGUIWindow(iconName.c_str());
				getWindow("MiniMap")->RemoveChildWindow((GUIString)(iconName.c_str()));
			}
			else if (it->second.time < m_stayTime)
			{
				if (index == -1)
				{
					diePlayerIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconName.c_str());
					diePlayerIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 10.f), UDim(0, 10.f));
					diePlayerIcon->SetImage("set:walkingDeadMiniMap.json image:die");
					diePlayerIcon->SetTouchable(false);
					getWindow("MiniMap")->AddChildWindow(diePlayerIcon);
				}
				index = getWindow("MiniMap")->GetChildIndex(iconName.c_str());
				diePlayerIcon = dynamic_cast<GUIStaticImage*>(getWindow("MiniMap")->GetChildByIndex(index));
				if (diePlayerIcon)
				{

					float diePlayerPositionX = positionXRatio(it->second.x);
					float diePlayerPositionZ = positionZRatio(it->second.z);
					if (m_isPanoramicMap)
					{
						diePlayerIcon->SetArea(UDim(diePlayerPositionX, -15.f), UDim(diePlayerPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
					}
					else
					{
						auto playerVector = Blockman::Instance()->m_pPlayer->position;
						float mapStartPositionX = positionXRatio(playerVector.x) - 0.1f;
						float mapStartPositionZ = positionZRatio(playerVector.z) - 0.1f;

						diePlayerPositionX = (diePlayerPositionX - mapStartPositionX) / 0.2f;
						diePlayerPositionZ = (diePlayerPositionZ - mapStartPositionZ) / 0.2f;

						diePlayerIcon->SetArea(UDim(diePlayerPositionX, -15.f), UDim(diePlayerPositionZ, -15.f), UDim(0, 30.f), UDim(0, 30.f));
					}
				}
			}
		}
	}

	bool gui_miniMap::onDiePlayerChange(ui64 platformUserId, float x, float y, float z)
	{
		auto it = m_diePlayer.find(platformUserId);
		if (it == m_diePlayer.end())
		{
			Point pos;
			pos.time = 0;
			pos.x = x;
			pos.y = y;
			pos.z = z;
			m_diePlayer[platformUserId] = pos;
		}
		else
		{
			it->second.time = 0;
			it->second.x = x;
			it->second.y = y;
			it->second.z = z;
		}
		return true;
	}
	bool gui_miniMap::isDiePlayer(ui64 platformUserId)
	{
		auto it = m_diePlayer.find(platformUserId);
		if (it != m_diePlayer.end() && it->second.time < m_stayTime)
		{
			return true;
		}
		return false;
	}
}

	/*--------------WalkingDead  End---------------------*/