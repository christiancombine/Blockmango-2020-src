BaseRank = class("BaseRank")

BaseRank.LangKey = {
    CountDown = "rank_refresh_countdown",
    Rank = "rank_ranking",
    Name = "rank_player_name",
    Captain = "rank_captain_name",
    KillAmount = "rank_kill_amount",
    Level = "rank_player_level",
    Teammate = "rank_teammate_name",
    PassTime = "rank_pass_time",
    TopPass = "rank_top_pass",
    BookAmount = "rank_book_amount",
}

local playerDataCache = {}

function BaseRank:ctor(rankConfig)
    self.config = rankConfig
    self.curPage = 1
    self.actorCache = {}
    self:init()
    if isClient then
        if self.config.effectPos then
            self:showEffect()
        end
    else
        if #self.config.actorPos > 0 then
            for _ = 1, #self.config.actorPos do
                table.insert(self.actorCache, {})
            end
        end
    end

    self:updateRecordDate()
    LuaTimer:scheduleTimer(function()
        self:updateRecordDate()
    end, (self.config.refreshTime + 1.5) * 1000)
end

function BaseRank:init()

end

function BaseRank:updateKey()

end

function BaseRank:updateRecordDate()
    if isClient then
        if self.config.effectPos then
            self.curPage = 1
            self:refreshData()
            ---重新开启倒计时
            LuaTimer:cancel(self.countdownKey or 0)
            self.curTick = self.config.refreshTime + 1
            self.countdownKey = LuaTimer:scheduleTimer(function()
                self.curTick = self.curTick - 1

                local globalRender = Blockman.Instance().m_globalRender
                if globalRender then
                    ---countdown_time
                    local pos = self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.countDownTimeOffset[1], self.countDownTimeOffset[2])
                    globalRender:luaAddWallText(5, -1, DateUtil.getFormatTime(self.curTick), pos,
                            self.config.scale, self.config.yaw, 0, { 255 / 255, 255 / 255, 255 / 255, 255 / 255 })
                end
            end, 1000)
        end
    else
        MaxRecordHelper:requestNewData(self.rankKey, 40, function()
            if #self.config.actorPos > 0 then
                self:refreshActor(#self.config.actorPos)
            end
        end)
    end
end

function BaseRank:showEffect()
    local globalRender = Blockman.Instance().m_globalRender
    if not globalRender then
        return
    end

    local config = self.config
    EngineWorld:addSimpleEffect("g1056_paihangbang0" .. tostring(self.pageSize) .. ".effect", config.effectPos, config.yaw, -1, config.scale * 5, config.scale * 5, config.scale * 5)

    ---title
    globalRender:luaAddWallText(6, -1, config.title, self:getNewPosition(config.effectPos, config.scale, config.yaw, self.titleOffset[1], self.titleOffset[2]),
            config.scale * 1.5, config.yaw, 0, { 185 / 255, 209 / 255, 255 / 255, 255 / 255 })
    ---countdown
    globalRender:luaAddWallText(6, -1, self.LangKey.CountDown, self:getNewPosition(config.effectPos, config.scale, config.yaw, self.countDownOffset[1], self.countDownOffset[2]),
            config.scale, config.yaw, 0, { 103 / 255, 209 / 255, 255 / 255, 255 / 255 })

    for i = 1, self.rowNum do
        globalRender:luaAddWallText(5, -1, self.rowKey[i],
                self:getNewPosition(config.effectPos, config.scale, config.yaw, self.rowKeyOffsetX[i], self.rowKeyOffsetY[1]),
                config.scale, config.yaw, 0, { 14 / 255, 65 / 255, 104 / 255, 255 / 255 })
    end
end

function BaseRank:showPlayerActor()

end

function BaseRank:disPlayData(callback)
    LuaTimer:cancel(self.turingKey or 0)
    if self.curList then
        local time = 1
        LuaTimer:scheduleTimerWithEnd(function()
            self:printData(self.curList, 1 - time * 1 / 14, 1 - time * 1 / 14)
            time = time + 1
        end, function()
            callback()
        end, 50, 14)
    else
        callback()
    end
end

function BaseRank:refreshData()
    self:disPlayData(function()
        MaxRecordHelper:getRecord(self.rankKey, self.curPage, self.pageSize, function(list)
            self.curList = list

            ---开始出现动画
            local time = 1
            LuaTimer:scheduleTimerWithEnd(function()
                self:printData(self.curList, time * 1 / 14, time * 1 / 14)
                time = time + 1
            end, function()
                ---结束后启动下一次翻页倒计时
                if self.config.pageCount > 1 then
                    MaxRecordHelper:getRecord(self.rankKey, self.curPage + 1, self.pageSize, function(nextList)
                        if #nextList > 0 and self.curPage < self.config.pageCount then
                            self.curPage = self.curPage + 1
                        else
                            self.curPage = 1
                        end
                        ---延迟几秒后翻页
                        self.turingKey = LuaTimer:schedule(function()
                            self:refreshData()
                        end, self.config.turningTime * 1000)
                    end)
                end
            end, 50, 14)
        end)
    end)
end

function BaseRank:printData(list, scale, alpha)

end

function BaseRank:refreshActor(number)

end

function BaseRank:changeActor(userIds)
    UserInfoCache:GetCacheByUserIds(userIds,function()
        local needRemove = {}
        for index, userId in pairs(userIds) do
            local info = UserInfoCache:GetCache(userId) or { name = "" }
            local name = info.name
            local cache = self.actorCache[index]
            if userId == cache.userId then
                return
            end
            ---记录改排行榜的前三名模型Id
            self.actorCache[index].userId = userId
            needRemove[tostring(cache.userId)] = (needRemove[tostring(cache.userId)] or 0) - 1
            needRemove[tostring(userId)] = (needRemove[tostring(userId)] or 0) + 1
            ---查找该玩家有没有缓存记录

            if playerDataCache[tostring(userId)] then
                local playerInfo = playerDataCache[tostring(userId)]
                self:createActor(playerInfo, index, name)
            else
                DBManager:getPlayerData(userId, DbUtil.GAME_DATA, function(userId, _, data)
                    local playerInfo = {}
                    playerInfo.level = data.level or 1
                    playerInfo.cur_exp = data.cur_exp or 0
                    playerInfo.skinList = data.skinList or {}
                    playerInfo.equipSkinId = data.equipSkinId or 0
                    local config = SkinStoreConfig:getConfigById(playerInfo.equipSkinId)
                    if config then
                        playerInfo.skin = config.actorName
                    end
                    playerInfo.payItemList = {}
                    playerInfo.shortCutBar = data.shortCutBar or {}
                    playerInfo.hasReport = data.hasReport or false

                    playerDataCache[tostring(userId)] = playerInfo
                    self:createActor(playerInfo, index, name)
                end)
            end
        end
        for userId, code in pairs(needRemove) do
            if code < 0 then
                playerDataCache[userId] = nil
            end
        end
    end)
end

function BaseRank:createActor(playerInfo, index, name)
    EngineWorld:removeEntity(self.actorCache[index].entityId)
    local disName = TextFormat.colorGreen .. "[Lv." .. tostring(playerInfo.level) .. "] " .. TextFormat.colorWrite .. name
    self.actorCache[index].entityId = EngineWorld:addActorNpc(self.config.actorPos[index], self.config.yaw + 180, playerInfo.skin or "g1056_yuhangyuanboy.actor", function(entity)
        entity:setSkillName("g1056_fly_idle")
        entity:setScale(self.config.actorScale[index] or 1)
        local actorSize = ActorSizeSetting.getActorSize(playerInfo.skin or "g1056_yuhangyuanboy.actor")
        ---headText
        local builder = DataBuilder.new()
        builder:addParam("text", self.config.headText)
        local offsetY = (actorSize.height + 1.1) * (self.config.actorScale[index] or 1)
        builder:addVector3Param("offset", VectorUtil.newVector3(0, offsetY, 0))
        entity:addCustomText("head text", builder:getData(), -1, 1.3)

        ---headName
        local builderName = DataBuilder.new()
        builderName:addParam("text", disName)
        local offsetYName = (actorSize.height + 0.8) * (self.config.actorScale[index] or 1)
        builderName:addVector3Param("offset", VectorUtil.newVector3(0, offsetYName, 0))
        entity:addCustomText("head name", builderName:getData(), -1, 1)
    end)
end

function BaseRank:getNewPosition(pos, scale, yaw, xOffset, yOffset)
    xOffset = xOffset or 0
    yOffset = yOffset or 0
    --local x = -math.sin(yaw * MathUtil.DEG2RAD)
    --local z = math.cos(yaw * MathUtil.DEG2RAD)
    --local frontPos = VectorUtil.newVector3(pos.x + x / 100, pos.y, pos.z + z / 100)
    local frontPos = pos
    local leftYaw = yaw + 90
    local x = -math.sin(leftYaw * MathUtil.DEG2RAD)
    local z = math.cos(leftYaw * MathUtil.DEG2RAD)
    local newPos = VectorUtil.newVector3(frontPos.x + x * scale * xOffset, frontPos.y + yOffset * scale, frontPos.z + z * scale * xOffset)

    return newPos
end
