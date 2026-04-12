#include "SectionRenderer.h"
#include "VisualGraph.h"
#include "RenderBlocks.h"
#include "Common.h"
#include "TessMeshCollection.h"
#include "GenSectionLOD.h"
#include "Rasterizer.h"

#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "cChunk/ChunkClient.h"
#include "World/ChunkCollection.h"
#include "cTileEntity/TileEntityRenderMgr.h"

#include "Scene/Camera.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/RasterizerHelpRender.h"
#include "VisualTessThread.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{

SectionRenderer::SectionRenderer(World* par1World, const BlockPos& pos)
{
	worldObj = par1World;
	// this.tileEntities = par2List;
	blockPos.x = -999;
	chunkIndex = 0;
	isChunkLit = false;
	setPosition(pos);
	needsUpdate = false;
	m_pChunk = nullptr;
	m_pSection = nullptr;
	m_secPos = Vector3i::INVALID;
	m_frameIndex = -1;
#ifdef SHOW_RASTERIZER_RENDER
	rasterRenderable = nullptr;
#endif
}

SectionRenderer::~SectionRenderer()
{
	releaseRenderalbes();

#ifdef SHOW_RASTERIZER_RENDER
	if (rasterRenderable)
	{
		TessManager::Instance()->destroyRasterizerRenderalbe(rasterRenderable);
		rasterRenderable = nullptr;
	}
#endif

	LordSafeDelete(m_rasterizerMesh);
}

void SectionRenderer::setPosition(const BlockPos& pos)
{
	if (pos != blockPos)
	{
		blockPos = pos;
		posPlus = pos + 8;
		posClip.x = pos.x & 1023;
		posClip.y = pos.y;
		posClip.z = pos.z & 1023;
		posMinus = pos - posClip;
		Vector3 bound = Vector3::ZERO;
		FailCnt = 0;
		rendererBoundingBox.setExtents(Vector3(pos) - bound, Vector3(pos) + 16 + bound);
		markDirty();
	}
}

float SectionRenderer::distanceToEntitySquared(Camera* pCamera) const
{
	Vector3 cameraPos = pCamera->getPosition();

	Vector3 disVec = cameraPos - posPlus;
	return disVec.lenSqr();
}

bool SectionRenderer::setFrameIndex(int frameIdx)
{
	if (frameIdx == m_frameIndex)
		return false;
	m_frameIndex = frameIdx;
	return true;
}

void SectionRenderer::markDirty()
{
	m_entities_section.clear();
	needsUpdate = true;
	m_secPos = Vector3i::INVALID;
	m_pChunk = nullptr;
	m_pSection = nullptr;
}

void SectionRenderer::exchangeRenderables()
{
	if (renderBlockCnt == 0)
	{
		releaseRenderalbes();
		return;
	}
	if (logic_tessLst.empty())
		return;

	releaseRenderalbes();
	render_tessLst.insert(render_tessLst.end(), logic_tessLst.begin(), logic_tessLst.end());
	logic_tessLst.clear();
	finishUpdateRender();
}	

void SectionRenderer::releaseRenderalbes()
{
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		TessManager::Instance()->destroyTessRenderable(*it);
	}
	render_tessLst.clear();
}

bool SectionRenderer::finishUpdateRender()
{
	bool result = false;
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		(*it)->endPolygon();
		result = true;
	}
	return result;
}

void SectionRenderer::exchangeEntities(TileEntitySet& rhs)
{
	rhs.insert(m_entities_section.begin(), m_entities_section.end());
}

bool SectionRenderer::updateRenderer()
{
	bool nResult = false;
	TessMeshCollection* pChunkCollection = NULL;

	do
	{
		ERROR_BREAK(needsUpdate);

		BlockPos startPos = blockPos;
		BlockPos endPos = startPos + 16;
		int extend = 1;

		pChunkCollection = LordNew TessMeshCollection((WorldClient*)worldObj);
		ERROR_BREAK(pChunkCollection);
		bool retReSet = pChunkCollection->reSetSection(blockPos);
		ERROR_BREAK(retReSet);

		if (pChunkCollection->extendedLevelsInChunkCollection())
		{
			nResult = true;
			renderBlockCnt = 0;
			m_entities_section.clear();
			break;
		}

		retReSet = pChunkCollection->buildData();
		ERROR_BREAK(retReSet);
		
		for (TessRenderableLst::iterator it = logic_tessLst.begin(); it != logic_tessLst.end(); ++it)
		{
			TessManager::Instance()->destroyTessRenderable(*it);
		}
		logic_tessLst.clear();

		TileEntitySet entities_swap;
		// releaseRenderalbes();
		needsUpdate = false;
		RenderBlocks* pRenderBlocks = LordNew RenderBlocks(pChunkCollection, this, Vector3(blockPos) + 8.f);
		bytesDrawn = 0;
		renderBlockCnt = 0;
		for (int iy = startPos.y; iy < endPos.y; ++iy)
		{
			for (int iz = startPos.z; iz < endPos.z; ++iz)
			{
				for (int ix = startPos.x; ix < endPos.x; ++ix)
				{
					BlockPos pos(ix, iy, iz);
					int blockID = pChunkCollection->getBlockId(pos);

					if (blockID <= 0)
						continue;

					Block* pBlock = BlockManager::sBlocks[blockID];
					if (!pBlock)
						continue;

					pBlock->isOpaqueCube();

					if (pBlock->hasTileEntity())
					{
						TileEntity* ptileEntity = pChunkCollection->getBlockTileEntity(pos);

						if (TileEntityRenderMgr::Instance()->hasSpecialRenderer(ptileEntity))
						{
							entities_swap.insert(ptileEntity);
						}
					}

					if(pRenderBlocks->renderBlockByRenderType(pBlock, pos))
						renderBlockCnt++;
				}
			}
		}

		m_entities_section.swap(entities_swap);
		
		// caculate the rasterize mesh.
		if (!m_rasterizerMesh)
			m_rasterizerMesh = LordNew RasterizerMesh();
		GenSectionLod genLod(m_rasterizerMesh, blockPos/16);
		genLod.generate(pChunkCollection->blocks);

		// debug show the rasterizerMesh.
#ifdef SHOW_RASTERIZER_RENDER
		if (!rasterRenderable)
		{
			rasterRenderable = TessManager::Instance()->createRasterizerRenderable();
		}
		int quadCount = m_rasterizerMesh->m_rasterVertex.size() / 4;
		rasterRenderable->beginPolygon();
		LineVertexFmt* pVertex;
		rasterRenderable->getLockPtr(quadCount, pVertex);
		for (int i = 0; i < quadCount * 4; ++i)
		{
			Vector3 position = m_rasterizerMesh->m_rasterVertex[i].GetPosition();
			pVertex[i].pos = position + blockPos;
			pVertex[i].color = 0x7FFF007F;
		}
		rasterRenderable->endPolygon();
#endif

		LordSafeDelete(pRenderBlocks);
		isInitialized = true;
		// finishUpdateRender();
		nResult = true;
	} while (0);

	LordSafeDelete(pChunkCollection);

	return nResult;
}

