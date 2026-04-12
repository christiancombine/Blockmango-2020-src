require "config.Area"

NestConfig = {}
NestConfig.nests = {}
NestConfig.index = {}

function NestConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.areaMinPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.areaMaxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.nestMinPos = Tools.CastToVector3i(v.x3, v.y3, v.z3)
        data.nestMaxPos = Tools.CastToVector3i(v.x4, v.y4, v.z4)
        self.nests[data.id] = data
    end
end

function NestConfig:initIndex(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.vec3 = Tools.CastToVector3i(v.x, v.y, v.z)
        table.insert(self.index, data)
    end

    table.sort(self.index, function(a, b)
        return a.id < b.id
    end)
end

function NestConfig:getNestInfoById(id)
    for i, v in pairs(self.nests) do
        if tonumber(v.id) == tonumber(id) then
            return v
        end
    end

    return nil
end

function NestConfig:getNestAreaPos(id)
    if self.nests[id] == nil then
        return nil
    end

    local pos = {}
    pos[1] = {}
    pos[1][1] = self.nests[id].areaMinPos.x
    pos[1][2] = self.nests[id].areaMinPos.y
    pos[1][3] = self.nests[id].areaMinPos.z

    pos[2] = {}
    pos[2][1] = self.nests[id].areaMaxPos.x
    pos[2][2] = self.nests[id].areaMaxPos.y
    pos[2][3] = self.nests[id].areaMaxPos.z

    return Area.new(pos)
end

function NestConfig:enterNestArea(player)
    if player.userNest == nil then
        return false
    end

    local nestAreaPos = self:getNestAreaPos(player.userNest.nestIndex)
    if nestAreaPos == nil then
        return false
    end

    if nestAreaPos:inArea(player:getPosition()) then
        if player.isInNestArea == false then
            player.isInNestArea = true
        end
    else
        if player.isInNestArea == true then
            player.isInNestArea = false
            player:setConvert(false)
            player:setBirdSimulatorPlayerInfo()
            if player.userBird ~= nil then
                player.userBird:setInterruptState()
            end
        end
    end
end

function NestConfig:getNestPosByIndex(nestIndex, curCarryMaxNum, isVec3)
    if self.nests[nestIndex] == nil then
        return nil
    end

    if self.index[curCarryMaxNum] == nil then
        return nil
    end

    local vec3 = self.index[curCarryMaxNum].vec3
    local x = self.nests[nestIndex].nestMinPos.x + vec3.x
    local y = self.nests[nestIndex].nestMinPos.y + vec3.y
    local z = self.nests[nestIndex].nestMinPos.z + vec3.z

    if isVec3 then
        return VectorUtil.newVector3(x, y, z)
    end

    return VectorUtil.newVector3i(x, y, z)
end

function NestConfig:getNestIndexByVec3(nestIndex, vec3)
    if self.nests[nestIndex] == nil then
        return 0
    end

    for _, v in pairs(self.index) do
        local x = self.nests[nestIndex].nestMinPos.x + v.vec3.x
        local y = self.nests[nestIndex].nestMinPos.y + v.vec3.y
        local z = self.nests[nestIndex].nestMinPos.z + v.vec3.z

        if x == vec3.x and y == vec3.y and z == vec3.z then
            return v.id
        end
    end

    return 0
end

return NestConfig