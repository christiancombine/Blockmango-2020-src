RankConfig = {}

RankConfig.Config = {}

local RankClassByType = {
    ["1"] = KillMonsterTotalRank,
    ["2"] = KillMonsterWeekRank,
    ["3"] = TowerBookRank,
    ["4"] = CheckPointTotalRank,
    ["5"] = CheckPointRank,
}

RankConfig.RankWeekKey = 0

RankConfig.Ranks = {}

function RankConfig:init(config)
    for _, vConfig in pairs(config) do
        local item = {}
        item.Id = tonumber(vConfig.Id) or 0
        item.title = tostring(vConfig.title) or ""
        item.type = tonumber(vConfig.type) or 0
        local effectPos = StringUtil.split(vConfig.effectPos or "", ",")
        if #effectPos == 3 then
            item.effectPos = VectorUtil.newVector3(effectPos[1] or 0, effectPos[2] or 0, effectPos[3] or 0)
        end
        local actorPoses = StringUtil.split(vConfig.actorPos or "", "#")
        item.actorPos = {}
        for _, posStr in pairs(actorPoses) do
            local pos = StringUtil.split(posStr or "", ",")
            if #pos == 3 then
                table.insert(item.actorPos, VectorUtil.newVector3(pos[1] or 0, pos[2] or 0, pos[3] or 0))
            end
        end
        item.actorScale = StringUtil.split(vConfig.actorScale or "", "#")
        item.headText = tostring(vConfig.headText) or ""
        item.param = tostring(vConfig.param) or ""
        item.scale = tonumber(vConfig.scale) or 0
        item.yaw = tonumber(vConfig.yaw) or 0
        item.yaw = math.fmod(item.yaw + 180, 360)
        item.pageSize = tonumber(vConfig.pageSize) or 0
        item.refreshTime = tonumber(vConfig.refreshTime) or 3600
        item.turningTime = tonumber(vConfig.turningTime) or 10
        item.pageCount = tonumber(vConfig.pageCount) or 1
        self.Config[item.Id] = item
    end
    if not isClient then
        self.RankWeekKey = DateUtil.getYearWeek()
        LuaTimer:scheduleTimer(function()
            ---每分钟检测一下是否已经换周
            if RankConfig.RankWeekKey ~= DateUtil.getYearWeek() then
                RankConfig.RankWeekKey = DateUtil.getYearWeek()
                MaxRecordHelper:addGameRecord(Define.RankKey.KillMonsterWeek .. RankConfig.RankWeekKey, 40)
                MaxRecordHelper:requestNewData(Define.RankKey.KillMonsterWeek .. RankConfig.RankWeekKey, 40, function()
                    ---请求成功后广播
                    for _, player in pairs(PlayerManager:getPlayers()) do
                        GamePacketSender:sendRankKey(player, RankConfig.RankWeekKey)
                    end
                end)
            end
        end, 60000)
    end
    self:createRank()
end

function RankConfig:createRank()
    for _, config in pairs(self.Config) do
        local class = RankClassByType[tostring(config.type)]
        if class then
            local rank = class.new(config)
            table.insert(RankConfig.Ranks, rank)
        end
    end
end

function RankConfig:updateKey(weekKey)
    RankConfig.RankWeekKey = weekKey
    for _, rank in pairs(RankConfig.Ranks) do
        rank:updateKey()
    end
end

RankConfig:init(FileUtil.getConfigFromCsv("setting/Rank.csv"))