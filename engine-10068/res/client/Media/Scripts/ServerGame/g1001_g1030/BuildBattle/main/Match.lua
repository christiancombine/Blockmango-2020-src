---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1023"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.ReadyToStart = 2
GameMatch.Status.Buiding = 3
GameMatch.Status.BuidOverInterval = 4
GameMatch.Status.Grade = 5
GameMatch.Status.GradeInterval = 6
GameMatch.Status.GradeOverInterval = 7
GameMatch.Status.Guess = 8
GameMatch.Status.GuessOverInterval = 9
GameMatch.Status.GameOver = 10
GameMatch.Status.WaitingReset = 11

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.hasStartGame = false

GameMatch.gameWaitTick = 0
GameMatch.gameReadyToStarTick = 0
GameMatch.gameBuidingTick = 0
GameMatch.gameBuildOverIntervalTick = 0
GameMatch.gameGradeTick = 0
GameMatch.gameGradeIntervalTick = 0
GameMatch.gameGradeOverIntervalTick = 0
GameMatch.gameGuessTick = 0
GameMatch.gameGuessOverIntervalTick = 0
GameMatch.gameOverTick = 0
GameMatch.gameWaitingResetTick = 0

GameMatch.curTick = 0

GameMatch.prestartPlayerCount = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    self:resetGame()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    -- self:ifSaveRankData()
    self:ifUpdateRank()

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifWaitingPlayerEnd()
    end

    if self.curStatus == self.Status.ReadyToStart then
        self:ifReadyToStartEnd()
    end

    if self.curStatus == self.Status.Buiding then
        self:ifBuildOver()
    end

    if self.curStatus == self.Status.BuidOverInterval then
        self:ifBuildOverInterval()
    end

    if self.curStatus == self.Status.Grade then
        self:ifGradeEnd()
    end

    if self.curStatus == self.Status.GradeInterval then
        self:ifGradeIntervalEnd()
    end

    if self.curStatus == self.Status.GradeOverInterval then
        self:ifGradeOverIntervalEnd()
    end

    if self.curStatus == self.Status.Guess then
        self:ifGuessEnd()
    end

    if self.curStatus == self.Status.GuessOverInterval then
        self:ifGuessOverIntervalEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

    if self.curStatus == self.Status.WaitingReset then
        self:ifResetEnd()
    end

end

function GameMatch:ifSaveRankData()
    if self.curTick == 0 or self.curTick % 30 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        RankNpcConfig:savePlayerRankScore(player)
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    local data = {}
    data.money = player.money
    data.unlockedCommodities = player.unlockedCommodities
    DBManager:savePlayerData(player.userId, 1, data, immediate)
end

function GameMatch:ifWaitingPlayerEnd()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        if self.curTick - self.gameWaitTick > GameConfig.prepareTime then
            self:readyToStart()
        else
            local seconds = GameConfig.prepareTime - (self.curTick - self.gameWaitTick)
            if seconds == 12 then
                self.prestartPlayerCount = PlayerManager:getPlayerCount()
                HostApi.sendStartGame(self.prestartPlayerCount)
            end
            if seconds <= 10 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12)
                else
                    HostApi.sendPlaySound(0, 11)
                end
            end
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
        if self.prestartPlayerCount ~= 0 then
            self.prestartPlayerCount = 0
            HostApi.sendResetGame(PlayerManager:getPlayerCount())
        end
    end
end