bool SectionRenderer::updateSpecifyPos(const BlockPos& pos, TessRenderable* render)
{
	bool nResult = false;
	ChunkCollection* pChunkCollection = NULL;

	do
	{
		BlockPos startPos = blockPos;
		BlockPos endPos = startPos + 16;
		int extend = 1;
		pChunkCollection = LordNew ChunkCollection(worldObj, startPos - extend, endPos + extend, extend);

		ERROR_BREAK(pChunkCollection);

		if (pChunkCollection->isInAsync())
		{
			break;
		}

		if (pChunkCollection->extendedLevelsInChunkCollection())
		{
			nResult = true;
			break;
		}

		RenderBlocks* pRenderBlocks = LordNew RenderBlocks(pChunkCollection, this, Vector3(blockPos) + 8.f);
		render->beginPolygon();
		pRenderBlocks->setSpecialRenderable(render);

		int blockID = pChunkCollection->getBlockId(pos);
		ERROR_BREAK(blockID > 0);

		Block* pBlock = BlockManager::sBlocks[blockID];
		ERROR_BREAK(pBlock);

		pRenderBlocks->renderBlockByRenderType(pBlock, pos, true);
		/*render->setVertexColor(0.4f);*/
		render->endPolygon();

		LordSafeDelete(pRenderBlocks);
		nResult = true;
	} while (0);

	LordSafeDelete(pChunkCollection);

	return nResult;
}

void SectionRenderer::render(const Vector3& eyePos)
{
	TessManager* tessManager = TessManager::Instance();
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		TessRenderable* renderable = *it;
		float dx = eyePos.x - blockPos.x;
		float dz = eyePos.z - blockPos.z;
		float lensqr = dx * dx + dz * dz;
		if (renderable->getpass() == TRP_SOLID_NOMIP && lensqr > 64.f * 64.f)
			continue;

		tessManager->addTessRenderable(renderable);
	}
}

TessRenderable* SectionRenderer::getRenderable(Texture* tex, TESS_RENDER_PASS pass, LORD::i8 lightStrength, int quadNum, std::uint64_t blockAnimKey)
{
	TessRenderable* pResult = NULL;
	bool isBloom = (lightStrength != 0);
	for (TessRenderableLst::iterator it = logic_tessLst.begin(); it != logic_tessLst.end(); ++it)
	{
		if ((*it)->getTexture() != tex)
			continue;
		if ((*it)->getRenderGroup() != pass)
			continue;
		if ((*it)->getFreeQuadNum() < quadNum)
			continue;
		/*if ((*it)->getBlockAnimKey() != blockAnimKey)
			continue;
		if ((*it)->isBloom() != isBloom)
			continue;*/


		pResult = *it;
	}

	if (!pResult)
	{
		pResult = TessManager::Instance()->createTessRenderable(tex, pass);
		/*pResult->setBlockAnimKey(blockAnimKey);
		pResult->setBloom(isBloom);*/
		pResult->beginPolygon();
		logic_tessLst.push_back(pResult);
	}
	pResult->setPosition(blockPos);
	return pResult;
}

SetVisibility* SectionRenderer::getSectionVisibility()
{
	if(m_pSection)
		return m_pSection->getVisibility();

	if (!worldObj->chunkExists(blockPos.x >> 4, blockPos.z >> 4))
		return nullptr;

	m_pChunk = worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z >> 4);
	//if (pChunk && pChunk->m_posX == m_chunkPos.x && pChunk->m_posZ == m_chunkPos.y)
	if (m_pChunk)
		m_pSection = (SectionClient*)(m_pChunk->m_pSections[blockPos.y >> 4]);

	if (m_pSection)
	{
		m_secPos = m_pSection->getSectionID();
		return m_pSection->getVisibility();
	}
	return nullptr;
}

void SectionRenderer::updateRedBlock(const BlockPos& pos, bool isRed)
{
	ChunkPtr pChunk = worldObj ? worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z >> 4) : nullptr;
	if (pChunk && pChunk->changeBlockRedStatusInThisChunk(pos.getChunkPos(), isRed))
	{
		markDirty();
	}
}

}
