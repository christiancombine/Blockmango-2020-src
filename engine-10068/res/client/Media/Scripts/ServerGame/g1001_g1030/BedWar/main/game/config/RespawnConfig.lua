---
--- Created by Jimmy.
--- DateTime: 2018/3/3 0003 14:47
---

RespawnConfig = {}
RespawnConfig.respawnGoods = {}

local RespawnSetting = {}
local KeepItemSetting = {}

local function init()
    local config = FileUtil.getConfigFromCsv("BuyRespawn.csv")
    for _, item in pairs(config) do
        table.insert(RespawnSetting, {
            Id = tonumber(item.Id),
            Price = tonumber(item.Price),
            Time = tonumber(item.Time)
        })
    end
    config = FileUtil.getConfigFromCsv("BuyKeepItem.csv")
    for _, item in pairs(config) do
        table.insert(KeepItemSetting, {
            Id = tonumber(item.Id),
            Price = tonumber(item.Price),
            Time = tonumber(item.Time)
        })
    end
    table.sort(RespawnSetting, function(a, b)
        return a.Id < b.Id
    end)
    table.sort(KeepItemSetting, function(a, b)
        return a.Id < b.Id
    end)
end

init()

function RespawnConfig:getRespawnSetting(index)
    return RespawnSetting[index]
end

function RespawnConfig:getKeepItemSetting(index)
    return KeepItemSetting[index] or KeepItemSetting[#KeepItemSetting]
end