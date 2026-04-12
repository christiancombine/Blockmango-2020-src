GameAchievement = class()

function GameAchievement:ctor(userId)
    self.achievements = {}
    self.exploreAchievementData = {}
    self.isHasExploreData = false
    self.userId = userId
    self:initAchievements()
end

function GameAchievement:initAchievements()
    for i, v in pairs(AchievementConfig.achievements) do
        if self.achievements[tostring(v.type)] == nil then
            self.achievements[tostring(v.type)] = {}
        end

        if self.achievements[tostring(v.type)][tostring(v.category)] == nil then
            self.achievements[tostring(v.type)][tostring(v.category)] = {}
        end

        if v.extendId1 ~= "0" then
            if self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)] == nil then
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)] = {}
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)].id = v.id
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)].count = 0
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)].level = 0
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)].extendId1 = v.extendId1
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId1)].extendId2 = v.extendId2
            end
        else
            if self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)] == nil then
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)] = {}
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)].id = v.id
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)].count = 0
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)].level = 0
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)].extendId1 = v.extendId1
                self.achievements[tostring(v.type)][tostring(v.category)][tostring(v.extendId2)].extendId2 = v.extendId2
            end
        end
    end

    -- 初始化 田地的方块 和 房子等级
    self:setCount(AchievementConfig.TYPE_PLANT, AchievementConfig.TYPE_PLANT_FILED_NUM, 0, 0, ManorConfig.initFieldNum)
    self:setCount(AchievementConfig.TYPE_FACTORY, AchievementConfig.TYPE_FACTORY_HOUSE_LEVEL, 0, 0, 1)
end

function GameAchievement:initDataFromDB(data)
    self:initAchievementsFromDB(data.achievements or {})
end

function GameAchievement:initAchievementsFromDB(data)
    for i, v in pairs(data) do
        local key = StringUtil.split(i, ":")
        if self.achievements[tostring(key[1])] == nil then
            return false
        end

        if self.achievements[tostring(key[1])][tostring(key[2])] == nil then
            return false
        end

        if self.achievements[tostring(key[1])][tostring(key[2])][tostring(key[3])] == nil then
            return false
        end

        self.achievements[tostring(key[1])][tostring(key[2])][tostring(key[3])].id = v.id
        self.achievements[tostring(key[1])][tostring(key[2])][tostring(key[3])].count = v.count
        self.achievements[tostring(key[1])][tostring(key[2])][tostring(key[3])].level = v.level
    end
end

function GameAchievement:initRanchExploreFromDB(data)
    if data and data.achievements then
        for k, v in pairs(data.achievements) do
            local category = tonumber(k)
            local count = tonumber(v)
            if category and count then
                self:addCount(AchievementConfig.TYPE_EXPLORE, category, 0, count)
            end
        end
    end

    return true
end

