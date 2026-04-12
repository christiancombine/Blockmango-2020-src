require "util.Tools"
require "config.StoreHouseItems"
require "manager.AreaInfoManager"

StoreHouseConfig = {}

function StoreHouseConfig:NewStoreTable(config)
    local storeTable = {
        minPos = Tools.CastToVector3i(config.x1, config.y1, config.z1),
        maxPos = Tools.CastToVector3i(config.x2, config.y2, config.z2),
        storeId = tonumber(config.StoreId)
    }
    AreaInfoManager:push(storeTable.storeId, AreaType.House, storeTable.minPos, storeTable.maxPos)
    storeTable.itemInfos = {}
    return storeTable
end

function StoreHouseConfig:Init(configs)
    self.storeHouseConfigs = {}
    for _, config in pairs(configs) do
        local tab = self:NewStoreTable(config)
        table.insert(self.storeHouseConfigs, tab)
    end
end

function StoreHouseConfig:GetAllConfigs()
    return self.storeHouseConfigs
end
