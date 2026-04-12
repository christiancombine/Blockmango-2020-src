---
--- Created by Jimmy.
--- DateTime: 2018/3/9 0009 11:29
---
require "data.GameSkill"

LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    ItemSkillAttackEvent.registerCallBack(self.onItemSkillAttack)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

function LogicListener.onItemSkillAttack(entityId, itemId, position)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return
    end
    local skill = SkillConfig:getSkill(itemId, player.occupationId)
    if skill ~= nil then
        GameSkill.new(skill, player, position)
    end
end


return LogicListener