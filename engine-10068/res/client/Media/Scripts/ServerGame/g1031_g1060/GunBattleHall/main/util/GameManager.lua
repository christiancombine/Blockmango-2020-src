---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:44
---
GameManager = {}
GameManager.modenpcs = {}
GameManager.exgunnpcs = {}

function GameManager:onTick(ticks)
    for _, npc in pairs(self.modenpcs) do
        npc:onTick(ticks)
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onTick()
    end
end

function GameManager:getNpcByEntityId(entityId)
    local npc = self:getModeNpc(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getExGunNpc(entityId)
    if npc ~= nil then
        return npc
    end
    return nil
end

function GameManager:onAddModeNpc(npc)
    self.modenpcs[tostring(npc.entityId)] = npc
end

function GameManager:onAddExGunNpc(npc)
    self.exgunnpcs[tostring(npc.entityId)] = npc
end

function GameManager:getModeNpc(entityId)
    return self.modenpcs[tostring(entityId)]
end

function GameManager:getExGunNpc(entityId)
    return self.exgunnpcs[tostring(entityId)]
end

function GameManager:getModeNpcByGameType(gameType)
    for _, npc in pairs(self.modenpcs) do
        if npc.gameType == gameType then
            return npc
        end
    end
end

return GameManager