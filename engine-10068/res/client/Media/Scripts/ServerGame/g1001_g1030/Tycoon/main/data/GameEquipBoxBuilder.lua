---
--- Created by Jimmy.
--- DateTime: 2018/7/10 0010 12:44
---
GameEquipBoxBuilder = class()

function GameEquipBoxBuilder:ctor(equipbox, initPos, yaw, actor, name)
    self.entityId = 0
    self.equipbox = equipbox
    self.initPos = initPos
    self.yaw = yaw
    self.name = name
    self.actor = actor
    self:onCreate()
end

function GameEquipBoxBuilder:onCreate()
    if self.name == "#" then
        self.name = ""
    end
    local name = self.name .. "=($" .. self.equipbox:getPrice() .. ")"
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.actor, function(entity)
        entity:setHeadName(name)
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
    GameManager:onAddEquipBoxBuilder(self)
end

function GameEquipBoxBuilder:isSamePlayer(player)
    return self.equipbox:getPlayer() == player
end

function GameEquipBoxBuilder:onPlayerHit(player)
    if self:isSamePlayer(player) == false then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:noSelfItem())
        return
    end
    local price = self:getPrice()
    local money = player:getCurrency()
    if price <= money then
        self:onBuild(player, price)
    else
        VideoAdHelper:checkShowBuildingAd(player, self)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:noMoney())
    end
end

function GameEquipBoxBuilder:getPrice()
    return self.equipbox:getPrice()
end

function GameEquipBoxBuilder:onBuild(player, price)
    player:subCurrency(price)
    self:onRemove()
    self.equipbox:buildEquipBox()
    MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:buildsucceed())
    table.insert(player.tmepownEquipIds, self.equipbox)
end

function GameEquipBoxBuilder:onRemove()
    EngineWorld:removeEntity(self.entityId)
    GameManager:onRemoveEquipBoxBuilder(self)
end
