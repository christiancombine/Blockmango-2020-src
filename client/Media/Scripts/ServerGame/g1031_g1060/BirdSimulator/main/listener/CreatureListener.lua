CreatureListener = {}

function CreatureListener:init()
    CreatureAttackPlayerEvent.registerCallBack(self.onCreatureAttackPlayer)
end

function CreatureListener.onCreatureAttackPlayer(targetId, monsterEntityId)
    local player = PlayerManager:getPlayerByEntityId(targetId)
    if player == nil then
        return
    end

    local players = PlayerManager:getPlayers()
    for _, p in pairs(players) do
        if p.userMonster ~= nil then
            p.userMonster:attackPlayer(monsterEntityId, player)
        end
    end
end

return CreatureListener