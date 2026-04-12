---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:41
---
require "config.Area"
require "config.BasicConfig"
require "data.GameOccupation"
require "data.GameGate"
require "data.GamePrivateResource"
require "data.GameEquipBox"
require "data.GameBuilding"
require "data.GamePortal"
require "data.GamePromoteResource"

GameDomain = class()

function GameDomain:ctor(occupation, config, maxBuildValue)
    self.occupation = occupation
    self.config = config
    self.domainId = self.config.id
    self.player = nil
    self.resource = nil
    self.gate = nil
    self.equipboxs = {}
    self.buildingRecord = {}
    self.buildingNpcRecord = {}
    self.gateBuildingRecord = {}
    self.equipboxBuildingRecord = {}
    self.promoteResourceRecord = {}
    self.portalRecord = {}
    self.curBuildProgress = 0
    self.maxBuildProgress = maxBuildValue
    self.buildTime = os.time()
    self.needSync = false
    self.isbuildOne = true
    self.isbuildTwo = true
    self.cdTime = config.cdTime
    self.healHp = config.healHp
    self.equipBoxBuilders = {}
    self.records = {}
    self.basicId = 0
    for _, tab in pairs(self.config.equipboxs) do
        if tab.floor == 1 then
            table.insert(self.equipBoxBuilders, tab.id)
        end
    end
end

function GameDomain:onCreate(basicId)
    local area = BasicConfig:getAreaByBasic(basicId, self.config.area)
    self.area = Area.new(area)
    self.basicId = basicId
    local pos = BasicConfig:getMetaPosByBasic(basicId)
    local xImage, zImage = BasicConfig:getXandZImageByBasics(basicId)
    EngineWorld:createSchematic(self.config.fileName, pos, xImage, zImage)
    pos = BasicConfig:getPosViByBasic(basicId, self.config.startPos)
    EngineWorld:createSchematic(self.config.baseFileName, pos, xImage, zImage)
    LuaTimer:schedule(function ()
        self:onPrepare()
    end, 1000)
end

function GameDomain:onRemoveOccMeta()
    local pos = BasicConfig:getMetaPosByBasic(self.basicId)
    local xImage, zImage = BasicConfig:getXandZImageByBasics(self.basicId)
    EngineWorld:destroySchematic(self.config.fileName, pos, xImage, zImage)
end

function GameDomain:onPlayerSelect(player)
    if self.player ~= nil then
        return
    end
    self.player = player
    self.player:onSelectDomain(self)
    self.player:onSelectOccuption(self.occupation.config)
end

function GameDomain:resetDomain(player)
    self.occupation:resetOcc(player)
    self.occupation = nil
    self.player = nil
end

function GameDomain:onTick()
    if self.resource then
        self.resource:onTick()
    end
    if self.gate then
        self.gate:onTick()
    end
    for i, record in pairs(self.records) do
        local second = os.time() - record.enterTime
        if second ~= 0 and second % self.cdTime == 0 then
            self:healPlayer(record)
        end
    end
end

function GameDomain:isDomainPlayer(player)
    return self.player == player
end

------------------------------
--domainAreaCheck
function GameDomain:inArea(pos)
    return self.area:inArea(pos)
end

function GameDomain:healPlayer(record)
    local player = PlayerManager:getPlayerByUserId(record.userId)
    if player ~= nil then
        player:addHealth(self.healHp)
    end
end

function GameDomain:addStayAtHomeEffect(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        local occupationInfo = OccupationConfig:getOccupation(player.occupationId)
        for _, stayAtHomeEffect in pairs(occupationInfo.stayAtHomeEffects) do
            local items = StringUtil.split(stayAtHomeEffect, ":")
            local effectId = tonumber(items[1])
            local time = tonumber(items[2])
            local value = tonumber(items[3])
            if effectId ~= 0 and value ~= 0 then
                if effectId == GamePotionEffect.ADD_ATTACK then
                    player.s_addAttack = value
                elseif effectId == GamePotionEffect.ADD_DEFENSE then
                    player.s_addDefense = value
                end
                MsgSender.sendBottomTipsToTarget(rakssid,  5, Messages:msgAddStayAtHomeEffect())
            end
        end
    end
end

function GameDomain:removeStayAtHomeEffect(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        local occupationInfo = OccupationConfig:getOccupation(player.occupationId)
        for _, stayAtHomeEffect in pairs(occupationInfo.stayAtHomeEffects) do
            local items = StringUtil.split(stayAtHomeEffect, ":")
            local effectId = tonumber(items[1])
            local time = tonumber(items[2])
            local value = tonumber(items[3])
            if effectId ~= 0 and value ~= 0 then
                if effectId == GamePotionEffect.ADD_ATTACK then
                    player.s_addAttack = 0
                elseif effectId == GamePotionEffect.ADD_DEFENSE then
                    player.s_addDefense = 0
                end
                MsgSender.sendBottomTipsToTarget(rakssid,  5, Messages:msgRemoveStayAtHomeEffect())
            end
        end
    end
end

function GameDomain:onPlayerEnter(player)
    if self.records[tostring(player.userId)] == nil then
        local record = {}
        record.enterTime = os.time()
        record.userId = tostring(player.userId)
        self.records[tostring(player.userId)] = record
        MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:enterDomainArea())
        self:addStayAtHomeEffect(player.rakssid)
    end
end

function GameDomain:onPlayerQuit(player)
    if self.records[tostring(player.userId)] then
        self.records[tostring(player.userId)] = nil
        MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:quitDomainArea())
        self:removeStayAtHomeEffect(player.rakssid)
    end