function GameMatch:ifReadyToStartEnd()
    local seconds = GameConfig.readyToStartTime - (self.curTick - self.gameReadyToStarTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:readyToStartTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        MsgSender.sendBottomTips(1, IMessages:msgGameStart())
        MsgSender.sendCenterTips(3, self:getQuestionId())
        self:startBuild()
    end
end

function GameMatch:ifBuildOver()

    local ticks = self.curTick - self.gameBuidingTick

    local seconds = GameConfig.buildTime - ticks
    local players = PlayerManager:getPlayers()

    --check whether player in legal area
    if self.curTick % 3 == 0 then
        for i, player in pairs(players) do
            if 0 < player.at_room_id and player.at_room_id < GameConfig.waitingRoomId then
                if not GameConfig.targetPos[player.at_room_id].area:inArea(player:getPosition()) then
                    player:teleBuildPos()
                    HostApi.log("player cheat and pull it back to hall,userId:"..tostring(player.userId), 2)
                end
            end
        end
    end

    if seconds == 180 or seconds == 60 then

        for i, v in pairs(players) do
            HostApi.showBuildWarLeftTime(v.rakssid, true, seconds, self:getQuestionId())
        end
        MsgSender.sendCenterTips(3, Messages:msgBuildEndTimeHint(seconds))
    end

    if seconds <= 10 and seconds > 0 then
        for i, v in pairs(players) do
            HostApi.showBuildWarLeftTime(v.rakssid, true, seconds, self:getQuestionId())
        end
        MsgSender.sendCenterTips(3, Messages:msgBuildEndTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doBuildOver()
    end

end

function GameMatch:ifBuildOverInterval()
    local seconds = GameConfig.buildOverInterval - (self.curTick - self.gameBuildOverIntervalTick)

    if seconds <= 0 then

        self:doBuildOverInterval()
    end
end

function GameMatch:ifGradeEnd()

    local seconds = GameConfig.gradeTime - (self.curTick - self.gameGradeTick)

    if seconds <= 0 then

        self:setCurGradeRoom(self.gradePlayer)
        local next_room, has_grade_all = self:getNextGradeRoom()
        if has_grade_all then

            self:doGradeOver()
            return
        else
            self:doGradeInterval()
        end

        -- self.gradePlayer = self.gradePlayer + 1
        self.gradePlayer = next_room
    else
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if self.gradePlayer == v.room_id then
                HostApi.showBuildGrade(v.rakssid, false, true, Messages:msgStartGuessSelf(), v:getCurGrade(), seconds)
            else
                HostApi.showBuildGrade(v.rakssid, false, true, Messages:msgStartGuessOther(), v:getCurGrade(), seconds)
            end
        end
    end

end

function GameMatch:ifGradeIntervalEnd()
    local seconds = GameConfig.gradeIntervalTime - (self.curTick - self.gameGradeIntervalTick)

    if seconds <= 0 then
        self.curStatus = self.Status.Grade
        self.gameGradeTick = self.curTick
        local players = PlayerManager:getPlayers()
        local targetPos = nil
        local yaw = nil
        for i, v in pairs(self.rankPlayers) do
            if self.gradePlayer == v.room_id then
                targetPos, yaw = self:getTargetPos(v.room_id)
                break
            end
        end

        if targetPos and yaw then
            for i, v in pairs(players) do
                v:teleportPosByRecord(targetPos, yaw)
            end
        end

        for i, v in pairs(players) do
            if self.gradePlayer == v.room_id then
                HostApi.showBuildGrade(v.rakssid, false, true, Messages:msgStartGuessSelf(), v:getCurGrade(), GameConfig.gradeTime)
            else
                HostApi.showBuildGrade(v.rakssid, false, true, Messages:msgStartGuessOther(), v:getCurGrade(), GameConfig.gradeTime)
            end
        end
    end
end

function GameMatch:ifGradeOverIntervalEnd()
    local seconds = GameConfig.gradeOverIntervalTime - (self.curTick - self.gameGradeOverIntervalTick)

    if seconds <= 0 then
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            HostApi.showBuildGrade(v.rakssid, true, false, Messages:msgStartGuessSelf(), 0, 0)
        end

        self.curStatus = self.Status.Guess
        self.gameGuessTick = self.curTick

        MsgSender.sendCenterTips(3, Messages:msgStartGuess())

        -- self:sendGuessInfo()
        self:doReward()
    end
end

function GameMatch:ifGuessEnd()
    local seconds = GameConfig.guessTime - (self.curTick - self.gameGuessTick)

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:msgGuessLeft(seconds))
    end

    if seconds <= 0 then
        self:doGuessOver()
    end
end

function GameMatch:ifGuessOverIntervalEnd()
    local seconds = GameConfig.guessOverIntervalTime - (self.curTick - self.gameGuessOverIntervalTick)

    if seconds <= 0 then

        self.curStatus = self.Status.GameOver
        self.gameOverTick = self.curTick
        local winner, winner_name = self:getWinner()

        local targetPos, yaw = self:getTargetPos(winner)
        local players = PlayerManager:getPlayers()
        if targetPos and yaw then
            for _, player in pairs(players) do
                player:teleportPosByRecord(targetPos, yaw)
            end
            for _, player in pairs(players) do
                UserExpManager:addUserExp(player.userId, winner == player.room_id)
                if winner == player.room_id then
                    ReportManager:reportUserWin(player.userId)
                end
            end
        end

        MsgSender.sendCenterTips(3, Messages:msgShowWinnner(winner_name))

        for k, v in pairs(players) do
            self:showBuildWarResult(v.rakssid)
            -- self:showBuildGuessResult(v.rakssid)
        end
    end
end

function GameMatch:ifGameOverEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick)

    if seconds <= 0 then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            RankNpcConfig:savePlayerRankScore(player)
        end

        self:cleanPlayer()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.WaitingPlayer
    self.isFirstReady = true
