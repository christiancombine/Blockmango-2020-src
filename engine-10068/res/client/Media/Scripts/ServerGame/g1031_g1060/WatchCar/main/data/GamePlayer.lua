---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "config.GameConfig"
require "config.XMoneyConfig"
require "config.XInitItemConfig"
require "config.XRespawnConfig"
require "config.XActorConfig"
require "data.XPlayerWeapon"
require "data.XGameRank"
require "util.XInventoryUtil"
require "config.XAppShopPrivilege"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self.isReport = false
    self.killPlayerCount = 0
    self.bindTeam = nil
    self.bindTeamId = 0
    self.currentMoney = 0
    self.sumTimer = 0
    self.timer = 0
    self.isWin = false
    self.bindWeapons = {}
    self.maxHp = GameConfig.basicHealth
    self.echantmentItemList = {}
    if self.appShopItems == nil then
        self.appShopItems = {}
    end
    self.isLife = true
    self:initStaticAttr()
    self:OnInitTeam()
    self:teleInitPos()
    self:InitPlayerInfo()
    self:setOccupation()
    self:changeMaxHealth(self.maxHp)
    self.extraAddHealth = 0
    self.buffs = {}
    self.tempItems = {}
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
    self.HoldingWeaponId = 0
    local actorName = XActorConfig:getRandActorByTeamId(self.bindTeamId)
    self:changActor(actorName)

    ReportManager:reportUserEnter(self.userId)
end

GameResult = {
    WIN = 1, -- 赢
    LOSE = 0, -- 输
    DRAW = 2 -- 平局
}

function GamePlayer:getKills()
    return self.killPlayerCount or 0
end

function GamePlayer:getSetHoldWeapon(id)
    if type(id) == 'number' and id ~= 0 then
        self.HoldingWeaponId = id
    end
    return self.HoldingWeaponId
end

-- 当击杀一个玩家的时候调用  参数是被杀死的玩家的lua对象
function GamePlayer:onKillPlayer(diePlayer)
    self.killPlayerCount = self.killPlayerCount + 1
    self.scorePoints[ScoreID.KILL] = self.killPlayerCount
    self.appIntegral = self.appIntegral + 1
    local money = GameConfig.killPlayerAddMoney
    if type(money) == 'number' and money > 0 then
        self:OnChangeMoney(money)
    end

end

function GamePlayer:InitPlayerInfo()
    self.currentMoney = 0
    self.attackAddMoneyPars = GameConfig.attackAddMoneyPars or 1
    local initItems = XInitItemConfig:getInitItems(self.bindTeamId)

    for index = 1, #initItems do
        local item = initItems[index]
        self:addItem(item.ItemId, item.ItemCount, 0)
    end
    -- 初始化武器
    local weaponIds = XPlayerShopItems:getPrimaryWeapon(self.bindTeamId)
    for _, weaponId in pairs(weaponIds) do
        local weapon = XPlayerWeapon.new(self, weaponId)
        self.bindWeapons[tostring(weaponId)] = weapon
        local fumoId, fomoLv = XPlayerShopItems:getFumoInfo(weaponId)
        if type(fumoId) == "number" and type(fomoLv) == "number" then
            self:addEnchantmentItem(weaponId, 1, 0, fumoId, fomoLv)
            self.echantmentItemList[weaponId] = {
                id = fumoId,
                lv = fomoLv
            }
        else
            self:addItem(weaponId, 1)
        end
    end
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local color = self.bindTeamId == 1 and TextFormat.colorBlack or TextFormat.colorWrite
    local disName = ""
    if self.bindTeam ~= nil then
        disName = string.format("%s  %s[%s]", self.name, color, self.bindTeam:getTeamName())
    else
        disName = self.name
    end

    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:OnBuyAppShopItem(itemId)
    local itemCount = self.appShopItems[itemId] or self:getItemNumById(itemId)
    local limitCount = XAppShopPrivilege:GetLimitCount(itemId)

    local callback = function()
        return false
    end
    if type(limitCount) == 'number' then
        if itemCount == nil then
            callback = function()
                self.appShopItems[itemId] = 1
                if self.tempItems ~= nil then
                    self.tempItems[itemId] = 1
                end

                self:addExtraItem(itemId)
                return true
            end
        elseif limitCount == -1 or itemCount < limitCount then
            callback = function()
                self.appShopItems[itemId] = itemCount + 1
                self:addExtraItem(itemId)
                return true
            end
        end
    end
    local result = callback()
    if result and itemId ~= 10000 then
        HostApi.notifyGetItem(self.rakssid, itemId, 0, 1)
    end
    return result
end

