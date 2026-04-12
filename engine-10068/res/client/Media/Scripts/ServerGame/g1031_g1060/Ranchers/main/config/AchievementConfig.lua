AchievementConfig = {}
AchievementConfig.achievements = {}

-- 成就种类
AchievementConfig.TYPE_MANOR = 1                    -- 领地成就
AchievementConfig.TYPE_MANOR_EXPAND = 1             -- 领地扩建次数
AchievementConfig.TYPE_MANOR_ROAD_NUM = 2           -- 领地中道路方块存在的数量

AchievementConfig.TYPE_RESOURCE = 2                 -- 资源成就
AchievementConfig.TYPE_RESOURCE_MONEY_EARN = 1      -- 赚取游戏币数量
AchievementConfig.TYPE_RESOURCE_MONEY_SELL = 2      -- 出售物品获得游戏币数量
AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME = 3   -- 消耗游戏币数量

AchievementConfig.TYPE_TASK = 3                     -- 订单成就
AchievementConfig.TYPE_TASK_COMMIT = 1              -- 订单提交指定物品数量
AchievementConfig.TYPE_TASK_COMPLETE = 2            -- 完成指定次数订单任务

AchievementConfig.TYPE_PLANT = 4                    -- 种植成就
AchievementConfig.TYPE_PLANT_PICK = 1               -- 收获指定物品次数
AchievementConfig.TYPE_PLANT_FILED_NUM = 2          -- 田地块数达到指定数量
AchievementConfig.TYPE_PLANT_PLANT = 3              -- 种植总次数达到指定数量

AchievementConfig.TYPE_ANIMAL = 5                   -- 养殖成就
AchievementConfig.TYPE_ANIMAL_GET = 1               -- 养殖建筑收获指定物品次数

AchievementConfig.TYPE_FACTORY = 6                  -- 建筑成就
AchievementConfig.TYPE_FACTORY_NUM = 1              -- 建造工厂达到指定数量
AchievementConfig.TYPE_FACTORY_GET = 2              -- 工厂建筑生产达到指定次数
AchievementConfig.TYPE_FACTORY_HOUSE_LEVEL = 3      -- 玩家房屋升至指定等级

AchievementConfig.TYPE_SOCIAL = 7                   -- 社区成就
AchievementConfig.TYPE_SOCIAL_GIFT_SEND = 1         -- 每日赠送好友、部落成员礼物达到指定次数
AchievementConfig.TYPE_SOCIAL_GIFT_RECEIVE = 2      -- 获得好友、部落成员赠送礼物达到指定次数
AchievementConfig.TYPE_SOCIAL_TASKS_HELP = 3        -- 完成好友求助达到指定次数
AchievementConfig.TYPE_SOCIAL_TEAM_FRIENDS = 4      -- 由好友组成指定人数探险队伍次数
AchievementConfig.TYPE_SOCIAL_TEAM_CLANS = 5        -- 由部落成员组成指定人数探险队伍次数

AchievementConfig.TYPE_EXPLORE = 8                  -- 探险成就
AchievementConfig.TYPE_EXPLORE_FIND_CHANGE_ROOM = 1 -- 探险发现机遇房间
AchievementConfig.TYPE_EXPLORE_FIND_DANGER_ROOM = 2 -- 探险发现危险房间
AchievementConfig.TYPE_EXPLORE_FIND_WEALTH_ROOM = 3 -- 探险发现财富房间
AchievementConfig.TYPE_EXPLORE_FIND_CAGE_ROOM = 4   -- 探险发现牢笼房间
AchievementConfig.TYPE_EXPLORE_FIND_BOX = 5         -- 探险发现宝箱
AchievementConfig.TYPE_EXPLORE_FINISH_TASK = 6      -- 探险完成探险任务

--赚取游戏币的途径
AchievementConfig.MONEY_TYPE_GIFT = 0
AchievementConfig.MONEY_TYPE_EARN = 1
AchievementConfig.MONEY_TYPE_SELL = 2
--AchievementConfig.MONEY_TYPE_CONSUME = 3

function AchievementConfig:initAchievement(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.category = tonumber(v.category)
        data.extendId1 = v.extendId1
        data.extendId2 = v.extendId2
        data.level = tonumber(v.level)
        data.count = tonumber(v.count)
        data.icon = v.icon
        data.name = v.name
        data.desc = v.desc
        data.exp = tonumber(v.exp)
        data.achievementPoint = tonumber(v.achievementPoint)
        table.insert(self.achievements, data)
    end

    table.sort(self.achievements, function(a, b)
        return a.id < b.id
    end)
end

function AchievementConfig:getAchievementByTypeAndCategory(type, category, extendId1, extendId2)
    local achievements = {}
    for i, v in pairs(self.achievements) do
        if tostring(v.type) == tostring(type) and
            tostring(v.category) == tostring(category) and
            tostring( v.extendId1) == tostring(extendId1) and
            tostring(v.extendId2) == tostring(extendId2) then
            table.insert(achievements, v)
        end
    end

    return achievements
end

function AchievementConfig:getAchievementCountByLevel(type, category, extendId1, extendId2, level)
    local achievements = self:getAchievementByTypeAndCategory(type, category, extendId1, extendId2)

    if #achievements == 0 then
        return 0
    end

    for i, v in pairs(achievements) do
        if v.level == level then
            return v.count
        end
    end

    return 0
end

function AchievementConfig:getAchievementInfoByLevel(type, category, extendId1, extendId2, level)
    local achievements = self:getAchievementByTypeAndCategory(type, category, extendId1, extendId2)
    for i, v in pairs(achievements) do
        if v.level == level then
            return v
        end
    end

    return achievements[#achievements]
end

function AchievementConfig:getAchievementMaxLevel(type, category, extendId1, extendId2)
    local achievements = self:getAchievementByTypeAndCategory(type, category, extendId1, extendId2)

    return #achievements
end

function AchievementConfig:getAchievementInfoById(id)
    for i, v in pairs(self.achievements) do
        if v.id == id then
            return v
        end
    end

    return nil
end

function AchievementConfig:getAchievementTotalCountUnderLevel(type, category, extendId1, extendId2, level)
    local achievements = self:getAchievementByTypeAndCategory(type, category, extendId1, extendId2)
    local totalCount = 0
    for i, v in pairs(achievements) do
        if v.level <= level then
            totalCount = totalCount + v.count
        end
    end

    return totalCount
end

return AchievementConfig