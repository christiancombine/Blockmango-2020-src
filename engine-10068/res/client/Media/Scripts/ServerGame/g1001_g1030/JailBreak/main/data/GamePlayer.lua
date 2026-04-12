---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.PoliceConfig"
require "config.CriminalConfig"
require "config.ScoreConfig"
require "config.WeaponConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.ROLE_PENDING = 0  -- 待定
GamePlayer.ROLE_POLICE = 1  -- 警察
GamePlayer.ROLE_CRIMINAL = 2  -- 逃犯
GamePlayer.ROLE_PRISONER = 3  -- 囚犯

GamePlayer.VIDEO_AD_DATA = 10

function GamePlayer:init()

    self:initStaticAttr(0)

    self.role = GamePlayer.ROLE_PENDING
    self.merit = 0
    self.kills = 0
    self.threat = 0
    self.todayWage = 0
    self.money = 0
    self.score = 0
    self.config = {}
    self.location = nil
    self.lastAddWage = os.time()
    self.enterLocationTime = os.time()
    self.vehicles = {}
    self.isLife = true

    self.threatMultiple = 0
    self.killPoliceThreat = 0

    self.subMeritMultiple = 0
    self.subMeritValue = 0
    self.meritMultiple = 0
    self.meritValue = 0
    self.inventory = {}

    self.DBData = {}
    self.DBData.police = {}
    self.DBData.criminal = {}

    self.RankData = {}
    self.RankData.day = {}
    self.RankData.week = {}

    self.gameScore = 0

    self.isFirstTimeGetRankData = false
    self.isCacheRankData = false

    self.oldMapTeleport = false

    self.isWatchRespawnAd = false
    self.dieData = {}

    -- ad data
    self.dayKey = 0
    self.respawnAdTimes = 0
    self.itemAdRecords = {}

    self:teleInitPos()

end

function GamePlayer:selectRoleRequire(role)
    GameMatch.selectRoleTrigger[tostring(self.userId)] = true
    DBManager:getPlayerData(self.userId, role)
    DBManager:getPlayerData(self.userId, GamePlayer.VIDEO_AD_DATA, function(userId, subKey, data)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        player:initVideoAdData(data)
        VideoAdHelper:initShopVideoAd(player)
    end)
end

function GamePlayer:selectRoleFromDBData(oldData, role)
    if self.role ~= 0 then
        return
    end
    if not oldData.__first then
        if role == GamePlayer.ROLE_POLICE then
            self:becomePoliceByData(oldData)
        end
        if role == GamePlayer.ROLE_CRIMINAL then
            self:becomeCriminalByData(oldData)
        end
    else
        if role == GamePlayer.ROLE_POLICE then
            self:becomePolice(true)
        end
        if role == GamePlayer.ROLE_CRIMINAL then
            self:becomeCriminal(true)
        end
    end
    self:useAppProps()

    if self.role == GamePlayer.ROLE_POLICE then
        self:setTeam(GamePlayer.ROLE_POLICE)
    else
        self:setTeam(GamePlayer.ROLE_CRIMINAL)
    end
end

function GamePlayer:initVideoAdData(data)
    if not data.__first then
        self.respawnAdTimes = data.respawnAdTimes or 0
        self.dayKey = data.dayKey or 0
        self.itemAdRecords = data.itemAdRecords or {}
    end
end

function GamePlayer:fillInvetory(inventory)
    if not inventory then
        return
    end
    self:clearInv()
    local armors = inventory.armor
    for i, armor in pairs(armors) do
        armor.meta = 0
        if armor.id > 0 and armor.id < 10000 then
            self:equipArmor(armor.id, armor.meta)
        end
    end

    local guns = inventory.gun
    for i, gun in pairs(guns) do
        gun.num = 1
        gun.meta = 0
        local max = WeaponConfig:getGunMaxBullet(gun.id)
        if gun.bullets then
            if gun.bullets > max then
                gun.bullets = max
            end
            self:addGunItem(gun.id, gun.num, gun.meta, gun.bullets)
        end
        if gun.clipBullet then
            if gun.clipBullet > max then
                gun.clipBullet = max
            end
            self:addGunItem(gun.id, gun.num, gun.meta, gun.clipBullet)
        end
        if gun.totalBullet and gun.totalBullet > 0 then
            local bulletId = WeaponConfig:getBulletIdByWeapon(gun.id)
            self:addItem(bulletId, gun.totalBullet, 0)
        end
    end
    local items = inventory.item
    for i, item in pairs(items) do
        if item.id > 0 and item.id < 10000 then
            if item.meta > 10 then
                item.meta = 0
            end
            self:addItem(item.id, item.num, item.meta)
        end
    end
