PartyConfig = {}
PartyConfig.List = {}

function PartyConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id),
            Time = tonumber(config.Time),
            Price = tonumber(config.Price),
            TimeText = tostring(config.TimeText)
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