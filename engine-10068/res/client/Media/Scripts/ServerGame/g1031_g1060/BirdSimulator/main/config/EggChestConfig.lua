require "util.Tools"
EggChestConfig = {}
EggChestConfig.eggChest = {}

function EggChestConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.modleName = v.modleName
        data.modleBody = v.modleBody
        data.modleBodyId = v.modleBodyId
        data.eggChestName = v.eggChestName
        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.minPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.maxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.refreshCD = tonumber(v.refreshCD)
        data.tips = v.tips
        data.pools = Tools.SplitNumber(v.pools, "#")
        self.eggChest[data.id] = data
    end
end
 
function EggChestConfig:GetEggChestConfig()
    return self.eggChest
end

return EggChestConfig