-- 当升级武器的时候调用
function GamePlayer:OnUpgradeWeapon(weaponId)
    if self.entityPlayerMP == nil then
        return
    end
    local weaponId = XPlayerShopItems:getInitWeaponId(tonumber(weaponId))
    local weapon = self.bindWeapons[tostring(weaponId)]
    if weapon ~= nil then
        local data = weapon:OnUpgrade()
        if data ~= nil then
            local lastItem, curItem = weapon:getItems()
            self.echantmentItemList[lastItem] = nil
            local fumoId, fomoLv = XPlayerShopItems:getFumoInfo(curItem)
            if self.isLife == false then
                self.beforeDeathItems[lastItem] = nil
                self.beforeDeathItems[curItem] = 1
            end
            self:removeItem(lastItem, 1)
            if type(fumoId) == "number" and type(fomoLv) == "number" then
                if self.isLife then
                    self:removeItem(lastItem, 1)
                    self:addEnchantmentItem(curItem, 1, 0, fumoId, fomoLv)
                end
                self.echantmentItemList[curItem] = {
                    id = fumoId,
                    lv = fomoLv
                }
            else
                if self.isLife then
                    self:addItem(curItem, 1)
                end
            end
            self.entityPlayerMP:updateCustomProps(json.encode(data))
        end
    end
end

function GamePlayer:OnGameStart()
    if self.bindTeam then
        local initPos = XTeamConfig:getSpawnPosByTeamId(self.bindTeamId) --self.bindTeam.initPosition
        local yaw = self.bindTeam.yaw
        self:teleportPosWithYaw(initPos, yaw)
    end
    for id, count in pairs(self.appShopItems) do
        local item = self.tempItems[id]
        if item == nil then
            self:addExtraItem(id,true)
        end
    end
    self:OnInitPlayerShop()
    XGameRank:sendRealRankData(self)
    self.tempItems = nil
end

function GamePlayer:OnTick(timer)
    self.sumTimer = timer
    self:HandleAddMoneyRule()
end
-- 处理金币的增长 规则
function GamePlayer:HandleAddMoneyRule()
    self.timer = self.timer + 1
    local interval, number = XMoneyConfig:getMoneyConfig(self.sumTimer)
    if interval == nil or number == nil then
        return
    end
    if self.timer >= interval then
        self:OnChangeMoney(number)
        self.timer = 0
    end
end

function GamePlayer:getTeamNameAndPlayerName()
    if self.bindTeam then
        local teamName = self.bindTeam:getTeamName()
        if self.bindTeamId == 1 then
            teamName = TextFormat.colorBlack .. teamName
        elseif self.bindTeamId == 2 then
            teamName = TextFormat.colorWrite .. teamName
        end
        local playerName = self:getDisplayName()
        return teamName, playerName
    end
    return nil
end

function GamePlayer:getTeamId()
    return self.bindTeamId
end

function GamePlayer:setOccupation()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOccupation(0)
    end
end

function GamePlayer:onUseCustomProp(privilegeId)
    if type(privilegeId) ~= 'number' then
        privilegeId = tonumber(privilegeId)
    end
    if type(privilegeId) == 'number' then
        if self.appShopItems == nil then
            self.appShopItems = {}
        end
        self.appShopItems[privilegeId] = 1
    end
end

function GamePlayer:OnChangeMoney(val)
    if type(val) ~= "number" then
        return -1
    end
    if self:getCurrency() + val < 0 then
        return -1
    end
    self:setCurrency(self:getCurrency() + val)
    self.currentMoney = self:getCurrency()
    return self.currentMoney
end
-- 当被攻击的时候调用  攻击的玩家  伤害值
function GamePlayer:OnBeAttack(player, hitValue)
end
-- 当攻击其他玩家的时候调用   伤害值
function GamePlayer:OnAttack(hitValue)
    local addMoneyNumberof = hitValue * (self.attackAddMoneyPars or 1)
    addMoneyNumberof = math.floor(addMoneyNumberof)
    self:OnChangeMoney(addMoneyNumberof)
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:changActor(actorName)
    if type(actorName) ~= "string" then
        return
    end
    EngineWorld:changePlayerActor(self, actorName)
end

function GamePlayer:reward()

end

function GamePlayer:OnInitTeam()
    self.bindTeamId = self.dynamicAttr.team
    local team = GameMatch:getTeamByTeamId(self.bindTeamId)
    if team == nil then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.NoThisTeam)
        return
    end
    self.bindTeam = team
    self.bindTeam:addPlayer(self)
    local respawnPos = XTeamConfig:getSpawnPosByTeamId(self.bindTeamId)
    local pos = VectorUtil.newVector3i(respawnPos.x, respawnPos.y, respawnPos.z)
    self:setRespawnPos(pos)
    self.isInitTeam = true
end

