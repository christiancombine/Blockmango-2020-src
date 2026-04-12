

CreatureListener = {}

function CreatureListener:init()
    CreatureAttackPlayerEvent.registerCallBack(self.onCreatureAttackPlayer)
end

function CreatureListener.onCreatureAttackPlayer(targetId, monsterEntityId)
    local player = PlayerManager:getPlayerByEntityId(targetId)
    if player == nil then
        return
    end

    MonsterManager:attackPlayer(monsterEntityId, player)
end

return CreatureListener