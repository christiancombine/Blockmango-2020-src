---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.GunsConfig"
require "config.ForgeProbabilityConfig"
require "config.ForgeGroupConfig"
require "config.SprayPaintConfig"
require "config.SpecialClothingConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.money = 0
    self.guns = {}
    self.score = 0
    self.teamId = 1
    self.gameType = nil
    self.gunSprayPaints = {}
    self.specialClothing = {}
    self.moveHash = ""
    self.standByTime = 0

    self:teleInitPos()
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money or 0
        self.guns = data.guns or {}
        self.gunSprayPaints = data.gunSprayPaints or {}
        self.specialClothing = data.specialClothing or {}
        self.teamId = data.teamId or 1
    end
    self:setCurrency(self.money)
    self:initGunsForgeData()
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:onTick()
    self.standByTime = self.standByTime + 1
    if self.standByTime == GameConfig.standByTime - 10 then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:msgNotOperateTip())
    end
    if self.standByTime >= GameConfig.standByTime then
        self:doPlayerQuit()
    end
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:initGunsForgeData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = self:setGunsForgeData()
    self.entityPlayerMP:changeCustomProps(json.encode(data))
end

function GamePlayer:updateGunRandomForgeValue(gun)
    if self.money < gun.price then
        return false
    end
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local weight = math.random(1, 100)
    local gunId = gun.gunId
    local gunType = gun.forgeType
    for _, pro in pairs(ForgeProbabilityConfig:getProbability()) do
        if (pro.gunId == gunId and pro.type == gunType) then
            if weight > 0 and weight <= pro.weight then
                self:updateForgeValue(gunId, gunType, pro.value)
                self:subMoney(gun.price)
                return true
            else
                weight = weight - pro.weight
            end
        end
    end
    return false
end

function GamePlayer:setGunsForgeData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    for groupId, group in pairs(ForgeGroupConfig.groups) do
        local item = {}
        item.id = group.id
        item.name = group.name
        item.props = {}
        if group.id == "4" then
            item.props = self:setGunsSprayPaint()
        elseif group.id == "5" then
            item.props = self:setPlayerActor()
        else
            local guns = GunsConfig:getGuns()
            for gunId, gun in pairs(guns) do
                if tostring(gun.forgeType) == group.id then
                    local _gun = self:getGunsData(gun)
                    table.insert(item.props, _gun)
                end
            end
            table.sort(item.props, function(a, b)
                return tonumber(a.id) < tonumber(b.id)
            end)
        end
        table.insert(data, item)
    end
    return data
end

function GamePlayer:setGunsSprayPaint()
    local props = {}
    for _, v in pairs(SprayPaintConfig.sprayPaint) do
        local item = {}
        item.id = v.sprayPaintId
        item.name = v.name
        item.image = v.image
        item.desc = v.desc
        item.price = v.price
        item.values = "0"
        item.status = 1
        if v.itemId == v.initialItemId and self.gunSprayPaints[tostring(v.itemId)] == nil then
            self.gunSprayPaints[tostring(v.itemId)] = 1
            item.status = 8
            item.price = 0
        end
        if self.gunSprayPaints[tostring(v.itemId)] == 1 then
            item.status = 8
            item.price = 0
        elseif self.gunSprayPaints[tostring(v.itemId)] == 2 then
            item.status = 4
            item.price = 0
        end
        table.insert(props, item)
    end
    table.sort(props, function(a, b)
        return tonumber(a.id) < tonumber(b.id)
    end)
    return props
end

function GamePlayer:setPlayerActor()
    local props = {}
    for _, v in pairs(SpecialClothingConfig.specialClothing) do
        local item = {}
        item.id = v.id
        item.name = v.name
        item.image = v.image
        item.desc = v.desc
        item.price = v.price
        item.status = 1
        if v.clothingId == "1" or v.clothingId == "2" then
            item.status = 8
            item.price = 0
        end
        if self.specialClothing[v.clothingId] ~= nil then
            if self.specialClothing[v.clothingId] ~= "0" then
                item.status = 8
                item.price = 0
            elseif self.specialClothing[v.clothingId] == "0" then
                item.status = 4
                item.price = 0
            end
        end
        table.insert(props, item)
    end
    table.sort(props, function(a, b)
        return tonumber(a.id) < tonumber(b.id)
    end)
    return props
end

