SupplyArea = class()

function SupplyArea:ctor(areaId)
    self.areaId = areaId
    self.areaTab = {}
end

function SupplyArea:addAreaSetting(config)
    self.areaTab = {
        areaId = config.areaId,
        supplyPool = config.supplyPool,
        totalNum = config.totalNum,
        defaultRefreshInterval = config.defaultRefreshInterval,
        pos = {}
    }
end

function SupplyArea:addPositionToRefreshTable(position)
    table.insert(self.areaTab.pos, position)
end

function SupplyArea:refreshAreaSupply(num)
    local failRefresh = num
    local maxPosNum = #self.areaTab.pos
    while (num > 0) do
        local SupplyIndex = MathUtil:randomIntNoRepeat(1, maxPosNum, maxPosNum)
        for _, index in pairs(SupplyIndex) do
            local pool = SupplyManager:getSupplyPoolById(self.areaTab.supplyPool)
            if pool ~= nil then
                local item = pool:randomItemFromPool()
                local pos = self.areaTab.pos[index]
                if pos ~= nil and item ~= nil and self:createSupply(pos, item) then
                    failRefresh = failRefresh - 1
                    break
                end
            end
        end
        num = num - 1
    end
    return failRefresh
end

function SupplyArea:createSupply(pos, item)
    local model = SupplyManager:getSupplyModel(item.supplyId)
    if model ~= nil then
        return ActorManager:createSupplyActor(pos, item.itemId, item.number, 0, model)
    end
    return false
end

return SupplyArea