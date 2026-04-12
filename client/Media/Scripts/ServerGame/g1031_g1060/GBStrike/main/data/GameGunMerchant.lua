---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 11:54
---
GameGunMerchant = class()

function GameGunMerchant:ctor(config)
    self.entityId = 0
    self.id = config.id
    self.teamId = tonumber(config.teamId)
    self.name = config.name
    self.actor = config.actor
    self.position = VectorUtil.newVector3(config.x, config.y, config.z)
    self.yaw = tonumber(config.yaw)
    self.startPos = VectorUtil.newVector3(config.startX, config.startY, config.startZ)
    self.endPos = VectorUtil.newVector3(config.endX, config.endY, config.endZ)
    self:onCreate()
end

function GameGunMerchant:onCreate()
    local entityId = EngineWorld:addSessionNpc(self.position, self.yaw, 6, self.actor, function(entity)
        entity:setNameLang(self.name or "")
    end)
    self.entityId = entityId
    GameManager:onAddGunMerchant(self)
end

function GameGunMerchant:onPlayerInGame(player)
    if player:getTeamId() == self.teamId then
        player:setPersonalShopArea(self.startPos, self.endPos)
    end
end

return GameGunMerchant