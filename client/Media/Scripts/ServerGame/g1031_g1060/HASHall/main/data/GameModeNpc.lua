---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
require "util.GameManager"

GameModeNpc = class()

function GameModeNpc:ctor(config)
    self.entityId = 0
    self.id = config.id
    self.name = config.name or ""
    self.actor = config.actor
    self.initPos = VectorUtil.newVector3(config.x, config.y, config.z)
    self.yaw = tonumber(config.yaw)
    self.gameType = config.gameType
    self.hint = config.hint
    self:onCreate()
end

function GameModeNpc:onCreate()
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.actor, function(entity)
        entity:setHeadName(self.name)
        entity:setSkillName("idle")
        entity:setHaloEffectName("flag_halo_2.effect")
    end)
    self.entityId = entityId
    GameManager:onAddModeNpc(self)
end

function GameModeNpc:onPlayerHit(player)
    HostApi.sendCustomTip(player.rakssid, self.hint, "Tip=" .. tostring(self.entityId))
end

function GameModeNpc:onPlayerSelected(player)
    EngineUtil.sendEnterOtherGame(player.rakssid, self.gameType, player.userId)
end

return GameModeNpc