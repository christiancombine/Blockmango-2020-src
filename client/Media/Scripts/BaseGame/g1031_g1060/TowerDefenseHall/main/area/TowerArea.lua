TowerArea = class("TowerArea", BaseArea)

function TowerArea:init()
    self.towerConfig = TowerConfig:getConfigById(tonumber(self.config.configId))
    self.bulletConfig = BulletConfig:getConfigById(tonumber(self.towerConfig.BulletId))
    self.luaTime = nil
end

function TowerArea:createActor()
    if isClient and #self.towerConfig.ActorName > 7 then
        LuaTimer:scheduleTimer(function()
            local facePos = PlayerManager:getClientPlayer():getPosition()
            local newYaw = self:getYaw(facePos)
            local oldYaw = self.entity.rotationYaw
            while math.abs(oldYaw - newYaw) > 180 do
                if oldYaw > newYaw then
                    newYaw = newYaw + 360
                else
                    newYaw = newYaw - 360
                end
            end
            self.entity.rotationYaw = newYaw
        end, 100)
        return EngineWorld:addActorNpc(self.config.centrePos, self.config.yaw, self.towerConfig.ActorName, function(entity)
            entity:setIgnoreFrustumCheck(true)
            entity:setRenderDistanceWeight(5)
            entity:setSkillName("idle")
            self.entity = entity
        end)
    end
end

function TowerArea:onClick(player)
    if isClient then
        if tostring(player.userId) == tostring(GamePlayer:Instance().userId) then
            ClickSelfTowerEvent:invoke(self.towerConfig.Id)
        end
    end
end

function TowerArea:playerIn(player)
    if not isClient or GamePlayer:Instance().entityId ~= player.entityId then
        return false
    end
    if isClient and self.entity then
        self:attack(player.entityId)
        self.timerKey = LuaTimer:scheduleTimer(function()
            self:attack(player.entityId)
        end, self.towerConfig.AttackSpeed)
    end
    self:setEffect(true)
    return true
end

function TowerArea:playerOut(player)
    if not isClient or GamePlayer:Instance().entityId ~= player.entityId then
        return false
    end
    if isClient and self.entity then
        self.entity:setSkillName("idle")
        LuaTimer:cancel(self.timerKey)
    end
    self:setEffect(false)
    return true
end

function TowerArea:attack(entityId)
    self.entity:setSkillName("attack")
    LuaTimer:schedule(function(throwerId, bulletYawY, targetId, bulletConfig)
        --发射弹道
        EngineWorld:addCreatureBullet(throwerId, targetId, function(entity)
            entity:setEnableTracker(false)
            entity:setYawY(bulletYawY)
            entity:setBox(bulletConfig.bulletBox)
            if #bulletConfig.bulletSprite == 2 then
                entity:setSprite(bulletConfig.bulletSprite[1] or "", bulletConfig.bulletSprite[2] or "")
                entity:setSpriteScale(bulletConfig.bulletSpriteMultiple or 0.5)
            end
            if bulletConfig.bulletEffect ~= "#" then
                entity:setBombEffect(bulletConfig.bulletEffect)
                entity:setEffectScale(bulletConfig.bulletEffectMultiple or 1)
            end
            entity:setSpeedFactor(bulletConfig.bulletSpeed)
        end)
    end, self.towerConfig.AttackTime, nil, self.entityId, self.towerConfig.bulletYawY, entityId, self.bulletConfig)
end

function TowerArea:bulletAttack(targetId)
    local targetEntity = EngineWorld:getEntity(targetId)
    if targetEntity then
        if self.bulletConfig.boomEffect ~= "#" then
            EngineWorld:addSimpleEffect(self.bulletConfig.boomEffect, targetEntity:getPosition(), 0, 2000, self.bulletConfig.boomRange, self.bulletConfig.boomRange, self.bulletConfig.boomRange)
        end
    end
end

function TowerArea:getYaw(facePos)
    local pos = self.config.centrePos
    local vector = math.atan2(facePos.x - pos.x, facePos.z - pos.z)
    return vector / math.pi * 180 * -1
end