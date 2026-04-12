---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:54
---
GameOccupation = class()

function GameOccupation:ctor(config)
    self.config = config
    self.occupationId = config.id
    local entityId = EngineWorld:addActorNpc(config.initPos, config.yaw, config.actor, function(entity)
        entity:setHeadName(config.name or "")
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
    OccupationConfig:onAddOccNpc(self)
end

function GameOccupation:onPlayerHit(player)
    HostApi.sendCustomTip(player.rakssid, self.config.hint, "Occupation=" .. tostring(self.entityId))
end

function GameOccupation:onPlayerSelected(player)
    if player.occupationId == 0 then
        player:onSelectRole(self.config)
        MsgSender.sendMsgToTarget(player.rakssid, Messages:selectRoleHint(self.config.displayName))
    else
        MsgSender.sendTopTipsToTarget(player.rakssid, 3, Messages:selectRoleAgain())
    end
end