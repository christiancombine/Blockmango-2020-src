---
--- Created by Jimmy.
--- DateTime: 2018/7/5 0005 10:35
---

GameGuard = class()

function GameGuard:ctor(gate, initPos, yaw, actor, name)
    self.entityId = 0
    self.gate = gate
    self.initPos = initPos
    self.yaw = yaw
    self.actor = actor
    self.name = name or ""
    self:onCreate()
end

function GameGuard:onPlayerHit(player)
    if player ~= self.gate:getPlayer() then
        return
    end
    self.gate:toggleGate()
end

function GameGuard:openDoor()
    if self.gate then
        self.gate:openGate()
    end
end

function GameGuard:onCreate()
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.actor, function(entity)
        entity:setHeadName(self.name)
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
    GameManager:onAddGuard(self)
end

function GameGuard:onRemove()
    EngineWorld:removeEntity(self.entityId)
    GameManager:onRemoveGuard(self)
end

return GameGuard