---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 17:55
---
require "config.Area"

GameFlagStation = class()

function GameFlagStation:ctor(config)
    self.entityId = 0
    self.teamId = tonumber(config.teamId)
    self.actor = config.actor
    self.flagPos = VectorUtil.newVector3(config.flagPos[1], config.flagPos[2], config.flagPos[3])
    self.flagYaw = tonumber(config.flagPos[4])
    self.area = Area.new(config.area)
    self.haloEffect = config.haloEffect or ""
    self.buff = {}
    self.buff.id = tonumber(config.buff[1])
    self.buff.time = tonumber(config.buff[2])
    self.buff.lv = tonumber(config.buff[3])
    self.hasFlag = false
    self.no_flag_records = {}
    self.has_flag_records = {}
    self.enemys = {}
end

function GameFlagStation:isSameTeam(player)
    return player:getTeamId() == self.teamId
end

function GameFlagStation:inArea(pos)
    return self.area:inArea(pos)
end

function GameFlagStation:onPlayerEnter(player)
    if self:isSameTeam(player) == false then
        return
    end
    if player.flag == nil then
        self:onNoFlagPlayerEnter(player)
        return
    end
    if self.hasFlag == false then
        player:onHandedInFlag()
        self.hasFlag = true
        local entityId = EngineWorld:addActorNpc(self.flagPos, self.flagYaw, self.actor, function(entity)
            entity:setSkillName("idle")
            entity:setHaloEffectName(self.haloEffect)
        end)
        self.entityId = entityId
        self:addBuffToPlayers()
        MsgSender.sendMsg(Messages:msgHandedInFlag(player:getDisplayName()))
        GameMatch:ifGameOverByFlag()
    else
        self:onPlayerEnterWhenHasFlag(player)
    end
end

function GameFlagStation:onNoFlagPlayerEnter(player)
    if self.hasFlag then
        return
    end
    if self.no_flag_records[tostring(player.userId)] == nil then
        local record = {}
        record.enterTime = os.time()
        record.times = 0
        self.no_flag_records[tostring(player.userId)] = record
    end
    local record = self.no_flag_records[tostring(player.userId)]
    local duration = os.time() - record.enterTime
    if duration >= 30 * record.times then
        record.times = (duration - duration % 30) / 30 + 1
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:msgNoFlagInStationHint())
    end
end

function GameFlagStation:onPlayerEnterWhenHasFlag(player)
    if self.has_flag_records[tostring(player.userId)] == nil then
        local record = {}
        record.enterTime = os.time()
        record.times = 0
        self.has_flag_records[tostring(player.userId)] = record
    end
    local record = self.has_flag_records[tostring(player.userId)]
    local duration = os.time() - record.enterTime
    if duration >= 30 * record.times then
        record.times = (duration - duration % 30) / 30 + 1
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHandFlagHasHint())
    end
end

function GameFlagStation:onEnemyEnter(player)
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
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:msgInEnemyFlagStationHint())
    end
end

function GameFlagStation:addBuffToPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if self:isSameTeam(player) then
            if self.buff.id ~= 0 then
                player:addEffect(self.buff.id, self.buff.time, self.buff.lv)
            end
        end
    end
end

function GameFlagStation:clearRecord()
    self.hasFlag = false
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
    end
    self.entityId = 0
    self.no_flag_records = {}
    self.has_flag_records = {}
    self.enemys = {}
end
