AreaEventConfig = {}

AreaEventConfig.settings = {}

AreaEventConfig.totalWeight = 0

function AreaEventConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.eventId = tonumber(vConfig.n_eventId) or 0 --事件ID
        data.eventType = tonumber(vConfig.n_eventType) or 0 --事件类型(1-摔跤擂台)
        data.areaRange = AreaEventConfig:parseAreaRange(vConfig.s_areaRange or "") --区域范围
        --data.areaCenter = AreaEventConfig:getAreaCenter(areaRange)
        data.strAreaTeleportPos = vConfig.s_areaTeleportPos or ""
        data.areaTeleportPos = AreaEventConfig:parseAreaPos(data.strAreaTeleportPos)
        --data.enterPos = StringUtil.split(vConfig.s_enterPos, ",")
        data.needMuscle = tonumber(vConfig.n_needMuscle) or 0 --所需肌肉值
        data.eventTime = tonumber(vConfig.n_eventTime) or 0 --持续时间
        data.weight = tonumber(vConfig.n_weight) or 0 --权重
        data.noticeMessage = vConfig.s_noticeMessage or "" --通知信息
        data.rewardType = tonumber(vConfig.n_rewardType) or 0 --奖励类型(1-肌肉量,2-金币)
        data.rewardNum = tonumber(vConfig.n_rewardNum) or 0 --奖励数量
        data.eventSound = self:parseIntTable(vConfig.s_eventSound or "") --音效
        data.eventEffect = self:parseIntTable(vConfig.s_eventEffect or "") --特效
        data.winEffect = tonumber(vConfig.n_winEffect) or 0 --胜利特效
        data.serverTime = StringUtil.split(vConfig.s_serverTime or "", ",")
        self.totalWeight = self.totalWeight + data.weight
        table.insert(self.settings, data)

        --print(inspect.inspect(data.areaTeleportPos))
    end

    --print(inspect.inspect(AreaEventConfig.settings))
end

function AreaEventConfig:parseAreaRange(str)
    if str == "" then
        return nil
    end

    local result = { {}, {} }
    for i, position in pairs(StringUtil.split(str, "&")) do
        local pos = StringUtil.split(position, ",")
        result[i].x = tonumber(pos[1])
        result[i].y = tonumber(pos[2])
    end
    return result
end

function AreaEventConfig:parseAreaPos(str)
    if str == "" then
        return nil
    end

    local result = {}
    for i, position in pairs(StringUtil.split(str, "&")) do
        local pos = StringUtil.split(position, ",")
        table.insert(result,
                {
                    x = tonumber(pos[1]),
                    y = tonumber(pos[2]),
                    z = tonumber(pos[3]),
                })
    end
    return result
end

function AreaEventConfig:parseIntTable(str)
    local result = {}
    for _, v in pairs(StringUtil.split(str, ",")) do
        table.insert(result, tonumber(v))
    end
    return result
end

function AreaEventConfig:randomAreaEvent()
    local rand = math.random(self.totalWeight)

    local sum = 0
    for _, config in pairs(self.settings) do
        sum = sum + config.weight
        if sum >= rand then
            return config
        end
    end

    return nil
end

--function AreaEventConfig:getAreaCenter(areaRange)
--    --TODO
--end

return AreaEventConfig
