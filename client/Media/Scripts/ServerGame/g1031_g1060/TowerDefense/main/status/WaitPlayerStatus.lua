WaitPlayerStatus = {}

function WaitPlayerStatus:onCreate(process)
end

function WaitPlayerStatus:onTick(process)
    if process.processTick == 1 then
        self:onCreate(process)
    end
    if #process.playerList >= process.teammateNum or #process.playerList >= process.maxPlayerNum or process.processTick > GameConfig.WaitTeamMateTime then
        self:stop(process)
    end
end

function WaitPlayerStatus:stop(process)
    process.startTick = process.processTotalTick
    process:updateStatus(Define.GameStatus.GamePrepare)
    process.startPlayerNum = #process.playerList
    process:broadHomeHp()
    for _, userId in pairs(process.playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        player.Wallet:addMoney(Define.Money.GameGold, process.checkPointConfig.initMoney)
        player.totalMoney = player.totalMoney + process.checkPointConfig.initMoney
        for _, teammateUserId in pairs(process.playerList) do
            local teammate = PlayerManager:getPlayerByUserId(teammateUserId)
            if player ~= teammate then
                GamePacketSender:sendTowerDefensePlayerList(player.rakssid, teammate.userId, teammate.name)
            end
        end
    end
end

return WaitPlayerStatus