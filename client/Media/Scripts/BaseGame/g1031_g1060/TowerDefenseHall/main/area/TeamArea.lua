TeamArea = class("TeamArea", BaseArea)

local function getFrontPosition(pos, scale, yaw)
    local x = -math.sin(yaw * MathUtil.DEG2RAD)
    local z = math.cos(yaw * MathUtil.DEG2RAD)
    local frontPos = VectorUtil.newVector3(pos.x + x / 100, pos.y, pos.z + z / 100)
    local leftYaw = yaw - 90
    x = -math.sin(leftYaw * MathUtil.DEG2RAD)
    z = math.cos(leftYaw * MathUtil.DEG2RAD)
    return VectorUtil.newVector3(frontPos.x + x * scale * 0.65, frontPos.y - 0.15 * scale, frontPos.z + z * scale * 0.65)
end

function TeamArea:init()
    self.teamConfig = TeamConfig:getConfigById(self.config.configId)
    self.teamList = {}
    self.nearList = {}
    self.needSyncList = {}
    self.luaTime = nil
    self.isLocal = true
    self.nearArea = IArea.new()
    local minPos = self.Area:getMinPos()
    local maxPos = self.Area:getMaxPos()
    self.nearArea:initByPos(VectorUtil.newVector3(minPos.x - 2, minPos.y, minPos.z - 2), VectorUtil.newVector3(maxPos.x + 2, maxPos.y, maxPos.z + 2))
    self:checkTeamList()
    --showMapPicture
    if isClient then
        WorldImageRender.Instance():addStaticImage("mapPicture" .. self.config.Id, "tower_defense_map.json", self.teamConfig.mapPicture, self.config.pos4, self.config.scale4, false, self.config.pYaw)
    end
end

function TeamArea:showPlayerNum(pos, scale, yaw, cur, max, key, levelLimit)
    if isClient and GamePlayer:Instance().level >= levelLimit then
        --更新人数表现
        local line = math.ceil(max / 4)
        pos = VectorUtil.newVector3(pos.x, pos.y + 0.6 * (line - 1) / 2, pos.z)
        local index = 0
        while max > 0 do
            index = index + 1
            local max_key = math.min(max, 4)
            local cur_key = math.min(cur, 4)
            cur = math.max(cur - 4, 0)
            max = math.max(max - 4, 0)
            local countNumKey = string.format("team_%d_%d", max_key, cur_key)
            WorldImageRender.Instance():removeStaticImage(key .. "#" .. index)
            WorldImageRender.Instance():addStaticImage(key .. "#" .. index, "tower_defense_world_sprite.json", countNumKey, pos, scale, false, yaw)
            pos = VectorUtil.newVector3(pos.x, pos.y - 0.6, pos.z)
        end
    end
end

