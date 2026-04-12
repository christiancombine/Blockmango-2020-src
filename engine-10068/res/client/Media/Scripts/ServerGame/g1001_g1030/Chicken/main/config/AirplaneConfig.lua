require "config.Area"

AirplaneConfig = {}
AirplaneConfig.area = {}
AirplaneConfig.airplane = {}

function AirplaneConfig:init(config)
    self:initArea(config.area)
    self:initAirplane(config.airplane)
end

function AirplaneConfig:initArea(area)
    self.area.startPoint = Area.new(area.startPoint)
    self.area.centerPoint = Area.new(area.centerPoint)
    self.area.landingPoint = Area.new(area.landingPoint)
end

function AirplaneConfig:initAirplane(airplane)
    local item = {}
    for i, v in pairs(airplane) do
        item.id = tonumber(v.id)
        item.speed = tonumber(v.speed)
        self.airplane[v.id] = item
    end
end

function AirplaneConfig:randomCenterPoint(centerPoint)
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local x = math.random(centerPoint.minX, centerPoint.maxX)
    local y = math.random(centerPoint.minY, centerPoint.maxY)
    local z = math.random(centerPoint.minZ, centerPoint.maxZ)
    return VectorUtil.newVector3(x, y, z)
end

function AirplaneConfig:randomStartPoint(startPoint)
    local x = 0
    local y = 0
    local z = 0

    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local randomNum = math.random(1, 100)
    if randomNum % 2 == 0 then
        x = math.random(startPoint.minX, startPoint.maxX)
        math.randomseed(os.clock()*1000000)
        randomNum = math.random(1, 100)
        if randomNum % 2 == 0 then
            y = startPoint.minY
            z = startPoint.minZ
        else
            y = startPoint.maxY
            z = startPoint.maxZ
        end
    else
        z = math.random(startPoint.minZ, startPoint.maxZ)
        math.randomseed(os.clock()*1000000)
        randomNum = math.random(1, 100)
        if randomNum % 2 == 0 then
            x = startPoint.minX
            y = startPoint.minY
        else
            x = startPoint.maxX
            y = startPoint.maxY
        end
    end
    return VectorUtil.newVector3(x, y, z)
end

function AirplaneConfig:getEndPoint(startPoint, centerPoint)
    local x = 0
    local y = 0
    local z = 0

    local minX = self.area.startPoint.minX
    local maxX = self.area.startPoint.maxX
    local minY = self.area.startPoint.minY
    local maxY = self.area.startPoint.maxY
    local minZ = self.area.startPoint.minZ
    local maxZ = self.area.startPoint.maxZ

    if centerPoint.x == startPoint.x or centerPoint.z == startPoint.z then
        if centerPoint.x == startPoint.x then
            x = centerPoint.x
            y = maxY
            z = maxZ
        end

        if centerPoint.z == startPoint.z then
            x = maxX
            y = maxY
            z = centerPoint.z
        end
    else
        local k = (centerPoint.z - startPoint.z) / (centerPoint.x - startPoint.x)
        local b = centerPoint.z - (k * centerPoint.x)

        local x1 = 0
        local z1 = 0

        x1 = minX
        z1 = k * x1 + b
        if x1 ~= startPoint.x and z1 >= minZ and z1 <= maxZ then
            x = x1
            y = maxY
            z = z1
        end

        x1 = maxX
        z1 = k *x1 + b
        if x1 ~= startPoint.x and z1 >= minZ and z1 <= maxZ then
            x = x1
            y = maxY
            z = z1
        end

        z1 = minZ
        x1 = (z1 - b) / k
        if z1 ~= startPoint.z and x1 >= minX and x1 <= maxX then
            x = x1
            y = maxY
            z = z1
        end

        z1 = maxZ
        x1 = (z1 - b) / k
        if z1 ~= startPoint.z and x1 >= minX and x1 <= maxX then
            x = x1
            y = maxY
            z = z1
        end
    end
    return VectorUtil.newVector3(math.ceil(x), math.ceil(y), math.ceil(z))
end

function AirplaneConfig:inLandingArea(vec3)
    return self.area.landingPoint:inArea(vec3)
end


function AirplaneConfig:getAirplaneById(id)
    return self.airplane[id]
end

return AirplaneConfig