function GameAchievement:prepareDataSaveToDB()
    local data = {
        achievements = self:prepareAchievementsToDB()
    }

    --HostApi.log("GameAchievement:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameAchievement:prepareAchievementsToDB()
    local achievements = {}
    for type, achievement in pairs(self.achievements) do
        for category, v in pairs(achievement) do
            for key, num in pairs(v) do
                achievements[tostring(type) ..":"..tostring(category)..":"..tostring(key)] = {}
                achievements[tostring(type) ..":"..tostring(category)..":"..tostring(key)].id = num.id
                achievements[tostring(type) ..":"..tostring(category)..":"..tostring(key)].count = num.count
                achievements[tostring(type) ..":"..tostring(category)..":"..tostring(key)].level = num.level
            end
        end
    end

    return achievements
end

function GameAchievement:addCount(type, category, key, count)
    if count <= 0 then
        return false
    end

    if self.achievements[tostring(type)] == nil then
        return false
    end

    if self.achievements[tostring(type)][tostring(category)] == nil then
        return false
    end

    for i, v in pairs(self.achievements[tostring(type)][tostring(category)]) do
        local ids = StringUtil.split(i, "#")
        for id = 1, #ids do
            if tostring(ids[id]) == tostring(key) then
                self.achievements[tostring(type)][tostring(category)][tostring(i)].count = self.achievements[tostring(type)][tostring(category)][tostring(i)].count + count
            end
        end
    end

    return false
end

function GameAchievement:setCount(type, category, key, level, count)
    if count <= 0 then
        return false
    end

    if self.achievements[tostring(type)] == nil then
        return false
    end

    if self.achievements[tostring(type)][tostring(category)] == nil then
        return false
    end

    if self.achievements[tostring(type)][tostring(category)][tostring(key)] == nil then
        return false
    end

    self.achievements[tostring(type)][tostring(category)][tostring(key)].count = count
    self.achievements[tostring(type)][tostring(category)][tostring(key)].level = level
end

function GameAchievement:initRanchAchievements()
    local achievements = {}
    local index = 1
    for type, achievement in pairs(self.achievements) do
        for category, v in pairs(achievement) do
            for key, num in pairs(v) do
                local extendId1 = num.extendId1
                local extendId2 = num.extendId2
                local nextNeedCount = AchievementConfig:getAchievementCountByLevel(type, category,extendId1, extendId2, num.level + 1)
                local maxLevel = AchievementConfig:getAchievementMaxLevel(type, category, extendId1, extendId2)
                local achievementInfo = AchievementConfig:getAchievementInfoByLevel(type, category, extendId1, extendId2, num.level + 1)

                local RanchAchievement = RanchAchievement.new()
                RanchAchievement.id = index
                RanchAchievement.exp = num.count
                RanchAchievement.level = num.level
                RanchAchievement.nextExp = nextNeedCount
                RanchAchievement.maxLevel = maxLevel
                RanchAchievement.playerExp = 0
                RanchAchievement.achievement = 0
                RanchAchievement.icon = ""
                RanchAchievement.desc = ""
                RanchAchievement.name = ""

                if achievementInfo ~= nil then
                    RanchAchievement.id = achievementInfo.id
                    RanchAchievement.playerExp = achievementInfo.exp
                    RanchAchievement.achievement = achievementInfo.achievementPoint
                    RanchAchievement.icon = achievementInfo.icon
                    RanchAchievement.desc = achievementInfo.desc
                    RanchAchievement.name = achievementInfo.name
                end

                achievements[index] = RanchAchievement
                index = index + 1
            end
        end
    end

    return achievements
end

function GameAchievement:updateAchievementLevelById(id)
    for type, achievement in pairs(self.achievements) do
        for category, v in pairs(achievement) do
            for key, num in pairs(v) do
                if num.id == id then
                    local extendId1 = self.achievements[tostring(type)][tostring(category)][tostring(key)].extendId1
                    local extendId2 = self.achievements[tostring(type)][tostring(category)][tostring(key)].extendId2
                    local level = self.achievements[tostring(type)][tostring(category)][tostring(key)].level
                    local curCount = self.achievements[tostring(type)][tostring(category)][tostring(key)].count
                    local nextNeedCount = AchievementConfig:getAchievementCountByLevel(type, category,extendId1, extendId2, level + 1)
                    local maxLevel = AchievementConfig:getAchievementMaxLevel(type, category, extendId1, extendId2)
                    if level < maxLevel then
                        if curCount >= nextNeedCount then
                            level = level + 1
                            local achievementInfo = AchievementConfig:getAchievementInfoByLevel(type, category, extendId1, extendId2, level + 1)
                            local achievementId = 0

                            if achievementInfo ~= nil then
                                achievementId = achievementInfo.id
                            end

                            self.achievements[tostring(type)][tostring(category)][tostring(key)].count = curCount
                            self.achievements[tostring(type)][tostring(category)][tostring(key)].level = level
                            self.achievements[tostring(type)][tostring(category)][tostring(key)].id = achievementId
                            return true
                        end
                    end
                end
            end
        end
    end

    return false
end

function GameAchievement:getAchievementInfo(type, category, key)
    if self.achievements[tostring(type)] == nil then
        return nil
    end

    if self.achievements[tostring(type)][tostring(category)] == nil then
        return nil
    end

    if self.achievements[tostring(type)][tostring(category)][tostring(key)] == nil then
        return nil
    end

    return self.achievements[tostring(type)][tostring(category)][tostring(key)]
end

