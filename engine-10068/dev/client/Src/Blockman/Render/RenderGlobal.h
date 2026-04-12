/********************************************************************
filename: 	RenderGlobal.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-5
*********************************************************************/
#pragma once

#ifndef __RENDER_GLOBAL_HEADER__
#define __RENDER_GLOBAL_HEADER__

#include "Util/Facing.h"
#include "World/IWorldEventListener.h"
#include "BM_Container_def.h"

using namespace LORD;

namespace BLOCKMAN
{
	class World;
	class SectionRenderer;
	class Blockman;
	class CloudSerializer;
	class RayTraceResult;
	class RenderBlocks;
	class TextureAtlasRegister;
	class EntityPlayer;
	class AtlasSprite;
	class EntityFx;
	class Rasterizer;

	class LocalRenderInfo : public ObjectAlloc
	{
	public:
		SectionRenderer* m_sectionRender = nullptr;
		Facing* m_facing = nullptr;
		i8 m_setFacing = 0;
		int m_counter = 0;

		LocalRenderInfo(SectionRenderer* sectionRender, Facing* facing, int counter);
		void setDirection(i8 dir, Facing* facing);
		bool hasDirection(Facing* facing);
	};
	typedef deque<LocalRenderInfo>::type LocalRenderInfoQueue;

	class DestroyBlockProgress : public ObjectAlloc
	{
	protected:
		int playerID = 0;
		BlockPos blockPos;
		/** damage ranges from 1 to 10. -1 causes the client to delete the partial block renderer. */
		int blockProgress = 0;
		/** keeps track of how many ticks this PartiallyDestroyedBlock already exists */
		int createdAtCloudUpdateTick = 0;

	public:
		DestroyBlockProgress(int id, const BlockPos& pos)
			: playerID(id), blockPos(pos), blockProgress(0), createdAtCloudUpdateTick(0) {
		}
		int getBlockX() { return blockPos.x; }
		int getBlockY() { return blockPos.y; }
		int getBlockZ() { return blockPos.z; }
		Vector3i getBlockPos() const { return blockPos; }
		void setBlockPos(const Vector3i& pos) { blockPos = pos; }
		void setPartialBlockDamage(int damage) { if (damage > 10) { damage = 10; } blockProgress = damage; }
		int getPartialBlockDamage() { return blockProgress; }
		/** saves the current Cloud update tick into the PartiallyDestroyedBlock */
		void setCloudUpdateTick(int ticks) { createdAtCloudUpdateTick = ticks; }
		/** retrieves the 'date' at which the PartiallyDestroyedBlock was created */
		int getCreationCloudUpdateTick() { return createdAtCloudUpdateTick; }
	};
	typedef multimap<int, DestroyBlockProgress*>::type DamageBlocks;

	typedef vector<SectionRenderer*>::type SectionRenderList;
	typedef vector<LineRenderable*>::type LineRenderList;
	typedef set<SectionRenderer*>::type SectionRenderSet;
	typedef list<TessRenderable*>::type TessRenderableLst;

	struct WallText
	{
		i32 type;
		i32 timeLeft;
		String wallText;
		Vector3 textPos;
		float scale;
		float yaw;
		float pitch;
		float r;
		float g;
		float b;
		float a;
	};

	struct RedBlock
	{
		Vector3i blockPos;
		int existenceTime;// ms
	};

	class RenderGlobal : public IWorldEventListener, public ObjectAlloc
	{
		friend class Rasterizer;
	public:
		RenderGlobal(Blockman* pmc);
		~RenderGlobal();

		/** set null to clear */
		void setWorldAndLoadRenderers(World* pWorld);

		/** Loads all the renderers and sets up the basic settings usage */
		void loadRenderers();

		void updateClouds();

		void teleport();

		void beginFrame();
		void endFrame();

		/*** render all the entities */
		void renderEntities(const Vector3& viewerPos, const Frustum& frustum, float rdt);

		/*** Goes through all the renderers setting new positions on them and those that have their position changed are adding to be updated */
		void markRenderersForNewPosition(const BlockPos& pos);

