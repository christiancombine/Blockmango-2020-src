require "config.YardConfig"

LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    GetMoneyExchangeRateEvent.registerCallBack(self.onGetMoneyExchangeRate)
    LoadManorConfigEvent.registerCallBack(self.onLoadManorConfig)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    if behavior == "break.block" then
        return false
    end
    return true
end

function LogicListener.onGetMoneyExchangeRate(rate)
    rate.value =  GameConfig.exchangeRate * rate.value
end

function LogicListener.onLoadManorConfig(response)
    if response ~= nil and string.len(response) > 0 then
        local data = json.decode(response)
        for i, v in pairs(data) do
            if v ~= nil then
                GameMatch.yardLevelInfo[v.level] = v
            end
        end
    end

    for i, v in pairs(YardConfig.yard) do
        for j, yard in pairs(GameMatch.yardLevelInfo) do
            if v.level == yard.level then
                v.praisePercent = yard.praisePercent
                v.vegetableNum = yard.vegetableNum
                v.praiseNum = yard.praiseNum
                v.potential = yard.potential
            end
        end
    end
    
end

return LogicListener