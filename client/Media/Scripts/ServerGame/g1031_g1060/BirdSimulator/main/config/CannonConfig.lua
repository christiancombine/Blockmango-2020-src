CannonConfig = {}
CannonConfig.cannons = {}

function CannonConfig:init(configs)
    for _, config in pairs(configs) do
        local cannon = {}
        cannon.id = tonumber(config.id)
        cannon.entityId = 0
        cannon.actor = config.actor
        cannon.bodyId = config.bodyId
        cannon.body = config.body
        cannon.x = tonumber(config.x)
        cannon.y = tonumber(config.y)
        cannon.z = tonumber(config.z)
        cannon.x1 = tonumber(config.x1)
        cannon.y1 = tonumber(config.y1)
        cannon.z1 = tonumber(config.z1)
        cannon.level = tonumber(config.level)
        cannon.tipId = tonumber(config.tipId)
        cannon.addHeight = tonumber(config.addHeight)
        table.insert(self.cannons, cannon)
    end
end

function CannonConfig:prepareCannons()
    for i, v in pairs(self.cannons) do
        local beginPos = VectorUtil.newVector3(v.x, v.y, v.z)
        local fallOnPos = VectorUtil.newVector3(v.x1, v.y1, v.z1)
        v.entityId = EngineWorld:getWorld():addActorCannon(beginPos, fallOnPos, v.actor, v.body, v.bodyId, v.addHeight)
    end
end

return CannonConfig