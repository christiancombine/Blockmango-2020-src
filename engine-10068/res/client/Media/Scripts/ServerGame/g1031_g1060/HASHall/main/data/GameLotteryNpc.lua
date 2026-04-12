---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
GameLotteryNpc = class()

function GameLotteryNpc:ctor(config)
    self.name = config.name or ""
    self.actor = config.actor
    self.position = VectorUtil.newVector3(config.x, config.y, config.z)
    self.yaw = tonumber(config.yaw)
    self.times = tonumber(config.times)
    self.coin = tonumber(config.coin)
    self.coinId = tonumber(config.coinId)
    self.coinImage = config.coinImage
    self.content = config.content
    self:onCreate()
end

function GameLotteryNpc:onCreate()
    local entityId = EngineWorld:addActorNpc(self.position, self.yaw, self.actor, function(entity)
        entity:setHeadName(self.name)
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
    GameManager:onAddLotteryNpc(self)
end

function GameLotteryNpc:onPlayerHit(player)
    HostApi.showConsumeCoinTip(player.rakssid, self.content, self.coinId, self.coin, "Tip=" .. tostring(self.entityId))
end

function GameLotteryNpc:onPlayerSelected(player)
    player:subMoney(self.coin)
    self:randomItem(player)
end

function GameLotteryNpc:randomItem(player)
    local rareProbability = GameConfig.lotteryOneTimesRare
    local rare = 0.1
    local itemList = {}
    if self.times == 1 then
        if player.lotteryTimes + 1 >= #rareProbability then
            rare = tonumber(rareProbability[#rareProbability])
        else
            rare = tonumber(rareProbability[player.lotteryTimes + 1])
        end
        LotteryUtil:initRandomSeed(ActorsConfig.RANDOM_ONE_TIMES_GROUP, { rare, 1 - rare })
        itemList = self:randomOneItem(player)
    elseif self.times == 10 then
        itemList = self:randomTenItem(player)
    end

    if itemList == nil or #itemList < 1 then
        return
    end

    local haveItemCount = 0
    for k, v in pairs(itemList) do
        if v.isHave then
            haveItemCount = haveItemCount + 1
        end
    end

    local returnMoney = math.ceil(haveItemCount * (self.coin / self.times) / 2)
    if returnMoney > 0 then
        player:addMoney(returnMoney)
        MsgSender.sendBottomTipsToTarget(player.rakssid, GameConfig.gameTipShowTime, Messages:msgHaveItemTip(haveItemCount, returnMoney))
    end

    self:sendHideAndSeekHallResult(itemList, returnMoney, player)
end

function GameLotteryNpc:randomOneItem(player)
    local itemList = {}
    player.lotteryTimes = player.lotteryTimes + 1
    local is_rare = LotteryUtil:get(ActorsConfig.RANDOM_ONE_TIMES_GROUP)
    if is_rare == ActorsConfig.ACTORS_IS_RARE then
        player.lotteryTimes = 0
    end
    local item = self:getrandomItem(is_rare, player)
    table.insert(itemList, item)

    return itemList
end

function GameLotteryNpc:randomTenItem(player)
    local itemList = {}
    local item = {}
    for i = 1, 10 do
        local is_rare = LotteryUtil:get(i)
        item = self:getrandomItem(is_rare, player)
        table.insert(itemList, item)
    end
    return itemList
end

function GameLotteryNpc:getrandomItem(is_rare, player)
    local actors = {}
    local item = {}
    local group = nil

    if is_rare == ActorsConfig.ACTORS_IS_RARE then
        actors = self:getRareActors()
        group = ActorsConfig.RARE_WEIGHTS_GROUP
        item.itembgImg = GameConfig.rareItemBgImg
    else
        actors = self:getOrdinaryActors()
        group = ActorsConfig.ORDINARY_WEIGHTS_GROUP
        item.itembgImg = GameConfig.ordinaryItemBgImg
    end

    local index = LotteryUtil:get(group)
    item.id = actors[index].id
    item.image = actors[index].image
    item.isHave = false
    if player.actors[item.id] == nil then
        player.actors[item.id] = GamePlayer.HAVE_ACTOR_STATE
    else
        item.isHave = true
    end
    return item
end

function GameLotteryNpc:getRareActors()
    return ActorsConfig:getRareActors()
end

function GameLotteryNpc:getOrdinaryActors()
    return ActorsConfig:getOrdinaryActors()
end

function GameLotteryNpc:sendHideAndSeekHallResult(itemList, returnMoney, player)
    local data = {}
    data.itemList = {}
    for _, v in pairs(itemList) do
        local item = {}
        item.itemId = tonumber(v.id)
        item.itemImg = string.gsub(v.image, "hidden", "hall")
        item.isHave = v.isHave
        item.itembgImg = v.itembgImg
        table.insert(data.itemList, item)
    end

    data.money = math.ceil(tonumber(self.coin))
    data.moneyImg = tostring(self.coinImage)
    data.coinId = tonumber(self.coinId)
    data.times = tonumber(self.times)
    data.entityId = tonumber(self.entityId)
    data.rusultMoney = tonumber(returnMoney)

    HostApi.sendHideAndSeekHallResult(player.rakssid, json.encode(data))
    player:sendActorsData()
end

return GameLotteryNpc

