#include "gui_renderItems.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIRenderManager.h"
#include "UI/GUICell.h"
#include "UI/GUICommonDef.h"

#include "Render/TextureAtlas.h"
#include "Setting/ConfigurableBlockSetting.h"

namespace BLOCKMAN
{

gui_renderItems::gui_renderItems()
{
	TextureAtlasRegister* atlasRegsiter = TextureAtlasRegister::Instance();

	m_pImagesetBlock = GUIImagesetManager::Instance()->createImageset("Blocks.json", RESOURCE_TYPE_OUTSIDE);
	MemTextureAtlas* atlas_block = dynamic_cast<MemTextureAtlas*>(atlasRegsiter->getTextureAtlas("Blocks.json"));
	int mip = 1;
	GUIImageset::ImageSetUnitArr blockimages;
	if (atlas_block)
	{
		atlas_block->getMipAtlasSpriteArr(mip, blockimages);
		m_pImagesetBlock->loadOutSide(atlas_block->getMipTexture(mip), blockimages);
	}

	m_pImagesetItem = GUIImagesetManager::Instance()->createImageset("items.json", RESOURCE_TYPE_OUTSIDE);
	TextureAtlas* atlas_item = atlasRegsiter->getTextureAtlas("items.json");
	GUIImageset::ImageSetUnitArr itemimages;
	if (atlas_item)
	{
		atlas_item->getAtlasSpriteArr(itemimages);
		m_pImagesetItem->loadOutSide(atlas_item->getTexture(), itemimages);
	}

	m_pImagesetChest_normal = GUIImagesetManager::Instance()->createImageset("normal_ui.json");
	m_pImagesetChest_normal->load();

	m_pImagesetChest_trapped = GUIImagesetManager::Instance()->createImageset("trapped_ui.json");
	m_pImagesetChest_trapped->load();

	m_pImagesetChest_christmas = GUIImagesetManager::Instance()->createImageset("christmas_ui.json");
	m_pImagesetChest_christmas->load();

	m_pImagesetChest_ender = GUIImagesetManager::Instance()->createImageset("ender_ui.json");
	m_pImagesetChest_ender->load();
}

gui_renderItems::~gui_renderItems()
{
//    LordSafeDelete(m_pImagesetBlock);
//    LordSafeDelete(m_pImagesetItem);
//    LordSafeDelete(m_pImagesetChest_normal);
//    LordSafeDelete(m_pImagesetChest_trapped);
//    LordSafeDelete(m_pImagesetChest_christmas);
//    LordSafeDelete(m_pImagesetChest_ender);
}


void gui_renderItems::renderItemInWindow(GUIWindow *pWindow, int itemID, int damage)
{
	cItem *pcItem = ItemClient::citemsList[itemID];
	cBlock *pcBlock = NULL;
	int blockId = -1;

	if (auto itemBlock = dynamic_cast<cItemBlock*>(pcItem))
	{
		blockId = itemBlock->getBlockId();
		pcBlock = cBlockManager::scBlocks[blockId];
	}

	//render as block
	if (pcItem && pcItem->getRenderType() == ItemRenderType::BLOCK && pcBlock && RenderBlocks::renderItemIn3d(pcBlock->getRenderType()))
	{
		renderBlockAsItem(pWindow, blockId, damage);
	}
	//render as item
	else
	{
		renderItem(pWindow, itemID, damage);
	}


	GUICell *pCell = dynamic_cast<GUICell*>(pWindow);
	LordAssert(pCell);
	pCell->SetVertexData(m_actionVertex);
}


void gui_renderItems::renderItem(GUIWindow *pWindow, int itemID, int damage)
{
	m_actionVertex.clear();

	Item *pItem = Item::itemsList[itemID];
	cItem *pcItem = ItemClient::citemsList[itemID];

	if (!pItem)
	{
		return;
	}

	const ERect& rtArea = pWindow->GetRenderArea();

	//vertex
	float left = 0.0f;
	float right = rtArea.getWidth();
	float top = 0.0f;
	float bottom = rtArea.getHeight();

	Vector3 t[4];
	t[0] = Vector3(left, top, 0.0f);
	t[1] = Vector3(left, bottom, 0.0f);
	t[2] = Vector3(right, bottom, 0.0f);
	t[3] = Vector3(right, top, 0.0f);

	//color
	ItemStackPtr pStack = LORD::make_shared<ItemStack>(itemID, 1, damage);
	Color color(pcItem->getColorFromItemStack(pStack, 0));
	color.a = 255;
	pStack = nullptr;

	//uv
	bool isBlock = pcItem->getRenderType() == ItemRenderType::BLOCK;
	AtlasSprite *atlas = pcItem->getIconFromDamage(damage);
	if (atlas == NULL)
	{
		return;
	}

	String imageName = atlas->getSpriteName();
	GUIImage *pImage = m_pImagesetItem->GetImage(imageName.c_str());
	if (isBlock)
	{
		pImage = m_pImagesetBlock->GetImage(imageName.c_str());
	}

	if (pImage == NULL)
	{
		return;
	}

	ERect uvRect = pImage->GetUV();

	if (m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[1];
		temp.rt = t[3];
		temp.uv = uvRect;
		temp.color = ColorRect(color);
		temp.imageset = m_pImagesetItem;
		if (isBlock)
		{
			temp.imageset = m_pImagesetBlock;
		}
		m_actionVertex.push_back(temp);
	}
}

void gui_renderItems::renderBlockAsItem(GUIWindow *pWindow, int blockID, int metadata)
{
	m_actionVertex.clear();
	if (blockID == BLOCK_ID_STAIRS_COBBLE_STONE ||
		blockID == BLOCK_ID_STAIRS_NETHER_BRICK ||
		blockID == BLOCK_ID_STAIRS_SAND_STONE	||
		blockID == BLOCK_ID_STAIRS_STONE_BRICK	||
		blockID == BLOCK_ID_STAIRS_WOOD_BIRCH	||
		blockID == BLOCK_ID_STAIRS_WOOD_JUNGLE	||
		blockID == BLOCK_ID_STAIRS_WOOD_OAK		||
		blockID == BLOCK_ID_STAIRS_WOOD_SPURCE	||
		blockID == BLOCK_ID_STAIRS_BRICK		|| 
		blockID == BLOCK_ID_STAIRS_NETHER_QUARTZ||
		blockID == BLOCK_ID_STAIRS_WOOD_DARK_OAK||
		blockID == BLOCK_ID_STAIRS_PRISMARINE ||
		blockID == BLOCK_ID_STAIRS_PRISMARINE_BRICK ||
		blockID == BLOCK_ID_STAIRS_DARK_PRISMARINE ||
		blockID == BLOCK_ID_STAIRS_RED_NETHER_BRICK ||
		(ConfigurableBlockSetting::Instance() && ConfigurableBlockSetting::Instance()->isStairsBlock(blockID)))

	{
		renderBlockStairs(pWindow, blockID, metadata);
	}
	else if (blockID == BLOCK_ID_CHEST ||
		blockID == BLOCK_ID_CHEST_TRAPPED ||
		blockID == BLOCK_ID_ENDER_CHEST ||
		blockID == BLOCK_ID_CHEST_NEW ||
		blockID == BLOCK_ID_ENDER_CHEST_NEW ||
		blockID == BLOCK_ID_CHEST_TRAPPED_NEW)
	{
		renderBlockChest(pWindow, blockID, metadata);
	}
	else if (blockID == BLOCK_ID_FENCE ||
		blockID == BLOCK_ID_FENCE_IRON ||
		blockID == BLOCK_ID_COBBLE_STONE_WALL ||
		blockID == BLOCK_ID_NETHER_FENCE ||
		blockID == BLOCK_ID_BIRCH_FENCE
		)
	{
		renderBlockFence(pWindow, blockID, metadata);
	}
	else if (blockID == BLOCK_ID_FENCE_GATE ||
		blockID == BLOCK_ID_BIRCH_FENCE_GATE)
	{
		renderBlockFenceGate(pWindow, blockID, metadata);
	}
	else
	{
		renderStandardBlock(pWindow, blockID, metadata);
	}

	GUIStaticImage *pImage = dynamic_cast<GUIStaticImage*>(pWindow);
	pImage->SetVertexData(m_actionVertex);
}

void gui_renderItems::renderBlockChest(GUIWindow* pWindow, int blockID, int metadata)
{
	GUIImageset* chestImageset = NULL;
	if (blockID == BLOCK_ID_CHEST || blockID == BLOCK_ID_CHEST_NEW)
		chestImageset = m_pImagesetChest_normal;
	else if (blockID == BLOCK_ID_CHEST_TRAPPED || blockID == BLOCK_ID_CHEST_TRAPPED_NEW)
		chestImageset = m_pImagesetChest_trapped;
	else if (blockID == BLOCK_ID_ENDER_CHEST || blockID == BLOCK_ID_ENDER_CHEST_NEW)
		chestImageset = m_pImagesetChest_ender;
	else
		return;

	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];
	pBlock->setBlockBoundsForItemRender();

