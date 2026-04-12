PrepareStatus = {}

function PrepareStatus:onCreate(process)
    process.roundNum = process.roundNum + 1
    if process.bulletCountPerSecond < 10 then
        process.needSyn = true
    else
        process.needSyn = false
    end
    local sortResult = process:sortPlayer()
    for _, userId in pairs(process.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            GamePacketSender:sendTowerDefenseRoundInfo(player.rakssid, process.roundNum, #process.roundConfig, process.roundConfig[process.roundNum].prepareTime, process.needSyn)
            GamePacketSender:sendTowerDefenseSettlement(player.rakssid, sortResult)
        end
    end
end

function PrepareStatus:onTick(process)
    if process.processTick == 1 then
        self:onCreate(process)
        for _, userId in pairs(process.playerList) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                GamePacketSender:sendTowerDefenseWaitTip(player.rakssid, Define.WaitMessageType.OnBegin,
                        0,
                        #process.playerList,
                        process.teammateNum)
            end
        end
    end
    if process.processTick == 5 then
        for _, userId in pairs(process.playerList) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                GamePacketSender:sendTowerDefenseWaitTip(player.rakssid, Define.WaitMessageType.Close, 0, 0)
            end
        end
    end
    if process.processTick > process.roundConfig[process.roundNum].prepareTime then
        self:stop(process)
    end
end

function PrepareStatus:stop(process)
    process:updateStatus(Define.GameStatus.GameFight)
end

return PrepareStatus