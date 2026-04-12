---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:54
---
require "config.DomainConfig"

GameOccupation = class()

function GameOccupation:ctor(domain, initPos, yaw, config, effect)
    self.id = config.id
    self.config = config
    self.effect = effect
    self.selectedMaxNum = GameConfig.occSelectMaxNum
    self.curSelectNum = 0
    self.initPos = initPos
    self.yaw = yaw
    self.domainConfig = domain
    self.domains = {}
    self.players = {}
    self:onCreate()
end

function GameOccupation:onCreate()
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.config.actor, function(entity)
        entity:setHeadName(self.config.name or "")
        entity:setSkillName("idle")
        entity:setHaloEffectName(self.effect or "")
    end)
    self.entityId = entityId
    GameManager:onAddOccupation(self)
end

function GameOccupation:onPlayerHit(player)
    self:onPlayerSelected(player, true)
end

function GameOccupation:onPlayerSelected(player, isHitByPlayer)
    local canSelect = false
    for _, id in pairs(player.baseOccIds) do
        if tonumber(id) == tonumber(self.config.id) then
            canSelect = true
        end
    end
    for _, id in pairs(player.supOccIds) do
        if tonumber(id) == tonumber(self.config.id) then
            canSelect = true
        end
    end
    for _, id in pairs(player.occGiftPack_common) do
        if tonumber(id) == tonumber(self.config.id) then
            canSelect = true
        end
    end
    if player.giftpackOccIds then
        canSelect = true
    end

    if player.privilege[tostring(self.config.id)] then
        canSelect = true
    end

    if player.occupationId ~= 0 then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:hasDomainHint())
        return
    end

    if self.curSelectNum >= self.selectedMaxNum then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    if canSelect == false then
        self:sendOccupationUnlock(player)
        return
    end

    if isHitByPlayer == true then
        HostApi.sendCustomTip(player.rakssid, self.config.hint, "Occupation=" .. tostring(self.entityId))
    else
        self:selectOccupation(player)
    end
end

function GameOccupation:resetOcc(player)
    for i, userId in pairs(self.players) do
        if userId == player.userId then
            table.remove(self.players, i)
            table.remove(self.domains, i)
            self.curSelectNum = self.curSelectNum - 1
            self:onCreate()
        end
    end
end

function GameOccupation:onRemove()
    EngineWorld:removeEntity(self.entityId)
    GameManager:onRemoveOccupation(self)
end

function GameOccupation:selectOccupation(player)
    if self.curSelectNum >= self.selectedMaxNum then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return false
    end
    self.curSelectNum = self.curSelectNum + 1
    self.players[self.curSelectNum] = player.userId
    self.domains[self.curSelectNum] = GameDomain.new(self, self.domainConfig, DomainConfig:getMaxBuildValue())
    self.domains[self.curSelectNum]:onPlayerSelect(player)
    if self.curSelectNum >= self.selectedMaxNum then
        self:onRemove()
    end
    return true
end

function GameOccupation:sendOccupationUnlock(player)
    local info = OccupationConfig:getUnlockOccupationInfoById(self.config.id)
    if not info then
        return
    end
    local data = {
        id = tonumber(self.config.id),
        entityId = tonumber(self.entityId),
        name = self.config.name,
        actorName = self.config.actor,
        singlePrice = info.singlePrice,
        perpetualPrice = info.perpetualPrice,
        singleCurrencyType = info.singleCurrencyType,
        perpetualCurrencyType = info.perpetualCurrencyType,
        radarIcon = info.radarIcon,
        orientations = {},
        isFreeUse = false
    }
    if info.orientations1 ~= "#" then
        table.insert(data.orientations, info.orientations1)
    end
    if info.orientations2 ~= "#" then
        table.insert(data.orientations, info.orientations2)
    end
    if info.orientations3 ~= "#" then
        table.insert(data.orientations, info.orientations3)
    end
    if tostring(player.freeOccId) == tostring(self.config.id) then
        data.isFreeUse = true
    end
    HostApi.sendOccupationUnlock(player.rakssid, json.encode(data))
end