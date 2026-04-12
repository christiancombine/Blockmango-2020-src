RoomConfig = {}

RoomConfig.settings = {}

function RoomConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.roomId = tonumber(vConfig.roomId) or 1
        data.checkPointId = tonumber(vConfig.checkPointId) or 1
        data.pathIds = self:splitToNumber(vConfig.pathIds)
        if self.settings[data.roomId] == nil then
            self.settings[data.roomId] = {}
        end
        self.settings[data.roomId][data.checkPointId] = data
    end
end

function RoomConfig:splitToNumber(str)
    local array = StringUtil.split(str, "#")
    for index, value in pairs(array) do
        array[index] = tonumber(value)
    end
    return array
end

function RoomConfig:getAllRoomConfig()
    return self.settings
end

function RoomConfig:getRoomConfigById(id)
    return self.settings[id]
end

RoomConfig:init(FileUtil.getConfigFromCsv("setting/Room.csv"))