-- require "common.base.util.DateUtil"
RankData = {}
RankData.AllData = {}

-- { RankData, Own }

RankData.Type =
{
    Week = 1,
    All = 2,
    Count = 2,
}

function RankData:init()
    for i = 1, RankData.Type.Count do
        RankData.AllData[i] = {}
        RankData.AllData[i].RankData = {}
        RankData.AllData[i].Own = {}
        RankData.AllData[i].Update = true
    end
end

function RankData:UpdateData(data)

    if self.AllData[data.type] then
        self.AllData[data.type] = {}
        self.AllData[data.type].RankData = data.ranks
        self.AllData[data.type].Own = data.own
        self.AllData[data.type].Update = true
    end

end

function RankData:getFormatTimeWithDay(seconds)
    local format = "dd:HH:mm:ss"
    local text = ""
    local day = math.floor(seconds / 3600 / 24)
    local hour = math.floor(seconds % (3600 * 24) / 3600)
    local minute = math.floor((seconds % 3600) / 60)
    local second = seconds % 60
    text = self:checkTime(text, day, format, "dd")
    text = self:checkTime(text, hour, format, "HH", "D ")
    text = self:checkTime(text, minute, format, "mm")
    text = self:checkTime(text, second, format, "ss")
    return text
end
--
function RankData:checkTime(text, append, format, pattern, split)
    split = split or ":"
    if string.find(format, pattern) ~= nil then
        if #text ~= 0 then
            text = text .. split
        end
        if append < 10 then
            text = text .. "0" .. append
        else
            text = text .. append
        end
    end
    return text
end

return RankData