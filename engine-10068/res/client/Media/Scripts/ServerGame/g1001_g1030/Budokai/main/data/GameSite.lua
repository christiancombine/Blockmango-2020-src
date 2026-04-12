---
--- Created by Jimmy.
--- DateTime: 2018/7/24 0024 10:26
---
GameSite = class()

function GameSite:ctor(config)
    self.id = tonumber(config.id)
    self.type = tonumber(config.type)
    self.apos = VectorUtil.newVector3(config.ax, config.ay, config.az)
    self.ayaw = tonumber(config.ayaw)
    self.bpos = VectorUtil.newVector3(config.bx, config.by, config.bz)
    self.byaw = tonumber(config.byaw)
    self:initArea(config)
    self.players = {}
end

function GameSite:initArea(config)
    self.minX = math.min(tonumber(config.startX), tonumber(config.endX))
    self.maxX = math.max(tonumber(config.startX), tonumber(config.endX))
    self.minZ = math.min(tonumber(config.startZ), tonumber(config.endZ))
    self.maxZ = math.max(tonumber(config.startZ), tonumber(config.endZ))
    self.centerPos = VectorUtil.newVector3((self.minX + self.maxX) / 2, self.apos.y, (self.minZ + self.maxZ) / 2)
end

function GameSite:inArea(pos)
    return pos.x >= self.minX and pos.x <= self.maxX and pos.z >= self.minZ and pos.z <= self.maxZ
end

function GameSite:onTick(tick)
    if tick % 3 ~= 0 then
        return
    end
    if self:ifGameEnd() then
        return
    end
    for _, player in pairs(self.players) do
        local pos = player:getPosition()
        if self:inArea(pos) == false then
            player:teleportPos(self.centerPos)
        end
    end
end

function GameSite:addPlayer(player)
    self.players[#self.players + 1] = player
end

function GameSite:hasPlayer(c_player)
    for _, player in pairs(self.players) do
        if c_player == player then
            return true
        end
    end
    return false
end

function GameSite:onPlayerQuit(player)
    if self:isEmpty() then
        return
    end
    if self:hasPlayer(player) == false then
        return
    end
    self:teleportWatch()
    player:onLost()
    self:removePlayer(player)
    if #self.players == 1 then
        local winner = self.players[1]
        winner:onWin()
        self:onMatchEnd(winner, player)
        MsgSender.sendMsgToTarget(winner.rakssid, Messages:msgFleeTip())
        MsgSender.sendMsg(Messages:msgDefeatTip(winner:getDisplayName(), player:getDisplayName()))
    end
    GameMatch:ifGameOver()
end

function GameSite:onPlayerDie(player)
    if self:isEmpty() then
        return
    end
    if self:hasPlayer(player) == false then
        return
    end
    self:teleportWatch()
    player:onLost()
    self:removePlayer(player)
    if #self.players == 1 then
        local winner = self.players[1]
        winner:onWin()
        self:onMatchEnd(winner, player)
        MsgSender.sendMsg(Messages:msgDefeatTip(winner:getDisplayName(), player:getDisplayName()))
    end
    GameMatch:ifGameOver()
end

function GameSite:onMatchEnd(winner, loser)
    MsgSender.sendTopTipsToTarget(winner.rakssid, 200,
    Messages:msgGameData(GameMatch.curRound.roundTip, loser:getDisplayName(), 1))
    MsgSender.sendTopTipsToTarget(loser.rakssid, 200,
    Messages:msgGameData(GameMatch.curRound.roundTip, winner:getDisplayName(), 2))
end

function GameSite:teleportWatch()
    for _, player in pairs(self.players) do
        if GameMatch.curRound ~= nil then
            player:teleportPos(GameMatch.curRound.watchPos)
        end
    end
end

function GameSite:removePlayer(player)
    local index = 0
    for _, mp in pairs(self.players) do
        index = index + 1
        if mp == player then
            table.remove(self.players, index)
            break
        end
    end
end

function GameSite:isEmpty()
    return #self.players == 0
end

function GameSite:onGameStart()
    if self:isEmpty() then
        return
    end
    if #self.players == 1 then
        self:teleportWatch()
        MsgSender.sendTopTipsToTarget(self.players[1].rakssid, 200, Messages:msgNoOpponentTip())
        return
    end
    if #self.players == 2 then
        self.players[1]:teleSitePos(self.apos, self.ayaw)
        self.players[2]:teleSitePos(self.bpos, self.byaw)
        local effect = GameMatch.curRound.effect
        if effect.id ~= 0 then
            self.players[1]:addEffect(effect.id, effect.time, effect.lv)
            self.players[2]:addEffect(effect.id, effect.time, effect.lv)
        end
        MsgSender.sendTopTipsToTarget(self.players[1].rakssid, 200,
        Messages:msgGameData(GameMatch.curRound.roundTip, self.players[2]:getDisplayName(), 0))
        MsgSender.sendTopTipsToTarget(self.players[2].rakssid, 200,
        Messages:msgGameData(GameMatch.curRound.roundTip, self.players[1]:getDisplayName(), 0))
    end
end

function GameSite:onGameEnd()
    if self:isEmpty() then
        return
    end
    if #self.players >= 2 then
        local hp1 = self.players[1]:getHealth()
        local hp2 = self.players[2]:getHealth()
        local winner, loser
        if hp1 > hp2 then
            winner = self.players[1]
            loser = self.players[2]
        elseif hp1 == hp2 then
            local random = math.random(1, 2)
            winner = self.players[random]
            if random == 1 then
                loser = self.players[2]
            else
                loser = self.players[1]
            end
        else
            winner = self.players[2]
            loser = self.players[1]
        end
        winner:onWin()
        loser:onLost()
    end
    GameMatch:ifGameOver()
end

function GameSite:ifGameEnd()
    return #self.players <= 1
end

function GameSite:onGameReset()
    self.players = {}
end

return GameSite