---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1035"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.WaitingPlayerToStart = 2
GameMatch.Status.ReadyToStart = 3
GameMatch.Status.Start = 4
GameMatch.Status.GameOver = 5
GameMatch.Status.WaitingReset = 6

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.hasStartGame = false

GameMatch.gameWaitTick = 0
GameMatch.gameWaitPlayerToStart = 0
GameMatch.gameReadyToStarTick = 0
GameMatch.gameTick = 0

GameMatch.gameOverTick = 0

GameMatch.curTick = 0

GameMatch.gameTime = 0
GameMatch.leftRoom = 0
GameMatch.findRoomIdList = {}

function GameMatch:initMatch()
    GameTimeTask:start()
    self:resetGame()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifWaitingPlayerEnd()
    end

    if self.curStatus == self.Status.WaitingPlayerToStart then
        self:ifWaitingPlayerToStartEnd()
    end

    if self.curStatus == self.Status.ReadyToStart then
        self:ifReadyToStartEnd()
    end

    if self.curStatus == self.Status.Start then
        self:ifStartOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

end

function GameMatch:saveAllInventoryByMail()
    local players = PlayerManager:getPlayers()
    for k, v in pairs(players) do
        self:saveInventoryByMail(v)
    end
end

function GameMatch:saveInventoryByMail(player)
    if player == nil then
        return
    end
    HostApi.log("saveInventoryByMail")

    if player.ranch_goods then
        local data = {}
        data.items = {}
        data.msg = GameConfig.itemMailContent
        for _, info in pairs(player.ranch_goods) do
            local item = {}
            item.itemId = info.itemId
            item.num = info.itemNum
            table.insert(data.items, item)
        end

        if #data.items > 0 then
            WebService:SendMails(0, player.userId, MailType.Explore, GameConfig.itemMailTilte, data, "g1031")
        end

    end
end

function GameMatch:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    local data = {}
    data.money = player.money
    data.achievements = player.achievements
    DBManager:savePlayerData(player.userId, DBDataListener.TAG_RANCHER_EXPLORE_INFO, data, immediate or false)
end

function GameMatch:ifWaitingPlayerEnd()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            self.gameWaitTick = self.curTick
            RewardManager:startGame()
            self:waitForPlayerToStart()
            self.isFirstReady = false
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
    end
end

function GameMatch:ifWaitingPlayerToStartEnd()
    local seconds = self.curTick - self.gameWaitPlayerToStart
    -- HostApi.log("ifWaitingPlayerToStartEnd " .. seconds)
    if seconds % 10 == 0 then
        MsgSender.sendBottomTips(3, Messages:WaitPlayerToStartHint())
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
        self:startGame()
    end
end

function GameMatch:ifStartOver()

    local ticks = self.curTick - self.gameTick

    local seconds = self.gameTime - ticks

    if seconds <= 10 and seconds > 0 then
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doGameOver()
    end

end

function GameMatch:ifGameOverEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick)

    if seconds <= 0 then

        HostApi.log("ifGameOverEnd " .. GameConfig.gameOverTime)

        self:saveAllInventoryByMail()
        local players = PlayerManager:getPlayers()
        for k, v in pairs(players) do
            EngineUtil.sendEnterOtherGame(v.rakssid, "g1031", v.userId)
        end

        self:cleanPlayer()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.WaitingPlayer
    self.isFirstReady = true
end

function GameMatch:waitForPlayerToStart()
    HostApi.log("waitForPlayerToStart")
    self.curStatus = self.Status.WaitingPlayerToStart
    self.gameWaitPlayerToStart = self.curTick
    self.hasStartGame = true
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendPlaySound(0, 10030)
end

function GameMatch:readyToStart()
    HostApi.log("readyToStart")
    self.curStatus = self.Status.ReadyToStart
    self.gameReadyToStarTick = self.curTick
end

function GameMatch:startGame()
    HostApi.sendPlaySound(0, 142)
    self.curStatus = self.Status.Start
    self.gameTick = self.curTick
    HostApi.showRanchExTip(1, self.gameTime)
    -- HostApi.showRanchExTip(2, self.leftRoom)
    HostApi.showRanchExTip(3, self.leftBox)
    HostApi.log("startGame")
    HostApi.showRanchExTask(true, json.encode(TaskConfig.match_task))
end

