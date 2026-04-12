BuffConfig = {}
local buffConfigs = {}

function BuffConfig:init(buffs)
    self:initBuffs(buffs)
end

function BuffConfig:initBuffs(buffs)
    for id,buff in pairs(buffs) do
        local item = {}
        item.id = tonumber(id) or 0
        item.type = buff.type or ""
        item.buffName = buff.buffName or ""
        item.buffIcon = buff.buffIcon or ""
        item.buffEffect = buff.buffEffect or ""
        item.value = tonumber(buff.value) or 0
        item.limit = tonumber(buff.limit) or 0
        item.cdTime = tonumber(buff.cdTime) or 0
        item.duration = tonumber(buff.duration) or 0
        item.overlayRule = tonumber(buff.overlayRule) or 0
        buffConfigs[item.id] = item
    end
end

function BuffConfig:getBuffById(id)
    return buffConfigs[tonumber(id)]
end

function BuffConfig:getAllBuffId()
    local allBuffId = {}
    for _,buffConfig in pairs(buffConfigs) do
        table.insert(allBuffId,buffConfig.id )
    end
    return allBuffId
end
