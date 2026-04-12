--require "config.Area"
require "config.TigerLotteryItems"
GameTigerLottery = class("GameTigerLottery")

function GameTigerLottery:ctor(config)
    self.tigerLotteryId = config.id
    self.actorName = config.actorName
    self.actorBody = config.actorBody
    self.actorBodyId = config.actorBodyId
    self.modleName = config.modleName
    self.modleBody = config.modleBody
    self.modleBodyId = config.modleBodyId
    self.tigerLotteryName = config.tigerLotteryName or ""
    self.pos = config.pos
    self.yaw = config.yaw
    self.minPos = config.minPos
    self.maxPos = config.maxPos
    self.refreshCD = config.refreshCD
    self.tipNor = config.tipNor
    self.tipPre = config.tipPre
    self.imageNor = config.imageNor
    self.imagePre = config.imagePre
    self.pools = config.pools
    self.countdownCD = config.countdownCD
    self.cubeMoney = config.cubeMoney
    self.data = {}
end

function GameTigerLottery:OnCreate()
    --self:CalcRectInfo()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, 104701, self.actorName, function(entity)
        entity:setNameLang(self.tigerLotteryName or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
        entity:setPerson(true)
    end)
    self.npcId = entityId
    AreaInfoManager:push(self.npcId, AreaType.TigerLottery, self.tipNor, self.tipPre, self.imageNor, self.imagePre, false, self.minPos, self.maxPos)
    self:setTigerLotteryItems()
    return self.npcId
end

--function GameTigerLottery:CalcRectInfo()
--    local pos = {}
--    pos[1] = {}
--    pos[1][1] = self.minPos.x
--    pos[1][2] = self.minPos.y
--    pos[1][3] = self.minPos.z
--    pos[2] = {}
--    pos[2][1] = self.maxPos.x
--    pos[2][2] = self.maxPos.y
--    pos[2][3] = self.maxPos.z
--    self.areaRect = Area.new(pos)
--end

function GameTigerLottery:SyncTigerLotteryInfo(player)
    local endTime = player:getTigerLotteryCountdowns(self.tigerLotteryId)
    if type(endTime) ~= "number" then
        player:setTigerLotteryCountdowns(self.tigerLotteryId, os.time())
        self:RefreshTigerLottery(player.rakssid, 0)
    else
        self:RefreshTigerLottery(player.rakssid, endTime - os.time())
    end
end

--function GameTigerLottery:CheckTrigger(pos)
--    return self.areaRect:inArea(pos)
--end

function GameTigerLottery:RefreshTigerLottery(rakssid, time)
    local name = (time > 0 and { self.modleName } or { self.actorName })[1]
    local body = (time > 0 and { self.modleBody } or { self.actorBody })[1]
    local bodyId = (time > 0 and { self.modleBodyId } or { self.actorBodyId })[1]
    local remainTime = time > 0 and time or 0
    local json_data = json.encode(self.data)

    EngineWorld:getWorld():updateSessionNpc(
            self.npcId,
            rakssid,
            self.tigerLotteryName,
            name,
            body,
            bodyId,
            json_data,
            "",
            remainTime * 1000,
            true
    )

    if time > 0 then

        LuaTimer:scheduleTimer(GameTigerLottery.RefreshTigerLottery, time * 1000, 1, self, rakssid, 0)

    end

end

