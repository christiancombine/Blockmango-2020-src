#include "WorldImageRender.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderEntity.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Model/ItemModelMgr.h"
#include "Render/RenderState.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/PlayerPrefs.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	WorldImageRender::WorldImageRender()
	{
		String mappings[19][2] = {
			{ "0", "0" }, { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
			{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" }, { "9", "9" },
			{ "B", "B" }, { "E", "E" }, { "K", "K" }, { "M", "M" }, { "P", "P" },
			{ "T", "T" }, { "Y", "Y" }, { "Z", "Z" }, { ".", "point" }
		};

		for (auto mapping : mappings)
		{
			// --- add red(1) numbers
			m_numberSpriteMap.insert({ StringUtil::Format("1_%s", mapping[0].c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite("float_number.json", StringUtil::Format("number_1_%s", mapping[1].c_str())) });
			// --- add yellow(2) numbers
			m_numberSpriteMap.insert({ StringUtil::Format("2_%s", mapping[0].c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite("float_number.json", StringUtil::Format("number_2_%s", mapping[1].c_str())) });
			// --- add blue(3) numbers
			m_numberSpriteMap.insert({ StringUtil::Format("3_%s", mapping[0].c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite("float_number.json", StringUtil::Format("number_3_%s", mapping[1].c_str())) });
			// --- add purple(4) numbers
			m_numberSpriteMap.insert({ StringUtil::Format("4_%s", mapping[0].c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite("float_number.json", StringUtil::Format("number_4_%s", mapping[1].c_str())) });
			// --- add green(5) numbers
			m_numberSpriteMap.insert({ StringUtil::Format("5_%s", mapping[0].c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite("float_number.json", StringUtil::Format("number_5_%s", mapping[1].c_str())) });
		}
	}

	WorldImageRender::~WorldImageRender()
	{
		m_numberSpriteMap.clear();
	}

	void WorldImageRender::addImageRender(const String& atlas, int color, const String& numberText, const String& image)
	{
		m_numberSpriteMap.insert({ StringUtil::Format("%d_%s", color, numberText.c_str()),
				TextureAtlasRegister::Instance()->getAtlasSprite(atlas, image) });
	}

	void WorldImageRender::render(int frameTime)
	{
		renderFloatNumber(frameTime);
		renderStaticNumber(frameTime);
		renderStaticImage(frameTime);
	}

	void WorldImageRender::addFloatNumber(const String & numberText, Vector3 position, float scale, int color)
	{
		FloatNumber number;
		number.m_position = position;
		number.m_duration = PlayerPrefs::Instance()->getIntPrefs("ShowFloatNumberTime");
		number.m_scale = scale;
		for (size_t i = 0; i < numberText.size(); i++)
		{
			String key = StringUtil::ToString(color) + "_" + String(1, numberText[i]);
			auto iter = m_numberSpriteMap.find(key);
			if (iter != m_numberSpriteMap.end())
			{
				number.m_spriteList.emplace_back(iter->second);
			}
		}
		if (number.m_spriteList.empty())
			return;
		m_floatNumberList.emplace_back(number);
	}

	void WorldImageRender::addStaticNumber(const String & name, const String & numberText, Vector3 position, int duration, float scale, int color, float yaw)
	{
		removeStaticNumber(name);
		StaticNumber number;
		number.m_name = name;
		number.m_position = position;
		number.m_duration = duration;
		number.m_scale = scale;
		number.m_yaw = yaw;
		for (size_t i = 0; i < numberText.size(); i++)
		{
			String key = StringUtil::ToString(color) + "_" + String(1, numberText[i]);
			auto iter = m_numberSpriteMap.find(key);
			if (iter != m_numberSpriteMap.end())
			{
				number.m_spriteList.emplace_back(iter->second);
			}
		}
		if (number.m_spriteList.empty())
			return;
		m_staticNumberList.emplace_back(number);
	}

	void WorldImageRender::removeStaticNumber(const String & name)
	{
		for (auto iter = m_staticNumberList.begin(); iter != m_staticNumberList.end(); iter++)
		{
			if ((*iter).m_name == name)
			{
				(*iter).m_spriteList.clear();
				m_staticNumberList.erase(iter);
				break;
			}
		}
	}

	void WorldImageRender::addStaticImage(const String & name, const String & atlas, const String & sprite, Vector3 position, float scale, bool isCamera, float yaw)
	{
		removeStaticImage(name);
		AtlasSprite* atlasSprite = NULL;
		if (Root::Instance()->isChina())
			atlasSprite = TextureAtlasRegister::Instance()->getAtlasSprite(atlas, sprite + "_china");
		if (atlasSprite == NULL)
			atlasSprite = TextureAtlasRegister::Instance()->getAtlasSprite(atlas, sprite);
		if (atlasSprite == NULL)
			return;
		StaticImage image;
		image.m_name = name;
		image.m_position = position;
		image.m_scale = scale;
		image.m_sprite = atlasSprite;
		image.m_isCamera = isCamera;
		image.m_yaw = yaw;
		m_staticImageList.emplace_back(image);
	}

	void WorldImageRender::removeStaticImage(const String & name)
	{
		for (auto iter = m_staticImageList.begin(); iter != m_staticImageList.end(); iter++)
		{
			if ((*iter).m_name == name)
			{
				(*iter).m_sprite = nullptr;;
				m_staticImageList.erase(iter);
				break;
			}
		}
	}

	void WorldImageRender::removeAllFloatNumber()
	{
		for (auto iter = m_floatNumberList.begin(); iter != m_floatNumberList.end(); )
		{
			(*iter).m_spriteList.clear();
			iter = m_floatNumberList.erase(iter);
		}
	}

	void WorldImageRender::renderFloatNumber(int frameTime)
	{
		RenderEntity* pRenderEntity = Blockman::Instance()->m_entityRender;
		NULL_RETURN_VOID(pRenderEntity);

		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		NULL_RETURN_VOID(pPlayer);

		float playerPitch = pRenderEntity->m_cameraController.getCameraPitch() * Math::DEG2RAD;
		float playerYaw = pRenderEntity->m_cameraController.getCameraYaw() * Math::DEG2RAD;

		int skyBlockLight = pPlayer->getBrightnessForRender(0.f);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);
		Color color = Color(brightness) * Color(0xFFFFFFFF);
		float dirX = -Math::Sin((pRenderEntity->m_cameraController.getCameraYaw() + 90.0f) * Math::DEG2RAD);
		float dirZ = Math::Cos((pRenderEntity->m_cameraController.getCameraYaw() + 90.0f) * Math::DEG2RAD);
		float numberWidth = 0.46f;
		bool bFloatNumberRenderTop = PlayerPrefs::Instance()->getBoolPrefs("FloatNumberRenderTop");
		float fShowFloatNumberTime = PlayerPrefs::Instance()->getIntPrefs("ShowFloatNumberTime");
		float fFloatNumberTimeSpeed = PlayerPrefs::Instance()->getFloatPrefs("FloatNumberTimeSpeed");
		for (auto& number : m_floatNumberList)
		{
			float distance = (frameTime * number.m_scale / fShowFloatNumberTime) * fFloatNumberTimeSpeed;
			number.m_position.y += distance;
			number.m_duration -= frameTime;
			Vector3 position = number.m_position;
			float halfSize = (float)number.m_spriteList.size() * 0.5F + 0.5F;
			position.x -= numberWidth * number.m_scale * dirX * halfSize;
			position.z -= numberWidth * number.m_scale * dirZ * halfSize;
			Vector3 scale = Vector3(number.m_scale * 0.5F, number.m_scale * 0.5F, number.m_scale * 0.5F);
			for (auto sprite : number.m_spriteList)
			{
				NULL_CONTINUE(sprite);

				position.x += numberWidth * number.m_scale * dirX;
				position.z += numberWidth * number.m_scale * dirZ;
				Matrix4 matBall, matTemp;
				matBall.identity();
				matBall.translate(position);
				matTemp.identity();
				matTemp.rotateY(-playerYaw + Math::PI);
				matBall.joint(matTemp);
				matTemp.identity();
				matTemp.rotateX(-playerPitch);
				matBall.joint(matTemp);
				matBall.scale(scale);

				PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(sprite->getTexture(), bFloatNumberRenderTop ? SRP_TRANSPARENT_NODEPTH : SRP_TRANSPARENT, 1);

				float minu = sprite->getMinU();
				float maxu = sprite->getMaxU();
				float minv = sprite->getMinV();
				float maxv = sprite->getMaxV();
				float x = 1.0F;
				float x1 = 0.5F;
				float y = 0.25F;		

				SkinedVertexFmt vertices[4];
				vertices[0].setEx2(Vector3(0.f - x1, 0.f - y, 0.f), brightness, minu, maxv, 0);
				vertices[1].setEx2(Vector3(x - x1, 0.f - y, 0.f), brightness, maxu, maxv, 0);
				vertices[2].setEx2(Vector3(x - x1, x - y, 0.f), brightness, maxu, minv, 0);
				vertices[3].setEx2(Vector3(0.f - x1, x - y, 0.f), brightness, minu, minv, 0);

				pPackRenderable->addPackMass(vertices, 1, color.getARGB(), matBall);
			}
		}
		removeExpiredFloatNumber();
	}

	void WorldImageRender::renderStaticNumber(int frameTime)
	{
		RenderEntity* pRenderEntity = Blockman::Instance()->m_entityRender;
		NULL_RETURN_VOID(pRenderEntity);

		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		NULL_RETURN_VOID(pPlayer);

		int skyBlockLight = pPlayer->getBrightnessForRender(0.f);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);
		Color color = Color(brightness) * Color(0xFFFFFFFF);
	
		float numberWidth = 0.46f;
		bool bStaticNumberRenderTop = PlayerPrefs::Instance()->getBoolPrefs("StaticNumberRenderTop");
		bool bStaticNumberRenderSolid = PlayerPrefs::Instance()->getBoolPrefs("bStaticNumberRenderSolid");
		for (auto& number : m_staticNumberList)
		{
			float dirX = -Math::Sin((number.m_yaw - 90.0f) * Math::DEG2RAD);
			float dirZ = Math::Cos((number.m_yaw - 90.0f) * Math::DEG2RAD);
			if (number.m_duration > 0)
			{
				number.m_duration -= frameTime;
				if (number.m_duration < 0)
					number.m_duration = 0;
			}
			Vector3 position = number.m_position;
			float halfSize = (float)number.m_spriteList.size() * 0.5F + 0.5F;
			position.x -= numberWidth * number.m_scale * dirX * halfSize;
			position.z -= numberWidth * number.m_scale * dirZ * halfSize;
			Vector3 scale = Vector3(number.m_scale * 0.5F, number.m_scale * 0.5F, number.m_scale * 0.5F);
			for (auto sprite : number.m_spriteList)
			{
				position.x += numberWidth * number.m_scale * dirX;
				position.z += numberWidth * number.m_scale * dirZ;
				Matrix4 matBall, matTemp;
				matBall.identity();
				matBall.translate(position);
				matTemp.identity();
				matTemp.rotateY((-number.m_yaw + 180) * Math::DEG2RAD + Math::PI);
				matBall.joint(matTemp);
				matBall.scale(scale);

				PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(sprite->getTexture(), bStaticNumberRenderTop ? SRP_TRANSPARENT_NODEPTH : bStaticNumberRenderSolid ? SRP_SOLID : SRP_TRANSPARENT, 1);
				float minu = sprite->getMinU();
				float maxu = sprite->getMaxU();
				float minv = sprite->getMinV();
				float maxv = sprite->getMaxV();
				float x = 1.0F;
				float x1 = 0.5F;
				float y = 0.25F;

				SkinedVertexFmt vertices[4];
				vertices[0].setEx2(Vector3(0.f - x1, 0.f - y, 0.f), brightness, minu, maxv, 0);
				vertices[1].setEx2(Vector3(x - x1, 0.f - y, 0.f), brightness, maxu, maxv, 0);
				vertices[2].setEx2(Vector3(x - x1, x - y, 0.f), brightness, maxu, minv, 0);
				vertices[3].setEx2(Vector3(0.f - x1, x - y, 0.f), brightness, minu, minv, 0);

				pPackRenderable->addPackMass(vertices, 1, color.getARGB(), matBall);
			}
		}
		removeExpiredStaticNumber();
	}

	void WorldImageRender::renderStaticImage(int frameTime)
	{
		RenderEntity* pRenderEntity = Blockman::Instance()->m_entityRender;
		NULL_RETURN_VOID(pRenderEntity);

		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		NULL_RETURN_VOID(pPlayer);

		int skyBlockLight = pPlayer->getBrightnessForRender(0.f);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);
		Color color = Color(brightness) * Color(0xFFFFFFFF);
	
		for (auto& number : m_staticImageList)
		{
			if (number.m_sprite == NULL)
				continue;

			Matrix4 matBall, matTemp;
			matBall.identity();
			matBall.translate(number.m_position);
			if (number.m_isCamera)
			{
				float playerPitch = pRenderEntity->m_cameraController.getCameraPitch() * Math::DEG2RAD;
				float playerYaw = pRenderEntity->m_cameraController.getCameraYaw() * Math::DEG2RAD;
				matTemp.identity();
				matTemp.rotateY(-playerYaw + Math::PI);
				matBall.joint(matTemp);
				matTemp.identity();
				matTemp.rotateX(-playerPitch);
				matBall.joint(matTemp);
			}
			else
			{
				matTemp.identity();
				matTemp.rotateY((-number.m_yaw + 180) * Math::DEG2RAD + Math::PI);
				matBall.joint(matTemp);
			}
			matBall.scale(Vector3(number.m_scale, number.m_scale, number.m_scale));

			PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(number.m_sprite->getTexture(), SRP_SOLID, 1);

			float minu = number.m_sprite->getMinU();
			float maxu = number.m_sprite->getMaxU();
			float minv = number.m_sprite->getMinV();
			float maxv = number.m_sprite->getMaxV();
	
			float x = (float)(number.m_sprite->getFrameSize().x) / 128;
			float y = (float)(number.m_sprite->getFrameSize().y) / 128;

			SkinedVertexFmt vertices[4];
			vertices[0].setEx2(Vector3(-x, -y, 0.f), brightness, minu, maxv, 0);
			vertices[1].setEx2(Vector3(x, -y, 0.f), brightness, maxu, maxv, 0);
			vertices[2].setEx2(Vector3(x, y, 0.f), brightness, maxu, minv, 0);
			vertices[3].setEx2(Vector3(-x, y, 0.f), brightness, minu, minv, 0);

			pPackRenderable->addPackMass(vertices, 1, color.getARGB(), matBall);
		}
	}

	void WorldImageRender::removeExpiredFloatNumber()
	{
		for (auto iter = m_floatNumberList.begin(); iter != m_floatNumberList.end(); )
		{
			if ((*iter).m_duration <= 0)
			{
				(*iter).m_spriteList.clear();
				iter = m_floatNumberList.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	void WorldImageRender::removeExpiredStaticNumber()
	{
		for (auto iter = m_staticNumberList.begin(); iter != m_staticNumberList.end(); )
		{
			if ((*iter).m_duration == 0)
			{
				(*iter).m_spriteList.clear();
				iter = m_staticNumberList.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
}


