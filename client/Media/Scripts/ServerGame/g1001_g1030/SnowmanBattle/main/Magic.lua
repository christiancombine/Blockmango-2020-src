require "Match"
Magic = { }


function Magic:setBlock(blockPos)
    local blockId = EngineWorld:getBlockId(blockPos)
    if blockId == BlockID.AIR then
        EngineWorld:setBlock(blockPos,  GameConfig.flooregg.id)
    end
end

function Magic:buildFloor(yaw, pos)

    yaw = yaw % 360
    local dx, dz = 0, 0
    if yaw <= 22.5 or yaw > 337.5 then
        dz = 1
        dx = 0
    end
    if yaw > 22.5 and yaw <= 67.5 then
        dz = 1
        dx = -1
    end
    if yaw > 67.5 and yaw <= 112.5 then
        dz = 0
        dx = -1
    end
    if yaw > 112.5 and yaw <= 157.5 then
        dz = -1
        dx = -1
    end
    if yaw > 157.5 and yaw <= 202.5 then
        dz = -1
        dx = 0
    end
    if yaw > 202.5 and yaw <= 247.5 then
        dz = -1
        dx = 1
    end
    if yaw > 247.5 and yaw <= 292.5 then
        dz = 0
        dx = 1
    end
    if yaw > 292.5 and yaw <= 337.5 then
        dz = 1
        dx = 1
    end

    local cen= (GameConfig.flooregg.size)/2+1
    local blockPos = VectorUtil.newVector3i(pos.x + dx * cen, pos.y, pos.z + dz * cen)

    local size = (GameConfig.flooregg.size-1)/2
    for x = -size, size do
        for z = -size, size do
            self:setBlock(VectorUtil.newVector3i(blockPos.x + x, blockPos.y, blockPos.z + z))
        end
    end

end
function Magic:buildWall(yaw, pos)

    yaw = yaw % 360
    local dx, dz = 0, 0
    if yaw <= 22.5 or yaw > 337.5 then
        dz = 1
        dx = 0
    end
    if yaw > 22.5 and yaw <= 67.5 then
        dz = 1
        dx = -1
    end
    if yaw > 67.5 and yaw <= 112.5 then
        dz = 0
        dx = -1
    end
    if yaw > 112.5 and yaw <= 157.5 then
        dz = -1
        dx = -1
    end
    if yaw > 157.5 and yaw <= 202.5 then
        dz = -1
        dx = 0
    end
    if yaw > 202.5 and yaw <= 247.5 then
        dz = -1
        dx = 1
    end
    if yaw > 247.5 and yaw <= 292.5 then
        dz = 0
        dx = 1
    end
    if yaw > 292.5 and yaw <= 337.5 then
        dz = 1
        dx = 1
    end

    for i = 1, 10 do
        local blockPos = VectorUtil.newVector3i(pos.x + dx * i, pos.y, pos.z + dz * i)
        local blockId = EngineWorld:getBlockId(blockPos)
        if blockId == BlockID.AIR then
            EngineWorld:setBlock(blockPos, 35)
        end
    end
end
return Magic