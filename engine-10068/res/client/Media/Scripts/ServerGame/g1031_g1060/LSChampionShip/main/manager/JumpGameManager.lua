JumpGameManager = {}
JumpGameManager.players = {}
JumpGameManager.jumpCd = 10

function JumpGameManager:addPlayer(player, gameType)
    LogUtil.log("JumpGameManager:addPlayer player.rakssid : "..tostring(player.rakssid).."gameType : "..tostring(gameType))
    EngineUtil.sendEnterOtherGame(player.rakssid, gameType, player.userId)
    self.players[tostring(player.userId)] = { rakssid = player.rakssid, time = os.time(), goToGameType = gameType }
end

function JumpGameManager:onTick()
    self:checkPlayers()
    self:tryPlayerMatching()
end

function JumpGameManager:checkPlayers()
    for userId, player in pairs(self.players) do
        if PlayerManager:getPlayerByRakssid(player.rakssid) == nil then
            self.players[userId] = nil
        end
    end
end

function JumpGameManager:tryPlayerMatching()
    for _, player in pairs(self.players) do
        if os.time() - player.time > self.jumpCd then
            player.time = os.time()
            local c_player = PlayerManager:getPlayerByRakssid(player.rakssid)
            if c_player ~= nil then
                EngineUtil.sendEnterOtherGame(c_player.rakssid, player.goToGameType, c_player.userId, "")
            end
        end
    end
end

return JumpGameManager