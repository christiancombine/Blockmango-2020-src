#include "RenderEntity.h"
#include "RenderGlobal.h"
#include "ItemRender.h"
#include "ActiveRenderInfo.h"
#include "VisualTessThread.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "World/GameSettings.h"
#include "World/RayTracyResult.h"
#include "World/World.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/Enchantment.h"
#include "Item/Potion.h"
#include "Item/Items.h"
#include "Chunk/ChunkService.h"

#include "cEntity/PlayerControlller.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "EntityRenders/EntityRenderable.h"
#include "cWorld/WorldClient.h"
#include "cEffects/EffectRenderer.h"
#include "cEffects/EntitySmokeFx.h"
#include "cEffects/EntityRainFx.h"

#include "Object/Root.h"
#include "Scene/Camera.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/FxRenderable.h"
#include "game.h"
#include "Common.h"
#include "Util/CPUTimer.h"

#include "rapidjson/document.h"
#include "Setting/LogicSetting.h"
#include "Entity/EntityBird.h"
#include "UI/Touch.h"
#include "Setting/SceneSetting.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Entity/EntityItem.h"
#include "Setting/MountSetting.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Plugins/GameClientEvents.h"
#include "Util/PlayerPrefs.h"

#undef max
#undef min

namespace BLOCKMAN
{

MouseFilter::MouseFilter()
	: a(0.f)
	, b(0.f)
	, c(0.f)
{}

float MouseFilter::smooth(float par1, float par2)
{
	a += par1;
	par1 = (a - b) * par2;
	c += (par1 - c) * 0.5F;

	if (par1 > 0.0F && par1 > c || par1 < 0.0F && par1 < c)
	{
		par1 = c;
	}

	b += par1;
	return par1;
}

//bool RenderEntity::anaglyphEnable = false;

void CameraController::setSeparate(bool isSeparate)
{
	if (m_isSeparate == isSeparate)
		return;
	m_isSeparate = isSeparate;
	m_cameraPrevYaw = m_cameraYaw;
	m_cameraPrevPitch = m_cameraPitch;
}

void CameraController::updateCameraAngles(float yaw, float pitch)
{
	float p = m_cameraPitch;
	float y = m_cameraYaw;
	m_cameraYaw = m_cameraYaw + yaw * 0.15f;
	m_cameraPitch = m_cameraPitch - pitch * 0.15f;

	if (m_cameraPitch < -90.0F)
		m_cameraPitch = -90.0F;

	if (m_cameraPitch > 90.0F)
		m_cameraPitch = 90.0F;

	m_cameraPrevPitch += m_cameraPitch - p;
	m_cameraPrevYaw += m_cameraYaw - y;
}

void CameraController::setCameraAngles(float yaw, float pitch, float prevYaw, float prevPitch)
{
	m_cameraYaw = yaw;
	m_cameraPitch = pitch;
	m_cameraPrevYaw = prevYaw;
	m_cameraPrevPitch = prevPitch;
}

RenderEntity::RenderEntity(Blockman* _mc)
{
	bm = _mc;

	itemRenderer = LordNew ItemRenderer(_mc);
	debugViewDirection = 0;
	rendererUpdateCount = 0;
	farPlaneDistance = 0;
	thirdPersonDistance = 3.f;
	thirdPersonDistanceTemp = 3.f;
	debugCamYaw = 0.f;
	prevDebugCamYaw = 0.f;
	debugCamPitch = 0.f;
	prevDebugCamPitch = 0.f;
	debugCamFOV = 0.f;
	prevDebugCamFOV = 0.f;
	camRoll = 0.f;
	prevCamRoll = 0.f;
	lightmapTexture = NULL;
	lightmapColors = NULL;
	fovModifierHand = 0.f;
	fovModifierHandPrev = 0.f;
	fovMultiplierTemp = 0.f;
	cloudFog = false;
	cameraZoom = 1.0;
	cameraYaw = 0.0;
	cameraPitch = 0.0;
	prevFrameTime = Root::Instance()->getCurrentTime();
	renderEndNanoTime = 0;
	lightmapUpdateNeeded = true;
	torchFlickerX = 0.f;
	torchFlickerDX = 0.f;
	torchFlickerY = 0.f;
	torchFlickerDY = 0.f;
	rainSoundCounter = 0;
	fogColor2 = 0.f;
	fogColor1 = 0.f;
	prevMouseX = 0;
	prevMouseY = 0;
	pointedEntity = NULL;
	cameraOffset = Vector3::ZERO;

	createLightMap();

	for (int i=0; i<24; ++i)
	{
		m_lightSettings[i].MainLightColor = Color(0.94f, 0.89f, 0.769f, 1.641f);
		m_lightSettings[i].SubLightColor = Color(0.531f, 0.765f, 0.751f, 0.645f);
	}
	loadLightSettings();
}

RenderEntity::~RenderEntity()
{
	LordSafeFree(lightmapColors);
	LordSafeDelete(itemRenderer);
	TextureManager::Instance()->releaseResource(lightmapTexture);
	if (rainXCoords)
	{
		LordSafeFree(rainXCoords);
		LordSafeFree(rainYCoords);
		TessManager::Instance()->destroyFxRenderable(m_rainRenderer);
		TessManager::Instance()->destroyFxRenderable(m_snowRenderer);
		TextureManager::Instance()->releaseResource(m_rainTex);
		TextureManager::Instance()->releaseResource(m_snowTex);
		m_rainRenderer = nullptr;
		m_snowRenderer = nullptr;
		m_rainTex = nullptr;
		m_snowTex = nullptr;
	}
}

int RenderEntity::createLightMap()
{
	LordAssert(!lightmapTexture);

	const int lightmapSize = 16;

	int nBufferSize = lightmapSize * lightmapSize * sizeof(int);
	lightmapColors = (int*)LordMalloc(nBufferSize);
	memset(lightmapColors, 1, nBufferSize);
	Buffer buff(nBufferSize, lightmapColors);

	lightmapTexture = TextureManager::Instance()->createManual("lightMap", Texture::TT_2D, PF_RGBA8_UNORM,
		Texture::TU_STATIC, lightmapSize, lightmapSize, 1, 1, buff);

	TessManager::Instance()->setLightMap(lightmapTexture);

	return true;
}

void RenderEntity::updateRenderer()
{
	PROFILE_CPU("RenderEntity::updateRenderer");
	updateFovModifierHand();
	updateTorchFlicker();
	fogColor2 = fogColor1;
	thirdPersonDistance = PlayerPrefs::Instance()->getFloatPrefs("ThirdPersonDistance");
	if (thirdPersonDistance <= 0)
		thirdPersonDistance = LogicSetting::Instance()->getThirdPersonDistance();

	thirdPersonDistanceTemp = thirdPersonDistance;
	prevDebugCamYaw = debugCamYaw;
	prevDebugCamPitch = debugCamPitch;
	prevDebugCamFOV = debugCamFOV;
	prevCamRoll = camRoll;
	float var1;
	float var2;

	if (!bm->renderViewEntity)
	{
		bm->renderViewEntity = bm->m_pPlayer;
	}

	// first use camera postion as the entity pos.
	var1 = bm->m_pWorld->getLightBrightness(bm->renderViewEntity->getPosition().getFloor());

	var2 = (float)(3 - bm->m_gameSettings->renderDistance) / 3.0F;
	float var3 = var1 * (1.0F - var2) + var2;
	fogColor1 += (var3 - fogColor1) * 0.1F;
	++rendererUpdateCount;
	itemRenderer->updateEquippedItem();
	// addRainParticles();
}

void RenderEntity::updateFovModifierHand()
{
	EntityPlayerSP* pPlayer = (EntityPlayerSP*)bm->renderViewEntity;
	fovMultiplierTemp = pPlayer->getFOVMultiplier();
	fovModifierHandPrev = fovModifierHand;
	fovModifierHand += (fovMultiplierTemp - fovModifierHand) * 0.5F;

	if (fovModifierHand > 1.5F)
	{
		fovModifierHand = 1.5F;
	}

	if (fovModifierHand < 0.1F)
	{
		fovModifierHand = 0.1F;
	}
}

void RenderEntity::updateTorchFlicker()
{
	torchFlickerDX = (float)((double)torchFlickerDX + (Math::UnitRandom() - Math::UnitRandom()) * Math::UnitRandom() * Math::UnitRandom());
	torchFlickerDY = (float)((double)torchFlickerDY + (Math::UnitRandom() - Math::UnitRandom()) * Math::UnitRandom() * Math::UnitRandom());
	torchFlickerDX = (float)((double)torchFlickerDX * 0.9);
	torchFlickerDY = (float)((double)torchFlickerDY * 0.9);
	torchFlickerX += (torchFlickerDX - torchFlickerX) * 1.0F;
	torchFlickerY += (torchFlickerDY - torchFlickerY) * 1.0F;
	lightmapUpdateNeeded = true;
}

void RenderEntity::loadLightSettings()
{
	DataStream* pJSONStream = ResourceGroupManager::Instance()->openResource("24HoursLighting.json");
	if (pJSONStream == NULL)
	{
		LordLogError("RenderQueueDefine file 24HoursLighting.json not found.");
		return;
	}

	size_t fileSize = pJSONStream->size();
	char* jsonData = (char*)LordMalloc(fileSize + 1);
	pJSONStream->read(jsonData, fileSize);
	jsonData[fileSize] = 0;

	rapidjson::Document* doc = new rapidjson::Document;
	doc->Parse(jsonData);
	if (doc->HasParseError())
	{
		LordLogError("invalid 24HoursLighting file.");
	}
	else
	{
		LordAssert(doc->IsObject());
		const rapidjson::Value& settings = doc->FindMember("LightSettings")->value;

		LordAssert(settings.IsArray());

		for (rapidjson::Value::ConstValueIterator it = settings.Begin(); it != settings.End(); ++it)
		{
			String mainColor = it->FindMember("MainLightColor")->value.GetString();
			String subColor = it->FindMember("SubLightColor")->value.GetString();
			//String fogColor = it->FindMember("FogColor")->value.GetString();
			//String clouldColor = it->FindMember("CloudLight")->value.GetString();
			//String skyColor = it->FindMember("SkyColor")->value.GetString();
			int idx = StringUtil::ParseInt(it->FindMember("time")->value.GetString());

			m_lightSettings[idx].MainLightColor = StringUtil::ParseColor(mainColor);
			m_lightSettings[idx].SubLightColor = StringUtil::ParseColor(subColor);
			//float v = StringUtil::ParseFloat(clouldColor);
			//m_lightSettings[idx].clouldLight = Color(v, v, v, 1.f);
			//m_lightSettings[idx].skyColor = StringUtil::ParseColor255(skyColor);
			//m_lightSettings[idx].fogColor = StringUtil::ParseColor255(fogColor);
		}
	}

	LordDelete(doc);
	LordFree(jsonData);
	LordDelete(pJSONStream);
}

void RenderEntity::updateLightSetting(float rdt)
{
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	int worldTime = pWorld->getWorldInfo().getWorldTime() % 24000;
	int index = worldTime / 1000;
	float ratio = (worldTime - index * 1000) / 1000.f;
	index += 8;
	if (index >= 24)
		index -= 24;
	float celestial = bm->getWorld()->getCelestialAngle(rdt);
	Quaternion rotateY(Vector3::UNIT_Y, -Math::PI_DIV3);
	Quaternion rotateX(Vector3::UNIT_X, celestial * Math::PI_2);
	Quaternion orientation = rotateY * rotateX;
	Vector3 lightDir = orientation * Vector3::UNIT_Y;
	lightDir.normalize();

	//Vector3 mainDir = m_lightSettings[index].MainLightDir + (m_lightSettings[(index + 1) % 24].MainLightDir - m_lightSettings[index].MainLightDir) * ratio;
	//mainDir.normalize();
	Color mainLight = m_lightSettings[index].MainLightColor + (m_lightSettings[(index + 1) % 24].MainLightColor - m_lightSettings[index].MainLightColor) * ratio;
	Color subLight = m_lightSettings[index].SubLightColor + (m_lightSettings[(index + 1) % 24].SubLightColor - m_lightSettings[index].SubLightColor) * ratio;
	//cloudColor = m_lightSettings[index].clouldLight + (m_lightSettings[(index + 1) % 24].clouldLight - m_lightSettings[index].clouldLight) * ratio;
	//fogColor = m_lightSettings[index].fogColor + (m_lightSettings[(index + 1) % 24].fogColor - m_lightSettings[index].fogColor) * ratio;
	//skyColor = m_lightSettings[index].skyColor + (m_lightSettings[(index + 1) % 24].skyColor - m_lightSettings[index].skyColor) * ratio;

	SceneManager::Instance()->setMainLightDir(lightDir);
	SceneManager::Instance()->setMainLightColor(mainLight);
	SceneManager::Instance()->setSubLightColor(subLight);
	//SceneManager::Instance()->setFogColor(fogColor);
}


void RenderEntity::updateLightmap(float rdt)
{
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);

