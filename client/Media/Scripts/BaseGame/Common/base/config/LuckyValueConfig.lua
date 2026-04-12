LuckyValueConfig = {}

local LuckyValue = {}

local function init()
    local path = FileUtil:getMapPath() .. "common/LuckyValue.csv"
    local file = io.open(path, "r")
    if not file then
        return
    end
    io.close(file)

    local config = CsvUtil.loadCsvFile(path, 3)
    for _, item in pairs(config) do
        local data = {}
        data.luckyValue = tonumber(item.luckyValue)
        data.luckyStars = tonumber(item.luckyStars)
        data.grandPrizeWeights = tonumber(item.grandPrizeWeights)
        LuckyValue[tonumber(item.seq)] = data
    end
end

function LuckyValueConfig:getWeightByLuckyValue(luckyValue)
    local weight = 0
    for _, luckyLevel in pairs(LuckyValue) do
        if luckyValue >= luckyLevel.luckyValue then
            weight = luckyLevel.grandPrizeWeights
        else
            return weight
        end
    end
    return weight
end

function LuckyValueConfig:getStarsByLuckyValue(luckyValue)
    local luckyStar = 0
    for _, luckyLevel in pairs(LuckyValue) do
        if luckyValue >= luckyLevel.luckyValue then
            luckyStar = luckyLevel.luckyStars
        else
            return luckyStar
        end
    end
    return luckyStar
end

init()