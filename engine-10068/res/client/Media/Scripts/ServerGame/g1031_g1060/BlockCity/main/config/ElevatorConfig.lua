ElevatorConfig = {}
ElevatorConfig.elevator = {}

function ElevatorConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.eId = tonumber(v.eId)
        data.floor = tonumber(v.floor)
        data.x1 = math.min(tonumber(v.x1), tonumber(v.x2))
        data.y1 = math.min(tonumber(v.y1), tonumber(v.y2))
        data.z1 = math.min(tonumber(v.z1), tonumber(v.z2))
        data.x2 = math.max(tonumber(v.x1), tonumber(v.x2))
        data.y2 = math.max(tonumber(v.y1), tonumber(v.y2))
        data.z2 = math.max(tonumber(v.z1), tonumber(v.z2))
        data.x3 = tonumber(v.x3)
        data.y3 = tonumber(v.y3)
        data.z3 = tonumber(v.z3)
        data.x4 = tonumber(v.x4)
        data.y4 = tonumber(v.y4)
        data.z4 = tonumber(v.z4)
        data.yaw = tonumber(v.yaw)
        data.speed = tonumber(v.speed)
        data.floorIcon = (v.floorIcon ~= "@@@" and {v.floorIcon} or {""})[1]
        self.elevator[data.id] = data
    end
end

function ElevatorConfig:getElevatorIdByPos(pos)
    for _, v in pairs(self.elevator) do
        if pos.x > v.x1 and pos.z > v.z1 and pos.x < v.x2 and pos.z < v.z2 then
            return v.eId
        end
    end

    return 0
end

function ElevatorConfig:getStartPosByFloorId(floorId, eId)
    for _, v in pairs(self.elevator) do
        if v.floor == floorId and v.eId == eId then
            return VectorUtil.newVector3(v.x3, v.y3, v.z3)
        end
    end

    return nil
end

function ElevatorConfig:getEndPosByFloorId(floorId, eId)
    for _, v in pairs(self.elevator) do
        if v.floor == floorId and v.eId == eId then
            return VectorUtil.newVector3(v.x4, v.y4, v.z4), v.yaw
        end
    end

    return nil, 0
end

function ElevatorConfig:getMaxYByFloorId(floorId, elevatorId)
    for _, v in pairs(self.elevator) do
        if floorId == v.floorId and v.eId == elevatorId then
            return v.y4
        end
    end

    return self.elevator[#self.elevator].y4
end

function ElevatorConfig:getFloorIdByPosY(y)
    if y <= self.elevator[1].y4 then
        return 1
    end

    local maxFloorId = self:getMaxFloorId()
    local maxY = self:getMaxYByFloorId(maxFloorId, 1)
    if y >= maxY then
        return maxFloorId
    end

    local floorId = 1
    for _, v in pairs(self.elevator) do
        if y < v.y4 then
            floorId = v.floor - 1
            return floorId
        end
    end

    return floorId
end

function ElevatorConfig:getMaxFloorId()
    local maxFloorId = 0
    for _, v in pairs(self.elevator) do
        if v.floor >= maxFloorId then
            maxFloorId = v.floor
        end
    end

    return maxFloorId
end

function ElevatorConfig:getSpeedByFloorId(floorId)
    for _, v in pairs(self.elevator) do
        if v.floor == floorId then
            return v.speed
        end
    end

    return 0.1
end

return ElevatorConfig