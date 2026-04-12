
LotteryUtil = {}
LotteryUtil.lotteryData = {}
LotteryUtil.data = {}
LotteryUtil.weights = {}

LotteryUtil.lastModel = nil

LotteryUtil.proNum = 0

LotteryUtil.RANDOM_TIGER = 11

function LotteryUtil:initLotteryConfig()
    for i, lotterydata in pairs(TigerLotteryConfig.tigerLottery) do
        local item = {}
        item.id = lotterydata.id
        item.name = lotterydata.name
        item.image = lotterydata.image
        item.actor = lotterydata.actor_name
        item.probability = lotterydata.weights
        self.proNum = self.proNum + item.probability
        table.insert(self.lotteryData, item)
    end

    for k, v in pairs(self.lotteryData) do
        if self.proNum ~= 0 then
            self.weights[k] = v.probability / self.proNum
        end
    end

    self:initRandomSeed()
    self:initLotteryData()
end

function LotteryUtil:initRandomSeed()
    SweepstakesUtil:initRandomSeed(LotteryUtil.RANDOM_TIGER, self.weights)
end

function LotteryUtil:initLotteryData()
    local items = {}
    local firstIds = {}
    local secondIds = {}
    for i, lottery in pairs(self.lotteryData) do
        local item = {}
        item.id = lottery.id
        item.name = lottery.name
        item.image = lottery.image
        table.insert(items, item)
        firstIds[i] = lottery.id
        secondIds[i] = lottery.id
    end
    self.data.items = items
    self.data.firstIds = firstIds
    self.data.secondIds = secondIds
end

function LotteryUtil:sendLotteryData(player)
    local data = self.data
    data.thirdIds = { player.thirdId }
    HostApi.setLotteryData(player.rakssid, json.encode(data))
end

function LotteryUtil:randomLottery()
    local m = self.lotteryData[self:randomWithPro()]
    if self.lastModel == nil then
        self.lastModel = m
        return m
    end
    while self.lastModel == m do
        m = self.lotteryData[self:randomWithPro()]
    end
    self.lastModel = nil
    return m
end

function LotteryUtil:randomWithPro()
    local index = SweepstakesUtil:get(LotteryUtil.RANDOM_TIGER)
    return index

    -- local pro = 0
    -- local random = math.random(1, self.proNum)
    -- local left = 0
    -- for i = 1, #self.lotteryData do
    --     pro = pro + self.lotteryData[i].probability
    --     if left < random and random <= pro then
    --         return i
    --     end
    --     left = pro
    -- end
end

function LotteryUtil:setThirdData(player)
    if #player.thirdId == 0 then
        if #player.haveActorIds ~= 0 then
            player.thirdId = player.haveActorIds[math.random(1, #player.haveActorIds)]
        end
    end
end

return LotteryUtil