function TeamArea:checkPlayerLevel(player)
    if self.isLocal then
        self:showPlayerNum(self.config.pos2, self.config.scale2, self.config.pYaw,
                #self.playerList, self.teamConfig.countLimit, "countLimit" .. self.config.Id, self.teamConfig.levelLimit)
    end
    if player.level < self.teamConfig.levelLimit then
        --显示等级限制
        self.isLocked = true
        self:openAirWall(true)
        self.teamConfig.levelLimit = math.floor(self.teamConfig.levelLimit)
        local numPos = getFrontPosition(self.config.pos3, self.config.scale3, self.config.pYaw)
        WorldImageRender.Instance():addStaticImage("levelLimitBg" .. self.config.Id, "tower_defense_world_sprite.json", "area_lock", self.config.pos3, 0.5 * self.config.scale3, false, self.config.pYaw)
        WorldImageRender.Instance():addStaticNumber("levelLimit" .. self.config.Id, self.teamConfig.levelLimit, numPos, -1, 0.8 * self.config.scale3, 6, self.config.pYaw)
    else
        --移除等级限制
        if not self.myPlayerIn then
            self.isLocked = false
            self:openAirWall(false)
        end
        WorldImageRender.Instance():removeStaticNumber("levelLimit" .. self.config.Id)
        WorldImageRender.Instance():removeStaticImage("levelLimitBg" .. self.config.Id)
    end
end

function TeamArea:isNear(userId, pos)
    if #self.teamList < self.teamConfig.countLimit then
        if #self.nearList > 0 and TableUtil.include(self.nearList, tostring(userId)) then
            self:hideNearTip(userId)
        end
        return
    end

    if self.nearArea:inArea(pos) then
        if not GUIMain then
            return
        end
        self:inNear(userId)
    elseif TableUtil.include(self.nearList, tostring(userId)) then
        self:hideNearTip(userId)
    end
end

function TeamArea:hideNearTip(userId)
    self.nearList = TableUtil.removeElementByFun(self.nearList, function(key, value)
        return value == tostring(userId)
    end)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player.nearTimer then
        LuaTimer:cancel(player.nearTimer)
    end
    player.nearTimer = LuaTimer:schedule(function(userId)
        local _player = PlayerManager:getPlayerByUserId(userId)
        if not _player then
            return
        end
        GUIMain:showTeamTip(tostring(userId), false)
        _player.nearTimer = nil
        LuaTimer:cancel(_player.nearTimer)
    end, 3000, nil, player.userId)
end

function TeamArea:inNear(userId)
    if TableUtil.include(self.nearList, tostring(userId)) then
        return
    end

    table.insert(self.nearList, tostring(userId))
    GUIMain:showTeamTip(tostring(userId), true)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player.nearTimer then
        LuaTimer:cancel(player.nearTimer)
    end
end

function TeamArea:playerIn(player)
    if not isClient and #self.teamList == self.teamConfig.countLimit then
        --传送走玩家
        HostApi.resetPos(player.rakssid, self.config.pos4.x, self.config.pos4.y + 0.5, self.config.pos4.z)
        return false
    end
    table.insert(self.teamList, tostring(player.userId))
    self:checkTeamList()

    -- 本客户端所在队伍区域封锁
    if isClient and GamePlayer:Instance().entityId == player.entityId then
        self:openAirWall(true)
        self.myPlayerIn = true
    end
    return true
end

function TeamArea:playerOut(player)
    for index, userId in pairs(self.teamList) do
        if userId == tostring(player.userId) then
            table.remove(self.teamList, index)
            break
        end
    end
    if isClient and GamePlayer:Instance().entityId == player.entityId then
        self.myPlayerIn = false
    end
    self:checkTeamList()
    return true
end

function TeamArea:checkTeamList()
    if self.isLocal then
        self:showPlayerNum(self.config.pos2, self.config.scale2, self.config.pYaw,
                #self.playerList, self.teamConfig.countLimit, "countLimit" .. self.config.Id, self.teamConfig.levelLimit)
    end
    if not isClient then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player.level >= self.teamConfig.levelLimit then
                GamePacketSender:sendTeamAreaPlayerNum(player, self.config.Id, #self.teamList)
            end
        end
    end

    --任意一个区域人数达到上限  封锁
    if isClient and not self.myPlayerIn and not self.isLocked then
        if #self.teamList >= self.teamConfig.countLimit then
            self:openAirWall(true)
        else
            self:openAirWall(false)
        end
    end

    --倒计时逻辑
    if #self.teamList == 0 then
        --关闭
        LuaTimer:cancel(self.luaTime or 0)
        self.luaTime = nil
    elseif not self.luaTime then
        --开启
        if not isClient then
            self.needSyncList = PlayerManager:copyPlayers()
        end

        self.tick = math.floor(self.teamConfig.countDown) + 1 + self.teamConfig.teleportTime
        self.luaTime = LuaTimer:scheduleTimer(function()
            self.tick = self.tick - 1

            if isClient then

            else
                if self.tick - self.teamConfig.teleportTime > 0 then
                    for _, player in pairs(self.needSyncList) do
                        GamePacketSender:sendTeamAreaCountdown(player, self.config.Id, self.tick)
                    end
                    self.needSyncList = {}
                end

                if self.tick == self.teamConfig.teleportTime then
                    local teamId = self.teamList[1]
                    local teammateNum = #self.teamList
                    for _, userId in pairs(self.teamList) do
                        local player = PlayerManager:getPlayerByUserId(userId)
                        if player then
                            player.hallInfo.checkPointId = self.teamConfig.checkPointId
                            player.hallInfo.teamId = teamId
                            player.hallInfo.teammateNum = teammateNum
                            DbUtil:savePlayerData(player, true)
                        end
                    end
                end
            end
            if self.tick == 0 then
                self:gameStart()
            end
        end, 1000)
    end
end

function TeamArea:gameStart()
    --
    LuaTimer:cancel(self.luaTime or 0)
    self.luaTime = nil
    local teammateNum = #self.teamList
    if not isClient and teammateNum > 0 then
        local teamId = self.teamList[1]
        for _, userId in pairs(self.teamList) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                player.hallInfo.checkPointId = self.teamConfig.checkPointId
                player.hallInfo.teamId = teamId
                player.hallInfo.teammateNum = teammateNum
            end
        end
        local player = PlayerManager:getPlayerByUserId(teamId)
        if player then
            ReportUtil.reportStartGameInfo(player, teammateNum, player.hallInfo.checkPointId)
        end
        GameGroupManager:addUsers(self.teamList, teamId)
        GameGroupManager:takeGroupStart(teamId, self.teamConfig.gameType, self.teamConfig.mapId, function(isSuccess, userIds)
            if isSuccess then
                return
            end
            ---请求服务器失败，玩家全部传送回去出生点
            for _, userId in pairs(userIds) do
                local player = PlayerManager:getPlayerByUserId(userId)
                if player then
                    --传送走玩家
                    HostApi.resetPos(player.rakssid, self.config.pos4.x, self.config.pos4.y + 0.5, self.config.pos4.z)
                    ---TODO 添加提示
                end
            end
        end)
        self.teamList = {}
    end
end

function TeamArea:initCountDown(time)
    LuaTimer:cancel(self.countDownTime or 0)
    self.countDownTime = nil
    self.countDownTick = time

    WorldImageRender.Instance():addFloatNumber(self.countDownTick - self.teamConfig.teleportTime, self.config.pos1, self.config.scale1, 5)
    self.countDownTime = LuaTimer:scheduleTimer(function()
        self.countDownTick = self.countDownTick - 1
        if self.countDownTick > 0 then
            if self.countDownTick - self.teamConfig.teleportTime > 0 then
                WorldImageRender.Instance():addFloatNumber(self.countDownTick - self.teamConfig.teleportTime, self.config.pos1, self.config.scale1, 5)
            end
            if self.countDownTick == self.teamConfig.teleportTime then
                self:setEffect(true)
            end
        end
        if self.countDownTick == 0 then
            self:setEffect(false)
        end
    end, 1000)

end