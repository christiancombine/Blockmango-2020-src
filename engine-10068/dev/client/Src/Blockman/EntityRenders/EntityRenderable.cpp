#include "EntityRenderable.h"
#include "EntityRender.h"
#include "EntityRenderManager.h"
#include "EntityRenderPlayer.h"

#include "cEntity/EntityPlayerSP.h"
#include "cItem/cItem.h"
#include "cWorld/Blockman.h"
#include "Model/ModelBase.h"
#include "Model/ModelBox.h"
#include "Model/ModelBiped.h"
#include "Render/RenderEntity.h"
#include "Render/TextureAtlas.h"
#include "Render/ItemRender.h"

#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"
#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"

#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/SkinedRenderQueue.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/EnchantRenderQueue.h"

namespace BLOCKMAN
{
const int EntityPlayerRenderable::s_armorBoneCount[4] = { 1, 3, 3, 2 };

EntityPlayerRenderable::EntityPlayerRenderable(EntityPlayer* pPlayer, SKINED_RENDER_PASS pass)
	: m_pPlayer(pPlayer)
	, m_mainSkin(NULL)
	, m_mainTexture(NULL)
	, m_mainTexSize(64, 32)
	, m_mainBoneMatrix(NULL)
	, m_mainBoneCount(0)
	, m_equipSkin(NULL)
	, m_enchantEquipSkin(NULL)
	, m_equipAtlas(NULL)
	, m_equipTexture(NULL)
	, m_equipTexSize(256, 128)
	, m_equipBoneCount(0)
	, m_equipChanged(true)
	, m_pass(pass)
	, m_worldMat(Matrix4::IDENTITY)
	, m_color(1.f, 1.f, 1.f, 1.f)
{
	LordAssert(m_pPlayer);
	EntityRender* pRender = EntityRenderManager::Instance()->getEntityClassRenderObject(m_pPlayer->getClassID());
	m_render = dynamic_cast<EntityRenderPlayer*>(pRender);
	LordAssert(m_render);

	for (int i = 0; i < InventoryPlayer::ARMOR_INVENTORY_COUNT; ++i)
	{
		m_equipments[i] = -1;
		m_enchantments[i] = -1;
	}

	invalid();
}

EntityPlayerRenderable::~EntityPlayerRenderable()
{
	if (m_mainSkin)
	{
		TessManager::Instance()->destroySkinedRenderable(m_mainSkin);
		m_mainSkin = nullptr;
	}
	LordSafeFree(m_mainBoneMatrix);

	LordSafeDelete(m_equipSkin);
	LordSafeDelete(m_enchantEquipSkin);

	m_mainTexture = NULL;	// not created.
	m_equipTexture = NULL;
	m_equipAtlas = NULL;
}

void EntityPlayerRenderable::compileMainGeometry()
{
	if (m_mainSkin)
		return;

	ModelBase* mainModel = m_render->getMainModel();
	if (!mainModel)
		return;

	int boneCount = mainModel->getBoneCount();
	LordAssert(boneCount <= SkinedRenderable::s_maxBone);

	if (boneCount != m_mainBoneCount)
	{
		LordSafeFree(m_mainBoneMatrix);
		m_mainBoneCount = boneCount;
	}

	if (!m_mainBoneMatrix)
	{
		m_mainBoneMatrix = (Vector4*)LordMalloc(sizeof(Vector4) * 3 * m_mainBoneCount);
	}

	if (!m_mainSkin)
	{
		m_mainSkin = TessManager::Instance()->createSkinedRenderable(m_mainTexture, m_pass);
		// fill the m_skin vertices
		m_mainSkin->beginPolygon();
		m_render->compileSkinMesh(m_mainSkin, Vector2i::ZERO, m_mainTexSize);
		m_mainSkin->endPolygon();
		m_mainSkin->setBoneMatrices(m_mainBoneMatrix);
		m_mainSkin->setBoneCount(m_mainBoneCount);
		m_mainSkin->setWorldMatrix(&m_worldMat);
	}
}

void EntityPlayerRenderable::compileEquipGeometry()
{
    //
}

void EntityPlayerRenderable::compileMainTexture()
{
	if (m_mainTexture)
		return;

	// todo. 
	// currently all living use the player's default texture.
	m_mainTexture = EntityRenderPlayer::getDefaultTexture();
	m_mainTexSize.x = m_mainTexture->getWidth();
	m_mainTexSize.y = m_mainTexture->getHeight();
}

void EntityPlayerRenderable::compileEquipTexture()
{
	//
}

void EntityPlayerRenderable::update(float rdt)
{
	// update the entity's bone matrices and current worldMatrix.
	LordAssert(m_mainTexture);
	LordAssert(m_mainSkin);
	LordAssert(m_mainBoneMatrix);

	refreshEquipment();
	//compileEquipGeometry();

	m_render->updateBones(m_pPlayer, rdt, m_mainBoneMatrix);

	Vector3 pos = m_pPlayer->getPosition(rdt);
	pos.y -= m_pPlayer->yOffset;
	// Vector3 pos = Vector3(210, 71, 246);
	//float pitch = (m_player->prevRotationPitch + (m_player->rotationPitch - m_player->prevRotationPitch) * rdt) * Math::DEG2RAD;
	float yaw = (m_pPlayer->prevRenderYawOffset + (m_pPlayer->rotationPitch - m_pPlayer->prevRotationPitch) * rdt) * Math::DEG2RAD;

	m_worldMat.identity();
	if (m_pPlayer->deathTime > 0)
	{
		float progress = ((float)m_pPlayer->deathTime + rdt - 1.0F) / 20.0F * 1.6F;
		progress = Math::Sqrt(progress);

		if (progress > 1.0F)
			progress = 1.0F;

		m_worldMat.rotateZ(progress * m_render->getDeathMaxRotation(m_pPlayer) * Math::DEG2RAD);
	}
	m_worldMat.rotateY(Math::PI - yaw);
	m_worldMat.translate(pos);
}

void EntityPlayerRenderable::getBoneMatrix(int boneID, Matrix4& outMat) const
{
	LordAssert(m_mainBoneMatrix);

	outMat.m00 = m_mainBoneMatrix[boneID * 3 + 0].x;
	outMat.m10 = m_mainBoneMatrix[boneID * 3 + 0].y;
	outMat.m20 = m_mainBoneMatrix[boneID * 3 + 0].z;
	outMat.m30 = m_mainBoneMatrix[boneID * 3 + 0].w;

	outMat.m01 = m_mainBoneMatrix[boneID * 3 + 1].x;
	outMat.m11 = m_mainBoneMatrix[boneID * 3 + 1].y;
	outMat.m21 = m_mainBoneMatrix[boneID * 3 + 1].z;
	outMat.m31 = m_mainBoneMatrix[boneID * 3 + 1].w;

	outMat.m02 = m_mainBoneMatrix[boneID * 3 + 2].x;
	outMat.m12 = m_mainBoneMatrix[boneID * 3 + 2].y;
	outMat.m22 = m_mainBoneMatrix[boneID * 3 + 2].z;
	outMat.m32 = m_mainBoneMatrix[boneID * 3 + 2].w;

	outMat.m03 = 0.f;
	outMat.m13 = 0.f;
	outMat.m23 = 0.f;
	outMat.m33 = 1.f;
}

void EntityPlayerRenderable::updateRender()
{
	// set brightness
	int brightUV = m_pPlayer->getBrightnessForRender(0.f);
	if (m_pPlayer->isBurning())
		brightUV = 0x00F000F0;

	Color bright(brightUV);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(bright.b, bright.r);
	m_mainSkin->setExtraColor(Color(brightness) * m_color);

	// post the renderable to tessolator manager.
	TessManager::Instance()->addSkinedRenderable(m_mainSkin);
	/*
	if (m_equipBoneCount > 0)
	{
		// render todo need color.
		m_equipSkin->setExtraColor(Color(brightness) * m_color);
		TessManager::Instance()->addSkinedRenderable(m_equipSkin);
	}
	if (m_enchantEquipSkin)
	{
		// render todo.
		m_enchantEquipSkin->setExtraColor(Color(brightness) * ItemRenderer::s_colorEnch_equip * m_color);
		TessManager::Instance()->addEnchantRenderable(m_enchantEquipSkin);
	}
	*/
}

void EntityPlayerRenderable::updateRender_ui(const Matrix4& world, const Matrix4& viewProj)
{
	// brightness has seted.
	Matrix4 temp = m_worldMat;
	m_worldMat = world;

	// post the renderable to tessolator manager.
	TessManager::Instance()->addSkinedRenderable(m_mainSkin);
	/*
	if (m_equipBoneCount > 0)
	{
		TessManager::Instance()->addSkinedRenderable(m_equipSkin);
	}
	if (m_enchantEquipSkin)
	{
		TessManager::Instance()->addEnchantRenderable(m_enchantEquipSkin);
	}
	*/

	// render the Entity in GUI.
	SceneManager::Instance()->setFogParam(Vector3(1000.f, 4000.f, 1.f));
	TessManager::Instance()->endRender1(&viewProj, false);

	m_worldMat = temp;
}

void EntityPlayerRenderable::refreshEquipment()
{

}

void EntityPlayerRenderable::invalid()
{
	compileMainTexture();
	compileMainGeometry();

	refreshEquipment();

	//compileEquipTexture();
	//compileEquipGeometry();
}

RightArmRenderable::RightArmRenderable()
	: m_player(NULL)
	, m_skin(NULL)
	, m_texture(NULL)
	, m_boneMatrix(NULL)
	, m_compile(false)
	, m_worldMat(Matrix4::IDENTITY)
	, m_model(NULL)
{
	EntityRender* pRender = EntityRenderManager::Instance()->getEntityClassRenderObject(ENTITY_CLASS_PLAYER_SP);
	m_render = dynamic_cast<EntityRenderPlayer*>(pRender);
	LordAssert(m_render);

	invalid();
}

RightArmRenderable::~RightArmRenderable()
{
	if (m_skin)
	{
		TessManager::Instance()->destroySkinedRenderable(m_skin);
		m_skin = nullptr;
	}
	LordSafeDelete(m_model);
	LordSafeFree(m_boneMatrix);
	// LordSafeDelete(m_texture);
}

void RightArmRenderable::compileGeometry()
{
	if (m_skin)
		return;

	ModelBase* mainModel = m_render->getMainModel();
	if (!mainModel)
		return;

	LordSafeFree(m_boneMatrix);
	if (!m_boneMatrix)
	{
		m_boneMatrix = (Vector4*)LordMalloc(sizeof(Vector4) * 3);
	}

	if (!m_model)
	{
		m_model = LordNew ModelBox(Vector2i::ZERO, Vector2i(64, 32), 0);
		float scale = 0.0625f;
		m_model->setPosition(Vector3(-3.0F, -2.0F, -2.0F) * scale, Vector3(4.f, 12.f, 4.f) * scale, 0.f);
		m_model->setTexCoord(Vector2i(40, 16), Vector3i(4, 12, 4));
		m_model->setInvertUV(true);
		m_model->setRotateAxis(Vector3(-5.f, 2.0F, 0.F) * scale);
		m_model->finish();
	}

	if (!m_skin)
	{
		m_skin = TessManager::Instance()->createSkinedRenderable(m_texture, SRP_SOLID_NODEPTH);
		// fill the m_skin vertices
		m_skin->beginPolygon();
		m_model->render(m_skin, Vector2i(0, 0), Vector2i(64, 32));
		m_skin->endPolygon();
		m_skin->setBoneMatrices(m_boneMatrix);
		m_skin->setBoneCount(1);
		m_skin->setWorldMatrix(&m_worldMat);
		m_skin->setViewSpace(true);
	}
}

void RightArmRenderable::compileTexture()
{
	if (m_texture)
		return;

	// todo. 
	// currently all living use the player's default texture.
	m_texture = EntityRenderPlayer::getDefaultTexture();
}

void RightArmRenderable::setZombie(bool isZombie)
{
	if (m_isZombie == isZombie)
		return;

	m_isZombie = isZombie;

	if (!m_skin)
		return;

	if (EntityRenderPlayer::getDefaultTexture() != m_texture) {
		TextureManager::Instance()->releaseResource(m_texture);
	}

	if (m_isZombie)
	{
		m_texture = TextureManager::Instance()->createTexture("zombie.png");
		m_texture->load();
	}
	else
		m_texture = EntityRenderPlayer::getDefaultTexture();

	m_skin->setMaterialParam(m_texture, SRP_SOLID_NODEPTH);
	if (m_skin->getTexture()->getType() == 0)
	{
		LordAssert(false);
	}
}

void RightArmRenderable::update(float equipProgress, float rdt)
{
	// update the entity's bone matrices and current worldMatrix.
	LordAssert(m_texture);
	LordAssert(m_skin);
	LordAssert(m_boneMatrix);

	// setup the hand matrix
	float var20 = 0.8F;
	float scale = 0.0625f;
	float swingProgress = m_player->getSwingProgress(rdt);
	float swings_liner = Math::Sin_tab(swingProgress * Math::PI);
	float swings_biner = Math::Sin_tab(swingProgress * swingProgress * Math::PI);
	float swings_exp = Math::Sin_tab(Math::Sqrt(swingProgress) * Math::PI);
	float swings_exp2 = Math::Sin_tab(Math::Sqrt(swingProgress) * Math::PI_2);

	Matrix4 handMat = Matrix4::IDENTITY;
	Matrix4 localMat = Matrix4::IDENTITY;
	Matrix4 temp = Matrix4::IDENTITY;

	localMat.translate(m_model->m_axis);
	temp.rotateZ(0.1f);
	localMat *= temp;

	temp.identity(); temp.translate(Vector3(5.6f, 0.f, 0.0f));
	handMat *= temp;
	temp.identity(); temp.rotateY(-135.0f * Math::DEG2RAD);
	handMat *= temp;
	temp.identity(); temp.rotateX(200.0f * Math::DEG2RAD);
	handMat *= temp;
	temp.identity(); temp.rotateZ(120.0f * Math::DEG2RAD);
	handMat *= temp;
	temp.identity(); temp.translate(Vector3(-1.0f, 3.6f, 3.5f));
	handMat *= temp;

	temp.identity(); temp.rotateZ(-swings_exp * 20.f * Math::DEG2RAD);
	handMat *= temp;
	temp.identity(); temp.rotateY(swings_exp * 70.f * Math::DEG2RAD);
	handMat *= temp;
	temp.identity(); temp.rotateY(Math::PI_DIV4);
	handMat *= temp;

	float fx = -swings_exp * 0.3F;
	float fy = swings_exp2 * 0.4F;
	float fz = -swings_liner * 0.4F;
	fx += 0.8f * var20;
	fy += -0.75F * var20 - (1.0F - equipProgress) * 0.6F;
	fz += -0.9F * var20;
	// handMat.translate(0.8F * var20, -7.5F * var20 - (1.0F - equipProgress) * 0.6F, );
	temp.identity(); temp.translate(fx, fy, fz);
	handMat *= temp;

	handMat = localMat * handMat;

	m_boneMatrix[0].x = handMat.m00;
	m_boneMatrix[0].y = handMat.m10;
	m_boneMatrix[0].z = handMat.m20;
	m_boneMatrix[0].w = handMat.m30;

	m_boneMatrix[1].x = handMat.m01;
	m_boneMatrix[1].y = handMat.m11;
	m_boneMatrix[1].z = handMat.m21;
	m_boneMatrix[1].w = handMat.m31;

	m_boneMatrix[2].x = handMat.m02;
	m_boneMatrix[2].y = handMat.m12;
	m_boneMatrix[2].z = handMat.m22;
	m_boneMatrix[2].w = handMat.m32;
}

void RightArmRenderable::updateRender()
{
	if (!m_skin)
		return;

	// set brightness
	int brightUV = m_player->getBrightnessForRender(0.f);
	if (m_player->isBurning())
		brightUV = 0x00F000F0;
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
	m_skin->setExtraColor(brightness);

	// post the renderable to tessolator manager.
	TessManager::Instance()->addSkinedRenderable(m_skin);
}

void RightArmRenderable::invalid()
{
	if (m_compile)
		return;

	EntityPlayer* pPlayer = (EntityPlayer*)(m_player);
	compileTexture();
	compileGeometry();
	m_compile = true;
}

}