end

------------------------------
--privateResource
function GameDomain:onPrepare()
    if self.player == nil then
        return
    end
    self:prepareResource()
    self:prepareBuildings()
end

function GameDomain:prepareResource()
    self.resource = GamePrivateResource.new(self, self.player, self.config.privateResource)
end

function GameDomain:subResourceStock(sub)
    if self.resource then
        self.resource:subResource(sub)
    end
end

function GameDomain:getAllPrivateStock()
    if self.resource then
        return self.resource.stock
    end
    return 0
end

-----------------------------
--NPC
function GameDomain:prepareGate(gate)
    GameGate.new(self, gate)
end

function GameDomain:prepareEquipBoxs(equipbox)
    if self.player ~= nil then
        if self.player.giftpackOccIds then
            GameEquipBox.new(self, equipbox.upgradeConfig)
            return
        end
        for _, id in pairs(self.player.occGiftPack_money) do
            if tonumber(id) == tonumber(self.occupation.id) then
                GameEquipBox.new(self, equipbox.upgradeConfig)
                return
            end
        end
        for i, equip in pairs(self.player.upgradeEquips) do
            if tonumber(equip.occid) == tonumber(self.occupation.id) then
                for _, type in pairs(equip.type) do
                    if tonumber(equipbox.type) == tonumber(type) then
                        GameEquipBox.new(self, equipbox.upgradeConfig)
                        return
                    end
                end
            end
        end
        GameEquipBox.new(self, equipbox)
    end
end

function GameDomain:preparePortal(portal)
    GamePortal.new(self, self.player, portal)
end

function GameDomain:preparePromote(build)
    GamePromoteResource.new(self, build)
end

function GameDomain:prepareBuildings()
    self.buildingRecord["0"] = true
    self:checkBuildings()
end

function GameDomain:checkBuildings()
    self:showBuildableBuilding()
    self:showGateAbleBuilding()
    self:showEquipboxAbleBuilding()
    self:showPortalableBuilding()
    self:showPromoteBuilderAbleBuilding()
end

---------------------------------------------
function GameDomain:addBuildingRecord(buildingId)
    self.buildingRecord[buildingId] = true
end

function GameDomain:hasBuilding(buildingId)
    return self.buildingRecord[buildingId] or false
end
-------------------------------
--building
function GameDomain:addBuildingNpcRecord(buildingId)
    self.buildingNpcRecord[buildingId] = true
end

function GameDomain:hasBuildingNpc(buildingId)
    return self.buildingNpcRecord[buildingId] or false
end

function GameDomain:showBuildableBuilding()
    for _, building in pairs(self.config.buildings) do
        if self:hasBuildingNpc(building.id) == false then
            local canShow = true
            for _, buildingId in pairs(building.buildingIds) do
                if self:hasBuilding(buildingId) == false then
                    canShow = false
                end
            end
            if canShow then
                GameBuilding.new(self, building)
            end
        end
    end
end
-----------------------------
--Gate
function GameDomain:addGateBuildingRecord(buildingId)
    self.gateBuildingRecord[buildingId] = true
end

function GameDomain:hasGateBuilding(buildingId)
    return self.gateBuildingRecord[buildingId] or false
end

function GameDomain:showGateAbleBuilding()
    if self:hasGateBuilding(self.config.gate.id) == false then
        local canShou = true
        for _, buildingId in pairs(self.config.gate.buildingIds) do
            if self:hasBuilding(buildingId) == false then
                canShou = false
            end
        end
        if canShou then
            self:prepareGate(self.config.gate)
        end
    end