end

function GameMatch:readyToStart()
    self.curStatus = self.Status.ReadyToStart
    self:showBuildWarBtn(false)
    self.gameReadyToStarTick = self.curTick
    self.hasStartGame = true
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:teleBuildPos()
    end
    RewardManager:startGame()
    HostApi.sendPlaySound(0, 10030)
end

function GameMatch:startBuild()
    self:showBuildWarBtn(true)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        HostApi.showBuildWarLeftTime(v.rakssid, true, GameConfig.buildTime, self:getQuestionId())
    end
    HostApi.sendPlaySound(0, 142)
    self.curStatus = self.Status.Buiding
    self.gameBuidingTick = self.curTick

    for k, v in pairs(players) do
        local list = {}
        list.room_id = v.room_id
        list.userId = v.userId
        list.name = v.name
        list.vip = v.vip
        list.grade_list = {}
        for k1, v1 in pairs(players) do
            local item = {}
            item.room_id = v1.room_id
            item.userId = v1.userId
            item.name = v1.name

            -- 自己给自己打分为0
            if v.userId == v1.userId then
                item.grade = 0
            else
                item.grade = GameConfig.gradeAverage
            end
            table.insert(list.grade_list, item)
        end
        table.insert(self.grade_list, list)
    end

    self:copyPlayers()
end

function GameMatch:doBuildOver()
    self:showBuildWarBtn(false)
    self.gradePlayer = 1
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        HostApi.showBuildWarLeftTime(v.rakssid, false, GameConfig.buildTime, self:getQuestionId())
    end
    self.curStatus = self.Status.BuidOverInterval
    self.gameBuildOverIntervalTick = self.curTick

    HostApi.sendPlaySound(0, 142)
    MsgSender.sendCenterTips(3, Messages:msgReadytoGrade())

    self:copyPlayers()
end

function GameMatch:doBuildOverInterval()
    local targetPos = nil
    local yaw = nil
    for i, v in pairs(self.rankPlayers) do
        if self.gradePlayer == v.room_id then
            targetPos, yaw = self:getTargetPos(v.room_id)
        end
    end
    local players = PlayerManager:getPlayers()
    if targetPos and yaw then

        for i, v in pairs(players) do
            v:teleportPosByRecord(targetPos, yaw)
        end
    end

    for i, v in pairs(players) do
        if self.gradePlayer == v.room_id then
            HostApi.showBuildGrade(v.rakssid, true, true, Messages:msgStartGuessSelf(), 0, GameConfig.gradeTime)
        else
            HostApi.showBuildGrade(v.rakssid, true, true, Messages:msgStartGuessOther(), 0, GameConfig.gradeTime)
        end
    end

    self.curStatus = self.Status.Grade
    self.gameGradeTick = self.curTick
end

function GameMatch:showFinalGrade()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        for k, grade_info in pairs(v.grade_list) do
            if grade_info.room_id == self.gradePlayer then
                MsgSender.sendMsgToTarget(v.rakssid, Messages:msgShowYourFinalGrade() + grade_info.grade - 1)
            end
        end
    end
end

function GameMatch:doGradeOver()
    self:showFinalGrade()
    HostApi.sendPlaySound(0, 142)
    self.curStatus = self.Status.GradeOverInterval
    self.gameGradeOverIntervalTick = self.curTick

    for k, v in pairs(self.grade_list) do
        local item = {}
        item.userId = v.userId
        item.room_id = v.room_id
        item.score = 0
        item.name = v.name
        item.vip = v.vip
        for k1, v1 in pairs(v.grade_list) do
            item.score = item.score + self:getGradeScore(v1.grade)
        end
        table.insert(self.score_list, item)
    end

    table.sort(self.score_list, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.vip ~= b.vip then
            return a.vip > b.vip
        end
        return a.userId > b.userId
    end)

    HostApi.log("doGradeOver")
    local players = PlayerManager:getPlayers()
    for rank, player in pairs(players) do
        for k, v in pairs(self.score_list) do
            if v.userId == player.userId then
                player.score_all = player.score_all + v.score
                player.appIntegral = player.appIntegral + v.score
            end
        end
    end

    self:copyPlayers()
    self:sortPlayerRank()

    local winner = self:getWinner()

    for rank, player in pairs(players) do
        if winner == player.room_id then
            player.winner_times = player.winner_times + 1
            player.appIntegral = player.appIntegral + 50
        end
    end
