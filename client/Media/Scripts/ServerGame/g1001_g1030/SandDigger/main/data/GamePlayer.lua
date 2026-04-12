---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "config.BackpackConfig"
require "config.MerchantConfig"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}

    self.money = 0
    self.score = 0
    self.gameScore = 0

    self.backpackLevel = 1
    self.breakChestCounts = 0
    self.breakBlockCounts = 0

    self.digStatus = false

    self:teleInitPos()
    self:setCurrency(self.money)
end

function GamePlayer:setWeekRank(rank, score)
    RankManager:addPlayerWeekRank(self, "digger", rank, score, function()
        GameMatch:sendRankDataToPlayer(self)
    end)
end

function GamePlayer:setDayRank(rank, score)
    RankManager:addPlayerDayRank(self, "digger", rank, score, function()
        GameMatch:sendRankDataToPlayer(self)
    end)
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:addMoney(money)
    if money ~= 0 then
        self:addCurrency(money)
    end
end

function GamePlayer:addScore(score)
    local backpack = BackpackConfig:getBackpackByLevel(self.backpackLevel)
    if backpack ~= nil then
        if self.score + score < backpack.capacity then
            self.score = self.score + score
            self.gameScore = self.gameScore + score
            self.appIntegral = self.appIntegral + score
            self:addBackpackCapacity(score)
        else
            self.score = backpack.capacity
            self.gameScore = self.gameScore + backpack.capacity - self.score
            self.appIntegral = self.appIntegral + backpack.capacity - self.score
            self:resetBackpack(self.score, self.score)
            HostApi.showGoNpcMerchant(self.rakssid, MerchantConfig.merchants[1].id, MerchantConfig.exchangePosition.x,
            MerchantConfig.exchangePosition.y, MerchantConfig.exchangePosition.z, 35)
        end
    end
end

function GamePlayer:upgradePackage(id)
    local backpack = BackpackConfig:getBackpackById(id)
    if backpack ~= nil then
        self.backpackLevel = backpack.level
        self:resetBackpack(self.score, backpack.capacity)
        self:changeClothes(backpack.model, backpack.modelID)
        MerchantConfig.merchants[1]:syncPlayer(self)
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:getDataFromDB(data)
    if not data.__first then
        self:setCurrency(tonumber(data.money))
        self.score = tonumber(data.score)
        self.backpackLevel = tonumber(data.backpackLevel)
        self:fillInvetory(data.inventory)

        local backpack = BackpackConfig:getBackpackByLevel(self.backpackLevel)
        self:resetBackpack(self.score, backpack.capacity)
        self:changeClothes(backpack.model, backpack.modelID)

        for i, v in pairs(MerchantConfig.merchants) do
            v:syncPlayer(self)
        end

    else
        self.money = 0
        self.score = 0
        self.backpackLevel = 1
        local backpack = BackpackConfig:getBackpackByLevel(self.backpackLevel)
        self:resetBackpack(self.score, backpack.capacity)
        self:changeClothes(backpack.model, backpack.modelID)

        for i, v in pairs(MerchantConfig.merchants) do
            v:syncPlayer(self)
        end

        local itemId = self:getHeldItemId()
        if itemId ~= nil then
            for i, v in pairs(GameConfig.initEquip) do
                self:addItem(v.id, v.num, 0)
            end
        end
    end
end

function GamePlayer:reward()
    local score = math.ceil(self.breakBlockCounts / 5) + self.breakChestCounts
    RewardManager:getUserGoldReward(self.userId, score, function(data)

    end)
end

return GamePlayer

--endregion


