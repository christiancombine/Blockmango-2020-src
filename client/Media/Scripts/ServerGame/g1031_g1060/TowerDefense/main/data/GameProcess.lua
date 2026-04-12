GameProcess = class("GameProcess")

function GameProcess:ctor(checkPointId, roomId, teamId, teammateNum)
    self:init(checkPointId, roomId, teamId, teammateNum)
end

function GameProcess:init(checkPointId, roomId, teamId, teammateNum)
    self.processId = roomId                    --房间id

    self.checkPointConfig = CheckPointConfig:getCheckPointConfigById(checkPointId)

    self.maxPlayerNum = self.checkPointConfig.maxPlayerNum
    self.startPlayerNum = 0
    self.teamId = teamId
    self.teammateNum = teammateNum
    self.playerList = {}                            --玩家列表
    self.status = Define.GameStatus.GameWaitPlayer  --游戏状态
    self.roundNum = 0                               --关卡数
    self.roundConfig = RoundConfig:getRoundGroupByCheckPointId(checkPointId)                           --关卡配置

    self.processTick = 0                            --当前状态持续时间
    self.processTotalTick = 0                       --整个进程总持续时间
    self.bulletCountPerSecond = 0                   --每秒子弹数

    self.missMonsterNum = 0

    self.mapManager = MapManager.new(self.checkPointConfig, self.processId)

    self.rewardMonster = 0
    self.rewardBoss = 0
    self.rewardExp = 0

    self.playAgainPlayerList = {}
    self.placeLockGrid = {}

    self.watcherList = {}                          --观战者列表
end

