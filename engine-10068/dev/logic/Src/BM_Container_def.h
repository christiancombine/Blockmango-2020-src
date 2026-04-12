/********************************************************************
filename: 	BM_Container_def.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-14
*********************************************************************/
#ifndef __BM_CONTAINER_DEF_HEADER__
#define __BM_CONTAINER_DEF_HEADER__

#include "Core.h"
#include "Item/ItemStack.h"
#include "Item/Ingredient.h"

using namespace LORD;

namespace BLOCKMAN
{

class Chunk;
class Block;
class SpawnListEntry;
class Entity;
class TileEntity;
class ItemStack;
class PotionEffect;
class EntityPlayer;
class EntityCreature;

using ChunkPtr = std::shared_ptr<Chunk>;

typedef vector<Block*>::type						BlockArr;
typedef vector<BlockPos>::type						BlockPosArr;
typedef vector<PotionEffect*>::type					PotionEffectArr;
typedef map<String, String>::type					StringMap;
typedef map<String, StringMap>::type				StringMapMap;
typedef vector<ChunkPtr>::type						ChunkArr;
typedef map<i64, ChunkPtr>::type					ChunkMap;
typedef vector<SpawnListEntry*>::type				SpwanLstEntryArr;
typedef vector<Box>::type							AABBList;
typedef map<int, Entity*>::type						EntityMap;
typedef vector<Entity*>::type						EntityArr;
typedef set<Entity*>::type				EntityList;
typedef vector<EntityList>::type					EntityArrays;
typedef vector<ItemStackPtr>::type					ItemStackArr;
typedef std::shared_ptr<Ingredient>					IngredientPtr;
typedef vector<IngredientPtr>::type					IngredientPtrArr;
typedef vector<TileEntity*>::type					TileEntities;
typedef set<TileEntity*>::type						TileEntitySet;
typedef map<int, EntityPlayer*>::type				EntityPlayers;
typedef map<int, EntityCreature*>::type				EntityCreatures;

}


#endif