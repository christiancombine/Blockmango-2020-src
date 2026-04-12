require "data.DrawLuckyPool"
require "config.BonusConfig"
require "config.BirdConfig"

DrawLuckyNpc = class("DrawLuckyNpc")

function DrawLuckyNpc:ctor(config)
    self.id = config.id
    self.name = config.name
    self.actor = config.actor
    self.actorBody = config.actorBody
    self.actorBodyId = config.actorBodyId
    self.moneyType = config.moneyType
    self.price = config.price
    self.pos = config.pos
    self.yaw = config.yaw
    self.minPos = config.minPos
    self.maxPos = config.maxPos
    self.pools = config.pools
    self.index = 1
    self.birds = config.birds
    self.prob = config.prob
    self.poolList = {}
    self.playerIndex = {}
end

function DrawLuckyNpc:OnCreate()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, 104102, self.actor, function(entity)
        entity:setNameLang(self.name or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
        entity:setPerson(true)
    end) --抽奖
    self.npcId = entityId
    AreaInfoManager:push(self.npcId, AreaType.DrawLucky, self.minPos, self.maxPos)
    self.NewBonusPool(self)
    return self.npcId
end

function DrawLuckyNpc:SyncDrawLuckyInfo(player)
    local drawLuckyInfo = {}
    drawLuckyInfo.name = self.name
    drawLuckyInfo.currencyType = self.moneyType
    drawLuckyInfo.price = self.price
    drawLuckyInfo.items = {}
    for index, birdId in pairs(self.birds) do
        local bird = BirdConfig:getBirdInfoById(birdId)
        if bird == nil then
            print("bird is nil  id = " .. birdId)
            return
        end
        local tab = {}
        tab.id = birdId
        tab.icon = bird.actorBodyId
        tab.prob = self.prob[index]
        tab.quality = bird.quality
        tab.isHave = player.userBird:isAlreadyDiscovered(birdId)
        table.insert(drawLuckyInfo.items, tab)
    end
    local json_data = json.encode(drawLuckyInfo)
    EngineWorld:getWorld():updateSessionNpc(
            self.npcId,
            player.rakssid,
            self.name,
            self.actor,
            self.actorBody,
            self.actorBodyId,
            json_data,
            "",
            0,
            true
    )
end

function DrawLuckyNpc:NewBonusPool()
    local configs = BonusConfig.bonusPools
    local index = 1
    for _, poolid in ipairs(self.pools) do
        local config = configs[poolid]
        if type(config) == "table" then
            local pool = DrawLuckyPool.new(index, config)
            table.insert(self.poolList, pool)
        end
        index = index + 1
    end
end

function DrawLuckyNpc:OnPlayerClick(player)
    if player.userBirdBag == nil then
        return false
    end

    if player.userBirdBag:isCanAddNumToCapacity(1) == false then
        HostApi.sendCommonTip(player.rakssid, "bird_capacity_full")
        return false
    end

    if self.moneyType == CurrencyType.EggTicket then
        if player:GetTicketNum() < self.price then
            return false
        end
        player:SubEggTicket(self.price)
        self:OnDeductionSuccess(nil, player.rakssid)
    elseif self.moneyType == CurrencyType.Money then
        if player:GetMoneyNum() < self.price then
            return false
        end
        player:subCurrency(self.price)
        self:OnDeductionSuccess(nil, player.rakssid)
    elseif self.moneyType == CurrencyType.Diamond then
        local key = string.format("%s%s:%d", "DrawLucky", tostring(player.rakssid), self.id)
        player:consumeDiamonds(55000 + self.id, self.price, key, false)
        ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.OnDeductionSuccess, self)
    end
end

function DrawLuckyNpc:OnDeductionSuccess(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    local allweight = 0
    local maxCount = self.poolList[1]:GetMaxCount()
    local offset = player:GetDrawLuckyIndexByPoolId(self.id, maxCount)
    for index, pool in ipairs(self.poolList) do
        local weight = pool:RefreshRange(player, allweight, offset)
        if weight > allweight then
            allweight = weight
        end
    end

    local randNumber = HostApi.random("poolList", 1, allweight)
    for _, pool in ipairs(self.poolList) do
        if pool:CheckRangeVal(randNumber) then
            pool:OnRandomPool(player, self.actorBodyId)

            if pool.isKey == true then
                player:OnRandKey(self.id)
            end
            self:SyncDrawLuckyInfo(player)
            TaskHelper.dispatch(TaskType.DrawLuckyEvent, player.userId, self.id, 1)
            return true
        end
    end
    return true
end
