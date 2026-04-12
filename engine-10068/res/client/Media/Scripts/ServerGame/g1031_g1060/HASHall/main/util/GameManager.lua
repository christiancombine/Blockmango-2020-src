---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
GameManager = {}
GameManager.modeNpcs = {}
GameManager.lotteryNpcs = {}

function GameManager:onTick(ticks)

end

function GameManager:getNpcByEntityId(entityId)
    local npc = self:getModeNpc(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getLotteryNpc(entityId)
    if npc ~= nil then
        return npc
    end
    return nil
end

function GameManager:onAddModeNpc(npc)
    self.modeNpcs[tostring(npc.entityId)] = npc
end

function GameManager:onAddLotteryNpc(npc)
    self.lotteryNpcs[tostring(npc.entityId)] = npc
end

function GameManager:getModeNpc(entityId)
    return self.modeNpcs[tostring(entityId)]
end

function GameManager:getLotteryNpc(entityId)
    return self.lotteryNpcs[tostring(entityId)]
end

return GameManager