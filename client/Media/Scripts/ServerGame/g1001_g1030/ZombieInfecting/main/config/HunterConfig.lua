
HunterConfig = {}

HunterConfig.hp = 0
HunterConfig.name = ""
HunterConfig.actor = {}
HunterConfig.inventory = {}
HunterConfig.gameTipTime = 0
HunterConfig.transformTime = 0

function HunterConfig:init(config)
    self:initHp(config.hp)
    self:initName(config.name)
    self:initActor(config.actor)
    self:initInventory(config.inventory)
    self:initGameTipTime(config.gameTipTime)
    self:initTransformTime(config.transformTime)
end

function HunterConfig:initHp(config)
    self.hp = tonumber(config)
end

function HunterConfig:initName(config)
    self.name = config
end
function HunterConfig:initActor(config)
    for i, v in pairs(config) do
        local actor = {}
        actor.first = v.first
        actor.second = tonumber(v.second)
        self.actor[i] = actor
    end
end

function HunterConfig:initInventory(config)
    for i, v in pairs(config) do
        local inventory = {}
        inventory.id = tonumber(v.id)
        inventory.num = tonumber(v.num)
        self.inventory[i] = inventory
    end
end

function HunterConfig:initGameTipTime(config)
    self.gameTipTime = tonumber(config)
end

function HunterConfig:initTransformTime(config)
    self.transformTime = tonumber(config)
end

return HunterConfig