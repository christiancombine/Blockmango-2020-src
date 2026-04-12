---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 12:08
---
require "config.Area"

GameBloodPool = class()

function GameBloodPool:ctor(config)
    self.teamId = tonumber(config.teamId)
    self.cdTime = tonumber(config.cdTime)
    self.hp = tonumber(config.hp)
    self.area = Area.new(config.area)
    self.records = {}
    self.enemys = {}
end

function GameBloodPool:isSameTeam(player)
    return player:getTeamId() == self.teamId
end

function GameBloodPool:inArea(pos)
    return self.area:inArea(pos)
end

function GameBloodPool:onTick(tick)
    for i, record in pairs(self.records) do
        local second = os.time() - record.enterTime
        if second ~= 0 and second % self.cdTime == 0 then
            self:healPlayer(record)
        end
    end
end

function GameBloodPool:healPlayer(record)
    local player = PlayerManager:getPlayerByUserId(record.userId)
    if player ~= nil then
        player:addHealth(self.hp)
    end
end

function GameBloodPool:onPlayerEnter(player)
    if self.records[tostring(player.userId)] == nil then
        local record = {}
        record.enterTime = os.time()
        record.userId = tostring(player.userId)
        self.records[tostring(player.userId)] = record
        MsgSender.sendBottomTipsToTarget(player.rakssid, 2, Messages:enterBloodPool())
    end
end

function GameBloodPool:onEnemyEnter(player)
    if self.enemys[tostring(player.userId)] == nil then
        local enemy = {}
        enemy.enterTime = os.time()
        self.enemys[tostring(player.userId)] = enemy
    end
end

function GameBloodPool:onPlayerQuit(player)
    if self.records[tostring(player.userId)] then
        self.records[tostring(player.userId)] = nil
        MsgSender.sendBottomTipsToTarget(player.rakssid, 2, Messages:quitBloodPool())
    end
end

function GameBloodPool:clearRecord()
    for i, record in pairs(self.records) do
        record = nil
    end
    self.records = {}
    self.enemys = {}
end