end

function GameMatch:doGradeInterval()
    self.curStatus = self.Status.GradeInterval
    self.gameGradeIntervalTick = self.curTick

    self:showFinalGrade()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        HostApi.showBuildGrade(v.rakssid, true, false, Messages:msgStartGuessSelf(), 0, GameConfig.gradeTime)
        v.cur_grade = 0
    end

end

function GameMatch:doGuessOver()
    self:doGuessReward()
    HostApi.sendPlaySound(0, 142)
    self.curStatus = self.Status.GuessOverInterval
    self.gameGuessOverIntervalTick = self.curTick
end

function GameMatch:doGuessReward()
    local players = PlayerManager:getPlayers()
    for rank, player in pairs(players) do
        local guess_reward = self:getGuessReward(player.rakssid)
        if guess_reward > 0 then
            player:addMoney(guess_reward)
        end
    end
end

function GameMatch:doReward()
    local players = self:getSortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        ReportManager:reportUserData(player.userId, 0, rank, 1)
    end
    RewardManager:getQueueReward(function(data)
        self:copyPlayers()
        self:sendGuessInfo()
    end)
end

function GameMatch:cleanPlayer()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame()
    end
    GameServer:closeServer()
end

function GameMatch:getGradeScore(grade)
    if grade <= #GameConfig.gradeList and grade >= 1 then
        return GameConfig.gradeList[grade].score
    end
    return 0
end

function GameMatch:getGuessReward(rakssid)
    local guess_right = self:getGuessRight(rakssid)
    if guess_right then
        return GameConfig.guessRightReward
    else
        return 0
    end
end

function GameMatch:getGuessRight(rakssid)
    local Players = self:getSortPlayerRank()

    local rank = 0
    local winner = self:getWinner()
    local guess_right = false
    for k, v in pairs(Players) do
        if v.rakssid == rakssid then
            if v.guess_room_id == 0 then
                v.guess_room_id = v.room_id
            end
            rank = k
            if winner == v.guess_room_id then
                guess_right = true
            end
            break
        end
    end

    return guess_right, rank
end

function GameMatch:getGuessName(rakssid)
    local Players = self:getSortPlayerRank()

    local guess_room_id = 0
    for k, v in pairs(Players) do
        if v.rakssid == rakssid then
            guess_room_id = v.guess_room_id
        end
    end

    for k, v in pairs(Players) do
        if v.room_id == guess_room_id then
            return v.name
        end
    end

    return ""
end

function GameMatch:showBuildGuessResult(rakssid)
    if not self.hasPlaySoundFire then
        HostApi.sendPlaySound(rakssid, 143)
        self.hasPlaySoundFire = true
    end
    local guess_right, rank = self:getGuessRight(rakssid)
    HostApi.log("showBuildGuessResult " .. tonumber(tostring(rakssid)) .. " " .. rank)

    HostApi.showBuildGuessResult(rakssid, guess_right, rank)
end

function GameMatch:guessVisit(rakssid, userId)
    local targetPos = nil
    local yaw = nil
    for i, v in pairs(self.rankPlayers) do
        if userId == v.userId then
            targetPos, yaw = self:getTargetPos(v.room_id)
            break
        end
    end

    if targetPos and yaw then
        local player = self:getPlayerByRakssid(rakssid)
        if player ~= nil then
            player:teleportPosByRecord(targetPos, yaw)
        end
    end
end

function GameMatch:guessSuc(rakssid, room_id)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if rakssid == v.rakssid then
            v.guess_room_id = room_id
            HostApi.showBuildGuessUi(rakssid, v.guess_room_id, json.encode(self:getGuessInfo(rakssid)))
            break
        end
    end

    for k, v in pairs(self.rankPlayers) do
        if rakssid == v.rakssid then
            v.guess_room_id = room_id
        end
    end
end

function GameMatch:getTargetPos(room_id)
    if room_id > 0 and room_id <= #GameConfig.targetPos then
        local pos = GameConfig.targetPos[room_id].pos
        local offset = GameConfig.targetPos[room_id].offset
        local xOffset = math.random(-offset[1], offset[1])
        local yOffset = math.random(-offset[2], offset[2])
        local zOffset = math.random(-offset[3], offset[3])
        local yaw = GameConfig.targetPos[room_id].yaw

        return VectorUtil.newVector3i(pos[1] + xOffset, pos[2] + yOffset, pos[3] + zOffset), yaw
    end

    return nil, nil
