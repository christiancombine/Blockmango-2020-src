---
--- Created by Jimmy.
--- DateTime: 2018/5/14 0014 16:58
---
require "Match"

CreatureListener = {}

function CreatureListener:init()
    CreatureAttackPlayerEvent.registerCallBack(self.onCreatureAttackPlayer)
    CreatureAttackCreatureEvent.registerCallBack(self.onCreatureAttackCreature)
end

function CreatureListener.onCreatureAttackPlayer(targetId, thowerId)
    local player = PlayerManager:getPlayerByEntityId(targetId)
    if player == nil then
        return
    end
    local monster = GameMatch:getCurGameScene():getMonsterByEntityId(thowerId)
    if monster == nil then
        return
    end
    monster:onAttack(player)
end

function CreatureListener.onCreatureAttackCreature(targetId, thowerId)
    local thower = GameMatch:getCurGameScene():getMonsterByEntityId(thowerId)
    if thower == nil then
        return
    end
    local basement = GameMatch:getCurGameScene():getBasementByEntityId(targetId)
    if basement == nil then
        return
    end
    basement:onAttacked(thower)
end

return CreatureListener