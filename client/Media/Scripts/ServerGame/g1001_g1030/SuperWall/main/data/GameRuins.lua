---
--- Created by Jimmy.
--- DateTime: 2018/8/23 0023 11:56
---
GameRuins = class()

function GameRuins:ctor(config)
    self.config = config
    self.entityId = 0
    self.teamId = self.config.teamId
    self.id = self.config.ruinsId
    self.name = self.config.ruinsName or ""
    self.actor = self.config.ruinsActor
    self.price = self.config.price
    self.position = self.config.position
    self.yaw = self.config.yaw
    self.canBuild = false
    self.buildTime = 0
    self:onCreate()
end

function GameRuins:onCreate()
    self.entityId = EngineWorld:addCreatureNpc(self.position, self.id, self.yaw, self.actor, function(entity)
        entity:setNameLang(self.name)
    end)
    GameManager:onAddRuins(self)
end

function GameRuins:canReBuild(player)
    if self.canBuild then
        return false
    end
    if self.entityId == 0 then
        return false
    end
    if self.teamId ~= player:getTeamId() then
        return false
    end
    return true
end

function GameRuins:onPlayerHit(player, attackType)
    if attackType ~= 0 then
        return false
    end
    if self:canReBuild(player) == false then
        return false
    end
    HostApi.showConsumeCoinTip(player.rakssid, self.config.tipHint, 1, self.price,
            "Ruins.ReBuild=" .. tostring(self.entityId))
    return false
end

function GameRuins:onPlayerConfirm(player)
    if self:canReBuild(player) == false then
        return
    end
    player:consumeDiamonds(self.id, self.price, "Ruins.ReBuild=" .. tostring(self.entityId))
end

function GameRuins:ReBuild(player)
    self.canBuild = true
    self.buildTime = os.time()
    self:onDeath()
    MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgTowerRebuildTip())
end

function GameRuins:onTick(ticks)
    if self.canBuild == false then
        return
    end
    if os.time() - self.buildTime >= 3 then
        self:buildTower()
    end
end

function GameRuins:buildTower()
    self.canBuild = false
    GameTower.new(self.config)
    GameManager:onRemoveRuins(self)
end

function GameRuins:onDeath()
    if self.entityId ~= 0 then
        EngineWorld:getWorld():killCreature(self.entityId)
        self.entityId = 0
    end
end

function GameRuins:onClear()
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
        self.entityId = 0
        GameManager:onRemoveRuins(self)
    end
end

return GameRuins