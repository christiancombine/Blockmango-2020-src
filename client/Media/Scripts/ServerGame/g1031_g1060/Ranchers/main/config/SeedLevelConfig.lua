SeedLevelConfig = {}
SeedLevelConfig.seeds = {}

function SeedLevelConfig:initSeedLevel(config)
    for i, v in pairs(config) do
        local data = {}
        data.seedId = tonumber(v.seedId)
        data.price = tonumber(v.price)
        data.playerLevel = tonumber(v.playerLevel)
        self.seeds[data.seedId] = data
    end
end

function SeedLevelConfig:getMoneyBySeedId(seedId)
    if self.seeds[seedId] == nil then
        return 0
    end

    return self.seeds[seedId].price
end

return SeedLevelConfig


