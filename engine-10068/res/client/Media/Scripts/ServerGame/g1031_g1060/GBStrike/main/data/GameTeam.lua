---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 12:13
---

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.kills = 0

function GameTeam:ctor(config)
    self.id = tonumber(config.id)
    self.name = config.name
    self.pos = VectorUtil.newVector3(config.x, config.y, config.z)
    self.color = config.color
    self.kills = 0
end

function GameTeam:getKillMsgColor()
    if self:isWin() then
        return TextFormat.colorRed
    end
    return TextFormat.colorWrite
end

function GameTeam:addPlayer()
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:getDisplayName()
    return self.color .. self.name
end

function GameTeam:onPlayerQuit()
    self.curPlayers = self.curPlayers - 1
end

function GameTeam:isDeath()
    return self.curPlayers <= 0
end

function GameTeam:onKill()
    self.kills = self.kills + 1
    GameManager:updateTeamKills()
    if self:isWin() then
        GameMatch:doRunningEnd()
    end
end

function GameTeam:isWin()
    return self.kills >= GameConfig.winKill
end

return GameTeam