	float width = pBlock->getBlockBoundsMaxX();
	float height = pBlock->getBlockBoundsMaxY();
	float depth = pBlock->getBlockBoundsMaxZ();

	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();


	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.f;
	float y2 = rtDest.top + w / 2.f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;
	float m1 = -f1 + f1 * 2.f / 3.f;
	float m2 = -f3 + (f2 + f3) / 3.f;

	float offsetWidth = 0.0f;
	float offsetHeight = (1.0f - height) * (f2);
	float offsetDepth = 0.0f;
	Vector3 t[10];
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0); //
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f);
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);
	t[7] = Vector3(f0 + x2, m1 + y2, 0.f);
	t[8] = Vector3(0.f + x2, m2 + y2, 0.f);
	t[9] = Vector3(-f0 + x2, m1 + y2, 0.f);

	/**
	1
	0       2
	9   3   7
	6   8   5
	4
	*/

	/*bright*/
	Color brightLeft(0.5f, 0.5f, 0.5f, 1.f);
	Color brightRight(0.7f, 0.7f, 0.7f, 1.f);

	Color topColor = Color::WHITE;
	Color sideColor = Color::WHITE;

	// top
	GUIImage* pTopImage = chestImageset->GetImage("lid_top");
	if (!pTopImage)	return;
	//side 1
	GUIImage* pLidSide1 = chestImageset->GetImage("lid_side");
	if (!pLidSide1) return;
	GUIImage* pLidSide2 = chestImageset->GetImage("lid_side2");
	if (!pLidSide2) return;
	//side 2
	GUIImage* pBelowSide1 = chestImageset->GetImage("below");
	if (!pBelowSide1) return;
	GUIImage* pBelowSide2 = chestImageset->GetImage("below2");
	if (!pBelowSide2) return;

	if (m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = pTopImage->GetUV();
		temp.color = ColorRect(topColor);
		temp.imageset = chestImageset;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[7];
		temp.lb = t[8];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = pLidSide1->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[0];
		temp.rb = t[8];
		temp.lb = t[9];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = pLidSide2->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[8];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[7];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = pBelowSide1->GetUV();
		m_actionVertex.push_back(temp);

		temp.lt = t[9];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[8];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = pBelowSide2->GetUV();
		m_actionVertex.push_back(temp);
	}
}