function GameTigerLottery:setTigerLotteryItems()
    self.items = {}
    self.firstItems = {}
    self.secondItems = {}
    self.thirdItems = {}
    self.fristProNum = 0
    self.secondProNum = 0
    self.thirdProNum = 0

    local tigerLotteryItems = TigerLotteryItems:GetTigerLotteryItems()
    for _, lotteryData in pairs(tigerLotteryItems) do
        local item = {}
        for _, poolId in ipairs(self.pools) do
            if lotteryData.pool == poolId then
                item.id = lotteryData.id
                item.name = lotteryData.name
                item.image = lotteryData.image
                item.itemId = lotteryData.itemId
                item.itemIdMate = lotteryData.itemIdMate
                item.pool = lotteryData.pool
                item.pro = lotteryData.pro
                item.type = lotteryData.type
                if poolId == self.pools[1] then
                    self.fristProNum = self.fristProNum + lotteryData.pro
                    table.insert(self.firstItems, item)
                elseif poolId == self.pools[2] then
                    self.secondProNum = self.secondProNum + lotteryData.pro
                    table.insert(self.secondItems, item)
                elseif poolId == self.pools[3] then
                    self.thirdProNum = self.thirdProNum + lotteryData.pro
                    table.insert(self.thirdItems, item)
                end
                table.insert(self.items, item)
                break
            end
        end
    end

    self:initLotteryData()

end

function GameTigerLottery:initLotteryData()
    local itemDatas = {}
    local firstIds = {}
    local secondIds = {}
    local thirdIds = {}
    for i, lottery in pairs(self.items) do
        local item = {}
        item.id = lottery.id
        item.type = tonumber(lottery.type)
        item.itemId = tonumber(lottery.itemId)
        item.itemIdMate = tonumber(lottery.itemIdMate)
        table.insert(itemDatas, item)
    end
    for k, v in pairs(self.firstItems) do
        firstIds[k] = v.id
    end
    for k, v in pairs(self.secondItems) do
        secondIds[k] = v.id
    end
    for k, v in pairs(self.thirdItems) do
        thirdIds[k] = v.id
    end

    self.data.items = itemDatas
    self.data.firstIds = firstIds
    self.data.secondIds = secondIds
    self.data.thirdIds = thirdIds
    self.data.countdownCD = tonumber(self.countdownCD)
    self.data.cubeMoney = tonumber(self.cubeMoney)

end

function GameTigerLottery:OnPlayerClick(player)
    self:OnReceive(nil, player.rakssid)
end

--function GameTigerLottery:onPlayerExit(player)
--local key = string.format("tigerLotteryDelay:%s:%d", tostring(player.rakssid), self.tigerLotteryId)
--TimerManager.RemoveListenerById(key)
--end

function GameTigerLottery:OnReceive(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local endTime = player:getTigerLotteryCountdowns(self.tigerLotteryId)
    if type(endTime) == "number" then
        if (endTime - os.time()) > 0 then
            player:consumeDiamonds(1100 + self.tigerLotteryId, self.cubeMoney, Messages.buyTigerLottery, true)
            player:refreshTigerLotteryAnalytics(self.tigerLotteryId)
        end
    end

    self:randomLottery(rakssid)

    local endTime = os.time() + self.refreshCD
    player:setTigerLotteryCountdowns(self.tigerLotteryId, endTime)
    self:RefreshTigerLottery(player.rakssid, self.refreshCD)
end

function GameTigerLottery:randomLottery(rakssid)
    local firstId = 0
    local secondId = 0
    local thirdId = 0
    for _, poolId in ipairs(self.pools) do
        local remark = "tigerLottery" .. tostring(poolId)
        if poolId == self.pools[1] then
            firstId = self:randomWithPro(remark, self.firstItems, self.fristProNum)
        elseif poolId == self.pools[2] then
            secondId = self:randomWithPro(remark, self.secondItems, self.secondProNum)
        elseif poolId == self.pools[3] then
            thirdId = self:randomWithPro(remark, self.thirdItems, self.thirdProNum)
        end
    end
    HostApi.sendLotteryResult(rakssid, tostring(firstId), tostring(secondId), tostring(thirdId))
end

function GameTigerLottery:randomWithPro(remark, items, itemsProNum)
    local randWeight = HostApi.random(remark, 1, itemsProNum)
    local num = 0
    for i, v in pairs(items) do
        if randWeight >= num and randWeight <= num + v.pro then
            return v.id
        end
        num = num + v.pro
    end

    return items[1].id

end