end

function GamePlayer:becomePolice(isInit)
    self.role = GamePlayer.ROLE_POLICE
    self.config = PoliceConfig:getDataByMerit(self.merit)
    self.meritValue = PoliceConfig:getMeritValue()
    self.meritMultiple = PoliceConfig:getMeritMultiple()
    self.subMeritValue = PoliceConfig:getSubMeritValue()
    self.subMeritMultiple = PoliceConfig:getSubMeritMultiple()
    self.inventory = PoliceConfig:getInventory()
    self:setShowName(self:buildShowName())
    MerchantConfig:prepareStore(self)
    if isInit then
        self:initByNoData()
    end
    for i, chothes in pairs(PoliceConfig.actor) do
        self:changeClothes(chothes.first, chothes.second)
    end
    GameMatch:getPlayerRankData(self)
end

function GamePlayer:becomePoliceByData(data)
    self.merit = data.merit or 0
    self:becomePolice(false)
    self:setCurrency(data.money or 0)
    self:teleportPos(data.position)
    if os.date("%Y-%m-%d", os.time()) == data.addWageDate then
        self.todayWage = data.todayWage or 0
    else
        self.todayWage = 0
    end
    self:fillInvetory(data.inventory)
    self:fillEnderInvetory(data.enderInv)
    self:addVehicles(data.vehicles)
    self:setHealth(data.hp or 20)
    self:setFoodLevel(data.food or 20)
    self.vehicles = data.vehicles
end

function GamePlayer:becomeCriminal(isInit)
    self.config = CriminalConfig:getDataByThreat(self.threat)
    if self.config.lv == 1 then
        self.role = GamePlayer.ROLE_PRISONER
    else
        self.role = GamePlayer.ROLE_CRIMINAL
    end
    self.threatMultiple = CriminalConfig:getThreatMultiple()
    self.killPoliceThreat = CriminalConfig:getKillPoliceThreat()
    self.inventory = CriminalConfig:getInventory()
    self:setShowName(self:buildShowName())
    MerchantConfig:prepareStore(self)
    if isInit then
        self:initByNoData()
    end
    local uid = tostring(self.userId)
    local key = GameMatch:getCriminalKey()
    HostApi.ZRemove(key, uid)
    HostApi.ZIncrBy(key, uid, self.threat * self.threatMultiple)
    GameMatch:getPlayerRankData(self)
end

function GamePlayer:becomeCriminalByData(data)
    self.threat = data.threat or 0
    self:becomeCriminal(false)
    self:setCurrency(data.money or 0)
    self:teleportPos(data.position)
    self:fillInvetory(data.inventory)
    self:fillEnderInvetory(data.enderInv)
    self:addVehicles(data.vehicles)
    self:setHealth(data.hp or 20)
    self:setFoodLevel(data.food or 20)
    self.vehicles = data.vehicles
end

function GamePlayer:initByNoData()
    self:teleInitPos()
    self:resetInv()
    self:setCurrency(0)
    self:resetFood()
end

function GamePlayer:onRespawn()
    self.isLife = true
    if self.isWatchRespawnAd then
        if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
            self:becomePoliceByData(self.dieData)
        else
            self:becomeCriminalByData(self.dieData)
        end
    else
        self.threat = 0
        self.location = nil
        if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
            self:becomePolice(true)
        else
            self:becomeCriminal(true)
        end
        self:addVehicles(self.dieData.vehicles)
        self:fillEnderInvetory(self.dieData.enderInv)
    end
    if self.role == GamePlayer.ROLE_POLICE then
        self:setTeam(GamePlayer.ROLE_POLICE)
    else
        self:setTeam(GamePlayer.ROLE_CRIMINAL)
    end
    self.isWatchRespawnAd = false