void gui_renderItems::renderStandardBlock(GUIWindow *pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}
		

	pBlock->setBlockBoundsForItemRender();

	float width = pBlock->getBlockBoundsMaxX();
	float height = pBlock->getBlockBoundsMaxY();
	float depth = pBlock->getBlockBoundsMaxZ();

	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getHeight();
	rtDest.bottom = rtArea.getWidth();;
	rtDest = rtDest.makeSquare();


	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.f;
	float y2 = rtDest.top + w / 2.f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;

	float offsetWidth = 0.0f;
	float offsetHeight = (1.0f - height) * (f2);
	float offsetDepth = 0.0f;
	Vector3 t[7];
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0);   // left top corner
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);   // center top corner
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);  // right top corner
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f); // center point
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);                 // center bottom corner
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);                  // right bottom corner
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);                 // left bottom corner

	/*bright*/
	Color brightLeft(0.5f, 0.5f, 0.5f, 1.f);
	Color brightRight(0.7f, 0.7f, 0.7f, 1.f);

	/*color*/
	Color topColor = Color::WHITE;
	if (blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_LEAVES)
	{
		topColor = pcBlock->getBlockColor();
		topColor.a = 1.f;
	}

	Color sideColor = Color::WHITE;
	if (blockID == BLOCK_ID_LEAVES)
	{
		sideColor = pcBlock->getBlockColor();
		topColor.a = 1.f;
	}

	// top
	AtlasSprite* sprite;
	if (blockID == BLOCK_ID_PISTON_BASE || blockID == BLOCK_ID_PISTON_STICKY_BASE)
		sprite = pcBlock->getIcon(BM_FACE_UP, 6);
	else
		sprite = pcBlock->getIcon(BM_FACE_UP, metadata);
	String strTopImageName = sprite->getSpriteName();
	auto imageset = GUIImagesetManager::Instance()->createOrRetrieveImageset(pcBlock->getAtlasName().c_str());
	if (!imageset->isLoaded())
	{
		imageset->load();
	}
	GUIImage *pTopImage = imageset->GetImage(strTopImageName.c_str());
	if (pTopImage == NULL)
	{
		return;
	}

	ERect rtTexUV_top = pTopImage->GetUV();
	if (pcBlock->hasBlockAnimation())
	{
		rtTexUV_top.top = sprite->getMinV();
		rtTexUV_top.bottom = sprite->getMaxV();
		rtTexUV_top.left = sprite->getMinU();
		rtTexUV_top.right = sprite->getMaxU();
	}

	//side 1
	sprite = pcBlock->getIcon(BM_FACE_SOUTH, metadata);
	String strSide1ImageName = sprite->getSpriteName();
	GUIImage *pSide1Image = imageset->GetImage(strSide1ImageName.c_str());
	ERect rtTexUV_side1 = pSide1Image->GetUV();
	rtTexUV_side1.top += (1.0f - height) * 0.03125f;
	if (pcBlock->hasBlockAnimation())
	{
		rtTexUV_side1.top = sprite->getMinV();
		rtTexUV_side1.bottom = sprite->getMaxV();
		rtTexUV_side1.left = sprite->getMinU();
		rtTexUV_side1.right = sprite->getMaxU();
	}

	//side 2
	sprite = pcBlock->getIcon(BM_FACE_WEST, metadata);
	String strSide2IamgeName = sprite->getSpriteName();
	GUIImage* pSide2Iamge = imageset->GetImage(strSide2IamgeName.c_str());
	ERect rtTexUV_side2 = pSide2Iamge->GetUV();
	rtTexUV_side2.top += (1.0f - height) * 0.03125f;
	if (pcBlock->hasBlockAnimation())
	{
		rtTexUV_side2.top = sprite->getMinV();
		rtTexUV_side2.bottom = sprite->getMaxV();
		rtTexUV_side2.left = sprite->getMinU();
		rtTexUV_side2.right = sprite->getMaxU();
	}
	
	if(m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);
		temp.imageset = imageset;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);
		//const ImageBuffer& temp1 = vertex.back();

		temp.lt = t[0];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);
		//const ImageBuffer& temp2 = vertex.back();
	}
}

