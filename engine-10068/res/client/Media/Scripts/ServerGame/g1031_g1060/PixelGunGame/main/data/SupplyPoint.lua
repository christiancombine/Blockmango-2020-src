require "util.SupplyUtil"

SupplyPoint = class()

function SupplyPoint:ctor(config)
    self.id = config.id
    self.refreshInterval = config.refreshInterval
    self.position = config.position
    self.entityId = 0
    self.setting = nil
    self.time = 0
    self.image = ""
    self:randomSupply(config)
    self.timerKey = 0
    self.isWaitting = false
end

function SupplyPoint:randomSupply(config)
    if #config.supplyIds == 0 then
        self:onReceiver()
        return
    end
    local random = math.random(1, #config.supplyIds)
    self.setting = SupplyConfig:getSupplySettingById(config.supplyIds[random])
    if self.setting == nil then
        self:onReceiver()
        return
    end
    self.time = self.setting.time
    self.image = self.setting.image
    local entityId = EngineWorld:addActorNpc(self.position, 0, self.setting.model, function(entity)
        entity:setSkillName("idle")
        entity:setHaloEffectName(self.setting.effect or "")
        entity:setCanCollided(false)
    end)
    self.entityId = entityId
    self:initArea(tonumber(config.range) or 1)
    SupplyManager:onAddSupplyPoint(self)
end

function SupplyPoint:initArea(range)
    self.minX = self.position.x - range
    self.maxX = self.position.x + range
    self.minY = self.position.y - range - 0.5
    self.maxY = self.position.y + range + 0.5
    self.minZ = self.position.z - range
    self.maxZ = self.position.z + range
end

function SupplyPoint:inArea(x, y, z)
    return x >= self.minX and x <= self.maxX
            and y >= self.minY and y <= self.maxY
            and z >= self.minZ and z <= self.maxZ
end

function SupplyPoint:onPlayerMove(player, x, y, z)
    if self:inArea(x, y, z) then
        if not self.isWaitting then
            self:onReceiver(player)
        end
    elseif self.pickingPlayer and self.pickingPlayer == player.userId then
        GamePacketSender:sendPixelGunChangeWeapon(player.rakssid, "", "", 0)
        self.isWaitting = false
        self.pickingPlayer = nil
    end
end

function SupplyPoint:onReceiver(player)
    SupplyManager:randomSupply(self.refreshInterval)
    if self.setting == nil then
        return
    end

    if self.time == 0 then
        self:onPlayerReceive(player.userId)
        return
    end

    GamePacketSender:sendPixelGunChangeWeapon(player.rakssid, self.image,
            "set:pixel_chicken_main.json image:circle_bg_1", self.time * 1000)
    self.pickingPlayer = player.userId
    self.isWaitting = true
end

function SupplyPoint:onRemove()
    if self.entityId ~= 0 then
        SupplyManager:onRemoveSupplyPoint(self)
        EngineWorld:removeEntity(self.entityId)
        self.entityId = 0
    end
end

function SupplyPoint:getDistance(x, y, z)

end

function SupplyPoint:setWarning(player)

end

function SupplyPoint:onPlayerReceive(userId)
    local pPlayer = PlayerManager:getPlayerByUserId(userId)
    if pPlayer == nil then
        return
    end
    self:onRemove()
    self.isWaitting = false
    if #self.setting.playerEffect > 7 and self.setting.playerEffectTime > 0 then
        pPlayer:addCustomEffect("SupplyPoint" .. self.id, self.setting.playerEffect, self.setting.playerEffectTime)
    end
    SupplyUtil:onPlayerPickup(self.setting.type, self.setting.value, pPlayer)
end