end

function GamePlayer:addThreat(threat)
    if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
        return
    end
    self.threat = self.threat + threat
    self.gameScore = self.gameScore + threat * CriminalConfig.threatMultiple
    self:resetConfig()
    self:showGameData()
end

function GamePlayer:subThreat(threat)
    if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
        return
    end
    self.threat = math.max(0, self.threat - threat)
    self:resetConfig()
    self:showGameData()
end

function GamePlayer:addMerit(merit)
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end
    self.merit = self.merit + merit
    self:resetConfig()
    self:showGameData()
end

function GamePlayer:subMerit(merit)
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end
    if self.merit - merit < 0 then
        self:teleConfineRoomPos()
    end
    self.merit = math.max(0, self.merit - merit)
    self:resetConfig()
    self:showGameData()
end

function GamePlayer:addWage()
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end
    if self.config == nil then
        return
    end
    if os.time() == self.lastAddWage then
        return
    end
    if self.todayWage >= self.config.maxWage then
        return
    end
    local wage = math.min(self.config.wage, self.config.maxWage - self.todayWage)
    self.todayWage = self.todayWage + wage
    self:addMoney(wage)
    MsgSender.sendMsgToTarget(self.rakssid, Messages:addWage(wage, self.config.maxWage - self.todayWage))
    self.lastAddWage = os.time()
end

function GamePlayer:addMoney(money)
    self:addCurrency(money)
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
    self:showGameData()
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

function GamePlayer:captureCriminal(criminal)
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end

    if criminal.config.lv < 2 then
        return
    end

    local merit = math.ceil(criminal.threat * self.meritMultiple + self.meritValue)
    local rewardMoney = math.ceil(criminal.threat * criminal.threatMultiple * criminal.config.arrest)
    self.gameScore = self.gameScore + 1

    self:addMerit(merit)
    self:addMoney(rewardMoney)
    self:addScore(ScoreConfig.captureCriminal)
    MsgSender.sendMsg(Messages:captureCriminal(criminal:getDisplayName(), self:getDisplayName()))

    local uid = tostring(criminal.userId)
    local key = GameMatch:getCriminalKey()
    HostApi.ZRemove(key, uid)
    HostApi.ZIncrBy(key, uid, criminal.gameScore)
end

function GamePlayer:killCriminal(criminal)
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end

    if criminal.config.lv < 2 then
        local subMerit = self.subMeritValue
        self:subMerit(subMerit)
        MsgSender.sendMsgToTarget(self.rakssid, Messages:policeKillPrisoner())
        return
    end

    local merit = math.ceil(criminal.threat * self.meritMultiple + self.meritValue)
    local rewardMoney = math.ceil(criminal.threat * criminal.threatMultiple * criminal.config.kill)
    self.gameScore = self.gameScore + 1

    self:addMerit(merit)
    self:addMoney(rewardMoney)
    self:addScore(ScoreConfig.captureCriminal)
    MsgSender.sendMsg(Messages:killCriminal(criminal:getDisplayName(), self:getDisplayName()))

    local uid = tostring(criminal.userId)
    local key = GameMatch:getCriminalKey()
    HostApi.ZRemove(key, uid)
    HostApi.ZIncrBy(key, uid, criminal.gameScore)
    self.kills = self.kills + 1
    self.appIntegral = self.appIntegral + 3
end

function GamePlayer:killPolice(police)
    if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
        return
    end
    local threat = self.killPoliceThreat
    self:addThreat(threat)
    self:addScore(ScoreConfig.killPolice)
    MsgSender.sendMsg(Messages:killPolice(police:getDisplayName(), self:getDisplayName()))
    self.kills = self.kills + 1
    self.appIntegral = self.appIntegral + 1
end

function GamePlayer:onDie()
    self.dieData = GameMatch:getPlayerSaveData(self)
    self.dieData.hp = 20
    self.dieData.food = 20
    self.isLife = false
    self:clearInv()
    VideoAdHelper:tryShowRespawnVideoAd(self)