end

function GameMatch:quitRoom(player)
    if self.room_pool and player then
        for k, v in pairs(self.room_pool) do
            if player.room_id == v.room_id then
                v.has_player = false
            end
        end
    end
end

function GameMatch:getRoomFormPool(userId)
    for k, v in pairs(self.room_pool) do
        if v.has_player == false then
            v.has_player = true
            v.can_grade = true
            return v.room_id
        end
    end
    HostApi.log("error getRoomFormPool " .. userId)
    return 0
end

function GameMatch:getNextGradeRoom()
    for k, v in pairs(self.room_pool) do
        if v.has_grade == false and v.can_grade == true then
            HostApi.log("Grade a room " .. v.room_id)
            v.has_grade = true
            return v.room_id, false
        end
    end

    HostApi.log("Grade all room")
    return 0, true
end

function GameMatch:setCurGradeRoom(room_id)
    for k, v in pairs(self.room_pool) do
        if v.room_id == room_id then
            v.has_grade = true
        end
    end
end

function GameMatch:resetMap()
    HostApi:resetMap()
end

function GameMatch:resetQuestion()
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    self.question_id = math.random(GameConfig.questionRange[1], GameConfig.questionRange[2])
end

function GameMatch:resetGradeList()
    self.grade_list = {}
    self.score_list = {}
end

function GameMatch:resetRoom()
    self.room_pool = {}
    for k, v in pairs(GameConfig.targetPos) do
        local item = {}
        item.has_player = false
        item.has_grade = false
        item.can_grade = false
        item.room_id = v.id
        table.insert(self.room_pool, item)
    end
end

function GameMatch:resetGame()
    self:resetMap()
    self:resetQuestion()
    self:resetGradeList()
    self:resetRoom()
    self.gradePlayer = 0
    self.curStatus = self.Status.WaitingPlayer
    self:showBuildWarBtn(true)
    -- self:resetPlayer()
    -- HostApi.sendGameStatus(0, 2)
    MsgSender.sendMsg(IMessages:msgGameResetHint())
    self.isFirstReady = true
    self.hasPlaySoundFire = false
end

function GameMatch:resetPlayer()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:reset()
    end
end

function GameMatch:getWinner()
    local players = self:getSortPlayerRank()

    if players and #players > 0 then
        return players[1].room_id, players[1].name
    end

    HostApi.log("GameMatch:getWinner error")

    return nil, nil
end

function GameMatch:getGuessInfo(rakssid)
    local info = {}
    info.player_info = {}
    for k, v in pairs(self.rankPlayers) do
        local item = {}
        item.room_id = v.room_id
        item.name = v.name
        item.platformUid = tonumber(tostring(v.userId))
        table.insert(info.player_info, item)
    end

    info.grade_room = {}
    local player = self:getPlayerByRakssid(rakssid)
    if player ~= nil then
        info.grade_room = player:getHighGrade()
    end

    table.sort(info.player_info, function(a, b)
        if a.room_id ~= b.room_id then
            return a.room_id < b.room_id
        end
        return a.platformUid < b.platformUid
    end)

    HostApi.log("getGuessInfo")
    return info
end

function GameMatch:sendGuessInfo()
    local players = PlayerManager:getPlayers()
    for k, v in pairs(players) do
        HostApi.showBuildGuessUi(v.rakssid, v.guess_room_id, json.encode(self:getGuessInfo(v.rakssid)))
    end
end

function GameMatch:showBuildWarResult(rakssid)
    local Players = self:getSortPlayerRank()

    local players = {}
    local winner = self:getWinner()

    if winner == nil then
        return
    end

    for rank, player in pairs(Players) do
        local isWin = false
        if winner == player.room_id then
            isWin = true
        end
        players[rank] = self:newSettlementItem(player, rank, isWin)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        local isWin = false
        if winner == player.room_id then
            isWin = true
        end

        if rakssid == player.rakssid then
            HostApi.log("showBuildWarResult " .. tonumber(tostring(player.userId)) .. " " .. player.name)
            result.own = self:newSettlementItem(player, rank, isWin)
            HostApi.sendGameSettlement(player.rakssid, json.encode(result), true)
            self:sendGameSettlementExtraInfo(player.rakssid)
        end
    end
end

