---GemStoneConfig

require "data.GemStoneActor"

GemStoneConfig = {}
GemStoneConfig.GemStoneActorMap = {}

function GemStoneConfig:initGemStone(pos, yaw, distance)
    if pos.x == nil or pos.y == nil or pos.z == nil  or yaw == nil or distance == nil then
        return
    end
    local pos = self:initPosByYaw(pos, yaw, distance)
    HostApi.log("initGemStone yaw : "..yaw.."  x : "..pos.x.." y : "..pos.y.." z : "..pos.z)
    table.insert(self.GemStoneActorMap, GemStoneActor.new(pos))
end

function GemStoneConfig:inGemStoneArea(player, pos)
    if next(self.GemStoneActorMap) == nil then
        return
    end
    for i,v in pairs(self.GemStoneActorMap) do
        if v:onPlayerMove(player, pos) then
            table.remove(self.GemStoneActorMap, i)
            return
        end
    end
end

function GemStoneConfig:initPosByYaw(pos, yaw, distance)
    if distance < 1 then
        return pos
    end
    local dis = 1
    local dyaw = yaw % 360
    local dx = math.sin(math.rad(dyaw + 180))
    local dz = math.cos(math.rad(dyaw))
    local posX = pos.x + dx
    local posZ = pos.z + dz
    local blockPos = VectorUtil.toBlockVector3(posX, pos.y, posZ)
    local blockId = EngineWorld:getBlockId(blockPos)
    while blockId == BlockID.AIR and dis < distance do
        dis = dis + 1
        blockPos = VectorUtil.toBlockVector3(posX + dx, pos.y, posZ + dz)
        blockId = EngineWorld:getBlockId(blockPos)
    end
    return VectorUtil.newVector3(blockPos.x + 0.5, pos.y, blockPos.z + 0.5)
end

return GemStoneConfig
