SupplyAreaConfig = {}

function SupplyAreaConfig:initArea(areaConfig)
    for _, areaData in pairs(areaConfig) do
        local area = {}
        area.areaId = tonumber(areaData.id) or 0
        area.supplyPool = tonumber(areaData.supplyPool) or 0
        area.totalNum = tonumber(areaData.totalNum) or 0
        area.refreshInterval = tonumber(areaData.defaultRefreshInterval) or 0
        SupplyManager:setRefreshSetting(area)
    end
end

function SupplyAreaConfig:initPosition(areaConfig)
    for _, config in pairs(areaConfig) do
        local blockPos = VectorUtil.newVector3(config.x, config.y, config.z)
        local area = {
            areaId = tonumber(config.areaId) or 0,
            pos = VectorUtil.add3(blockPos, VectorUtil.newVector3(0.5, 0, 0.5))
        }
        SupplyManager:setSupplyPosition(area.areaId, area.pos)
    end
end

return SupplyAreaConfig