	if (!pWorld)
		return;

	bool enableLightVersion = bm->m_pPlayer->isPotionActive(Potion::nightVision);

	for (int i = 0; i < 256; ++i)
	{
		float blue = 0;
		float green = 0;
		float red = 0;
		{
			float var4 = pWorld->getSunBrightness(1.0F) * 0.95F + 0.05F;
			float var5 = pWorld->lightBrightnessTable[i / 16] * var4;
			float var6 = pWorld->lightBrightnessTable[i % 16] * (torchFlickerX * 0.1F + 1.5F);

			if (pWorld->m_lastLightningBolt > 0)
			{
				var5 = pWorld->lightBrightnessTable[i / 16];
			}

			float var7 = var5 * (pWorld->getSunBrightness(1.0F) * 0.65F + 0.35F);
			float var8 = var5 * (pWorld->getSunBrightness(1.0F) * 0.65F + 0.35F);
			float var11 = var6 * ((var6 * 0.6F + 0.4F) * 0.6F + 0.4F);
			float var12 = var6 * (var6 * var6 * 0.6F + 0.4F);
			blue = var7 + var6;
			green = var8 + var11;
			red = var5 + var12;
		}
		blue = blue * 0.96F + 0.03F;
		green = green * 0.96F + 0.03F;
		red = red * 0.96F + 0.03F;

		if (enableLightVersion)
		{
			if (i == 0)
			{
				float var16 = 0;
				if (enableLightVersion)
					var16 = getNightVisionBrightness(bm->m_pPlayer, rdt);

				float minRatio = std::min({ 1.0F / blue, 1.0F / green, 1.0F / red });

				blue = blue * (1.0F - var16) + blue * minRatio * var16;
				green = green * (1.0F - var16) + green * minRatio * var16;
				red = red * (1.0F - var16) + red * minRatio * var16;
			}
			else
			{
				float minRatio = std::min({ 1.0F / blue, 1.0F / green, 1.0F / red });

				blue = blue * minRatio;
				green = green * minRatio;
				red = red * minRatio;
			}
		}

		if (blue > 1.0F)
			blue = 1.0F;
		if (green > 1.0F)
			green = 1.0F;
		if (red > 1.0F)
			red = 1.0F;

		float oneMinusBlue = 1.0F - blue;
		float oneMinusGreen = 1.0F - green;
		float oneMinusRed = 1.0F - red;
		blue = 1.0F - oneMinusBlue * oneMinusBlue * oneMinusBlue * oneMinusBlue;
		green = 1.0F - oneMinusGreen * oneMinusGreen * oneMinusGreen * oneMinusGreen;
		red = 1.0F - oneMinusRed * oneMinusRed * oneMinusRed * oneMinusRed;
		blue = blue * 0.96F + 0.03F;
		green = green * 0.96F + 0.03F;
		red = red * 0.96F + 0.03F;

		red = Math::Clamp(red, 0.f, 1.f);
		blue = Math::Clamp(blue, 0.f, 1.f);
		green = Math::Clamp(green, 0.f, 1.f);

		short alphaInt = 255;
		int blueInt = (int)(blue * 255.0F);
		int greenInt = (int)(green * 255.0F);
		int redInt = (int)(red * 255.0F);
		lightmapColors[i] = alphaInt << 24 | blueInt << 16 | greenInt << 8 | redInt;
	}

	ERect rt(0, 0, 16, 16);
	lightmapTexture->updateSubTex2D(0, rt, lightmapColors);
	lightmapUpdateNeeded = false;
}


float RenderEntity::getNightVisionBrightness(EntityPlayer* pPlayer, float rdt)
{
	int duration = pPlayer->getActivePotionEffect(Potion::nightVision)->getDuration();
	return duration > 200 ? 1.0F : 0.7F + Math::Sin((duration - rdt) * Math::PI * 0.2F) * 0.3F;
}

