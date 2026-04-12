---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 16:47
---
require "config.Area"

GameTransferGate = class()

function GameTransferGate:ctor(config)
    self.teamId = tonumber(config.teamId)
    self.time = tonumber(config.time)
    self.area = Area.new(config.area)
    self.records = {}
    self.enemys = {}
    self.destination = VectorUtil.newVector3(config.destination[1], config.destination[2], config.destination[3])
end

function GameTransferGate:isSameTeam(player)
    return player:getTeamId() == self.teamId
end

function GameTransferGate:inArea(pos)
    return self.area:inArea(pos)
end

function GameTransferGate:onTick(tick)
    for i, record in pairs(self.records) do
        local second = os.time() - record.enterTime
        if second >= self.time then
            self:telePlayerToDestination(record)
        else
            local player = PlayerManager:getPlayerByUserId(record.userId)
            if player then
                MsgSender.sendBottomTipsToTarget(player.rakssid, 2, Messages:transferGateHint(self.time - second))
            end
        end
    end
end

function GameTransferGate:telePlayerToDestination(record)
    local player = PlayerManager:getPlayerByUserId(record.userId)
    if player ~= nil then
        player:teleportPos(self.destination)
    end
    self:removeRecord(record.userId)
end

function GameTransferGate:onPlayerEnter(player)
    if self.records[tostring(player.userId)] == nil then
        local record = {}
        record.enterTime = os.time()
        record.userId = tostring(player.userId)
        self.records[tostring(player.userId)] = record
        MsgSender.sendBottomTipsToTarget(player.rakssid, 2, Messages:enterTransferGate())
    end
end

function GameTransferGate:onEnemyEnter(player)
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
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:msgTransferGateHint())
    end
end

function GameTransferGate:onPlayerQuit(player)
    if self.records[tostring(player.userId)] then
        self.records[tostring(player.userId)] = nil
        MsgSender.sendBottomTipsToTarget(player.rakssid, 2, Messages:quitTransferGate())
    end
end

function GameTransferGate:removeRecord(userId)
    if self.records[userId] then
        self.records[userId] = nil
    end
end

function GameTransferGate:clearRecord()
    for i, record in pairs(self.records) do
        record = nil
    end
    self.records = {}
    self.enemys = {}
end