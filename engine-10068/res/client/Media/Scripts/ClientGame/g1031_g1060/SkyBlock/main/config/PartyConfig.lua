PartyConfig = {}
PartyConfig.List = {}

function PartyConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 1,
            Time = tonumber(config.Time) or 0,
            Price = tonumber(config.Price) or 0,
            TimeText = tostring(config.TimeText) or ""
        }
        self.List[item.Id] = item
    end
end

function PartyConfig:getPartyPriceData()
    return self.List or {}
end

function PartyConfig:getPriceDataById(id)
    return self.List[id]
end

return PartyConfig