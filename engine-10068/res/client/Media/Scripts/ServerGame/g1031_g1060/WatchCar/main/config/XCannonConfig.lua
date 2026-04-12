XCannonConfig = {}
XCannonConfig.cannons = {}

function XCannonConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local cannon = {}
        cannon.ID = config.ID
        cannon.ActorName = config.ActorName ~= '#' and config.ActorName or nil
        cannon.ActorBeginPosX = tonumber(config.ActorBeginPosX)
        cannon.ActorBeginPosY = tonumber(config.ActorBeginPosY)
        cannon.ActorBeginPosZ = tonumber(config.ActorBeginPosZ)
        cannon.FallOnPosX = tonumber(config.FallOnPosX)
        cannon.FallOnPosY = tonumber(config.FallOnPosY)
        cannon.FallOnPosZ = tonumber(config.FallOnPosZ)
        table.insert(self.cannons, cannon)
    end
end

function XCannonConfig:prepareCannons()
    for i, v in pairs(self.cannons) do
        local beginPos = VectorUtil.newVector3(v.ActorBeginPosX, v.ActorBeginPosY, v.ActorBeginPosZ)
        local fallOnPos = VectorUtil.newVector3(v.FallOnPosX, v.FallOnPosY, v.FallOnPosZ)
        EngineWorld:getWorld():addActorCannon(beginPos, fallOnPos, v.ActorName, "", "", 5.0)
    end
end

return XCannonConfig