void gui_renderItems::renderBlockFence(GUIWindow* pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}
	/*bright*/
	float brightLeft = 0.5f;
	float brightRight = 0.7f;

	/*color*/
	Color topColor = Color::WHITE;
	topColor.a = 255;
	Color sideColor = Color::WHITE;
	sideColor.a = 255;

	/*uv*/
	// top
	String strTopImageName = pcBlock->getIcon(1, metadata)->getSpriteName();
	GUIImage *pTopImage = m_pImagesetBlock->GetImage(strTopImageName.c_str());
	ERect rtTexUV_leg = pTopImage->GetUV();
	rtTexUV_leg.left += 0.5f * 0.03125f;
	ERect rtTexUV_top = pTopImage->GetUV();
	rtTexUV_top.top += 0.5f * 0.03125f;
	
	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	//rtDest.top = PixelAligned(rtArea.top);
	//rtDest.left = PixelAligned(rtArea.left);
	//rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth()); // *scale
	//rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight()); // *scale
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();

	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float thickness = 0.18f;
	//float slopex = 0.14f;
	float slopex = 0.07f;
	float slopey = 0.07f;
	float offx = 0.06f;
	float offy = 0.06f;
	float wcenter = 0.4f;

	/*
	float x1 = rtDest.left + (thickness - offx) * w;
	float x1o = rtDest.left + (thickness + slopex - offx) * w;
	float x2 = rtDest.left + (thickness * 2 - offx) * w;
	float x2o = rtDest.left + (thickness * 2 + slopex - offx) * w;
	float x3 = rtDest.right - (thickness * 2 + offx) * w;
	float x4 = rtDest.right - (thickness + offx) * w;
	float x4o = rtDest.right - (thickness - slopex + offx) * w;
	*/

	float x1  = rtDest.left + (thickness - slopex) * w;
	float x1o = rtDest.left + thickness * w;
	float x2  = rtDest.left + (thickness * 2 - 2 * offx) * w;
	float x2o = rtDest.left + (thickness * 2 + slopex - 2 * offx) * w;
	float x3  = rtDest.right - (thickness * 2 + offx) * w;
	float x3o = rtDest.right - (thickness * 2 - slopex + offx) * w;
	float x4  = rtDest.right - (thickness + 2 * offx) * w;
	float x4o = rtDest.right - (thickness - slopex + 2 * offx) * w;
	float x5  = rtDest.left + (thickness * 2 - 1.8f * offx) * w;
	float x5o = rtDest.left + (thickness * 2 - 1.2f * offx) * w;

	float y1 = rtDest.top + (wcenter - thickness) * h;
	float y2 = rtDest.top + wcenter * h;
	float y3 = rtDest.bottom - 0.1f * h;
	float y4 = rtDest.top + (wcenter - slopey) * h;
	float y5 = rtDest.top + wcenter * h;
	float y6 = rtDest.bottom - (wcenter + slopey) * h;
	float y7 = rtDest.bottom - wcenter* h;

	float y1o = y1 - slopey * h;
	float y2o = y2 - slopey * h;
	float y3o = y3 - slopey * h;
	float y4o = y4 - 0.5f * slopey * h;
	float y5o = y5 - slopey * h;
	float y6o = y6 - 0.5f * slopey * h;
	float y7o = y7 - slopey * h;
	
	/*
	Vector3 lleg[6];
	lleg[0] = Vector3(x1, y2, 0.f);	//left
	lleg[1] = Vector3(x2, y2, 0.f);	//up
	lleg[2] = Vector3(x2, y3, 0.f);	//right
	lleg[3] = Vector3(x1, y3, 0.f);	//down
	lleg[4] = Vector3(x2o, y2o, 0.f);
	lleg[5] = Vector3(x2o, y3o, 0.f);

	Vector3 rleg[6];
	rleg[0] = Vector3(x3, y2, 0.f);	//left
	rleg[1] = Vector3(x4, y2, 0.f);	//up
	rleg[2] = Vector3(x4, y3, 0.f);	//right
	rleg[3] = Vector3(x3, y3, 0.f);	//down
	rleg[4] = Vector3(x4o, y2o, 0.f);
	rleg[5] = Vector3(x4o, y3o, 0.f);

	Vector3 tleg[7];
	tleg[0] = Vector3(x1, y1, 0.f);	//left
	tleg[1] = Vector3(x4, y1, 0.f);	//up
	tleg[2] = Vector3(x4, y2, 0.f);	//right
	tleg[3] = Vector3(x1, y2, 0.f);	//down
	tleg[4] = Vector3(x1o, y1o, 0.f);
	tleg[5] = Vector3(x4o, y1o, 0.f);
	tleg[6] = Vector3(x4o, y2o, 0.f);

	*/

	Vector3 lleg[7];
	lleg[0] = Vector3(x1, y1, 0.f);	//left
	lleg[1] = Vector3(x2, y1, 0.f);	//up
	lleg[2] = Vector3(x2, y3, 0.f);	//right
	lleg[3] = Vector3(x1, y3, 0.f);	//down
	lleg[4] = Vector3(x2o, y1o, 0.f);
	lleg[5] = Vector3(x2o, y3o, 0.f);
	lleg[6] = Vector3(x1o, y1o, 0.f);

	Vector3 rleg[7];
	rleg[0] = Vector3(x3, y1, 0.f);	//left
	rleg[1] = Vector3(x4, y1, 0.f);	//up
	rleg[2] = Vector3(x4, y3, 0.f);	//right
	rleg[3] = Vector3(x3, y3, 0.f);	//down
	rleg[4] = Vector3(x4o, y1o, 0.f);
	rleg[5] = Vector3(x4o, y3o, 0.f);
	rleg[6] = Vector3(x3o, y1o, 0.f);

	Vector3 tleg[7];
	tleg[0] = Vector3(x5, y4, 0.f);	//left
	tleg[1] = Vector3(x3, y4, 0.f);	//up
	tleg[2] = Vector3(x3, y5, 0.f);	//right
	tleg[3] = Vector3(x5, y5, 0.f);	//down
	tleg[4] = Vector3(x5o, y4o, 0.f);
	tleg[5] = Vector3(x3, y4o, 0.f);
	tleg[6] = Vector3(x4o, y5o, 0.f);

	Vector3 dleg[7];
	dleg[0] = Vector3(x5, y6, 0.f);	//left
	dleg[1] = Vector3(x3, y6, 0.f);	//up
	dleg[2] = Vector3(x3, y7, 0.f);	//right
	dleg[3] = Vector3(x5, y7, 0.f);	//down
	dleg[4] = Vector3(x5o, y6o, 0.f);
	dleg[5] = Vector3(x3, y6o, 0.f);
	dleg[6] = Vector3(x4o, y7o, 0.f);


	if (m_actionVertex.empty())
	{
		/*
		//stairs nether
		ImageBuffer temp;
		temp.lt = lleg[0];
		temp.rt = lleg[1];
		temp.rb = lleg[2];
		temp.lb = lleg[3];
		temp.uv = rtTexUV_leg;
		temp.color = ColorRect(topColor);
		temp.imageset = m_pImagesetBlock;
		m_actionVertex.push_back(temp);

		temp.lt = lleg[1];
		temp.rt = lleg[4];
		temp.rb = lleg[5];
		temp.lb = lleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[0];
		temp.rt = rleg[1];
		temp.rb = rleg[2];
		temp.lb = rleg[3];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[1];
		temp.rt = rleg[4];
		temp.rb = rleg[5];
		temp.lb = rleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[0];
		temp.rt = tleg[1];
		temp.rb = tleg[2];
		temp.lb = tleg[3];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[4];
		temp.rt = tleg[5];
		temp.rb = tleg[1];
		temp.lb = tleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[1];
		temp.rt = tleg[5];
		temp.rb = tleg[6];
		temp.lb = tleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);
		*/

		ImageBuffer temp;
		temp.lt = lleg[0];
		temp.rt = lleg[1];
		temp.rb = lleg[2];
		temp.lb = lleg[3];
		temp.uv = rtTexUV_leg;
		temp.color = ColorRect(topColor);
		temp.imageset = m_pImagesetBlock;
		m_actionVertex.push_back(temp);

		temp.lt = lleg[1];
		temp.rt = lleg[4];
		temp.rb = lleg[5];
		temp.lb = lleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = lleg[6];
		temp.rt = lleg[4];
		temp.rb = lleg[1];
		temp.lb = lleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[0];
		temp.rt = rleg[1];
		temp.rb = rleg[2];
		temp.lb = rleg[3];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[1];
		temp.rt = rleg[4];
		temp.rb = rleg[5];
		temp.lb = rleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[6];
		temp.rt = rleg[4];
		temp.rb = rleg[1];
		temp.lb = rleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[3];
		temp.rt = tleg[0];
		temp.rb = tleg[1];
		temp.lb = tleg[2];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[4];
		temp.rt = tleg[5];
		temp.rb = tleg[1];
		temp.lb = tleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = dleg[3];
		temp.rt = dleg[0];
		temp.rb = dleg[1];
		temp.lb = dleg[2];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = dleg[4];
		temp.rt = dleg[5];
		temp.rb = dleg[1];
		temp.lb = dleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

	}
}

