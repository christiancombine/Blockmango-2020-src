CheckInConfig = {}
CheckInConfig.data = {}

function CheckInConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.day = tonumber(v.day)
        data.rewardId1 = tonumber(v.rewardId1)
        data.type1 = tonumber(v.type1)
        data.num1 = tonumber(v.num1)
        data.icon1 = v.icon1
        data.name1 = v.name1
        data.rewardId2 = tonumber(v.rewardId2)
        data.type2 = tonumber(v.type2)
        data.num2 = tonumber(v.num2)
        data.icon2 = v.icon2
        data.name2 = v.name2
        self.data[data.day] = data
    end
end

--  about data.state :
--      1 -> can not checkIn yet
--      2 -> can checkIn now
--      3 -> already checkIn
function CheckInConfig:getRewardData1()
    local items = {}
    for i, v in pairs(self.data) do
        local data = {}
        data.id = v.day
        data.name = v.name1
        data.icon = v.icon1
        data.itemId = v.rewardId1
        data.num = v.num1
        data.type = v.type1
        data.state = 1
        items[data.id] = data
    end

    return items
end

--  about data.state :
--      1 -> can not checkIn yet
--      2 -> can checkIn now
--      3 -> already checkIn
function CheckInConfig:getRewardData2()
    local items = {}
    for i, v in pairs(self.data) do
        local data = {}
        data.id = v.day
        data.name = v.name2
        data.icon = v.icon2
        data.itemId = v.rewardId2
        data.num = v.num2
        data.type = v.type2
        data.state = 1
        items[data.id] = data
    end

    return items
end

return CheckInConfig