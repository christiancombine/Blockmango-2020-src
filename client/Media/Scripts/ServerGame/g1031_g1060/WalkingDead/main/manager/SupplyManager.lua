SupplyManager = {}
SupplyManager.needRefreshArea = {}
SupplyManager.time = os.time()
SupplyManager.SupplyPosition = {}
SupplyManager.SupplyArea = {}
SupplyManager.SupplyPool = {}

function SupplyManager:onTick()
    self:refreshSupply()   --刷新箱子
end

function SupplyManager:addSettingToArea(config)
    local areaTab = self.SupplyArea[config.areaId]
    if not areaTab then
        areaTab = SupplyArea.new(config.areaId)
        self.SupplyArea[config.areaId] = areaTab
    end
    areaTab:addAreaSetting(config)
end

function SupplyManager:addPositionToArea(areaId, pos)
    if self.SupplyArea[areaId] ~= nil then
        self.SupplyArea[areaId]:addPositionToRefreshTable(pos)
    end
end

function SupplyManager:addItemToPool(itemConfig)
    local pool = self.SupplyPool[itemConfig.supplyPool]
    if not pool then
        pool = SupplyPool.new(itemConfig.supplyPool)
        self.SupplyPool[itemConfig.supplyPool] = pool
    end
    pool:addItem(itemConfig)
end

function SupplyManager:getSupplyPoolById(supplyPoolId)
    return self.SupplyPool[supplyPoolId]
end

function SupplyManager:setSupplyPosition(areaId, position)
    local StringPos = tostring(position.x) .. tostring(position.y) .. tostring(position.z)
    self.SupplyPosition[StringPos] = areaId
    SupplyManager:addPositionToArea(areaId, position)
end

function SupplyManager:setRefreshSetting(area)
    self.needRefreshArea[area.areaId] = {
        areaId = area.areaId,
        RefreshInterval = area.refreshInterval,
        RefreshNum = area.totalNum
    }
    SupplyManager:addSettingToArea(area)
end

function SupplyManager:getSupplyModel(supplyId)
    return SupplyPoolConfig.SupplySetting[supplyId].model
end

function SupplyManager:onOpenSupply(position)
    local StringPos = tostring(position.x) .. tostring(position.y) .. tostring(position.z)
    local areaId = self.SupplyPosition[StringPos]
    self.needRefreshArea[areaId].RefreshNum = self.needRefreshArea[areaId].RefreshNum + 1
end

function SupplyManager:refreshSupply()
    for _, area in pairs(self.needRefreshArea) do
        if self.SupplyArea[area.areaId] ~= nil and (self.time - os.time()) % area.RefreshInterval == 0 then
            local failNum = self.SupplyArea[area.areaId]:refreshAreaSupply(area.RefreshNum)
            self.needRefreshArea[area.areaId].RefreshNum = failNum
        end
    end
end

function SupplyManager:initRefreshSupply()
    for _, area in pairs(self.needRefreshArea) do
        if self.SupplyArea[area.areaId] ~= nil then
            local failNum = self.SupplyArea[area.areaId]:refreshAreaSupply(area.RefreshNum)
            self.needRefreshArea[area.areaId].RefreshNum = failNum
        end
    end
end

return SupplyManager