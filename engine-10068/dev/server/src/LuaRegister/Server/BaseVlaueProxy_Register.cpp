#include "LuaRegister/Template/LuaRegister.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "Script/VarProxy/LogicVarProxy.h"

_BEGIN_REGISTER_CLASS(ShoppingAttr)
_CLASSREGISTER_AddVar(isConsumeCoin, ShoppingAttr::isConsumeCoin)
_CLASSREGISTER_AddVar(isAddGoods, ShoppingAttr::isAddGoods)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ExplosionAttr)
_CLASSREGISTER_AddVar(isBreakBlock, ExplosionAttr::isBreakBlock)
_CLASSREGISTER_AddVar(isCanHurt, ExplosionAttr::isCanHurt)
_CLASSREGISTER_AddVar(isHurtEntityItem, ExplosionAttr::isHurtEntityItem)
_CLASSREGISTER_AddVar(custom_explosionSize, ExplosionAttr::custom_explosionSize)
_CLASSREGISTER_AddVar(custom_explosionDamageFactor, ExplosionAttr::custom_explosionDamageFactor)
_CLASSREGISTER_AddVar(custom_explosionRepelDistanceFactor, ExplosionAttr::custom_explosionRepelDistanceFactor)
_CLASSREGISTER_AddVar(sourceItemId, ExplosionAttr::sourceItemId)
_CLASSREGISTER_AddVar(needSync, ExplosionAttr::needSync)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(CommodityInfo)
_CLASSREGISTER_AddVar(goodsId, CommodityInfo::goodsId)
_CLASSREGISTER_AddVar(goodsNum, CommodityInfo::goodsNum)
_CLASSREGISTER_AddVar(goodsMeta, CommodityInfo::goodsMeta)
_CLASSREGISTER_AddVar(coinId, CommodityInfo::coinId)
_CLASSREGISTER_AddVar(coinNum, CommodityInfo::coinNum)
_END_REGISTER_CLASS()
