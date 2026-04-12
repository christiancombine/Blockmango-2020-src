RanchersRankManager = {}

RanchersRankManager.prosperity = {}
RanchersRankManager.achievement = {}
RanchersRankManager.clan = {}

function RanchersRankManager:updateRankData()
    local prosperityKey = self:getProsperityKey()
    local achievementKey = self:getAchievementKey()
    HostApi.ZRange(prosperityKey, 0, 99)
    HostApi.ZRange(achievementKey, 0, 49)
end

function RanchersRankManager:getProsperityKey()
    return GameMatch.gameType .. "." .. "prosperity"
end

function RanchersRankManager:getAchievementKey()
    return GameMatch.gameType .. "." .. "achievement"
end

function RanchersRankManager:getClanKey()
    return GameMatch.gameType .. "." .. "clan"
end

function RanchersRankManager:addProsperityData(data)
    self.prosperity[#self.prosperity + 1] = data
end

function RanchersRankManager:addAchievementData(data)
    self.achievement[#self.achievement + 1] = data
end

function RanchersRankManager:setClanData(data)
    self.clan = data
end

function RanchersRankManager:rebuildRank(type)
    local items = {}
    local key = ""
    if type == RankListener.type_prosperity then
        items = self.prosperity
        key = self:getProsperityKey()
    end

    if type == RankListener.type_achievement then
        items = self.achievement
        key = self:getAchievementKey()
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
                v.name = info.name
            end
        end

        if type == RankListener.type_prosperity then
            self.prosperity = items
            local ids = {}
            for i, v in pairs(self.prosperity) do
                ids[i] = v.userId
            end

            RanchersWeb:checkLandsInfo(ids, function (data)
                for i, v in pairs(data) do
                    for j, k in pairs(self.prosperity) do
                        if v.userId == k.userId then
                            k.level = v.level
                        end
                    end
                end
            end)

        end

        if type == RankListener.type_achievement then
            self.achievement = items
            local ids = {}
            for i, v in pairs(self.achievement) do
                ids[i] = v.userId
            end
            RanchersWeb:checkLandsInfo(ids, function (data)
                for i, v in pairs(data) do
                    for j, k in pairs(self.achievement) do
                        if v.userId == k.userId then
                            k.level = v.level
                        end
                    end
                end
            end)
        end

    end, key, type, items)

end