---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 17:57
---
GameFlag = class()

function GameFlag:ctor(config)
    self.entityId = 0
    self.teamId = tonumber(config.teamId)
    self.time = tonumber(config.time)
    self.actor = config.actor
    self.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    self.yaw = tonumber(config.initPos[4])
    self.flagPart = {}
    self.flagPart.name = config.flagPart[1]
    self.flagPart.id = config.flagPart[2]
    self.buff = {}
    self.buff.id = tonumber(config.buff[1])
    self.buff.time = tonumber(config.buff[2])
    self.buff.lv = tonumber(config.buff[3])
    self.player = nil
end

function GameFlag:isSameTeam(player)
    return player:getTeamId() == self.teamId
end

function GameFlag:onTick(tick)
    if self.player ~= nil then
        return
    end
    if self.entityId ~= 0 then
        return
    end
    if tick ~= 0 and tick % self.time == 0 then
        self:onAddFlag()
    end
end

function GameFlag:onAddFlag()
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.actor, function(entity)
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
end

function GameFlag:onPlayerHit(player)
    if self:isSameTeam(player) then
        return
    end
    if self.entityId == 0 then
        return
    end
    if self.player ~= nil then
        return
    end
    if player.flag ~= nil then
        return
    end
    EngineWorld:removeEntity(self.entityId)
    player:onGetFlag(self)
    self.player = player
    self.entityId = 0
    self:sendHintToOtherPlayers()
    MsgSender.sendMsg(Messages:msgGetFlag(player:getDisplayName()))
end

function GameFlag:sendHintToOtherPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if self:isSameTeam(player) then
            MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgGetFlagEnemyHint(self.player:getDisplayName()))
        else
            if player ~= self.player then
                MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgGetFlagTeammateHint(self.player:getDisplayName()))
            end
        end
    end
end

function GameFlag:onPlayerQuit(player)
    if self.player == nil then
        return
    end
    if self.player == player then
        self.player = nil
    end
end

function GameFlag:onPlayerDie(player)
    if self.player == nil then
        return
    end
    if self.player == player then
        player.flag = nil
        self.player = nil
    end
end

function GameFlag:onPlayerHandedInFlag()
    self.player = nil
end

function GameFlag:clearRecord()
    self.player = nil
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
    end
    self.entityId = 0
end