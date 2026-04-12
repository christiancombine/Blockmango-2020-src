GameOverStatus = {}

function GameOverStatus:onCreate(process)
    local settlementData = process:sortPlayer()

    process.mapManager:stopCreateMonster()
    process:broadCastPlayAgainStatus()
    for _, path in pairs(process.mapManager.pathList) do
        for _, userId in pairs(path.protectUserId) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                local passTick = process.processTotalTick - process.startTick - 1
                GamePacketSender:sendTowerDefenseSettlement(player.rakssid, settlementData)
                GamePacketSender:sendTowerDefenseReward(player.rakssid, process.roundNum, passTick, process.rewardMonster, process.rewardBoss, player:getGoldNum(true), #process.roundConfig, process.checkPointConfig.id)
                ManagerPacketSender:sendMapManagerDestroy(player.rakssid)
            end
        end
    end
    process.mapManager:onDestroy()
end

function GameOverStatus:onTick(process)
    if process.processTick == 1 then
        self:onCreate(process)
    end

    if process.processTick > GameConfig.GameOverTime then
        self:stop(process)
    end

end

function GameOverStatus:stop(process)
    if TableUtil.isEmpty(process.playAgainPlayerList) then
        for _, userId in pairs(process.playerList) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player ~= nil then
                HallManager:addEnterRoomQueue(player)
            end
        end
        process:onDestroy()
    else
        for _, userId in pairs(process.playerList) do
            if not TableUtil.include(process.playAgainPlayerList, userId) then
                local player = PlayerManager:getPlayerByUserId(userId)
                if player ~= nil then
                    process:subPlayer(player)
                    HallManager:addEnterRoomQueue(player)
                end
            end
        end
        process:onPlayAgain()
    end
end

return GameOverStatus