void RenderEntity::renderRainSnow(float rdt, int weatherType)
{
	if (weatherType != 1 && weatherType != 2)
		return;

	Blockman* pBm = Blockman::Instance();
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	float strength = pWorld->getRainStrength(rdt);

	// debug.
	strength = 0.8f;

	if (strength <= 0.0F)
		return;

	if (rainXCoords == nullptr)
	{
		rainXCoords = (float*)LordMalloc(1024 * sizeof(float));
		rainYCoords = (float*)LordMalloc(1024 * sizeof(float));
		m_snowTex = TextureManager::Instance()->createTexture("snow_r.png");
		m_snowTex->load();
		m_rainTex = TextureManager::Instance()->createTexture("rain.png");
		m_rainTex->load();
		m_rainRenderer = TessManager::Instance()->createFxRenderable(m_rainTex, FRP_ALPHA_CHANEL);
		m_snowRenderer = TessManager::Instance()->createFxRenderable(m_snowTex, FRP_ALPHA_CHANEL);
		for (int i = 0; i < 32; ++i)
		{
			for (int j = 0; j < 32; ++j)
			{
				float fa = (float)(j - 16);
				float fb = (float)(i - 16);
				float fc = Math::Sqrt(fa * fa + fb * fb);
				rainXCoords[i << 5 | j] = -fb / fc;
				rainYCoords[i << 5 | j] = fa / fc;
			}
		}
	}

	EntityLivingBase* pPlayer = pBm->renderViewEntity;
	BlockPos ipos = pPlayer->getPosition().getFloor();
	Vector3 rpos = pPlayer->getLastTickPosition(rdt);
	//Vector3 rpos = pPlayer->lastTickPos + (pPlayer->position - pPlayer->lastTickPos) * rdt;
	int baseY = int(Math::Floor(rpos.y));
	i8 range = 5;

// 	if (pBm->m_gameSettings->fancyGraphics)
// 		range = 10;

	i8 iflag = -1;
	float fcount = rendererUpdateCount + rdt;

// 	if (pBm->m_gameSettings->fancyGraphics)
// 		range = 10;

	m_rainRenderer->beginPolygon();
	m_snowRenderer->beginPolygon();

	for (int z = ipos.z - range; z <= ipos.z + range; ++z)
	{
		for (int x = ipos.x - range; x <= ipos.x + range; ++x)
		{
			int idx = (z - ipos.z + 16) * 32 + x - ipos.x + 16;
			float ax = rainXCoords[idx] * 0.5F;
			float az = rainYCoords[idx] * 0.5F;
			
			// for debug.
			// if (pBiome->canSpawnLightningBolt() || pBiome->getEnableSnow())
			{
				int height = pWorld->getPrecipitationHeight(x, z);
				int minh = ipos.y - range;
				int maxh = ipos.y + range;

				if (minh < height)
					minh = height;
				if (maxh < height)
					maxh = height;

				float vz = 1.0F;
				int vy = height;

				if (height < baseY)
					vy = baseY;

				if (minh != maxh)
				{
					random.setSeed((long)(x * x * 3121 + x * 45238971 ^ z * z * 418711 + z * 13761));
					float temperature = 0.5f;
					float fz;
					float fx;

					//if (pWorld->getWorldChunkManager()->getTemperatureAtHeight(temperature, height) >= 1.15F)
					if(weatherType == 1) // rain
					{
						FxVertexFmt* vertex;
						if (!m_rainRenderer->getLockPtr(1, vertex))
							continue;

						fz = ((float)(rendererUpdateCount + x * x * 3121 + x * 45238971 + z * z * 418711 + z * 13761 & 31) + rdt) / 32.0F * (3.0F + random.nextFloat());
						float cx = x + 0.5F - pPlayer->position.x;
						fx = z + 0.5F - pPlayer->position.z;
						float var37 = Math::Sqrt(cx * cx + fx * fx) / (float)range;
						int bright = pWorld->getLightBrightnessForSkyBlocks(BlockPos(x, vy, z), 0, 0, 0);
						int color = Color(1.f, 1.f, 1.f, ((1.0F - var37 * var37) * 0.5F + 0.5F) * strength).getARGB();
						vertex[0].set(x - ax + 0.5f - rpos.x, minh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz, minh * vz / 4.0F + fz * vz);
						vertex[1].set(x + ax + 0.5f - rpos.x, minh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz, minh * vz / 4.0F + fz * vz);
						vertex[2].set(x + ax + 0.5f - rpos.x, maxh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz, maxh * vz / 4.0F + fz * vz);
						vertex[3].set(x - ax + 0.5f - rpos.x, maxh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz, maxh * vz / 4.0F + fz * vz);
					}
					else if (weatherType == 2) // snow
					{
						FxVertexFmt* vertex;
						if (!m_snowRenderer->getLockPtr(1, vertex))
							continue;

						fz = ((float)(rendererUpdateCount & 511) + rdt) / 512.0F;
						float u = random.nextFloat() + fcount * 0.01F * (float)random.nextGaussian();
						float v = random.nextFloat() + fcount * (float)random.nextGaussian() * 0.001F;
						fx = x + 0.5F - pPlayer->position.x;
						float ez = z + 0.5F - pPlayer->position.z;
						float alpha = Math::Sqrt(fx * fx + ez * ez) / (float)range;
						int bright = (pWorld->getLightBrightnessForSkyBlocks(BlockPos(x, vy, z), 0, 0, 0) * 3 + 15728880) / 4;
						int color = Color(1.f, 1.f, 1.f, ((1.0F - alpha * alpha) * 0.3F + 0.5F) * strength).getARGB();
						vertex[0].set(x - ax + 0.5f - rpos.x, minh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz + u, minh * vz / 4.0F + fz * vz + v);
						vertex[1].set(x + ax + 0.5f - rpos.x, minh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz + u, minh * vz / 4.0F + fz * vz + v);
						vertex[2].set(x + ax + 0.5f - rpos.x, maxh - rpos.y, z + az + 0.5f - rpos.z, color, bright, 1.0F * vz + u, maxh * vz / 4.0F + fz * vz + v);
						vertex[3].set(x - ax + 0.5f - rpos.x, maxh - rpos.y, z - az + 0.5f - rpos.z, color, bright, 0.0F * vz + u, maxh * vz / 4.0F + fz * vz + v);
					}
				}
			}
		}
	}

	m_rainRenderer->endPolygon();
	m_snowRenderer->endPolygon();
	if (m_rainRenderer->getQuadNum() > 0)
		TessManager::Instance()->addFxRenderable(m_rainRenderer);
	if (m_snowRenderer->getQuadNum() > 0)
		TessManager::Instance()->addFxRenderable(m_snowRenderer);
}

void RenderEntity::updateEnchantTexMats(float rdt)
{
	Matrix4 matTex, mattemp;
	float s = 0.125f;
	matTex.identity(); matTex.scale(s, s, s);
	float t = (Blockman::Instance()->getSystemTime() % 3000) / 3000.f * 6.f;
	mattemp.identity(); mattemp.translate(t, 0.f, 0.f);
	matTex.joint(mattemp);
	mattemp.identity(); mattemp.rotateZ(-50 * Math::DEG2RAD);
	matTex.joint(mattemp);
	TessManager::Instance()->setEnchantTexMat(0, matTex);

	matTex.identity(); matTex.scale(s, s, s);
	t = (Blockman::Instance()->getSystemTime() % 4873) / 4873.f * 6.f;
	mattemp.identity(); mattemp.translate(-t, 0.f, 0.f);
	matTex.joint(mattemp);
	mattemp.identity(); mattemp.rotateZ(10 * Math::DEG2RAD);
	matTex.joint(mattemp);
	TessManager::Instance()->setEnchantTexMat(1, matTex);
}

int RenderEntity::getLightMapColor(float u, float v)
{
	NULL_RETURN(lightmapColors, 0);
	u = Math::Clamp(u, 0.f, 1.f);
	v = Math::Clamp(v, 0.f, 1.f);
	int index = int(v * 16) * 16 + int(u * 16);
	return lightmapColors[index];
}

int RenderEntity::getLightMapColor(int skyBlockLight)
{
	int br = skyBlockLight & 0xF;
	int bg = (skyBlockLight >> 4) & 0xF;
	int bb = (skyBlockLight >> 8) & 0xF;
	int bs = (skyBlockLight >> 12) & 0xF;
	float fr = (br << 4) / 255.f;
	float fg = (bg << 4) / 255.f;
	float fb = (bb << 4) / 255.f;
	float fs = (bs << 4) / 255.f;
	Color lr = Color(getLightMapColor(fr, fs));
	Color lg = Color(getLightMapColor(fg, fs));
	Color lb = Color(getLightMapColor(fb, fs));

	return (lr * lg * lb).getABGR();
}