end

------------------------------
--equipbox
function GameDomain:addEquipboxBuildingRecord(buildingId)
    self.equipboxBuildingRecord[buildingId] = true
    for index = 1, #self.equipBoxBuilders do
        local id = self.equipBoxBuilders[index]
        if id == buildingId then
            table.remove(self.equipBoxBuilders, index)
            break
        end
    end
end

function GameDomain:hasEquipboxBuilding(buildingId)
    return self.equipboxBuildingRecord[buildingId] or false
end
--创建剩余的  装备柜
function GameDomain:CreateRemainingEquipBox()
    local count = 0
    for _, equipbox in pairs(self.config.equipboxs) do
        local isCreate = false
        for index = 1, #self.equipBoxBuilders do
            if equipbox.id == self.equipBoxBuilders[index] then
                isCreate = true
                break
            end
        end
        if isCreate then
            local equip = GameEquipBox.new(self, equipbox)
            equip:buildEquipBox()
            equip.builder:onRemove()
            count = count + 1
        end
    end
end

function GameDomain:showEquipboxAbleBuilding()
    for _, equipbox in pairs(self.config.equipboxs) do
        if self:hasEquipboxBuilding(equipbox.id) == false then
            local canShow = true
            for _, buildingId in pairs(equipbox.buildingIds) do
                if self:hasBuilding(buildingId) == false then
                    canShow = false
                end
            end
            if canShow then
                self:prepareEquipBoxs(equipbox)
            end
        end
    end
end

------------------------------------
--promoteResourceRecord
function GameDomain:addPromoteResourceRecord(buildingId)
    self.promoteResourceRecord[buildingId] = true
end

function GameDomain:hasPromoteResourceBuilding(buildingId)
    return self.promoteResourceRecord[buildingId] or false
end

function GameDomain:showPromoteBuilderAbleBuilding()
    for _, build in pairs(self.config.privateResource.builds) do
        if self:hasPromoteResourceBuilding(build.id) == false then
            local canPromote = true
            for _, buildingId in pairs(build.buildingIds) do
                if self:hasBuilding(buildingId) == false then
                    canPromote = false
                end
            end
            if canPromote then
                self:preparePromote(build)
                self:addPromoteResourceRecord(build.id)
            end
        end
    end
end

-------------------------------------
--portalRecord
function GameDomain:addPortalRecord(buildingId)
    self.portalRecord[buildingId] = true
end

function GameDomain:hasPortalBuilding(buildingId)
    return self.portalRecord[buildingId] or false
end

function GameDomain:showPortalableBuilding()
    if self:hasPortalBuilding(self.config.portal.id) == false then
        local canShou = true
        for _, buildingId in pairs(self.config.portal.buildingIds) do
            if self:hasBuilding(buildingId) == false then
                canShou = false
            end
        end
        if canShou then
            self:preparePortal(self.config.portal)
        end
    end
end

--------------------------------------------
function GameDomain:addBuildProgress(progress)
    self.needSync = true
    self.buildTime = os.time()
    self.curBuildProgress = self.curBuildProgress + progress
    if self.curBuildProgress >= self.maxBuildProgress then
        GameMatch:doGameOver()
    end
    GameManager:showFastestBuilding()
    self.needSync = false
    if self:getBuildProgress() >= self.config.buildTwo and self.isbuildTwo then
        MsgSender.sendCenterTips(2, Messages:succeedBuildingTwo(self:getDomainPlayer():getDisplayName()))
        self.isbuildTwo = false
        return
    elseif self:getBuildProgress() >= self.config.buildOne and self.isbuildOne then
        MsgSender.sendCenterTips(2, Messages:succeedBuildingOne(self:getDomainPlayer():getDisplayName()))
        self.isbuildOne = false
        return
    end
end

function GameDomain:getRespwanPos()
    if self.basicId ~= 0 then
        return BasicConfig:getPosByBasic(self.basicId, self.config.respwanPos)
    end
    return self.config.respwanPos
end

function GameDomain:getBuildProgress()
    return self.curBuildProgress
end

function GameDomain:getMaxProgress()
    return self.maxBuildProgress
end

function GameDomain:getBuildTime()
    return self.buildTime
end

function GameDomain:getGoBackExitPos()
    if self.basicId ~= 0 then
        return BasicConfig:getPosByBasic(self.basicId, self.config.portal.gobackExitPos)
    end
    return self.config.portal.gobackExitPos
end

function GameDomain:getDomainPlayer()
    return self.player
end

function GameDomain:getDimains()
    return self
end

return GameDomain