		/*** use section's visibility graph to build visual SectionRenders. */
		void swapRenderList();

		void preSetupTerrain();
		void setupTerrain(float rdt);
		void updateSectionLighting();
		void updateSectionMesh();
		SectionRenderer* checkSectionUpdateLighting();
		SectionRenderer* checkSectionGenerateMesh(bool t);
		void updateRenders(const Vector3& eyePos);

		/** for render the visual helps. */
		void drawVisualHelpBox();

		/** Marks the blocks in the given range for update */
		void markBlocksForUpdate(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
		void markBlocksForUpdate(const BlockPos& imin, const BlockPos& imax);

		void renderDebugRaster();
		/** Renders the sky with the partial tick time. Args: partialTickTime */
		void renderSky(float rdt);
		/** Render the skybox */
		void renderSkybox(float rdt);
		/** Renders the sun and moon */
		void renderSun(float rdt);
		/** Renders the mark */
		void renderMark(float rdt);
		/** Render poison circle */
		void renderPoisonCircle(float rdt, i64 currentTime);
		/** Render the sunset sunrise glow */
		void renderGlow(float rdt);
		/** Renders the 3d fancy clouds */
		void renderClouds(float dtTime);
		/** Render the 2d clouds */
		void renderClouds2D(float dtTime);
		/** Render the meteor */
		void renderMeteor();
		/** Render the star */
		void renderStar();
		/** Render world effects**/
		void renderWorldEffects(const Vector3& viewerPos);

		/*** clear the select render.*/
		void clearSelection();
		/** Apply the changes of the SceneSetting switch */
		void applySceneSwitchChanges();

		/** Draws the selection box for the player. */
		void drawSelectionBox(const RayTraceResult& tracy, int itemstack, float rdt);
		void registerDestroyBlockIcons(TextureAtlasRegister* atlas);
		void drawBlockDamageTexture(EntityPlayer* pPlayer, float rdt);

		void enableMarkRender(bool enble);

		void setMarkRenderArea(Vector3 beginPos, Vector3 size, Color color);

		void setPoisonCircleRangeAndSpeed(vector<Vector2>::type range, float speed, String image);

		bool isStart();

		void renderWallText(float yaw);
		void addWallText(i32 type, i32 timeLeft, String txt, Vector3 pos, float scale, float yaw, float pitch, float r, float g, float b, float a);
		void luaAddWallText(int type, int timeLeft, String txt, Vector3 pos, float scale, float yaw, float pitch, std::vector<float> color) {
			return addWallText(type, timeLeft, txt, pos, scale, yaw, pitch, color[0], color[1], color[2], color[3]);
		};
		void deleteWallText(Vector3 pos, float precision = 1.0f);

		Vector3 getPoisonCircleStartPosition();
		Vector3 getPoisonCircleEndPosition();

		Vector3 getSafeCircleStartPosition();
		Vector3 getSafeCircleEndPosition();

		size_t getRenderersBeingRendered() const { return renderersBeingRendered; }
		size_t getRenderersUpdateCount() const { return renderersUpdateCount; }
		size_t getRenderersRasterCullCnt() const { return renderersRasterCullCnt; }

		LineRenderable* getLineRenderable(int points);

		void updateRedBlocks(int frameTime);
		void addRedBlock(const BlockPos& pos);
		void deleteRedBlock(const BlockPos& pos);

		/** implement override functions from IWorldEventListener */
		virtual void markBlockForUpdate(const BlockPos& pos);
		virtual void markSectionForUpdate(const BlockPos& pos);
		virtual void markBlockForRenderUpdate(const BlockPos& pos);
		virtual void markBlockRangeForRenderUpdate(int minx, int miny, int minz, int maxx, int maxy, int maxz);
		virtual void playSound(const Vector3& pos, SoundType soundType) {}
		virtual void playSoundToNearExcept(EntityPlayer* exceptPlayer, const Vector3& pos, SoundType soundType) {}
		virtual void spawnParticle(const String& particleType, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr);
		virtual void onEntityCreate(Entity* pEntity);
		virtual void onEntityDestroy(Entity* pEntity);
		virtual void playRecord(const String& recordName, const BlockPos& pos) {}
		virtual void broadcastSound(int soundID, const BlockPos& pos, int data);
		virtual void playAuxSFX(EntityPlayer* player, int type, const BlockPos& pos, int data);
		virtual void destroyBlockPartially(int entityID, const BlockPos& pos, int damage);
		virtual void onPlayerChangeActor(EntityPlayer* pEntityPlayer);
		virtual void onEntityChangeActor(Entity* pEntity, const String& actorName);
		virtual void onPlayerRestoreActor(Entity* pEntity);

	protected:
		void createPrevRenders();
		void createPostRenders();

		void initAllRenders();
		void initPrevRenders();
		void initPostRenders();

		void initSkyRender();
		void initSkyboxRender();
		void initSunRender();
		void initCloudRender();
		void initCloud2DRender();
		void initGlowRender();
		void initMarkRender();
		void initPoisonRender();
		void initDebugRaster();
		void initMeteorRender();
		void initStarRender();

		void destroyPrevObjectAndNode();

		SectionRenderer* getSectionRender(const BlockPos& pos);
		SectionRenderer* getRenderChunkOffset(const Vector3i& camPos, SectionRenderer* from, Facing* facing);
		FacingArr getVisibleFacing(const Vector3i& blockPos);
		/** Draws lines for the edges of the bounding box. */
		void drawOutlinedBoundingBox(const Box& aabb);
		EntityFx* spawnParticle_impl(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr);
		void markRenderersForNewPositionIfUpdated();
	public:
		vector <std::pair<Box, ui32>>::type visualHelps;
		int m_lightCalFrame = 2;

	protected:
		World* theWorld = nullptr;
		Blockman* bm = nullptr;
		SectionRenderer** sectionRenderer = nullptr;
		int sectionRendererLen = 0;
		SectionRenderList sectionRenderersToRender;
		SectionRenderList sectionRenderersToSwap;
		SectionRenderList sectionRenderersToGenerate;
		static const int maxMeshGenCnt = 8;
		int sectionGenStage = 0;
		bool m_cloudInited = false;
		int m_entityStartCnt = 10;

		LineRenderable* m_selectFrame = nullptr;
		LineRenderable* m_visual_collisions = nullptr;
		LineRenderList  m_visualHelps;
		TessRenderable* m_selectBlock = nullptr;
		BlockPos		m_selectPos;
		int				m_selectID = 0;
		int				m_selectMeta = 0;
		Camera* m_mainCamera = nullptr;
		Rasterizer* m_rasterizer;

		int teleportIdle = 10;

		int renderChunksWide = 0;
		int renderChunksTall = 0;
		int renderChunksDeep = 0;

		/** Minimum Maximum block X Y Z */
		int minBlockX = 0;
		int minBlockY = 0;
		int minBlockZ = 0;
		int maxBlockX = 0;
		int maxBlockY = 0;
		int maxBlockZ = 0;

		int frameIndex = 0;
		/** How many renderers are actually being rendered this frame */
		size_t renderersBeingRendered = 0;
		/** Global renders update count */
		size_t renderersUpdateCount = 0;
		/** Rasterizer Cull count*/
		size_t renderersRasterCullCnt = 0;
		/** World renderers check index */
		int worldRenderersCheckIndex = 0;
		/** counts the cloud render updates. Used with mod to stagger some updates */
		int cloudTickCounter = 0;
		/** Previous x position when the renderers were sorted. (Once the distance moves more than 4 units they will be resorted) */
		Vector3 prevSort;

		int countEntitiesTotal = 0;
		int countEntitiesRendered = 0;
		int countEntitiesHidden = 0;

		Vector3i frustumUpdatePosChunk;
		/** The offset used to determine if a renderer is one of the sixteenth that are being updated this frame */
		int frustumCheckOffset = 0;
		/** the offset used to determine if a renderer is one of the sixteenth that are being updated this frame */
		int skyLightCheckOffset = 0;

		const static int DAMAGE_COUNT = 10;
		RenderBlocks* globalRenderBlocks = nullptr;
		TessRenderable* breakRenderable = nullptr;
		DamageBlocks damagedBlocks;
		AtlasSprite* destroyBlockIcons[DAMAGE_COUNT] = { nullptr };
		TileEntitySet m_tileEntities;

		/** sky renderables */
		SceneNode* m_skyUnderNode = nullptr;
		SceneNode* m_skyUpperNode = nullptr;
		TessObject* m_skyUnderObj = nullptr;
		TessObject* m_skyUpperObj = nullptr;

		/** skybox renderables */
		map<int, SceneNode*>::type m_skyboxNodes;
		map<int, TessObject*>::type m_skyboxObjs;

		/** sun, moon phase renderables*/
		SceneNode* m_sunNode = nullptr;
		SceneNode* m_moonNode = nullptr;
		TessObject* m_sunObj = nullptr;
		TessObject* m_moonObj = nullptr;

		/** post object renderables */
		SceneNode* m_markNode = nullptr;
		SceneNode* m_poisonNode = nullptr;
		SceneNode* m_rasterNode = nullptr;
		TessObject* m_markObj = nullptr;
		TessObject* m_poisonObj = nullptr;

#ifdef SHOW_RASTERIZER_MAP
		TessObject* m_rasterObj = nullptr;
		ui32* m_rasterSysMem = nullptr;
		Texture* m_rasterTex = nullptr;
#endif

		/** glow renderables*/
		SceneNode* m_glowNode = nullptr;
		TessObject* m_glowObj = nullptr;

		/** clouds renderables*/
		SceneNode* m_cloudNode = nullptr;
		TessObject* m_cloudObj = nullptr;
		CloudSerializer* m_cloudSerializer = nullptr;

		/** clouds renderables*/
		SceneNode* m_cloud2DNode = nullptr;
		TessObject* m_cloud2DObj = nullptr;

		/** meteor renderables*/
		SceneNode* m_meteorNode = nullptr;
		TessObject* m_meteorObj = nullptr;

		/** star renderables*/
		SceneNode* m_starNode = nullptr;
		TessObject* m_starObj = nullptr;

		vector<WallText>::type m_wallTextList;

		vector<RedBlock>::type m_redBlocks;

		map<int, Color>::type m_cloud2DCustomColor{
				{0, Color(0.000f, 0.000f, 0.000f)},
				{1, Color(0.000f, 0.000f, 0.000f)},
				{2, Color(0.000f, 0.000f, 0.000f)},
				{3, Color(0.000f, 0.000f, 0.000f)},
				{4, Color(0.000f, 0.000f, 0.000f)},
				{5, Color(0.000f, 0.000f, 0.000f)},
				{6, Color(0.000f, 0.000f, 0.000f)},
				{7, Color(0.131f, 0.145f, 0.224f)},
				{8, Color(0.922f, 0.816f, 0.718f)},
				{9, Color(1.000f, 1.000f, 1.000f)},
				{10, Color(1.000f, 1.000f, 1.000f)},
				{11, Color(1.000f, 1.000f, 1.000f)},
				{12, Color(1.000f, 1.000f, 1.000f)},
				{13, Color(1.000f, 1.000f, 1.000f)},
				{14, Color(1.000f, 1.000f, 1.000f)},
				{15, Color(1.000f, 1.000f, 1.000f)},
				{16, Color(1.000f, 1.000f, 1.000f)},
				{17, Color(1.000f, 1.000f, 1.000f)},
				{18, Color(1.000f, 1.000f, 1.000f)},
				{19, Color(1.000f, 1.000f, 1.000f)},
				{20, Color(0.478f, 0.336f, 0.616f)},
				{21, Color(0.000f, 0.000f, 0.000f)},
				{22, Color(0.000f, 0.000f, 0.000f)},
				{23, Color(0.000f, 0.000f, 0.000f)}
		};
	};
}

#endif