void RenderEntity::setCameraRotationParam(float firstStageTime, float secondStageTime, float angleTotal, float angleDelta, float rotateRadius)
{
	m_isCameraRotation = true;
	m_cameraRotationFirstStageTime = firstStageTime;
	m_cameraRotationSecondStageTime = secondStageTime;
	m_cameraRotationAngleTotal = angleTotal;
	m_cameraRotationAngleDelta = angleDelta;
	m_cameraRotationRadius = rotateRadius;
	m_cameraRotationAngleSum = 0.0f;
	m_cameraRotationTimeSum = 0.0f;
}

void RenderEntity::resetCameraRotationParam()
{
	m_isCameraRotation = false;
	m_cameraRotationFirstStageTime = 0.0f;
	m_cameraRotationSecondStageTime = 0.0f;
	m_cameraRotationAngleTotal = 0.0f;
	m_cameraRotationAngleDelta = 0.0f;
	m_cameraRotationRadius = 0.0f;
	m_cameraRotationAngleSum = 0.0f;
	m_cameraRotationTimeSum = 0.0f;
}

void RenderEntity::getMouseOver(float rdt)
{
	if (!bm->renderViewEntity)
		return;

	if (!bm->m_pWorld)
		return;

	clearMouseOver();
	if (m_isOnVehicle)
		return;
	PROFILE_CPU("RenderEntity::getMouseOver");
	float effectiveDistance = 0.f;
	auto player = bm->m_pPlayer;
	if (player && player->inventory && player->inventory->getCurrentItem())
	{
		if (auto item = dynamic_cast<ItemTool*>(player->inventory->getCurrentItem()->getItem()))
		{
			effectiveDistance = item->getEffectiveDistance();
		}

		if (auto item = dynamic_cast<ItemBuild*>(player->inventory->getCurrentItem()->getItem()))
		{
			effectiveDistance = 4.f;
		}
	}

	float reachDis = bm->m_playerControl->getBlockReachDistance()  + effectiveDistance;

	Vector3 viewerPos = bm->renderViewEntity->getPosition(rdt);
	Vector3 playerPos = bm->renderViewEntity->getPosition(rdt);

	GameSettings* pSetting = bm->m_gameSettings;
	if (pSetting->getPersonView() == GameSettings::SPVT_FIRST)
	{
		bm->objectMouseOver = bm->renderViewEntity->rayTrace(reachDis, rdt);
	}
	else if (pSetting->getPersonView() == GameSettings::SPVT_THIRD)
	{
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Ray ray;
		mainCamera->getCameraRay(ray, pSetting->getMousePos());
		
		Vector3 pos = ray.o;
		Vector3 lookat = ray.dir;
		Vector3 end = pos + lookat * reachDis * 2.0f;
		bm->objectMouseOver = bm->m_pWorld->rayTraceBlocks(pos, end);
		if (bm->objectMouseOver.result)
		{
			float mouseToPlayer = playerPos.distanceTo(bm->objectMouseOver.hitPointPos);
			if (mouseToPlayer > reachDis)
			{
				bm->objectMouseOver.reset();
			}
			else
			{
				RayTraceResult limitRange = bm->m_pWorld->rayTraceBlocks(playerPos, bm->objectMouseOver.hitPointPos);
				if (limitRange.result && mouseToPlayer > playerPos.distanceTo(limitRange.hitPointPos))
				{
					bm->objectMouseOver.reset();
				}
			}
		}
		viewerPos = pos;
	}
	else
		return;

	float reachDis2 = reachDis;

	if (bm->m_playerControl->extendedReach())
	{
		reachDis = 6.0f;
		reachDis2 = 6.0f;
	}
	else
	{
		reachDis2 = bm->m_playerControl->getEntityReachDistance();
		reachDis = reachDis2;
	}

	if (bm->objectMouseOver.result)
	{
		reachDis2 = bm->objectMouseOver.hitPointPos.distanceTo(viewerPos);
	}

	Vector3 lookDir = bm->renderViewEntity->getLook(rdt);
	Vector3 lookAt = viewerPos + lookDir * reachDis;
	if (pSetting->getPersonView() == GameSettings::SPVT_THIRD)
	{
		lookAt += lookDir * reachDis;
	}
	EntityArr entities = bm->m_pWorld->getEntitiesWithinAABBExcludingEntity(bm->renderViewEntity, bm->renderViewEntity->boundingBox.addCoord(lookDir * reachDis).expand(1.f));
	float var11 = reachDis2;

	for (int i = 0; i < int(entities.size()); ++i)
	{
		Entity* pEntity = entities[i];
		if (!pEntity->getBoolProperty("banClicked"))
		{
			float borderSize = pEntity->getCollisionBorderSize();
			Box aabb = pEntity->boundingBox.expand(borderSize);
			RayTraceResult var17 = RayTraceResult::calculateIntercept(aabb, viewerPos, lookAt);

			if (aabb.isPointInside(viewerPos))
			{
				if (0.0f <= var11)
				{
					pointedEntity = pEntity;
					var11 = 0.0f;
				}
			}
			else if (var17.result)
			{
				float var18 = playerPos.distanceTo(var17.hitPointPos);

				if (var18 <= var11)
				{
					RayTraceResult limitRange = bm->m_pWorld->rayTraceBlocks(playerPos, var17.hitPointPos);
					if (limitRange.result && var18 > playerPos.distanceTo(limitRange.hitPointPos))
					{
						continue;
					}

					pointedEntity = pEntity;
					var11 = var18;
				}
			}
		}
	}

	if (LogicSetting::Instance()->isDisableSelectEntity() && dynamic_cast<EntityPlayer*>(pointedEntity)
		|| dynamic_cast<EntityBird*>(pointedEntity) || dynamic_cast<EntityItem*>(pointedEntity))
	{
		return;
	}

	if (pointedEntity && (var11 < reachDis2 || bm->objectMouseOver.result))
	{
		bm->objectMouseOver.set(pointedEntity);

		if (pointedEntity->isClass(ENTITY_CLASS_LIVING_BASE))
		{
			bm->pointedEntityLiving = (EntityLivingBase*)pointedEntity;
		}
	}
}

void RenderEntity::updateCameraAndRender(float dt, float rdt)
{
	PROFILE_CPU("RenderEntity::updateCameraAndRender");
	if (lightmapUpdateNeeded)
	{
		updateLightmap(rdt);
	}

	prevFrameTime = Root::Instance()->getCurrentTime();

	float dx, dy;
	if (!bm->m_gameSettings->isMouseMoving())
	{
		bm->m_gameSettings->m_prevMousePos = Vector2::INVALID;
		dx = dy = 0;
	}
	else if (bm->m_gameSettings->m_prevMousePos == Vector2::INVALID)
	{
		bm->m_gameSettings->m_prevMousePos = bm->m_gameSettings->getMousePos();
		dx = dy = 0;
	}
	else
	{
		float sensitivity = bm->m_gameSettings->getMouseSensitivity() * 0.6F * (bm->m_gameSettings->getCameraSensitive() * 0.2F + 0.1F) + 0.2F;
		float mouseLen = sensitivity * sensitivity * sensitivity * 16.0F;
		auto curMousePos = bm->m_gameSettings->getMousePos();
		auto mouseDelta = curMousePos - bm->m_gameSettings->m_prevMousePos;
		bm->m_gameSettings->m_prevMousePos = curMousePos;
		dx = mouseDelta.x * mouseLen;
		dy = -mouseDelta.y * mouseLen;
	}

	int mode = 1;

	if (bm->m_gameSettings->invertMouse)
	{
		mode = -1;
	}
	m_cameraController.setSeparate(PlayerPrefs::Instance()->getBoolPrefs("IsSeparateCamera") && bm->m_gameSettings->getPersonView() > GameSettings::SPVT_FIRST);
	if (m_cameraController.isSeparate())
	{
		m_cameraController.updateCameraAngles(dx, dy * mode);
	}
	else
	{
		if (bm->m_pPlayer->isOnFlyingMount()) {
    		bm->m_pPlayer->getFlyingMount()->handleMouseInput(dx, dy * mode);
    	}
    	else if (bm->m_pPlayer->isOnFloatingMount())
    	{
    		bm->m_pPlayer->getFloatingMount()->handleMouseInput(dx, dy * mode);
    	}
    	else {
    		bm->m_pPlayer->handleMouseInput(dx, dy * mode);
    	}
		m_cameraController.setCameraAngles(bm->m_pPlayer->rotationYaw, bm->m_pPlayer->rotationPitch, bm->m_pPlayer->prevRotationYaw, bm->m_pPlayer->prevRotationPitch);
	}

	if (bm->m_pWorld)
	{
		int var18 = performanceToFps(bm->m_gameSettings->limitFramerate);
		if (bm->m_gameSettings->limitFramerate == 0)
		{
			renderWorld(dt, rdt, 0L);
		}
		else
		{
			renderWorld(dt, rdt, renderEndNanoTime + (long)(1000000000 / var18));
			renderEndNanoTime = Root::Instance()->getCurrentTime();
		}
	}
}

