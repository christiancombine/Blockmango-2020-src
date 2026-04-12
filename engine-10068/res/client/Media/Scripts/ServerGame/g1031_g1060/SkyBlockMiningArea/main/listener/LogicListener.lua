---
--- Created by Yang.
--- DateTime: 2018/7/5 15:30
---
require "data.GameSkill"
LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    EntityDeathDropItemEvent.registerCallBack(self.onEntityDeathDropItem)
	UseCustomItemEvent.registerCallBack(self.onUseCustomItem)
    ItemSkillSpHandleEvent.registerCallBack(self.onItemSkillSpHandle)

end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior, discard)
    if behavior == "break.block" then
        return false
    end
    return true
end

function LogicListener.onEntityDeathDropItem(entityId, damageType)
	return false
end

function LogicListener.onUseCustomItem(userId, itemID)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end
    return false
end

function LogicListener.onItemSkillSpHandle(entityId, itemId, position)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return true
    end
    local skill = SkillConfig:getSkill(itemId)
    if skill ~= nil then
        GameSkill.new(skill, player, position)
    end
    return false
end

return LogicListener