void gui_renderItems::renderBlockStairs(GUIWindow *pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}


	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;
	//rtDest.top = PixelAligned(rtArea.top);
	//rtDest.left = PixelAligned(rtArea.left);
	//rtDest.right = PixelAligned(rtDest.left + rtArea.getWidth()); // *scale
	//rtDest.bottom = PixelAligned(rtDest.top + rtArea.getHeight()); // *scale
	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();

	/*vertex*/
	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float x2 = rtDest.left + w / 2.0f;
	float y2 = rtDest.top + w / 2.0f;
	float f0 = 0.441941738f * w;
	float f1 = 0.270632952f * h;
	float f2 = 0.491603792f * h;
	float f3 = 0.0496620759f * h;

	float offsetHeight = 0.5f * f2;
	float offsetX = 0.5f * f0;
	float offsetY = 0.5f * f1;
	
	Vector3 t[14];
	//下半部分
	t[0] = Vector3(-f0 + x2, -f1 + y2 + offsetHeight, 0);	//left
	t[1] = Vector3(0.f + x2, -f2 + y2 + offsetHeight, 0);	//up
	t[2] = Vector3(f0 + x2, -f1 + y2 + offsetHeight, 0.f);	//right
	t[3] = Vector3(0.f + x2, -f3 + y2 + offsetHeight, 0.f);	//down
	t[4] = Vector3(0.f + x2, f2 + y2, 0.f);
	t[5] = Vector3(f0 + x2, f1 + y2, 0.f);
	t[6] = Vector3(-f0 + x2, f1 + y2, 0.f);

	//上半部分
	t[7] = t[0];
	t[8] = (t[1] + t[2]) / 2.0f;
	t[9] = (t[0] + t[3]) / 2.0f;
	t[10] = t[7] - Vector3(0, offsetHeight, 0);			//left
	t[11] = t[1] - Vector3(0, offsetHeight, 0);			//up
	t[12] = t[8] - Vector3(0, offsetHeight, 0);			//right
	t[13] = t[9] - Vector3(0, offsetHeight, 0);			//down

	/*bright*/
	float brightLeft = 0.5f;
	float brightRight = 0.7f;

	/*color*/
	Color topColor = Color::WHITE;	
	topColor.a = 255;
	Color sideColor = Color::WHITE;	
	sideColor.a = 255;

	/*uv*/
	// top
	String strTopImageName = pcBlock->getIcon(1, metadata)->getSpriteName();
	GUIImage *pTopImage = m_pImagesetBlock->GetImage(strTopImageName.c_str());
	ERect rtTexUV_top = pTopImage->GetUV();
	rtTexUV_top.top += 0.5f * 0.03125f;
	//side 1
	String strSide1ImageName = pcBlock->getIcon(3, metadata)->getSpriteName();
	GUIImage *pSide1Image = m_pImagesetBlock->GetImage(strSide1ImageName.c_str());
	ERect rtTexUV_side1 = pSide1Image->GetUV();	
	rtTexUV_side1.top += 0.5f * 0.03125f;
	//side 2
	String strSide2IamgeName = pcBlock->getIcon(5, metadata)->getSpriteName();
	GUIImage *pSide2Iamge = m_pImagesetBlock->GetImage(strSide2IamgeName.c_str());
	ERect rtTexUV_side2 = pSide2Iamge->GetUV();	
	rtTexUV_side2.top += 0.5f * 0.03125f;
	
	if (m_actionVertex.empty())
	{
		//stairs nether
		ImageBuffer temp;
		temp.lt = t[0];
		temp.rb = t[2];
		temp.lb = t[3];
		temp.rt = t[1];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);
		temp.imageset = m_pImagesetBlock;
		m_actionVertex.push_back(temp);

		temp.lt = t[3];
		temp.rb = t[5];
		temp.lb = t[4];
		temp.rt = t[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);		

		temp.lt = t[0];
		temp.rb = t[4];
		temp.lb = t[6];
		temp.rt = t[3];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);		

		//stairs upper
		
		// 0.0625	16/256		
