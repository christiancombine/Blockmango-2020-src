---
--- Created by Jimmy.
--- DateTime: 2018/5/14 0014 16:58
---
require "util.GameManager"
require "Match"

CreatureListener = {}

function CreatureListener:init()
    CreatureAttackPlayerEvent.registerCallBack(self.onCreatureAttackPlayer)
end

function CreatureListener.onCreatureAttackPlayer(targetId, entityId)
    local player = PlayerManager:getPlayerByEntityId(targetId)
    if player == nil then
        return
    end
    local attacker = GameManager:getAttackNpc(entityId)
    if attacker == nil then
        return
    end
    attacker:onAttack(player)
end

return CreatureListener