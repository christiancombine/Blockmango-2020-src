require "data.SupplyPoint"

SupplyConfig = {}
SupplyConfig.SupplySetting = {}
SupplyConfig.SupplyConfig = {}
SupplyConfig.SumOfWeight = 0

function SupplyConfig:initSupplySetting(configs)
    for id, config in pairs(configs) do
        local item = {}
        item.id = config.Id
        item.type = tonumber(config.Type)
        item.model = config.Model
        item.value = tonumber(config.Value)
        if config.Effect == "#" then
            item.effect = ""
        else
            item.effect = config.Effect or ""
        end
        item.playerEffect = config.PlayerEffect
        item.playerEffectTime = tonumber(config.PlayerEffectTime)
        item.weight = tonumber(config.Weight) or 0
        item.image = config.Image or ""
        item.time = tonumber(config.Time) or 0
        self.SupplySetting[id] = item
        self.SumOfWeight = self.SumOfWeight + item.weight
    end
end

function SupplyConfig:initSupply(configs)
    for id, config in pairs(configs) do
        local item = {}
        item.id = config.Id
        local pos = StringUtil.split(config.Position, "#")
        item.position = VectorUtil.newVector3(pos[1], pos[2], pos[3])
        item.supplyIds = StringUtil.split(config.SupplyIds, "#")
        item.refreshInterval = tonumber(config.RefreshInterval) or 10
        table.insert(self.SupplyConfig, item)
    end
end

function SupplyConfig:getSupplySettingById(id)
    return self.SupplySetting[id]
end

function SupplyConfig:randomSupply()
    local random = math.random(1, #self.SupplyConfig)
    return self.SupplyConfig[random]
end

function SupplyConfig:getMaxSupplyCount()
    return math.min(#self.SupplyConfig, GameConfig.maxSupplyNum)
end

function SupplyConfig:randomDieSupply()
    local rs = math.random(0, self.SumOfWeight)
    for id, supply in pairs(self.SupplySetting) do
        rs = rs - supply.weight
        if rs < 0 then
            local item = {
                id = os.time() + supply.id,
                refreshInterval = 1200,
                position = VectorUtil.newVector3(0, 0, 0),
                supplyIds = {supply.id}
            }
            return item
        end
    end
end