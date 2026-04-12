require "util.Tools"
ChestConfig = {}
ChestConfig.chest = {}

function ChestConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.chestName = v.chestName
        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.minPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.maxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.type = tonumber(v.type)
        data.itemId = tonumber(v.itemId)
        data.minNum = tonumber(v.minNum)
        data.maxNum = tonumber(v.maxNum)
        data.refreshCD = tonumber(v.refreshCD)
        data.modleName = v.modleName
        data.modleBody = v.modleBody
        data.modleBodyId = v.modleBodyId
        data.bindld = tonumber(v.bindld)
        data.icon = v.icon
        data.tips = v.tips
        data.birdRation = v.birdRation == "@@@" and 0 or v.birdRation
        data.itemName = v.itemName == "@@@" and "" or v.itemName
        self.chest[data.id] = data
    end
end
 
function ChestConfig:GetChestConfig()
    return self.chest
end

return ChestConfig
