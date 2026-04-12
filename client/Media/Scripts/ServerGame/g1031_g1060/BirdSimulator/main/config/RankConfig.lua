require "data.GameRank"

RankConfig = {}
RankConfig.ranks = {}

RankConfig.TotalCollect = 1
RankConfig.DailyCollect = 2
RankConfig.TotalFight = 3

RankConfig.totalCollectData = {}
RankConfig.dailyCollectData = {}
RankConfig.totalFightData = {}

function RankConfig:init(config)
    for i, value in pairs(config) do
        self.ranks[tonumber(value.id)] = GameRank.new(value)
    end
end

function RankConfig:getTotalCollectKey()
    return GameMatch.gameType .. ".TotalCollect"
end

function RankConfig:getDailyCollectKey()
    return GameMatch.gameType .. ".DailyCollect." .. DateUtil.getYearDay()
end

function RankConfig:getTotalFightKey()
    return GameMatch.gameType .. ".TotalFight"
end

function RankConfig:setZExpireat()
    HostApi.ZExpireat(RankConfig:getDailyCollectKey(), tostring(DateUtil.getDayLastTime()))
end

function RankConfig:updateRank()
    for i, v in pairs(self.ranks) do
        local key = ""
        if v.id == RankConfig.TotalCollect then
             key = RankConfig:getTotalCollectKey()
        end
        if v.id == RankConfig.DailyCollect then
            key = RankConfig:getDailyCollectKey()
        end
        if v.id == RankConfig.TotalFight then
            key = RankConfig:getTotalFightKey()
        end

        HostApi.ZRange(key, 0, 9)
    end
end

function RankConfig:addTotalCollectRank(data)
    RankConfig.totalCollectData[#RankConfig.totalCollectData + 1] = data
end

function RankConfig:addDailyCollectRank(data)
    RankConfig.dailyCollectData[#RankConfig.dailyCollectData + 1] = data
end

function RankConfig:addTotalFightRank(data)
    RankConfig.totalFightData[#RankConfig.totalFightData + 1] = data
end

function RankConfig:rebuildRank(type)
    local items = {}
    local key = ""
    if type == RankListener.TotalCollect then
        items = RankConfig.totalCollectData
        key = RankConfig:getTotalCollectKey()
    end

    if type == RankListener.DailyCollect then
        items = RankConfig.dailyCollectData
        key = RankConfig:getDailyCollectKey()
    end

    if type == RankListener.TotalFight then
        items = RankConfig.totalFightData
        key = RankConfig:getTotalFightKey()
    end

    local userIds = {}
    for i, v in pairs(items) do
        userIds[i] = v.userId
    end

    if #userIds == 0 then
        return false
    end

    UserInfoCache:GetCacheByUserIds(userIds, function(type, items)
        for i, v in pairs(items) do
            local info = UserInfoCache:GetCache(v.userId)
            if info ~= nil then
                v.vip = info.vip
                v.name = info.name
            end
        end

        if type == RankListener.TotalCollect then
            RankConfig.totalCollectData = items
        end

        if type == RankListener.DailyCollect then
            RankConfig.dailyCollectData = items
        end

        if type == RankListener.TotalFight then
            RankConfig.totalFightData = items
        end

        RankConfig:updateRankActor(type)

    end, key, type, items)
end


function RankConfig:updateRankActor(type)
    local data = {}
    local bulletinData = {}
    for i, v in pairs(self.ranks) do
        if v.id == type then
            if type == RankListener.TotalCollect then
                data = RankConfig.totalCollectData
            end

            if type == RankListener.DailyCollect then
                data = RankConfig.dailyCollectData
            end

            if type == RankListener.TotalFight then
                data = RankConfig.totalFightData
            end

            for _, v in pairs(data) do
                local temp = BulletinRank.new()
                temp.rank = v.rank
                temp.userId = v.userId
                temp.score = v.score
                temp.name = v.name
                table.insert(bulletinData, temp)
            end
            
            --print(" data = ", StringUtil.v2s(data))
            EngineWorld:getWorld():updateEntityBulletin(0, v.entityId, bulletinData)
        end
    end
end

return RankConfig