function GameMatch:doGameOver()
    if self.curStatus >= self.Status.GameOver then
        return
    end

    HostApi.log("doGameOver")
    
    self.curStatus = self.Status.GameOver
    self.gameOverTick = self.curTick

    HostApi.showRanchExTip(1, 0)
    -- HostApi.showRanchExTip(2, 0)
    HostApi.showRanchExTip(3, 0)
    HostApi.showRanchExTask(false, "")

    local players = PlayerManager:getPlayers()
    for k, v in pairs(players) do
        v:teleInitPos()
    end

    MsgSender.sendBottomTips(5, Messages:GameOver())
end



function GameMatch:cleanPlayer()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame()
    end
    GameServer:closeServer()
end

function GameMatch:resetMap()
    HostApi:resetMap()
end

function GameMatch:resetGame()
    self:resetMap()
    self.curStatus = self.Status.WaitingPlayer
    MsgSender.sendMsg(IMessages:msgGameResetHint())
    self.isFirstReady = true
    self.gameTime = GameConfig.gameTime
    self.leftRoom = RoomConfig:getRoomSize()
    self.leftBox = RoomConfig.boxNum
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
end

function GameMatch:resetPlayer()
    local players = PlayerManager:getPlayers()
    for userId, player in pairs(players) do
        player:reset()
    end
end

function GameMatch:onFindBox()
    self.leftBox = self.leftBox - 1
    HostApi.showRanchExTip(3, self.leftBox)
end

function GameMatch:onFinishTask(rakssid, id, num)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local tip = false
    for k, v in pairs(TaskConfig.match_task.task_list) do
        if id == v.task_param then
            HostApi.log("onFinishTask " .. id .. " " .. num .. " " .. v.task_param .. " " .. v.task_num)
            if v.task_status == TaskConfig.Status.NotFinish then
                if num >= v.task_num then
                    v.task_status = TaskConfig.Status.Finish
                    local addmoney = TaskConfig:getTaskRewardMoneyById(v.task_id)
                    player:addMoney(addmoney)
                    HostApi.showRanchExTask(true, json.encode(TaskConfig.match_task))

                    local addtime = TaskConfig:getTaskRewardTimeById(v.task_id)
                    self.gameTime = self.gameTime + addtime

                    local ticks = self.curTick - self.gameTick
                    local seconds = self.gameTime - ticks

                    HostApi.showRanchExTip(1, seconds)

                    player:onAddAchievement(GamePlayer.Achievement.FinishTask, 1)

                    local inv = player:getInventory()
                    if inv then
                        HostApi.log("consumeInventoryItem " .. id .. " " .. v.task_num)
                        inv:consumeInventoryItem(id, v.task_num)
                    end
                    tip = false
                    break
                end
            else
                tip = true
            end
        end
    end

    if tip then
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:TaskHasFinished())
    end
end

function GameMatch:onFindRoom(player, roomId, roomType)
    if player == nil then
        return
    end

    if self.leftRoom > 0 then

        local hasfound = false

        for k, v in pairs(self.findRoomIdList) do
            if roomId == v then
                hasfound = true
                break
            end
        end

        if hasfound == false then
            self.leftRoom = self.leftRoom - 1
            -- HostApi.showRanchExTip(2, self.leftRoom)

            HostApi.log("onFindRoom: " .. self.leftRoom .. " " .. roomType)

            local players = PlayerManager:getPlayers()
            for k, v in pairs (players) do
                v.find_room = v.find_room + 1
            end

            -- if roomType == 1 then
            --     player:onAddAchievement(GamePlayer.Achievement.FindChanceRoom, 1)
            -- elseif roomType == 2 then
            --     player:onAddAchievement(GamePlayer.Achievement.FindDangerRoom, 1)
            -- elseif roomType == 3 then
            --     player:onAddAchievement(GamePlayer.Achievement.FindWealthRoom, 1)
            -- elseif roomType == 4 then
            --     player:onAddAchievement(GamePlayer.Achievement.FindCageRoom, 1)
            -- end

            table.insert(self.findRoomIdList, roomId)
        end
    end
end

function GameMatch:onPlayerQuit(player)
    self:saveInventoryByMail(player)
    MsgSender.sendMsg(IMessages:msgPlayerLeft(PlayerManager:getPlayerCount()))
end

function GameMatch:getDBDataRequire(player)
    DBManager:getPlayerData(player.userId, DBDataListener.TAG_RANCHER_EXPLORE_INFO)
end

return GameMatch