void RenderEntity::setupViewBobbing(float rdt, Matrix4& matView, float scale)
{
	if (m_isOnVehicle)
		return;

	if (bm->renderViewEntity->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
		float deltaDis = pPlayer->distanceWalkedModified - pPlayer->prevDistanceWalkedModified;
		float walkDis = -(pPlayer->distanceWalkedModified + deltaDis * rdt);
		float yaw = (pPlayer->prevCameraYaw + (pPlayer->cameraYaw - pPlayer->prevCameraYaw) * rdt) * scale;
		float pitch = (pPlayer->prevCameraPitch + (pPlayer->cameraPitch - pPlayer->prevCameraPitch) * rdt) * scale;
		float viewBobingX = Math::Sin(walkDis * Math::PI) * yaw * 0.5F;
		float viewBobingY = -Math::Abs(Math::Cos(walkDis * Math::PI) * yaw);
		matView.translate(viewBobingX, viewBobingY, 0.f);
		Matrix4 temp = Matrix4::IDENTITY;
		temp.rotateZ(Math::Sin(walkDis * Math::PI) * yaw * Math::DEG2RAD * 3.0F);
		matView.joint(temp);
		temp.identity(); temp.rotateX(Math::Abs(Math::Cos(walkDis * Math::PI - 0.2F) * yaw * Math::DEG2RAD) * 5.0F);
		matView.joint(temp);
		temp.identity(); temp.rotateX(pitch * Math::DEG2RAD);
		matView.joint(temp);
	}
}