end

function GamePlayer:resetInv()
    self:clearInv()
    self:sendRefreshActor()
    for i, v in pairs(self.inventory) do
        self:addItem(v.id, v.num, 0)
    end
end

function GamePlayer:sendRefreshActor()
    HostApi.sendRefreshActor(self:getEntityId())
end

function GamePlayer:resetConfig()
    if self.role == GamePlayer.ROLE_POLICE then
        local police = PoliceConfig:getDataByMerit(self.merit)
        if police.lv ~= self.config.lv then
            if self.config.lv < police.lv then
                MsgSender.sendMsgToTarget(self.rakssid, Messages:policeLevelUp(police.name))
            end
            self.config = police
            self:setShowName(self:buildShowName())
        end
    elseif self.role == GamePlayer.ROLE_CRIMINAL or self.role == GamePlayer.ROLE_PRISONER then
        local criminal = CriminalConfig:getDataByThreat(self.threat)
        if criminal.lv ~= self.config.lv then
            if self.config.lv < criminal.lv then
                MsgSender.sendMsgToTarget(self.rakssid, Messages:criminalLevelUp(criminal.name))
            end
            self.config = criminal
            if self.config.lv == 1 then
                self.role = GamePlayer.ROLE_PRISONER
                HostApi.changePlayerTeam(0, self.userId, self.role)
            else
                if self.role ~= GamePlayer.ROLE_CRIMINAL then
                    self.role = GamePlayer.ROLE_CRIMINAL
                    HostApi.changePlayerTeam(0, self.userId, self.role)
                end
            end
            self:setShowName(self:buildShowName())
        end
    end
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

function GamePlayer:resetHP()
    self:changeMaxHealth(20)
end

function GamePlayer:getDisplayName()
    return self.name .. self:getRoleName()
end

function GamePlayer:getRoleName()
    if self.config == nil then
        return ""
    end
    if self.role == GamePlayer.ROLE_POLICE then
        return TextFormat.colorBlue .. "[" .. self.config.name .. "]"
    elseif self.role == GamePlayer.ROLE_CRIMINAL then
        return TextFormat.colorRed .. "[" .. self.config.name .. "]"
    elseif self.role == GamePlayer.ROLE_PRISONER then
        return TextFormat.colorGreen .. "[" .. self.config.name .. "]"
    else
        return ""
    end
end

function GamePlayer:showGameData()
    if self.role == GamePlayer.ROLE_POLICE then
        self:showPoliceInfoGameTip(self.merit)
    elseif self.role == GamePlayer.ROLE_CRIMINAL or self.role == GamePlayer.ROLE_PRISONER then
        self:showCriminalInfoGameTip(self.threat)
    end
end

function GamePlayer:getDisplayName()
    return self.name .. self:getRoleName()
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    local roleName = self:getRoleName()
    if #roleName ~= 0 then
        table.insert(nameList, roleName)
    end

    -- pureName line
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:showPoliceInfoGameTip(merit)
    MsgSender.sendBottomTipsToTarget(self.rakssid, 3600, Messages:showPoliceInfo(merit))
end

function GamePlayer:showCriminalInfoGameTip(threat)
    MsgSender.sendBottomTipsToTarget(self.rakssid, 3600, Messages:showCriminalInfo(threat))
end

function GamePlayer:teleInitPos()
    local pos
    if self.role == GamePlayer.ROLE_POLICE then
        pos = PoliceConfig:randomInitPos()
    elseif self.role == GamePlayer.ROLE_CRIMINAL or self.role == GamePlayer.ROLE_PRISONER then
        pos = CriminalConfig:randomInitPos()
    else
        pos = GameConfig.initPos
    end
    self:teleportPos(pos)
end

function GamePlayer:teleConfineRoomPos()
    local pos = PoliceConfig:randomConfineRoomPos()
    HostApi.resetPos(self.rakssid, pos.x, pos.y + 0.5, pos.z)
end

function GamePlayer:enterLocation(location)
    if self.location == nil then
        self.location = location
        self.enterLocationTime = os.time()
        MsgSender.sendBottomTipsToTarget(self.rakssid, GameConfig.gameTipShowTime, Messages:enterLocation(self.location.name))
    end