-- 添加额外的 特权道具
function GamePlayer:addExtraItem(privilegeId,isAddMoney)

    local result = XPrivilegeConfig:getPrivilegeInfobById(privilegeId)

    local hp = self:getHealth()
    self:setHealth(hp)
    for _, prop in pairs(result.Items) do
        local itemId = prop.PropId
        local itemCount = prop.PropCount
        if type(itemId) == 'number' and type(itemCount) == 'number' then
            local fumoid, fumoLv = XPrivilegeConfig:GetRewardFumoInfo(privilegeId)
            if type(fumoid) == 'number' and type(fumoLv) == 'number' then
                self:addEnchantmentItem(itemId, itemCount, 0, fumoid, fumoLv)
                self.echantmentItemList[itemId] = {
                    id = fumoid,
                    lv = fumoLv
                }
            elseif itemId == 10000 and isAddMoney then
                self:addItem(itemId, itemCount, 0)
            elseif itemId ~= 10000 then
                self:addItem(itemId, itemCount, 0)
            end
        end
    end

    local addhp = result.AddHp
    if type(addhp) == 'number' then
        if type(self.maxHp) ~= 'number' then
            self.maxHp = GameConfig.basicHealth
        end
        self.maxHp = self.maxHp + addhp
        self.curhp = self:getHealth()
        self:changeMaxHealth(self.maxHp)
        self:setHealth(self.curhp)
        self:addHealth(addhp)
        --- self:addHealth(self.extraAddHealth)
    end
    local time = result.RespawnTime
    if type(time) == 'number' then
        self.spawnTime = self.spawnTime or time
        self.spawnTime = math.min(time, self.spawnTime)
    end
end

function GamePlayer:OnInitPlayerShop()
    if self.entityPlayerMP == nil then
        return
    end

    local playerShops = XPlayerShopItems:getPrimaryItemByTeamId(self.bindTeamId)
    local shopJson = json.encode(playerShops)
    self.entityPlayerMP:changeCustomProps(shopJson)
end

function GamePlayer:OnBuyNormalItemFromPlayerShop(itemid)
    local limit = XPlayerShopItems:getNormalItemLimit(itemid)
    local number = 0
    if self.isLife == false then
        number = self.beforeDeathItems[itemid] or 0
    else
        number = self:getItemNumById(itemid) or 0
    end

    if number < limit then
        local price = XPlayerShopItems:GetNormalItemPrice(itemid)
        if type(price) == 'number' and price >= 0 then
            if self.isLife == false then
                self.beforeDeathItems[itemid] = number + 1
            end
            local fumoId, fumoLv = XPlayerShopItems:GetNormalItemFumoInfo(itemid)
            if type(fumoId) == 'number' and type(fumoLv) == 'number' then
                if self.isLife then
                    self:addEnchantmentItem(itemid, 1, 0, fumoId, fumoLv)
                end
                self.echantmentItemList[itemid] = {
                    id = fumoId,
                    lv = fumoLv
                }
            elseif itemid ~= 10000 and self.isLife then
                self:addItem(itemid, 1, 0)
            end
            self:OnChangeMoney(-price)
            HostApi.notifyGetItem(self.rakssid, itemid, 0, 1)
            if self.isLife == false then
                number = self.beforeDeathItems[itemid] or 0
            else
                number = self:getItemNumById(itemid) or 0
            end
            if number >= limit then
                local data = XPlayerShopItems:UpdateItemBuyState(itemid)
                local shopJson = json.encode(data)
                self.entityPlayerMP:updateCustomProps(shopJson)
            end
            return true
        end
    end
    return false
end
function GamePlayer:OnPlayerDeath(iskillByPlayer, killPlayer)

    self.isLife = false
    local waitTime = XRespawnConfig:getWaitTimeByPeriodTime(self.sumTimer)
    if type(self.spawnTime) == "number" then
        waitTime = self.spawnTime
    end
    RespawnHelper.sendRespawnCountdown(self, waitTime)
    HostApi.sendShowPersonalShop(self.rakssid)
    self.beforeDeathItems = XInventoryUtil:getAllItems(self)
end

function GamePlayer:OnChangPlayerHp(hp)
    if type(hp) == 'number' then
        self.maxHp = self.maxHp + hp
        self:changeMaxHealth(self.maxHp)
    end
end

function GamePlayer:onPlayerRespawn()
    self:clearInv()
    self:changeMaxHealth(self.maxHp)
    self.isLife = true
    if self.beforeDeathItems ~= nil then
        for itemId, itemCount in pairs(self.beforeDeathItems) do
            if itemId ~= 311 then       -- 防火甲的特殊判断
                if type(tonumber(itemId)) == 'number' and type(itemCount) == 'number' then
                    local fumoInfo = self.echantmentItemList[itemId]
                    if fumoInfo ~= nil and type(fumoInfo.id) == 'number' and type(fumoInfo.lv) == 'number' then
                        self:addEnchantmentItem(itemId, itemCount, 0, fumoInfo.id, fumoInfo.lv)
                    else
                        self:addItem(itemId, itemCount, 0)
                    end
                end
            end
        end
    end
    if GameMatch.hasStartGame then
        local actorName = XActorConfig:getRandActorByTeamId(self.bindTeamId)
        self:changActor(actorName)
    end
end

function GamePlayer:OnBuyNpcCommodity(goodsId)
    if goodsId == nil then
        return false
    end
    local limitCount = XNpcShopGoodsConfig:getLimitCountByGoodsId(goodsId)
    local count = self:getItemNumById(goodsId)
    return count < limitCount and true or false
end

return GamePlayer

--endregion
