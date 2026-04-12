---
--- Created by Jimmy.
--- DateTime: 2018/3/6 0006 11:30
---
RoomConfig = {}
RoomConfig.room = {}
RoomConfig.roomSize = 0

function RoomConfig:init(config)
    self:initSpaceStart(config.spaceStart)
    self:initSpaceEnd(config.spaceEnd)
    self:initRoom(config.roomInfo)
    self:initFindBlock(config.findblockIdlist)
    self.boxNum = tonumber(config.boxCount)
end

function RoomConfig:initSpaceStart(config)
    self.spaceStart = config
end

function RoomConfig:initSpaceEnd(config)
    self.spaceEnd = config
end

function RoomConfig:initRoom(config)
    local target_config = config

    if target_config ~= nil then
        for i, v in pairs(target_config) do
            local room = {}
            room.id = v.id
            room.startPos = v.startPos
            room.size = v.size
            room.type = v.type
            room.jumpblock = v.jumpblock
            room.pressure = v.pressure
            room.chest = v.chest
            room.file = tostring(v.file)
            room.range = v.range
            room.items = v.items
            room.findblockId = v.findblockId
            self.room[room.id] = room
            self.roomSize = self.roomSize + 1
        end
    end
end

function RoomConfig:initFindBlock(config)
    self.findblockIdlist = config
end

function RoomConfig:getRoomSize()
    return self.roomSize
end

function RoomConfig:getRoomId(vec3i)
    for k, v in pairs(self.room) do
        local start_x = tonumber(v.startPos[1])
        local start_y = tonumber(v.startPos[2])
        local start_z = tonumber(v.startPos[3])
        local end_x = start_x + tonumber(v.size[1])
        local end_y = start_y + tonumber(v.size[2])
        local end_z = start_z + tonumber(v.size[3])

        if vec3i.x >= start_x 
            and vec3i.x <= end_x
            and vec3i.y >= start_y
            and vec3i.y <= end_y
            and vec3i.z >= start_z
            and vec3i.z <= end_z then

            return v.id
        end
    end

    return 0
end

function RoomConfig:getChest(chest, vec3i)
    if chest then
        for k, v in pairs(chest) do
            if tonumber(v.pos[1]) == vec3i.x and tonumber(v.pos[2]) == vec3i.y and tonumber(v.pos[3]) == vec3i.z then
                return v
            end
        end
    end
    return nil
end

function RoomConfig:getRoomFindBlockId(roomId)
    for k, v in pairs(self.room) do
        if v.id == roomId then
            return tonumber(v.findblockId)
        end
    end

    return 0
end

function RoomConfig:isFindBlockId(blockId)
    for k, v in pairs(self.findblockIdlist) do
        if tonumber(v) == blockId then
            return true
        end
    end

    return false
end

function RoomConfig:isArea(vec3)
    if vec3.y > tonumber(self.spaceStart[2])
        and vec3.y <= tonumber(self.spaceEnd[2] + 1)
        and vec3.x > tonumber(self.spaceStart[1])
        and vec3.x < tonumber(self.spaceEnd[1])
        and vec3.z > tonumber(self.spaceStart[3])
        and vec3.z < tonumber(self.spaceEnd[3]) then
        return true
    end

    return false
end

function RoomConfig:randomItem(subconfig, numMin, numMax)

    -- 随机出该类型  Item 可以选出的种类数
    local number = 0
    number = math.random(numMax - numMin + 1)
    number = numMin + number

    -- 计算概率范围
    local total = 0
    for i, v in pairs(subconfig) do
        v.choose = false
        v.probfloor = total

        total = total + v.probability
        v.probUpper = total
    end

    local choose = 1
    local rsItems = {}
    while (choose < number) do
        local rs = math.random(0, total)

        for i, v in pairs(subconfig) do
            if (rs >= v.probfloor and rs < v.probUpper and v.choose == false) then

                local num = self:randomItemNum(v)

                rsItems[choose] = {}
                rsItems[choose].id = v.id
                rsItems[choose].num = num

                v.choose = true
                choose = choose + 1
                break
            end
        end
    end

    return rsItems
end

function RoomConfig:randomItemNum(item)
    local rs = 0
    local range = item.randomRange[2] - item.randomRange[1]
    if (range ~= 0) then
        rs = math.random(0, range)
    end

    rs = item.randomRange[1] + rs

    return rs
end

return RoomConfig