void RenderEntity::orientCamera(float rdt, Matrix4& matView)
{
	EntityLivingBase* pViewer = bm->renderViewEntity;
	constexpr float kEyeOffsetFromFoot = 1.62F;
	float offsetY = pViewer->yOffset - kEyeOffsetFromFoot;
	//Vector3 viewPos = pViewer->prevPos + (pViewer->position - pViewer->prevPos) * rdt;
	Vector3 viewPos = pViewer->getPosition(rdt);
	float collisionHeight = PlayerPrefs::Instance()->getBoolPrefs("IsSeparateCamera") ? viewPos.y - pViewer->yOffset + 0.2f : 9999.0f;
	viewPos.y -= offsetY;
	float currScale = getRenderEntityActorScale(pViewer);
	viewPos.y -= 1.62F * (1.0F - currScale);

	Matrix4 temp;
	temp.identity(); temp.rotateZ((prevCamRoll + (camRoll - prevCamRoll) * rdt) * Math::DEG2RAD);
	matView = matView *  temp;
	float currentYaw = (m_cameraController.getCameraPrevYaw() + (m_cameraController.getCameraYaw() - m_cameraController.getCameraPrevYaw()) * rdt) * Math::DEG2RAD;
	float currentPitch = (m_cameraController.getCameraPrevPitch() + (m_cameraController.getCameraPitch() - m_cameraController.getCameraPrevPitch()) * rdt) * Math::DEG2RAD;
	
	Vector3 offset = Vector3::ZERO;

	if (bm->m_closeup_isShow)
	{
		float distance = bm->m_closeup_farDis;
		if (m_closeupDeltaDis < bm->m_closeup_farDis - bm->m_closeup_nearDis)
		{
			distance -= m_closeupDeltaDis;
			m_closeupDeltaDis += bm->m_closeup_velocity * Root::Instance()->getFrameTime() * 0.001f;
		}
		else
		{
			m_closeupDeltaDis = bm->m_closeup_farDis - bm->m_closeup_nearDis;
			distance = bm->m_closeup_nearDis;
		}
		currentYaw = bm->m_closeup_yaw * Math::DEG2RAD;
		currentPitch = bm->m_closeup_pitch * Math::DEG2RAD;
		float cx = Math::Sin(currentYaw) * Math::Cos(currentPitch);
		float cz = -Math::Cos(currentYaw) * Math::Cos(currentPitch);
		float cy = Math::Sin(currentPitch);
		offset = Vector3(cx, cy, cz) * distance;
	}
	else if (pViewer->isPlayerSleeping())
	{
		offsetY = offsetY + 1.0f;
		temp.identity(); temp.translate(Vector3(0.f, 0.3f, 0.f));
		matView = matView *  temp;

		BlockPos pos = pViewer->getPosition().getFloor();
		int blockID = bm->m_pWorld->getBlockId(pos);

		if (blockID == BLOCK_ID_BED)
		{
			int meta = bm->m_pWorld->getBlockMeta(pos);
			int rot = meta & 3;
			temp.identity(); temp.rotateY(rot * Math::PI_DIV2);
			matView = matView * temp;
		}

		temp.identity(); temp.rotateY(-currentYaw - Math::PI);
		matView = matView * temp;
		temp.identity(); temp.rotateX(-currentPitch);
		matView = matView * temp;
	}
	else if (bm->m_pPlayer->isOnFlyingMount())
	{
		auto mount = bm->m_pPlayer->getFlyingMount();
		Quaternion interpolatedViewOrientation = mount->getCurrentViewOrientation(rdt);
		Matrix4 orientation;
		orientation.fromQuan(interpolatedViewOrientation);
		matView *= orientation;
		Quaternion interpolatedBodyOrientation;
		Quaternion::Slerp(interpolatedBodyOrientation, mount->getPrevOrientation(), mount->getOrientation(), rdt, true);
		const auto& setting = mount->getSetting();
		auto rotCenterOffset = interpolatedBodyOrientation * setting.cameraRotCenterOffset;
		auto rotCenter = mount->getPosition(rdt) + rotCenterOffset;
		cameraOffset = setting.cameraOffset * orientation;
		if (!setting.penetrateBlocks)
		{
			cameraOffset = calculateMaxCameraPushOffset(rotCenter, cameraOffset);
		}
		cameraOffset += rotCenter - pViewer->getPosition(rdt);
		return;
	}
	else if (bm->m_pPlayer->isOnFloatingMount())
	{
		Matrix4 temp, cameraMat;
		cameraMat.identity();
		temp.identity(); temp.rotateX(currentPitch);
		cameraMat *= temp;
		temp.identity(); temp.rotateY(-currentYaw);
		cameraMat *= temp;
		matView *= cameraMat;

		auto mount = bm->m_pPlayer->getFloatingMount();
		const auto& setting = mount->getSetting();
		auto bodyYaw = (mount->prevRotationYaw + (mount->rotationYaw - mount->prevRotationYaw)) * Math::DEG2RAD;
		temp.identity(); temp.rotateY(-bodyYaw);
		auto rotCenterOffset = setting.cameraRotCenterOffset * temp;
		auto rotCenter = mount->getPosition(rdt) + rotCenterOffset;

		cameraOffset = setting.cameraOffset * cameraMat;
		if (!setting.penetrateBlocks)
		{
			cameraOffset = calculateMaxCameraPushOffset(rotCenter, cameraOffset);
		}
		cameraOffset += rotCenter - pViewer->getPosition(rdt);
		return;
	}
	else if (bm->m_gameSettings->getPersonView() > GameSettings::SPVT_FIRST || m_isOnVehicle)
	{
		float distance = thirdPersonDistanceTemp + (thirdPersonDistance - thirdPersonDistanceTemp) * rdt;
		float debugYaw = (m_cameraController.getCameraPrevYaw() + (m_cameraController.getCameraYaw() - m_cameraController.getCameraPrevYaw()) * rdt) * Math::DEG2RAD;
        float debugPitch = (m_cameraController.getCameraPrevPitch() + (m_cameraController.getCameraPitch() - m_cameraController.getCameraPrevPitch()) * rdt) * Math::DEG2RAD;

		if (bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
		{
			debugPitch += Math::PI;
		}

		float cx = Math::Sin(debugYaw) * Math::Cos(debugPitch);
		float cz = -Math::Cos(debugYaw) * Math::Cos(debugPitch);
		float cy = Math::Sin(debugPitch);

		float cdx = cx * distance;
		float cdy = cy * distance;
		float cdz = cz * distance;

		EntityPlayerSPMP* player = dynamic_cast<EntityPlayerSPMP*>(pViewer);
		if (player)
		{
			cdx += player->m_thirdPersonCameraOffset.x;
			cdy += player->m_thirdPersonCameraOffset.y;
			cdz += player->m_thirdPersonCameraOffset.z;
		}

		if (m_isCameraRotation)
		{
			Vector3 dir = Vector3(cdx, cdy, cdz);
			distance = dir.len();
			dir.normalize();
			cx = dir.x;
			cy = dir.y;
			cz = dir.z;

			m_cameraRotationTimeSum += (float)Root::Instance()->getFrameTime();
			if (m_cameraRotationSecondStageTime >= 0.0f && m_cameraRotationTimeSum > m_cameraRotationFirstStageTime + m_cameraRotationSecondStageTime)
			{
				bm->stopCameraRotation();
			}
			else if(m_cameraRotationTimeSum > m_cameraRotationFirstStageTime)
			{
				m_cameraRotationAngleSum = Math::Clamp(m_cameraRotationAngleDelta * Root::Instance()->getFrameTime() + m_cameraRotationAngleSum, -Math::Abs(m_cameraRotationAngleTotal), Math::Abs(m_cameraRotationAngleTotal));
			}

			temp.identity(); temp.rotateAxisReplace(Vector3::UNIT_Y, -m_cameraRotationAngleSum * Math::DEG2RAD);
			matView *= temp;

			if (m_cameraRotationRadius < 0.0f)
			{
				offset = Vector3(cx, cy, cz) * distance;
			}
			else
			{
				offset = Vector3(cx, cy, cz) * m_cameraRotationRadius;
			}
			float cosTemp = Math::Cos(m_cameraRotationAngleSum * Math::DEG2RAD);
			float sinTemp = Math::Sin(m_cameraRotationAngleSum * Math::DEG2RAD);
			float newX = offset.x * cosTemp - offset.z * sinTemp;
			float newZ = offset.x * sinTemp + offset.z * cosTemp;
			offset.x = newX;
			offset.z = newZ;
		}
		else
		{
			offset = calculateMaxCameraPushOffset(viewPos, { cdx, cdy, cdz }, collisionHeight);
		}

		if (bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
		{
			temp.identity(); temp.rotateY(Math::PI);
			matView *= temp;
		}
	}
	else if (bm->m_pPlayer->isOnFloatingMount())
	{
		auto player = bm->m_pPlayer;
		auto mount = player->getFloatingMount();
		offset = mount->getPosition(rdt) + mount->getSetting().cameraRotCenterOffset - player->getPosition(rdt);
	}
	else
	{
		temp.identity(); temp.translate(Vector3(0, 0, -0.1f));
		matView *= temp;
	}
	temp.identity(); 
	temp.rotateX(currentPitch);
	matView *= temp;

	temp.identity();
	temp.rotateY(-currentYaw);
	matView *= temp;

	if (pViewer->yOffset > 1.6F)
	{
		temp.identity();
		temp.translate(Vector3(0.0F, offsetY, 0.0F));
		matView *= temp;
	}

	cameraOffset = offset;
}

Vector3 RenderEntity::calculateMaxCameraPushOffset(const Vector3& startPos, const Vector3& pushOffset, float collisionHeight)
{
	const Vector3 sideOffsets[] = {
		{-0.15f, -0.15f, -0.15f},
		{-0.15f, -0.15f,  0.15f},
		{-0.15f,  0.15f, -0.15f},
		{-0.15f,  0.15f,  0.15f},
		{ 0.15f, -0.15f, -0.15f},
		{ 0.15f, -0.15f,  0.15f},
		{ 0.15f,  0.15f, -0.15f},
		{ 0.15f,  0.15f,  0.15f}
	};
	const auto endPos = startPos + pushOffset;
	float minLenSqr = pushOffset.lenSqr();
	for (const auto& sideOffset : sideOffsets)
	{
		auto rayStartPos = startPos + sideOffset;
		RayTraceResult tracyResult = bm->m_pWorld->rayTraceBlocks(
			rayStartPos,
			endPos + sideOffset);

		if (tracyResult.result && tracyResult.hitPointPos.y <= collisionHeight)
		{
			float lenSqr = (tracyResult.hitPointPos - rayStartPos).lenSqr();

			if (lenSqr < minLenSqr)
			{
				minLenSqr = lenSqr;
			}
		}
	}
	return pushOffset.normalizedCopy() * std::sqrt(minLenSqr);
}

void RenderEntity::setupCameraTransform(float rdt)
{
	if (bm->m_gameSettings->getPersonView() == 0 || m_isCameraRotation || PlayerPrefs::Instance()->getBoolPrefs("IsSeparateCamera"))
	{
		bm->m_pPlayer->m_thirdPersonCameraOffset = Vector3::ZERO;
	}
	else
	{
		float horizontalOffset = 0.7f;
		float verticalOffset = 0.6f;
		float yaw = m_cameraController.getCameraYaw() + 90.0f;
		float offsetX = -horizontalOffset * Math::Sin(yaw * Math::DEG2RAD);
		float offsetZ = horizontalOffset * Math::Cos(yaw * Math::DEG2RAD);
		bm->m_pPlayer->m_thirdPersonCameraOffset.x = offsetX;
		bm->m_pPlayer->m_thirdPersonCameraOffset.y = verticalOffset;
		bm->m_pPlayer->m_thirdPersonCameraOffset.z = offsetZ;
	}
	bm->m_pPlayer->m_thirdPersonCameraOffset.y += PlayerPrefs::Instance()->getFloatPrefs("ThirdPersonCameraYOffset");

	farPlaneDistance = float(bm->m_gameSettings->getRenderRange());

	float fov = getFOVModifier(rdt, true);
	float fov2 = getFOVModifier(rdt, false);

	Matrix4 effectView = Matrix4::IDENTITY;
	hurtCemaraEffect(rdt, effectView);

	// 这里分离 player 与 player 手持物或者 model 的 bobbing matrix
	Matrix4 bobbingView = Matrix4::IDENTITY;
	Matrix4 bobbingViewItem = Matrix4::IDENTITY;
	if (PlayerPrefs::Instance()->getBoolPrefs("IsViewBobbing") && !bm->renderViewEntity->getParentEntity())
	{
		setupViewBobbing(rdt, bobbingView, PlayerPrefs::Instance()->getFloatPrefs("PlayerBobbingScale"));
		setupViewBobbing(rdt, bobbingViewItem);
	}
	TessManager::Instance()->setViewBobing(bobbingViewItem);
	float bobingTransRatal = 0.1f;
	bobbingView.m30 *= bobingTransRatal;
	bobbingView.m31 *= bobingTransRatal;
	bobbingView.m32 *= bobingTransRatal;

	Matrix4 orientView = Matrix4::IDENTITY;
	orientCamera(rdt, orientView);
	orientView = effectView * orientView * bobbingView;

	Vector3 cameraPos = Vector3::ZERO * orientView;
	Vector3 cameraDir = Vector3::UNIT_Z * orientView;
	Vector3 cameraUp = Vector3::UNIT_Y * orientView;

	float offsetY = bm->renderViewEntity->yOffset - 1.62F;
	Vector3 viewPos = bm->renderViewEntity->getPosition(rdt);
	float currScale = getRenderEntityActorScale(bm->renderViewEntity);
	viewPos.y -= offsetY;
	viewPos.y -= 1.62F * (1.0F - currScale);

	if (bm->m_closeup_isShow)
	{
		viewPos = bm->m_closeup_closeupPos;
		if (Math::Abs(bm->m_closeup_farDis - bm->m_closeup_nearDis - m_closeupDeltaDis) < 0.00001f)
		{
			m_closeupDuration += Root::Instance()->getFrameTime() * 0.001f;
			if (m_closeupDuration >= bm->m_closeup_duration)
			{
				resetCloseup();
				bm->resetCloseup();
			}
		}
	}

	if (bm->m_pPlayer->m_isShooting)
	{
		bm->m_pPlayer->m_isShooting = false;
		m_screenShakingValue = 1.4f;
	}
	if (m_screenShakingValue > 0.0f)
	{
		fov += m_screenShakingValue;
		m_screenShakingValue -= Root::Instance()->getFrameTime() * 0.001f;
		if (m_screenShakingValue < 0.0f)
		{
			m_screenShakingValue = 0.0f;
		}
	}

	/** set effect system view pos (include the viewbobing.) */
	EntityFx::interpPos = viewPos + cameraPos + cameraOffset;

	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	pCamera->setPosition(EntityFx::interpPos);
	pCamera->setDirection(cameraDir.normalizedCopy());
	pCamera->setUp(cameraUp.normalizedCopy());
	pCamera->setFov(fov * Math::DEG2RAD);
	pCamera->setFov2(fov2 * Math::DEG2RAD);
	pCamera->update();
}

void RenderEntity::hurtCemaraEffect(float rdt, Matrix4& matView)
{
	if (m_isOnVehicle)
		return;

	EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
	float hurtTime = (float)pPlayer->hurtTime - rdt;
	float deadTime;

	if (pPlayer->capabilities.isWatchMode)
		return;

	if (pPlayer->getHealth() <= 0.0F)
	{
		deadTime = (float)pPlayer->deathTime + rdt;
		matView.rotateZ((40.0F - 8000.0F / (deadTime + 200.0F)) * Math::DEG2RAD);
	}

	if (hurtTime >= 0.0F)
	{
		if (pPlayer->maxHurtTime)
		{
			hurtTime /= (float)pPlayer->maxHurtTime;
		}
		hurtTime = Math::Sin(hurtTime * hurtTime * hurtTime * hurtTime * Math::PI);
		deadTime = pPlayer->attackedAtYaw;
		if (!SCRIPT_EVENT::HurtCemaraEffectEvent::invoke(pPlayer))
		{
			return;
		}
		Matrix4 temp;
		temp.identity(); temp.rotateY(-deadTime * Math::DEG2RAD);
		matView.joint(temp);
		temp.identity(); temp.rotateZ(-hurtTime * 14.f * Math::DEG2RAD);
		matView.joint(temp);
		temp.identity(); temp.rotateY(deadTime * Math::DEG2RAD);
		matView.joint(temp);
	}
}

float RenderEntity::getFOVModifier(float rdt, bool useSetting)
{
	if (debugViewDirection > 0)
		return 90.0F;

	EntityPlayer* pPlayer = (EntityPlayer*)bm->renderViewEntity;
	float fov = 70.0F;

	if (useSetting)
	{
		fov = 45.f + bm->m_gameSettings->getFovSetting() * 30.0F;
		fov *= fovModifierHandPrev + (fovModifierHand - fovModifierHandPrev) * rdt;
	}

	if (pPlayer->getHealth() <= 0.0F)
	{
		float deadTime = pPlayer->deathTime + rdt;
		fov /= (1.0F - 500.0F / (deadTime + 500.0F)) * 2.0F + 1.0F;
	}

	int blockID = getBlockIdAtEntityViewpoint(bm->m_pWorld, pPlayer, rdt);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		fov = fov * 60.0F / 70.0F;
	}

	return fov + prevDebugCamFOV + (debugCamFOV - prevDebugCamFOV) * rdt;
}

void RenderEntity::renderHand(float rdt)
{
	if (m_isOnVehicle)
		return;

	if (debugViewDirection > 0)
		return;
	
	const GameSettings& gameSetting = *(bm->m_gameSettings);
	// GL11.glMatrixMode(GL11.GL_PROJECTION);
	// GL11.glLoadIdentity();
	float var3 = 0.07F;

	if (cameraZoom != 1.0f)
	{
		// GL11.glTranslatef((float)cameraYaw, (float)(-cameraPitch), 0.0F);
		// GL11.glScaled(cameraZoom, cameraZoom, 1.0D);
	}

	// Project.gluPerspective(getFOVModifier(par1, false), (float)bm.displayWidth / (float)bm.displayHeight, 0.05F, farPlaneDistance * 2.0F);

	if (bm->m_playerControl->enableEverythingIsScrewedUpMode())
	{
		float var4 = 0.6666667F;
		//GL11.glScalef(1.0F, var4, 1.0F);
	}

	//GL11.glMatrixMode(GL11.GL_MODELVIEW);
	//GL11.glLoadIdentity();

	//GL11.glPushMatrix();
	//hurtCameraEffect(rdt);
	itemRenderer->beforeRender();
	if (gameSetting.getPersonView() == GameSettings::SPVT_FIRST &&
		!bm->renderViewEntity->isPlayerSleeping() &&
		!gameSetting.hideGUI &&
		!bm->m_playerControl->enableEverythingIsScrewedUpMode() &&
		!Blockman::Instance()->m_closeup_isShow)
	{
		itemRenderer->renderItemInFirstPerson(rdt);
		itemRenderer->renderEffectInFirstPerson(rdt);
	}

	// GL11.glPopMatrix();

	if (gameSetting.getPersonView() == GameSettings::SPVT_FIRST &&
		!bm->renderViewEntity->isPlayerSleeping())
	{
		itemRenderer->renderOverlays(rdt);
	}
}

void RenderEntity::renderWorld(float dt, float rdt, i64 currentTime)
{
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	if (!pCamera)
		return;

	setupCameraTransform(rdt);

	RenderGlobal* renderGlobal = bm->m_globalRender;
	WorldClient* world = dynamic_cast<WorldClient*>(bm->m_pWorld);
	EntityLivingBase* pViewer = bm->renderViewEntity;
	Vector3 expectPos = pViewer->getLastTickPosition(rdt);

	renderGlobal->preSetupTerrain();
	renderGlobal->swapRenderList();

	Blockman::Instance()->m_exchangeBegin = LORD::Time::Instance()->getMicroseconds();
	Blockman::Instance()->beginTessVisual_stage1(renderGlobal, world);// m_visualTessThread->Update(renderGlobal);

	//bm->getWorld()->getChunkService()->cancelAsyncGets();
	renderGlobal->updateRenders(expectPos);
	Blockman::Instance()->m_renderBegin = LORD::Time::Instance()->getMicroseconds();
	//ChunkRebuilder::Instance()->flushTask(bm->m_pWorld->getChunkService());
	Blockman::Instance()->beginTessVisual_stage2(renderGlobal, world);
	// begin async virtualTessThread
	
	if (lightmapUpdateNeeded)
	{
		updateLightmap(rdt);
	}

	updateLightSetting(rdt);

	if (!bm->renderViewEntity)
	{
		bm->renderViewEntity = bm->m_pPlayer;
	}

	if (bm->m_pPlayer && (bm->m_pPlayer->isOnVehicle() || bm->m_pPlayer->isOnAircraft()))
		m_isOnVehicle = true;
	else
		m_isOnVehicle = false;

	updateEnchantTexMats(rdt);

	TessManager::Instance()->tick(dt);
	updateFogColor(rdt);
	setupFog(1, rdt);
	ActiveRenderInfo::updateRenderInfo(bm->m_pPlayer, bm->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND);
	// getMouseOver(par1);
	
	
	// EffectRenderer var6 = bm.effectRenderer;
	renderGlobal->beginFrame();

	switch (SceneSetting::Instance()->getSkyType())
	{
	case SceneSetting::RST_SKY_2D:
		renderGlobal->renderSky(rdt);
		break;
	case SceneSetting::RST_SKY_CUBE:
		renderGlobal->renderSkybox(rdt);
		break;
	default:
		break;
	}

	if (SceneSetting::Instance()->showSunAndNoon())
	{
		renderGlobal->renderSun(rdt);
	}

	if (SceneSetting::Instance()->showStarEffect())
	{
		renderGlobal->renderStar();
	}

	if (SceneSetting::Instance()->showMeteor())
	{
		renderGlobal->renderMeteor();
	}

	renderGlobal->renderMark(rdt);
	renderGlobal->renderGlow(rdt);
	renderGlobal->renderPoisonCircle(rdt, currentTime);

	switch (SceneSetting::Instance()->getCloudType())
	{
	case SceneSetting::RCT_CLOUD_MODEL:
		renderGlobal->renderClouds(dt);
		break;
	case SceneSetting::RCT_CLOUD_EFFECT:
		renderGlobal->renderClouds2D(rdt);
		break;
	default:
		break;
	}

	// renderGlobal->clipRenderersByFrustum(camera, dtime);
	// renderGlobal->updateRenderers(camera);
	

	// render all the entities.
	renderGlobal->renderEntities(expectPos, pCamera->getFrustum(), rdt);

	// render all the effects.
	EffectRenderer* effectRM = EffectRenderer::Instance();
	effectRM->beginRender();
	effectRM->renderLitParticles(pViewer, rdt);
	effectRM->renderParticles(pViewer, rdt);
	effectRM->endRender();

	if (cameraZoom == 1.0f
		&& !bm->m_gameSettings->hideGUI
		&& bm->objectMouseOver.result
		&& !pViewer->isInsideOfMaterial(BM_Material::BM_MAT_water)
		&& TouchManager::Instance()->getSceneTouchCount() > 0)
	{
		renderGlobal->drawSelectionBox(bm->objectMouseOver, 0, rdt);
	}
	else
		renderGlobal->clearSelection();

	renderGlobal->drawBlockDamageTexture((EntityPlayer*)pViewer, rdt);

	renderRainSnow(rdt, LogicSetting::Instance()->getGameWeather());

	renderGlobal->updateRedBlocks(Root::Instance()->getFrameTime());

	if (cameraZoom == 1.0f && !m_isOnVehicle)
	{
		renderHand(rdt);
	}

	EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
	if (player)
	{
		for (auto iter = player->m_guideArrowList.begin(); iter != player->m_guideArrowList.end(); iter++)
		{
			auto& arrow_guide = *iter;
			Vector3 posTemp = player->getLastTickPosition(rdt);
			posTemp.y -= 1.0f;
			if (arrow_guide.effect)
			{
				arrow_guide.effect->setExtenalParam(posTemp, arrow_guide.endPos);
				arrow_guide.effect->mPosition = posTemp;
			}
		}	
	}

	renderGlobal->renderWorldEffects(expectPos);

	renderGlobal->endFrame();
}

float RenderEntity::getRenderEntityActorScale(EntityLivingBase * pViewer)
{
	NULL_RETURN(pViewer, 1.0F);

	LORD::ActorObject* pViewerActor = EntityRenderManager::Instance()->getEntityActor(pViewer);
	NULL_RETURN(pViewerActor, 1.0F);

	float scale = 1;

	EntityLivingBase* pParent = dynamic_cast<EntityLivingBase*>(pViewer->getParentEntity());
	if (pParent)
	{
		LORD::ActorObject* pParentActor = EntityRenderManager::Instance()->getEntityActor(pParent);
		if (pParentActor)
		{
			scale = 1;
		}
	}

	return scale;
}

int RenderEntity::performanceToFps(int par0)
{
	int var1 = 200;

	if (par0 == 1)
	{
		var1 = 120;
	}

	if (par0 == 2)
	{
		var1 = 35;
	}

	return var1;
}

int RenderEntity::getBlockIdAtEntityViewpoint(World* pWorld, EntityLivingBase* pLiving, float rdt)
{
	Vector3 pos = pLiving->prevPos + (pLiving->position - pLiving->prevPos) * rdt;
	pos.y += pLiving->getEyeHeight();

	pos += cameraOffset;
		
	BlockPos iPos = pos.getFloor();
	int blockID = pWorld->getBlockId(iPos);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial().isLiquid())
	{
		float var6 = BlockFluid::getFluidHeightPercent(pWorld->getBlockMeta(iPos)) - 0.1111111F;
		float var7 = (float)(pos.y + 1) - var6;

		if (pos.y >= var7)
		{
			blockID = pWorld->getBlockId(iPos.getPosY());
		}
	}

	return blockID;
}

