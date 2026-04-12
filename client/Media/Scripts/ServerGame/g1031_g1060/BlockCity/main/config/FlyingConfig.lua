FlyingConfig = {}
FlyingConfig.flying = {}

function FlyingConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.time = tonumber(v.time)
        data.day = tonumber(v.day)
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.flyMsg = v.flyMsg
        self.flying[data.id] = data
    end
end

function FlyingConfig:getFlyingDataById(id)
    if self.flying[id] then
        return self.flying[id]
    end

    return nil
end

function FlyingConfig:getFlyingInfo()
    local items = {}
    for _, v in pairs(self.flying) do
        local data = BlockCityFlyingInfo.new()
        data.id = v.id
        data.day = v.day
        data.moneyType = v.moneyType
        data.price = v.price
        data.flyMsg = v.flyMsg
        table.insert(items, data)
    end


    return items
end

return FlyingConfig