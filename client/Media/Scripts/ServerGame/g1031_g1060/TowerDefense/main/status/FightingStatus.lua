FightingStatus = {}

function FightingStatus:onCreate(process)
    process.mapManager:start(process.roundNum)
end

function FightingStatus:onTick(process)
    if process.processTick == 1 then
        self:onCreate(process)
    end

    if process.mapManager:checkStop() or process.mapManager:getMapHp() <= 0 then
        self:stop(process)
    end
end

function FightingStatus:stop(process)
    process:ReportRoundMiss()
    if process.roundConfig[process.roundNum + 1] ~= nil and process.mapManager:getMapHp() > 0 then
        process:rewardPlayers(process.roundNum)
        for _, path in pairs(process.mapManager.pathList) do
            for _, userId in pairs(path.protectUserId) do
                local player = PlayerManager:getPlayerByUserId(userId)
                if player then
                    GamePacketSender:sendTowerDefenseRoundPassTip(player.rakssid, process.roundNum,
                            process.roundNum + 1, process.roundConfig[process.roundNum].goldNum,
                            process.roundConfig[process.roundNum + 1].isWarning)
                end
            end
        end
        ---累计奖励
        local roundConfig = process.roundConfig[process.roundNum]
        if roundConfig.isWarning then
            process.rewardBoss = process.rewardBoss + roundConfig.hallGold
        else
            process.rewardMonster = process.rewardMonster + roundConfig.hallGold
        end
        process.rewardExp = process.rewardExp + roundConfig.exp

        process:updateStatus(Define.GameStatus.GamePrepare)
    else
        if process.roundConfig[process.roundNum + 1] == nil and process.mapManager:getMapHp() > 0 and process.playerList ~= nil then
            process:rewardPlayers(process.roundNum)
            for _, userId in pairs(process.playerList) do
                local player = PlayerManager:getPlayerByUserId(userId)
                if player then
                    ReportUtil.reportGameVictory(player, process.checkPointConfig.id)
                    break
                end
            end
            ---累计奖励
            local roundConfig = process.roundConfig[process.roundNum]
            if roundConfig.isWarning then
                process.rewardBoss = process.rewardBoss + roundConfig.hallGold
            else
                process.rewardMonster = process.rewardMonster + roundConfig.hallGold
            end
            process.rewardExp = process.rewardExp + roundConfig.exp

            ---上报通关记录
            local settlementData = process:sortPlayer()
            local value = ""
            for index, playerInfo in pairs(settlementData) do
                value = value .. playerInfo.userId
                if index ~= #settlementData then
                    value = value .. "#"
                end
            end
            local score = process.checkPointConfig.id .. "00000"
            local passTick = process.processTotalTick - process.startTick

            if passTick < 100000 then
                score = process.checkPointConfig.id .. tostring(100000 - passTick)
            end
            MaxRecordHelper:reportOneRecord(Define.RankKey.CheckPointTotal, value, score)
            MaxRecordHelper:reportOneRecord(Define.RankKey.CheckPoint .. process.checkPointConfig.id, value, score)

        elseif process.mapManager:getMapHp() <= 0 and process.playerList ~= nil then
            for _, userId in pairs(process.playerList) do
                local player = PlayerManager:getPlayerByUserId(userId)
                if player then
                    ReportUtil.reportGameFailure(player, process.checkPointConfig.id, process.roundNum)
                    break
                end
            end
        end

        process:updateStatus(Define.GameStatus.GameOver)
    end
end

return FightingStatus