void RenderEntity::updateFogColor(float rdt)
{
	// Vector3 lastPos = camera->getLastPostion();
	WorldClient* pWorld = (WorldClient*)(bm->m_pWorld);
	EntityLivingBase* pViewer = bm->renderViewEntity;
	float var4 = 1.0F / (float)(4 - bm->m_gameSettings->renderDistance);
	var4 = 1.0F - (float)Math::Pow((double)var4, 0.25);
	Color skyColor = pWorld->getSkyColor(rdt);
	float skyColor_R = skyColor.r;
	float skyColor_G = skyColor.g;
	float skyColor_B = skyColor.b;
	fogColor = pWorld->getFogColor(rdt);
	float var11;

	// if (bm->m_gameSettings->renderDistance < 2)
	{
		Vector3 var10 = Math::Sin_tab(pWorld->getCelestialAngleRadians(rdt)) > 0.0F ? Vector3::NEG_UNIT_X : Vector3::UNIT_X;

		var11 = pViewer->getLookVec().dot(var10);

		if (var11 < 0.0F)
		{
			var11 = 0.0F;
		}

		if (var11 > 0.0F)
		{
			float* var12 = pWorld->calcSunriseSunsetColors(pWorld->getCelestialAngle(rdt), rdt);

			if (var12 != NULL)
			{
				var11 *= var12[3];
				fogColor.r = fogColor.r * (1.f - var11) + var12[0] * var11;
				fogColor.g = fogColor.g * (1.f - var11) + var12[1] * var11;
				fogColor.b = fogColor.b * (1.f - var11) + var12[2] * var11;
			}
		}
	}
	fogColor += (skyColor - fogColor) * var4;
	float var19 = pWorld->getRainStrength(rdt);
	float var20;

	if (var19 > 0.0F)
	{
		var11 = 1.0F - var19 * 0.5F;
		var20 = 1.0F - var19 * 0.4F;
		fogColor.r *= var11;
		fogColor.g *= var11;
		fogColor.b *= var20;
	}

	var11 = pWorld->getWeightedThunderStrength(rdt);

	if (var11 > 0.0F)
	{
		var20 = 1.0F - var11 * 0.5F;
		fogColor *= var20;
	}

	int blockID = getBlockIdAtEntityViewpoint(pWorld, pViewer, rdt);
	float var22 = 0.f;

	if (cloudFog)
	{
		fogColor = pWorld->getCloudColour(rdt);
	}
	else if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		var22 = (float)EnchantmentHelper::getRespiration(pViewer) * 0.2F;
		fogColor.r = 0.02F + var22;
		fogColor.g = 0.02F + var22;
		fogColor.b = 0.2F + var22;
	}
	else if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_lava)
	{
		fogColor.r = 0.6F;
		fogColor.g = 0.1F;
		fogColor.b = 0.0F;
	}

	var22 = fogColor2 + (fogColor1 - fogColor2) * rdt;
	fogColor *= var22;
	return;
	double var14 = pViewer->getLastTickPosition(rdt).y *  0.03125f;

	if (var14 < 1.0)
	{
		if (var14 < 0.0)
		{
			var14 = 0.0;
		}

		var14 *= var14;
		fogColor *= (float)var14;
	}
}