// 		rtTexUV_top.right -= 0.5f * 0.0625f;		
// 		rtTexUV_side1.right -= 0.5f * 0.0625f;		
// 		rtTexUV_side2.right -= 0.5f * 0.0625f;

		temp.lt = t[10];
		temp.rb = t[12];
		temp.lb = t[13];
		temp.rt = t[11];
		temp.uv = rtTexUV_top;
		temp.color = ColorRect(topColor);		
		m_actionVertex.push_back(temp);

		temp.lt = t[13];
		temp.rb = t[8];
		temp.lb = t[9];
		temp.rt = t[12];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_side1;
		m_actionVertex.push_back(temp);

		temp.lt = t[10];
		temp.rb = t[9];
		temp.lb = t[7];
		temp.rt = t[13];
		temp.color = ColorRect(sideColor * brightLeft);
		temp.uv = rtTexUV_side2;
		m_actionVertex.push_back(temp);
	}
		
}

void gui_renderItems::renderBlockFenceGate(GUIWindow* pWindow, int blockID, int metadata)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	if (!pBlock)
	{
		return;
	}
	/*bright*/
	float brightLeft = 0.5f;
	float brightRight = 0.7f;

	/*color*/
	Color topColor = Color::WHITE;
	topColor.a = 255;
	Color sideColor = Color::WHITE;
	sideColor.a = 255;

	/*uv*/
	// top
	String strTopImageName = pcBlock->getIcon(1, metadata)->getSpriteName();
	GUIImage *pTopImage = m_pImagesetBlock->GetImage(strTopImageName.c_str());
	ERect rtTexUV_leg = pTopImage->GetUV();
	rtTexUV_leg.left += 0.5f * 0.03125f;
	ERect rtTexUV_top = pTopImage->GetUV();
	rtTexUV_top.top += 0.5f * 0.03125f;

	const ERect& rtArea = pWindow->GetRenderArea();
	ERect rtDest;

	rtDest.top = 0;
	rtDest.left = 0;
	rtDest.right = rtArea.getWidth();
	rtDest.bottom = rtArea.getHeight();
	rtDest = rtDest.makeSquare();

	float w = rtDest.getWidth();
	float h = rtDest.getHeight();
	float thickness = 0.18f;
	float slopex = 0.07f;
	float slopey = 0.07f;
	float offx = 0.06f;
	float offy = 0.06f;
	float wcenter = 0.4f;

	float x1 = rtDest.left + (thickness - 2 * slopex) * w;
	float x1o = rtDest.left + (thickness - slopex) * w;
	float x2 = rtDest.left + (thickness * 2 - 2 * offx - slopex) * w;
	float x2o = rtDest.left + (thickness * 2 - 2 * offx) * w;
	float x3 = rtDest.right - (thickness * 2 + offx - slopex) * w;
	float x3o = rtDest.right - (thickness * 2 - 2 * slopex + offx) * w;
	float x4 = rtDest.right - (thickness + 2 * offx - slopex) * w;
	float x4o = rtDest.right - (thickness - 2 * slopex + 2 * offx) * w;
	float x5 = rtDest.left + (thickness * 2 - 1.8f * offx - slopex) * w;
	float x5o = rtDest.left + (thickness * 2 - 1.2f * offx - slopex) * w;
	float x6 = x5 + (thickness) * w;
	float x6o = x5 + (thickness + slopex) * w;
	float x7 = x6 + 0.1f * w;
	float x7o = x6 + (0.1f + 0.5f * slopex) * w;


	float y1 = rtDest.top + (wcenter - thickness) * h;
	float y2 = rtDest.top + wcenter * h;
	float y3 = rtDest.bottom - 0.2f * h;
	float y4 = rtDest.top + (wcenter - slopey) * h;
	float y5 = rtDest.top + (wcenter + offy) * h;
	float y6 = rtDest.bottom - (wcenter) * h;
	float y7 = rtDest.bottom - (wcenter - slopey) * h;

	float y1o = y1 - slopey * h;
	float y2o = y2 - slopey * h;
	float y3o = y3 - slopey * h;
	float y4o = y4 - 0.5f * slopey * h;
	float y5o = y5 - slopey * h;
	float y6o = y6 - 0.5f * slopey * h;
	float y7o = y7 - slopey * h;

	Vector3 lleg[7];
	lleg[0] = Vector3(x1, y1, 0.f);	//left
	lleg[1] = Vector3(x2, y1, 0.f);	//up
	lleg[2] = Vector3(x2, y3, 0.f);	//right
	lleg[3] = Vector3(x1, y3, 0.f);	//down
	lleg[4] = Vector3(x2o, y1o, 0.f);
	lleg[5] = Vector3(x2o, y3o, 0.f);
	lleg[6] = Vector3(x1o, y1o, 0.f);

	Vector3 rleg[7];
	rleg[0] = Vector3(x3, y1, 0.f);	//left
	rleg[1] = Vector3(x4, y1, 0.f);	//up
	rleg[2] = Vector3(x4, y3, 0.f);	//right
	rleg[3] = Vector3(x3, y3, 0.f);	//down
	rleg[4] = Vector3(x4o, y1o, 0.f);
	rleg[5] = Vector3(x4o, y3o, 0.f);
	rleg[6] = Vector3(x3o, y1o, 0.f);

	Vector3 tleg[7];
	tleg[0] = Vector3(x5, y4, 0.f);	//left
	tleg[1] = Vector3(x3, y4, 0.f);	//up
	tleg[2] = Vector3(x3, y5, 0.f);	//right
	tleg[3] = Vector3(x5, y5, 0.f);	//down
	tleg[4] = Vector3(x5o, y4o, 0.f);
	tleg[5] = Vector3(x3, y4o, 0.f);
	tleg[6] = Vector3(x4o, y5o, 0.f);

	Vector3 dleg[7];
	dleg[0] = Vector3(x5, y6, 0.f);	//left
	dleg[1] = Vector3(x3, y6, 0.f);	//up
	dleg[2] = Vector3(x3, y7, 0.f);	//right
	dleg[3] = Vector3(x5, y7, 0.f);	//down
	dleg[4] = Vector3(x5o, y6o, 0.f);
	dleg[5] = Vector3(x3, y6o, 0.f);
	dleg[6] = Vector3(x4o, y7o, 0.f);
	
	Vector3 mleg[6];
	mleg[0] = Vector3(x6, y5, 0.f);
	mleg[1] = Vector3(x7, y5, 0.f);
	mleg[2] = Vector3(x7, y6, 0.f);
	mleg[3] = Vector3(x6, y6, 0.f);
	mleg[4] = Vector3(x7o, y5, 0.f);
	mleg[5] = Vector3(x7o, y6, 0.f);

	if (m_actionVertex.empty())
	{
		ImageBuffer temp;
		temp.lt = lleg[0];
		temp.rt = lleg[1];
		temp.rb = lleg[2];
		temp.lb = lleg[3];
		temp.uv = rtTexUV_leg;
		temp.color = ColorRect(topColor);
		temp.imageset = m_pImagesetBlock;
		m_actionVertex.push_back(temp);

		temp.lt = lleg[1];
		temp.rt = lleg[4];
		temp.rb = lleg[5];
		temp.lb = lleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = lleg[6];
		temp.rt = lleg[4];
		temp.rb = lleg[1];
		temp.lb = lleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[0];
		temp.rt = rleg[1];
		temp.rb = rleg[2];
		temp.lb = rleg[3];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[1];
		temp.rt = rleg[4];
		temp.rb = rleg[5];
		temp.lb = rleg[2];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = rleg[6];
		temp.rt = rleg[4];
		temp.rb = rleg[1];
		temp.lb = rleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[3];
		temp.rt = tleg[0];
		temp.rb = tleg[1];
		temp.lb = tleg[2];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = tleg[4];
		temp.rt = tleg[5];
		temp.rb = tleg[1];
		temp.lb = tleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = dleg[3];
		temp.rt = dleg[0];
		temp.rb = dleg[1];
		temp.lb = dleg[2];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = dleg[4];
		temp.rt = dleg[5];
		temp.rb = dleg[1];
		temp.lb = dleg[0];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_top;
		m_actionVertex.push_back(temp);

		temp.lt = mleg[1];
		temp.rt = mleg[2];
		temp.rb = mleg[3];
		temp.lb = mleg[0];
		temp.color = ColorRect(topColor);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

		temp.lt = mleg[4];
		temp.rt = mleg[5];
		temp.rb = mleg[2];
		temp.lb = mleg[1];
		temp.color = ColorRect(sideColor * brightRight);
		temp.uv = rtTexUV_leg;
		m_actionVertex.push_back(temp);

	}
}


}
