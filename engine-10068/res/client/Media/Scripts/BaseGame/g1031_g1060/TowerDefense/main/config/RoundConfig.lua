RoundConfig = {}

local settings = {}

function RoundConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.id) or 0
        data.checkPointId = tonumber(vConfig.checkPointId) or 0
        data.roundSeq = tonumber(vConfig.roundSeq) or 0
        data.monsterGroupId = tonumber(vConfig.monsterGroupId) or 0
        data.prepareTime = tonumber(vConfig.prepareTime) or 5
        data.goldNum = tonumber(vConfig.goldNum) or 1
        data.hallGold = tonumber(vConfig.hallGold) or 1
        data.exp = tonumber(vConfig.exp) or 1
        data.needSyn = tonumber(vConfig.needSyn) or 0
        if vConfig.isWarning then
            data.isWarning = vConfig.isWarning == "1"
        else
            data.isWarning = false
        end
        if settings[data.checkPointId] == nil then
            settings[data.checkPointId] = {}
        end
        settings[data.checkPointId][data.roundSeq] = data
    end
    for _, data in pairs(settings) do
        table.sort(data, function(a, b)
            return a.roundSeq < b.roundSeq
        end)
    end
end

function RoundConfig:getRoundGroupByCheckPointId(checkPointId)
    return settings[tonumber(checkPointId)]
end

function RoundConfig:getRewardGroup(checkPointId, passRound)
    if settings[tonumber(checkPointId)] == nil then
        return 0
    end
    local config = settings[tonumber(checkPointId)][tonumber(passRound)]
    if config ~= nil then
        return config.rewardGroupId
    end
    return 0
end

RoundConfig:init(FileUtil.getConfigFromCsv("setting/Round.csv"))