end

function GamePlayer:onEnterJail()
    if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
        return
    end
    if self.config.lv == 1 then
        for i, chothes in pairs(CriminalConfig.actor) do
            self:changeClothes(chothes.first, chothes.second)
        end
    end
end

function GamePlayer:onQuitJail()
    if self.role ~= GamePlayer.ROLE_CRIMINAL and self.role ~= GamePlayer.ROLE_PRISONER then
        return
    end
    self:resetClothes()
end

function GamePlayer:openChest(name)
    if name ~= nil and name ~= 0 then
        MsgSender.sendMsg(Messages:getChestLocationName(name))
    end
end

function GamePlayer:policeOpenChestMsg(name)
    if self.role ~= GamePlayer.ROLE_POLICE then
        return
    end

    if name ~= nil and name ~= 0 then
        MsgSender.sendMsgToTarget(self.rakssid, Messages:policeOpenChestMsg())
    end
end

function GamePlayer:quitLocation()
    if self.location ~= nil then
        --MsgSender.sendMsgToTarget(self.rakssid, Messages:quitLocation(self.location.name))
        self:onPrisonBreak()
        self.location = nil
    end
end

function GamePlayer:onPrisonBreak()
    if self.location == nil then
        return false
    end

    if self.location.id == LocationConfig.JAIL and (self.role == GamePlayer.ROLE_CRIMINAL or self.role == GamePlayer.ROLE_PRISONER) then
        if self.config.lv < 2 then
            local config = CriminalConfig:getDataByLevel(2)
            self:addThreat(config.threat)
            self:onQuitJail()
            return true
        end
    end

    return false
end

function GamePlayer:onBreakSand()
    if self.role == GamePlayer.ROLE_CRIMINAL or self.role == GamePlayer.ROLE_PRISONER then
        if self.config.lv < 2 then
            local config = CriminalConfig:getDataByLevel(2)
            self:addThreat(config.threat)
            self:onQuitJail()
        end
    end
end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
    ReportManager:reportUserData(self.userId, self.kills)
end

function GamePlayer:saveVehicle(vehicleId)
    self.vehicles[#self.vehicles + 1] = vehicleId
end

function GamePlayer:setPoliceRank(rank, score)
    local data = {}
    data.rank = rank
    data.userId = tonumber(tostring(self.userId))
    data.score = tonumber(score)
    data.name = self.name
    data.vip = self.vip
    self.RankData.day = data

    local defaultData = {}
    defaultData.rank = 0
    defaultData.userId = tonumber(tostring(self.userId))
    defaultData.score = 0
    defaultData.name = self.name
    defaultData.vip = self.vip
    self.RankData.week = defaultData

    if self.RankData.day.score == 0 then
        self.RankData.day.rank = 0
    else
        GameMatch:addPoliceRank(data, self)
    end

    if self.isFirstTimeGetRankData == false then
        GameMatch:sendRankDataToPlayers()
        self.isFirstTimeGetRankData = true
    end

    if self.isFirstTimeGetRankData then
        self.isCacheRankData = true
    end
end

function GamePlayer:setCriminalRank(rank, score)
    local data = {}
    data.rank = rank
    data.userId = tonumber(tostring(self.userId))
    data.score = tonumber(score)
    data.name = self.name
    data.vip = self.vip
    self.RankData.week = data

    local defaultData = {}
    defaultData.rank = 0
    defaultData.userId = tonumber(tostring(self.userId))
    defaultData.score = 0
    defaultData.name = self.name
    defaultData.vip = self.vip
    self.RankData.day = defaultData

    if self.RankData.week.score == 0 then
        self.RankData.week.rank = 0
    else
        GameMatch:addCriminalRank(data, self)
    end

    if self.isFirstTimeGetRankData == false then
        GameMatch:sendRankDataToPlayers()
        self.isFirstTimeGetRankData = true
    end

    if self.isFirstTimeGetRankData then
        self.isCacheRankData = true
    end
end

return GamePlayer

--endregion