function GamePlayer:isCanUpgradeGun(gun)
    local p_gun = self.guns[tostring(gun.itemId)]
    if p_gun == nil then
        return true
    end
    local _gun, maxValue = self:getPlayerGun(gun.gunId)
    return _gun[gun.forgeType] < maxValue
end

function GamePlayer:getGunsData(gun)
    local item = {}
    item.id = gun.gunId
    item.name = gun.name
    item.image = gun.image
    item.desc = gun.desc
    item.values = self:getPlayerGunStringValue(gun.itemId, gun.forgeType)
    item.price = gun.price
    item.status = 5
    if self:isCanUpgradeGun(gun) == false then
        item.status = 9
    end
    return item
end

function GamePlayer:getPlayerGunStringValue(itemId, type)
    local gun = self.guns[tostring(itemId)]
    if gun == nil then
        return "0"
    else
        if type == 3 then
            return tostring(gun[type] * 100)
        else
            return tostring(gun[type])
        end
    end
end

function GamePlayer:updateForgeValue(gunId, type, value)
    local gun, maxValue = self:getPlayerGun(gunId)
    local c_value = gun[type]
    c_value = math.min(maxValue, c_value + value)
    gun[type] = c_value
    if self.entityPlayerMP == nil then
        return
    end
    local c_status = 5
    if c_value == maxValue then
        c_status = 9
    end
    if type == 3 then
        c_value = c_value * 100
    end
    local data = {
        {
            groupId = tostring(type),
            id = gunId,
            values = tostring(c_value),
            status = c_status
        }
    }
    self.entityPlayerMP:updateCustomProps(json.encode(data))
    HostApi.sendPlaySound(self.rakssid, 301)
end

function GamePlayer:updateGunSprayPaint(sprayPaint)
    local price = sprayPaint.price
    if self.gunSprayPaints[tostring(sprayPaint.itemId)] ~= nil then
        price = 0
    end
    if self.money < price then
        return false
    end
    if price > 0 then
        self:subMoney(price)
    end
    local SP = SprayPaintConfig:getSameGanByInitialItemId(sprayPaint.initialItemId)
    local data = {}
    for i, v in pairs(SP) do
        if self.gunSprayPaints[tostring(v.itemId)] then
            self.gunSprayPaints[tostring(v.itemId)] = 2
            local item = {
                groupId = "4",
                id = v.sprayPaintId,
                status = 4,
                price = 0
            }
            table.insert(data, item)
        end
    end
    self.gunSprayPaints[tostring(sprayPaint.itemId)] = 1
    local item = {
        groupId = "4",
        id = sprayPaint.sprayPaintId,
        status = 8,
        price = 0
    }
    table.insert(data, item)
    self.entityPlayerMP:updateCustomProps(json.encode(data))
    HostApi.sendPlaySound(self.rakssid, 301)
    return true
end

function GamePlayer:updatePlayerActor(playerActor)
    local price = playerActor.price
    if tonumber(playerActor.clothingId) <= 2 or self.specialClothing[playerActor.clothingId] ~= nil then
        price = 0
    end
    if self.money < price then
        return false
    end
    if price > 0 then
        self:subMoney(price)
    end
    local data = {}
    local actors = SpecialClothingConfig:getSameClothingByType(playerActor.type)
    for i, v in pairs(actors) do
        if tonumber(v.clothingId) <= 2 or self.specialClothing[v.clothingId] ~= nil then
            self.specialClothing[v.clothingId] = "0"
            local item = {
                groupId = "5",
                id = v.id,
                status = 4,
                price = 0
            }
            table.insert(data, item)
        end
    end
    self.specialClothing[playerActor.clothingId] = playerActor.type
    local item = {
        groupId = "5",
        id = playerActor.id,
        status = 8,
        price = 0
    }
    table.insert(data, item)
    self.entityPlayerMP:updateCustomProps(json.encode(data))
    HostApi.sendPlaySound(self.rakssid, 301)
    return true
end

function GamePlayer:setWeekRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerWeekRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:setDayRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerDayRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:getPlayerGun(gunId)
    local gun = GunsConfig:getGunById(gunId)
    if gun == nil then
        return { 0, 0, 0 }, 0
    end
    local p_gun = self.guns[tostring(gun.itemId)]
    if p_gun == nil then
        p_gun = { 0, 0, 0 }
        self.guns[tostring(gun.itemId)] = p_gun
    end
    return p_gun, gun.maxValue
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:doPlayerQuit()
    BaseListener.onPlayerLogout(self.userId)
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

return GamePlayer

--endregion