---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 15:36
---
require "config.ScoreConfig"

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.score = 0

function GameTeam:ctor(config)
    self.id = config.id
    self.name = config.name
    self.initPos = config.initPos
    self.area = config.area
    self.color = self:getTeamColor(self.id)
    self.score = 0
    self.flags = 0
    self.enemys = {}
end

function GameTeam:inArea(pos)
    return self.area:inArea(pos)
end

function GameTeam:isSameTeam(player)
    return player:getTeamId() == self.id
end


function GameTeam:onPlayerEnter(player)

end

function GameTeam:onEnemyEnter(player)
    if self.enemys[tostring(player.userId)] == nil then
        local enemy = {}
        enemy.enterTime = os.time()
        enemy.times = 0
        self.enemys[tostring(player.userId)] = enemy
    end
    local enemy = self.enemys[tostring(player.userId)]
    local duration = os.time() - enemy.enterTime
    if duration >= 30 * enemy.times then
        enemy.times = (duration - duration % 30) / 30 + 1
        MsgSender.sendBottomTipsToTarget(player.rakssid, 5, Messages:msgEnemyAreaHint())
    end
end

function GameTeam:getTeamColor(id)
    if id == 1 then
        return TextFormat.colorRed
    elseif id == 2 then
        return TextFormat.colorBlue
    elseif id == 3 then
        return TextFormat.colorGreen
    else
        return TextFormat.colorYellow
    end
end

function GameTeam:addPlayer()
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:subPlayer()
    self.curPlayers = math.max(0, self.curPlayers - 1)
end

function GameTeam:getDisplayName()
    return self.color .. " [" .. self.name .. "]"
end

function GameTeam:reset()
    self.score = 0
    self.flags = 0
    self.enemys = {}
end

function GameTeam:addFlag()
    self.flags = self.flags + 1
    self.score = self.score + ScoreConfig.FLAG_SCORE
    MsgSender.sendTopTips(GameConfig.gameTime, Messages:msgScore(GameMatch.Teams))
end