function GameMatch:sendGameSettlementExtraInfo(rakssid)
    local Players = self:getSortPlayerRank()

    for rank, player in pairs(Players) do
        if rakssid == player.rakssid then
            local guess_right = self:getGuessRight(player.rakssid)
            local guess_name = self:getGuessName(player.rakssid)
            local guess_reward = self:getGuessReward(player.rakssid)
            HostApi.sendGameSettlementExtra(player.rakssid, guess_right, guess_name, guess_reward)
        end
    end
end

function GameMatch:newSettlementItem(player, rank, isWin)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.kills
    item.build_war_score = player.score_all
    item.user_id = tonumber(tostring(player.userId))
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then
        item.adSwitch = 0
    end
    if isWin then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:getSortPlayerRank()
    return self.rankPlayers
end

function GameMatch:sortPlayerRank()
    if self.rankPlayers then
        table.sort(self.rankPlayers, function(a, b)
            if a.score_all ~= b.score_all then
                return a.score_all > b.score_all
            end
            if a.vip ~= b.vip then
                return a.vip > b.vip
            end
            return a.userId > b.userId
        end)
    end
end

function GameMatch:showBuildWarBtn(sign)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        HostApi.showBuildWarBlockBtn(v.rakssid, sign)
    end
end

function GameMatch:onPlayerQuit(player)
    if self.curStatus < self.Status.GradeOverInterval then
        player.score_all = 0
        player.winner_times = 0
    end
    self:quitRoom(player)
    RankNpcConfig:savePlayerRankScore(player)
    HostApi.showBuildWarPlayerNum(PlayerManager:getPlayerCount(), PlayerManager:getMaxPlayer())
end

function GameMatch:copyPlayers()
    if self.rankPlayers == nil then
        self.rankPlayers = {}
    end
    local players = PlayerManager:getPlayers()
    for k, v in pairs(players) do
        local item = {}
        item.name = v.name
        item.rakssid = v.rakssid
        item.userId = v.userId
        item.userId = v.userId
        item.score_all = v.score_all
        item.gold = v.gold
        item.vip = v.vip
        item.kills = v.kills
        item.hasGet = v.hasGet
        item.available = v.available
        item.room_id = v.room_id
        item.scorePoints = {}
        item.scorePoints[ScoreID.KILL] = 0
        item.guess_room_id = v.guess_room_id
        item.inGameTime = v.inGameTime
        item.adSwitch = v.adSwitch

        local insert = true
        for k1, v1 in pairs(self.rankPlayers) do
            if v1.userId == v.userId then
                insert = false
                v1.gold = item.gold
                v1.score_all = item.score_all
                v1.hasGet = item.hasGet
                v1.available = item.available
                v1.guess_room_id = item.guess_room_id
                v1.adSwitch = item.adSwitch
                break
            end
        end

        if insert then
            table.insert(self.rankPlayers, item)
        end
    end
end

function GameMatch:recordGrade(rakssid, grade)
    if self.curStatus ~= self.Status.Grade then
        return
    end

    local player = self:getPlayerByRakssid(rakssid)
    if player ~= nil then
        if player.room_id == self.gradePlayer then
            MsgSender.sendBottomTipsToTarget(rakssid, 1, Messages:msgCannotGradeSelf())
            return
        end

        -- MsgSender.sendMsgToTarget(rakssid, Messages:msgShowYourGrade() + grade - 1)
        player.cur_grade = grade

        HostApi.sendPlaySound(rakssid, 135 + grade - 1)

        local seconds = GameConfig.gradeTime - (self.curTick - self.gameGradeTick)
        HostApi.showBuildGrade(rakssid, false, true, Messages:msgStartGuessSelf(), player:getCurGrade(), seconds)

        for k, v in pairs(self.grade_list) do
            if v.room_id == self.gradePlayer then
                for k1, v1 in pairs(v.grade_list) do
                    if v1.room_id == player.room_id then
                        v1.grade = grade
                        player:insertGradeList(v.room_id, grade)
                    end
                end
            end
        end
    end
end

function GameMatch:getPlayerByRakssid(rakssid)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if (v.rakssid == rakssid) then
            return v
        end
    end
    return nil
end

function GameMatch:getPlayerByEntityId(entityId)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v:getEntityId() == entityId then
            return v
        end
    end
    return nil
end

function GameMatch:getQuestionId()
    if self.question_id == nil then
        return GameConfig.questionRange[1]
    end

    return self.question_id
end

function GameMatch:getDBDataRequire(player)
    DBManager:getPlayerData(player.userId, 1)
end

return GameMatch