void RenderEntity::setupFog(int fogMode, float rdt)
{
	EntityLivingBase* pViewer = bm->renderViewEntity;
	bool creativeMode = false;

	if (pViewer->isClass(ENTITY_CLASS_PLAYER))
	{
		creativeMode = ((EntityPlayer*)pViewer)->capabilities.isCreativeMode;
	}


	int blockID = getBlockIdAtEntityViewpoint(bm->m_pWorld, pViewer, rdt);
	farPlaneDistance = (float)(bm->m_gameSettings->getRenderRange()) / 2.f;

	float var6;
	int flogMode = 0;
	float fogDensity = 1.0f;
	float fog_start = 0.f;
	float fog_end = farPlaneDistance;

	if (cloudFog)
	{
		flogMode = 1;
		fogDensity = 0.f;
	}
	else if (blockID > 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_water)
	{
		flogMode = 1;
		fogDensity = 5.5f;
	}
	else if (blockID > 0 && BlockManager::sBlocks[blockID]->getMaterial() == BM_Material::BM_MAT_lava)
	{
		flogMode = 1;
		fogDensity = 2.0f;
	}
	else
	{
		var6 = farPlaneDistance;
		flogMode = 0;
		if (fogMode < 0)
		{
			fog_start = 0.f;
			fog_end = var6 * 0.8f;
		}
		else
		{
			fog_start = var6 * 0.8f;
			fog_end = var6;
		}
		
		//fog_start = var6 * 0.05F;
		//fog_end = Math::Min(var6, 192.f) * 0.5f;
		
	}
	if (PlayerPrefs::Instance()->getBoolPrefs("DisableFog"))
	{
		fogDensity = 0.0F;
	}
	// TessManager::Instance()->setFogParam(fog_start, fog_end, fogDensity);
	// TessManager::Instance()->setFogColor(fogColor);
	SceneManager::Instance()->setFogParam(Vector3(fog_start, fog_end*1.5f, fogDensity));
	SceneManager::Instance()->setFogColor(fogColor);
}

void RenderEntity::clearMouseOver()
{
	RayTraceResult result;
	bm->pointedEntityLiving = NULL;
	pointedEntity = NULL;
	bm->objectMouseOver = result;
}

}
