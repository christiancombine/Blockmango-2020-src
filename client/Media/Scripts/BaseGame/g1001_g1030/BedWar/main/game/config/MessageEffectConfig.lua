MessageEffectConfig = {}

local Config = {}

function MessageEffectConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tostring(vConfig.id)
        data.headEffect = tostring(vConfig.headEffect)
        data.headDuration = tonumber(vConfig.headDuration)
        data.headScale = tonumber(vConfig.headScale)
        data.flaresEffect = tostring(vConfig.flaresEffect)
        data.flaresSpeed = tonumber(vConfig.flaresSpeed)
        data.flaresScale = tonumber(vConfig.flaresScale)
        data.boomHeight = tonumber(vConfig.boomHeight)
        data.boomEffect = tostring(vConfig.boomEffect)
        data.boomDuration = tonumber(vConfig.boomDuration)
        Config[data.id] = data
    end
end

function MessageEffectConfig:getConfigById(id)
    return Config[tostring(id)]
end

function MessageEffectConfig:shootFlares(senderId, id, startPosition)
    ---Client Function
    local config = Config[tostring(id)]
    if not config then
        return
    end
    SoundUtil.playSound3D(SoundConfig.shootSignal, startPosition.x, startPosition.y, startPosition.z)
    local effect = EngineWorld:addSimpleEffect(config.flaresEffect, startPosition, 0, -1, 1, 1, 1)
    local endPos = nil
    --- 转换速度单位,  每秒 ->每25毫秒
    local speed = config.flaresSpeed / 40
    --- 根据爆炸高度计算出, 需要飞行多少个 25毫秒
    local times = config.boomHeight / config.flaresSpeed * 40
    local sender = PlayerManager:getPlayerByServerId(senderId)
    if not sender or not sender.Player then
        return
    end
    local yaw = sender.Player:getYaw()
    local x = -math.sin(yaw * MathUtil.DEG2RAD)
    local z = math.cos(yaw * MathUtil.DEG2RAD)
    LuaTimer:scheduleTimerWithEnd(function()
        endPos = effect.mPosition
        endPos.y = endPos.y + speed
        endPos.x = endPos.x + x * 0.2
        endPos.z = endPos.z + z * 0.2
        effect.mPosition = endPos
    end, function()
        SoundUtil.playSound3D(SoundConfig.boomSignal, endPos.x, endPos.y, endPos.z)
        EngineWorld:removeSimpleEffect(effect.mEffectName)
        EngineWorld:addSimpleEffect(config.boomEffect, endPos, 0, config.boomDuration * 1000, 1, 1, 1)
    end, 25, times)
end

MessageEffectConfig:init(FileUtil.getConfigFromCsv("setting/MessageEffect.csv"))