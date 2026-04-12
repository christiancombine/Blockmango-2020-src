---
--- Created by Yang.
--- DateTime: 2018/7/5 15:30
---
LogicListener = {}

function LogicListener:init()
    PlacingConsumeBlockEvent.registerCallBack(self.onPlacingConsumeBlock)
    HitEntityWithCurrentItem.registerCallBack(self.onHitEntityWithCurrentItem)
    EntityDeathDropItemEvent.registerCallBack(self.onEntityDeathDropItem)
end

function LogicListener.onPlacingConsumeBlock(itemId, itemMeta)
    return true
end

function LogicListener.onHitEntityWithCurrentItem(itemId, itemDemage, hitEntityId, fromEntityId)
	return false
end

function LogicListener.onEntityDeathDropItem(entityId, damageType)
	return false
end

return LogicListener