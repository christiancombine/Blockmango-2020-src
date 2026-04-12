require "skill.BaseSkill"

WaterBallSkill = class("WaterBallSkill", BaseSkill)

function WaterBallSkill:createSkillEffect(name, time)
    local originalPos = VectorUtil.toBlockVector3(self.initPos.x, self.initPos.y + 0.5, self.initPos.z)
    local player = self:getCreator()
    local blockId = EngineWorld:getBlockId(originalPos)
    if blockId ~= BlockID.AIR then
        local yaw = player:getYaw()
        yaw = yaw % 360
        local dx, dz = 0, 0
        if yaw <= 22.5 or yaw > 337.5 then
            dz = -1
            dx = 0
        end
        if yaw > 22.5 and yaw <= 67.5 then
            dz = -1
            dx = 1
        end
        if yaw > 67.5 and yaw <= 112.5 then
            dz = 0
            dx = 1
        end
        if yaw > 112.5 and yaw <= 157.5 then
            dz = 1
            dx = 1
        end
        if yaw > 157.5 and yaw <= 202.5 then
            dz = 1
            dx = 0
        end
        if yaw > 202.5 and yaw <= 247.5 then
            dz = 1
            dx = -1
        end
        if yaw > 247.5 and yaw <= 292.5 then
            dz = 0
            dx = -1
        end
        if yaw > 292.5 and yaw <= 337.5 then
            dz = -1
            dx = -1
        end
        local newPos = VectorUtil.newVector3i(originalPos.x + dx, originalPos.y, originalPos.z + dz)
        local distance = 1
        while EngineWorld:getBlockId(newPos) ~= BlockID.AIR do
            newPos = VectorUtil.newVector3i(originalPos.x, originalPos.y - distance, originalPos.z)
            if EngineWorld:getBlockId(newPos) == BlockID.AIR then
                break
            end
            newPos = VectorUtil.newVector3i(originalPos.x, originalPos.y + distance, originalPos.z)
            distance = distance + 1
        end
        EngineWorld:setBlock(newPos, BlockID.WATER_MOVING)
    else
        EngineWorld:setBlock(originalPos, BlockID.WATER_MOVING)
    end
end

return WaterBallSkill