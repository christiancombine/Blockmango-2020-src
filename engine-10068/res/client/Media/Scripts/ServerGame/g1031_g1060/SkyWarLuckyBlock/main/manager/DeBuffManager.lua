require "deBuff.BleedDeBuff"
require "deBuff.DisarmDeBuff"

DeBuffManager = {}
DeBuffManager.DeBuffType = {
    Disarm = "6",
    Bleed = "7"
}

DeBuffManager.DeBuffMapping = {
    [DeBuffManager.DeBuffType.Disarm] = DisarmDeBuff,
    [DeBuffManager.DeBuffType.Bleed] = BleedDeBuff
}

DeBuffManager.DeBuffs = {}

function DeBuffManager:createDeBuff(player, target, config)
    local cache = self.DeBuffs[tostring(target.userId) .. "#" .. config.id]
    if cache ~= nil then
        cache:addDeBuffTime(player)
        return
    end
    local deBuff = DeBuffManager.DeBuffMapping[config.type]
    if deBuff ~= nil then
        deBuff.new(player, target, config)
    end
end

function DeBuffManager:addDeBuff(deBuff)
    local uniqueId = tostring(deBuff.targetUid) .. "#" .. deBuff.id
    deBuff.uniqueId = uniqueId
    self.DeBuffs[uniqueId] = deBuff
end

function DeBuffManager:onTick(tick)
    for uniqueId, deBuff in pairs(self.DeBuffs) do
        deBuff:onTick(tick)
    end
end

function DeBuffManager:removeDeBuff(deBuff)
    if deBuff.uniqueId then
        self.DeBuffs[deBuff.uniqueId] = nil
    end
end

function DeBuffManager:clearDeBuff()
    for i, deBuff in pairs(self.DeBuffs) do
        deBuff = nil
    end
    self.DeBuffs = {}
end

return DeBuffManager