function GameProcess:updateWaitTip()
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            GamePacketSender:sendTowerDefenseWaitTip(player.rakssid, Define.WaitMessageType.Update,
                    GameConfig.WaitTeamMateTime - self.processTick,
                    #self.playerList, self.teammateNum)
        end
    end
end

function GameProcess:addPlayer(player)
    if self.status ~= Define.GameStatus.GameWaitPlayer then
        return false
    end
    if #self.playerList > self.maxPlayerNum or player.hallInfo.teamId ~= self.teamId then
        return false
    end
    player:setProcess(self.processId)
    table.insert(self.playerList, player.userId)
    self.mapManager:addPlayer(player, #self.playerList)
    player:initPaySkillList(self.checkPointConfig.paySkill)
    self:updateWaitTip()
    GamePacketSender:sendTowerDefenseWaitTip(player.rakssid, Define.WaitMessageType.StartShow,
            GameConfig.WaitTeamMateTime - self.processTick,
            #self.playerList, self.teammateNum)
    return true
end

function GameProcess:subPlayer(player)
    for index, userId in pairs(self.playerList) do
        if tostring(userId) == tostring(player.userId) then
            table.remove(self.playerList, index)
            ---结算奖励
            local coinReward = {
                RewardId = Define.Money.HallGold,
                RewardType = tonumber(Define.RewardType.Money) or 0,
                RedressRewardId = 0,
                Count = self.rewardBoss + self.rewardMonster,
                Time = 0,
                IsNotification = false,
                NotificationContent = ""
            }
            GameReward:onPlayerGetReward(player, coinReward)

            local coinReward = {
                RewardId = 0,
                RewardType = tonumber(Define.RewardType.Exp) or 0,
                RedressRewardId = 0,
                Count = self.rewardExp,
                Time = 0,
                IsNotification = false,
                NotificationContent = ""
            }
            GameReward:onPlayerGetReward(player, coinReward)

            self:updateWaitTip()
        end
    end
end

function GameProcess:addWatcher(player)
    table.insert(self.watcherList, player.userId)
    player.entityPlayerMP:setIntProperty("TrackerGroupId", self.processId)
    GamePacketSender:sendTowerDefenseProcessId(player.rakssid, self.processId)
    self.mapManager:addWatcher(player, #self.playerList)

    ---同步现有的塔
    for _, _userId in pairs(self.playerList) do
        local _player = PlayerManager:getPlayerByUserId(_userId)
        if _player then
            for _, StarLevelList in pairs(_player.formulaList) do
                for _, tower in pairs(StarLevelList) do
                    tower:syncTowerToAudience(player)
                end
            end
        end
    end
    return true
end

function GameProcess:subWatcher(player)
    for index, userId in pairs(self.watcherList) do
        if tostring(userId) == tostring(player.userId) then
            table.remove(self.watcherList, index)
        end
    end
end

function GameProcess:rewardPlayers(roundNum)
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            player.Wallet:addMoney(Define.Money.GameGold, self.roundConfig[roundNum].goldNum)
            player.totalMoney = player.totalMoney + self.roundConfig[roundNum].goldNum
            player.passRound = roundNum
        end
    end
end

function GameProcess:onTick(tick)
    if #self.playerList == 0 then
        self:onDestroy()
        return
    end
    self.processTick = self.processTick + 1
    self.processTotalTick = self.processTotalTick + 1
    if self.status == Define.GameStatus.GameWaitPlayer then
        WaitPlayerStatus:onTick(self)
    elseif self.status == Define.GameStatus.GamePrepare then
        PrepareStatus:onTick(self)
    elseif self.status == Define.GameStatus.GameFight then
        FightingStatus:onTick(self)
    elseif self.status == Define.GameStatus.GameOver then
        GameOverStatus:onTick(self)
    end
end

function GameProcess:updateStatus(newStatus)
    self:sortPlayer()
    self.status = newStatus
    self.processTick = 0
end

function GameProcess:sortPlayer()
    local result = {}
    for _, userId in ipairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player ~= nil then
            local playerInfo = {
                userId = tostring(player.userId),
                rank = 0,
                name = player.name or "",
                kill = player.killNum or 0,
                miss = player.missNum or 0,
                maxRound = player.checkPointRecord[player.hallInfo.checkPointId or 0] or 0,
                totalMoney = player.totalMoney or 0
            }
            table.insert(result, playerInfo)
        end
    end
    table.sort(result, function(info1, info2)
        if info1.miss ~= info2.miss then
            return info1.miss < info2.miss
        end
        if info1.kill ~= info2.kill then
            return info1.kill > info2.kill
        end
        if info1.totalMoney ~= info2.totalMoney then
            return info1.totalMoney > info2.totalMoney
        end
        if info1.maxRound ~= info2.maxRound then
            return info1.maxRound > info2.maxRound
        end
        return tonumber(info1.userId) < tonumber(info2.userId)
    end)
    for rank, info in pairs(result) do
        info.rank = rank
    end
    return result

end

function GameProcess:broadCastPacket(key, data)
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            PacketSender:sendLuaCommonData(player.rakssid, key, data)
        end
    end
end

function GameProcess:broadCastSupportInfo(sender, recipient, moneyNum)
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            local lang = player:getLanguage()
            local text = tostring(sender) .. TextFormat.argsSplit .. tostring(recipient) .. TextFormat.argsSplit .. tostring(moneyNum)
            local str = MultiLanTipSetting.getMessageToLua(lang, 1056001, text)
            GamePacketSender:sendTowerDefenseBroadcastInfo(player.rakssid, str)
        end
    end
end

function GameProcess:broadHomeHp()
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            GamePacketSender:sendTowerDefenseHomeHp(player.rakssid, self.mapManager:getMapInitHp(), self.mapManager:getMapHp())
        end
    end
end

function GameProcess:useProps(propsId, userId)
    self.mapManager.propsTrigger:useProps(propsId, userId)

    --broadCast
    for _, pUserId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(pUserId)
        if player then
            GamePacketSender:sendUseProps(player.rakssid, propsId, userId)
        end
    end

    for _, pUserId in pairs(self.watcherList) do
        local Watcher = PlayerManager:getWatcherByUserId(pUserId)
        if Watcher then
            GamePacketSender:sendUseProps(Watcher.rakssid, propsId, userId)
        end
    end

    local propStr = SkillStoreConfig:getNameById(propsId)

    self:broadCastScreenEffect(propsId, userId)
    self:broadCastUsePropTip(userId, propStr)
end

function GameProcess:broadCastUsePropTip(userId, propStr)
    local player1 = PlayerManager:getPlayerByUserId(userId)
    for _, pUserId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(pUserId)
        if player then
            GamePacketSender:sendTowerDefenseUsePropInfo(player.rakssid, player1.name, propStr)
        end
    end
end

function GameProcess:broadCastScreenEffect(propsId, userId)
    local propConfig = PropsConfig:getConfigById(propsId)
    if propConfig.targetType == PropsTrigger.TargetType.Monster
            or propConfig.targetType == PropsTrigger.TargetType.Self then
        for _, pUserId in pairs(self.playerList) do
            local player = PlayerManager:getPlayerByUserId(pUserId)
            if player then
                GamePacketSender:sendShowScreenEffect(player.rakssid, propsId, true)
            end
        end
    elseif propConfig.targetType == PropsTrigger.TargetType.Tower then
        local player = PlayerManager:getPlayerByUserId(userId)
        GamePacketSender:sendShowScreenEffect(player.rakssid, propsId, true)
    end
end

function GameProcess:broadCastPlayAgainStatus()
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            GamePacketSender:sendTowerDefensePlayAgainStatus(player.rakssid, #self.playAgainPlayerList, #self.playerList)
        end
    end
end

function GameProcess:requestPlayAgain(userId, isPlayAgain)
    if not TableUtil.include(self.playerList, userId) or self.status ~= Define.GameStatus.GameOver then
        return
    end
    if isPlayAgain == true then
        for i, playAgainUserId in pairs(self.playAgainPlayerList) do
            if playAgainUserId == userId then
                break
            end
            if i == #self.playAgainPlayerList then
                table.insert(self.playAgainPlayerList, userId)
            end
        end
        if #self.playAgainPlayerList == 0 then
            table.insert(self.playAgainPlayerList, userId)
        end
    else
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            self:subPlayer(player)
        end
        for i, playAgainUserId in pairs(self.playAgainPlayerList) do
            if playAgainUserId == userId then
                table.remove(self.playAgainPlayerList, i)
                break
            end
        end
    end
    self:broadCastPlayAgainStatus()

    if #self.playAgainPlayerList == #self.playerList then
        self:onPlayAgain()
    end
end

function GameProcess:onPlayAgain()
    for _, userId in pairs(self.playAgainPlayerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            player:onPlayAgainInit()
        end
    end
    ProcessManager:reset(self.checkPointConfig.id, self.processId, self.teamId, #self.playAgainPlayerList, self.playAgainPlayerList)
    --重新开始游戏
end

function GameProcess:onDestroy()
    BaseGridListManager:resetGridList(self.processId)
    self.mapManager:onDestroy()
    for _, userId in pairs(self.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            self:subPlayer(player)
        end
    end
    self.playerList = {}                            --玩家列表
    self.roundConfig = {}                           --关卡配置
    self.playAgainPlayerList = {}
    ProcessManager:closeProcess(self.processId)
end

function GameProcess:onMissMonster()
    self.missMonsterNum = self.missMonsterNum + 1
end

function GameProcess:ReportRoundMiss()
    if self.missMonsterNum ~= 0 then
        for _, userId in pairs(self.playerList) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                ReportUtil.reportMissMonster(player, self.checkPointConfig.id, self.roundNum, self.missMonsterNum)
                break
            end
        end
    end

    self.missMonsterNum = 0
end

return GamePlayer