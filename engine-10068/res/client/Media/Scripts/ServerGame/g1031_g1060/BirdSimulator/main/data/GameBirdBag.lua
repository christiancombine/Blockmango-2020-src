GameBirdBag = class()

function GameBirdBag:ctor(userId)
    self.userId = userId
    self.bagItems = {} -- 背包里面的物品

    self.curCarryBirdNum = 0
    self.maxCarryBirdNum = GameConfig.initMaxCarryBirdNum
    self.curCapacityBirdNum = 0
    self.maxCapacityBirdNum = GameConfig.initMaxCapacityBirdNum
end

function GameBirdBag:initDataFromDB(data)
    self:initMaxCarryBirdNumFromDB(data.maxCarry or GameConfig.initMaxCarryBirdNum)
    self:initMaxCapacityBirdNumFromDB(data.maxCapacity or GameConfig.initMaxCapacityBirdNum)
    self:initItemsFromDB(data.items or {})
end

function GameBirdBag:initMaxCarryBirdNumFromDB(maxCarry)
    self.maxCarryBirdNum = maxCarry
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end
    TaskHelper.dispatch(TaskType.HaveBirdShellsEvent, player.userId)
end

function GameBirdBag:initMaxCapacityBirdNumFromDB(maxCapacity)
    self.maxCapacityBirdNum = maxCapacity
end

function GameBirdBag:initItemsFromDB(items)
    for _, v in pairs(items) do
        self:addBagItem(v.id, v.num)
    end
end

function GameBirdBag:prepareDataSaveToDB()
    local data = {
        maxCarry = self.maxCarryBirdNum,
        maxCapacity = self.maxCapacityBirdNum,
        items = self:prepareItemsToDB()
    }

    return data
end

function GameBirdBag:prepareItemsToDB()
    local items = {}
    for i, v in pairs(self.bagItems) do
        local item = {}
        item.id = i
        item.num = v
        table.insert(items, item)
    end

    return items
end

function GameBirdBag:addBagItem(itemId, num)
    if itemId <= 0 or num <= 0 then
        return
    end

    if self.bagItems[itemId] == nil then
        self.bagItems[itemId] = 0
    end

    self.bagItems[itemId] = self.bagItems[itemId] + num
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player ~= nil then
        player:setBirdSimulatorFood()
    end
end

function GameBirdBag:subBagItem(itemId, num)
    if self:isEnoughBagItems(itemId, num) then
        self.bagItems[itemId] = self.bagItems[itemId] - num
        if self.bagItems[itemId] <= 0 then
            self.bagItems[itemId] = nil
        end
    end
end

function GameBirdBag:isEnoughBagItems(itemId, num)
    if itemId <= 0 or num <= 0 then
        return false
    end

    if self.bagItems[itemId] == nil then
        return false
    end

    if self.bagItems[itemId] < num then
        return false
    end

    return true
end

function GameBirdBag:getNumById(itemId)
    if self.bagItems[itemId] ~= nil then
        return self.bagItems[itemId]
    end

    return 0
end

function GameBirdBag:setCurCarryBirdNum(num)
    self.curCarryBirdNum = num
end

function GameBirdBag:addCurCarryBirdNum(num)
    self.curCarryBirdNum = self.curCarryBirdNum + num
    if self.curCarryBirdNum > self.maxCarryBirdNum then
        self.curCarryBirdNum = self.maxCarryBirdNum
    end
end

function GameBirdBag:subCurCarryBirdNum(num)
    self.curCarryBirdNum = self.curCarryBirdNum - num
    if self.curCarryBirdNum < 0 then
        self.curCarryBirdNum = 0
    end
end

function GameBirdBag:setCurCapacityBirdNum(num)
    self.curCapacityBirdNum = num
end

function GameBirdBag:isCanAddNumToCapacity(num)
    return (self.curCapacityBirdNum + num) <= self.maxCapacityBirdNum
end

function GameBirdBag:addCurCapacityBirdNum(num)
    self.curCapacityBirdNum = self.curCapacityBirdNum + num
    if self.curCapacityBirdNum > self.maxCapacityBirdNum then
        self.curCapacityBirdNum = self.maxCapacityBirdNum
    end
end

function GameBirdBag:subCurCapacityBirdNum(num)
    self.curCapacityBirdNum = self.curCapacityBirdNum - num
    if self.curCapacityBirdNum < self.curCarryBirdNum then
        self.curCapacityBirdNum = self.curCarryBirdNum
    end
end

function GameBirdBag:setMaxCarryBirdNum(num)
    self.maxCarryBirdNum = num
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end
    TaskHelper.dispatch(TaskType.HaveBirdShellsEvent, player.userId)
end

function GameBirdBag:addMaxCarryBirdNum(num)
    self.maxCarryBirdNum = self.maxCarryBirdNum + num
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end
    TaskHelper.dispatch(TaskType.HaveBirdShellsEvent, player.userId)
end

function GameBirdBag:setMaxCapacityBirdNum(num)
    self.maxCapacityBirdNum = num
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end
    TaskHelper.dispatch(TaskType.HaveBirdShellsEvent, player.userId)
end

function GameBirdBag:addMaxCapacityBirdNum(num)
    self.maxCapacityBirdNum = self.maxCapacityBirdNum + num
end

function GameBirdBag:expandCarryBag(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if self.maxCarryBirdNum >= BirdBagConfig:getMaxCarryBag() then
        HostApi.sendCommonTip(rakssid, "bird_carry_already_max_level")
        return
    end

    if self.maxCarryBirdNum >= self.maxCapacityBirdNum then
        HostApi.sendCommonTip(rakssid, "update_maxCapacity_first")
        return
    end

    local price = BirdBagConfig:getNextExpandCarryPrice(self.maxCarryBirdNum)
    local moneyType = BirdBagConfig:getNextExpandCarryMoneyType(self.maxCarryBirdNum)

    if moneyType == 1 then
        local key = string.format("%s:%s", "expandCarryBag", tostring(rakssid))
        player:consumeDiamonds(ConsumeId.ExpandCarryBagId, price, key, false)
        ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.onExpandBagSuccess, self)
    end

    if moneyType == 3 then
        if player.money >= price then
            player:subMoney(price)
            self:addMaxCarryBirdNum(1)

            -- 开启鸟巢格子
            if player.userNest ~= nil then
                local vec3 = NestConfig:getNestPosByIndex(player.userNest.nestIndex, self.maxCarryBirdNum)
                if vec3 ~= nil then
                    EngineWorld:setBlock(vec3, 96, 2)
                end
            end
            -- 同步门能否走过
            for i, v in pairs(FieldConfig.door) do
                if tonumber(v.type) == 0 then
                    if tonumber(v.entityId) ~= 0 and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                        EngineWorld:getWorld():updateSessionNpc(v.entityId, player.rakssid, "", v.actorName, v.bodyName, v.bodyId2, "", "", 0, false)
                    end
                elseif tonumber(v.type) == 1 then
                    -- vip
                    if player.isVipArea and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                        EngineWorld:getWorld():updateSessionNpc(v.entityId, player.rakssid, "", v.actorName, v.bodyName, v.bodyId2, "", "", 0, false)
                    end
                end
            end
        else
            -- todo : direct to UI : buy money and add maxCarryBirdNum
            HostApi.sendCommonTip(rakssid, "not_enough_money")
        end
    end
end

function GameBirdBag:expandCapacityBag(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if self.maxCapacityBirdNum >= BirdBagConfig:getMaxCapacityBag() then
        HostApi.sendCommonTip(rakssid, "bird_capacity_already_max_level")
        return
    end

    local price = BirdBagConfig:getNextExpandCapacityPrice(self.maxCapacityBirdNum)
    local moneyType = BirdBagConfig:getNextExpandCapacityMoneyType(self.maxCapacityBirdNum)

    if moneyType == 1 then
        local key = string.format("%s:%s", "expandCapacityBag", tostring(rakssid))
        player:consumeDiamonds(ConsumeId.ExpandCapacityBagId, price, key, false)
        ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.onExpandBagSuccess, self)
    end

    if moneyType == 3 then
        if player.money >= price then
            player:subMoney(price)
            local num = BirdBagConfig:getNextExpandCapacityNum(self.maxCapacityBirdNum)
            self:setMaxCapacityBirdNum(num)
            player:setBirdSimulatorBag()
        else
            -- todo : direct to UI : buy money and add maxCapacityBirdNum
            HostApi.sendCommonTip(rakssid, "not_enough_money")
        end
    end
end

function GameBirdBag:onExpandBagSuccess(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if key == "expandCarryBag:" .. tostring(rakssid) then
        self:addMaxCarryBirdNum(1)
        -- 开启鸟巢格子
        local vec3 = NestConfig:getNestPosByIndex(player.userNest.nestIndex, self.maxCarryBirdNum)
        if vec3 ~= nil then
            EngineWorld:setBlock(vec3, 96, 2)
        end

        -- 同步门能否走过
        for i, v in pairs(FieldConfig.door) do
            if tonumber(v.type) == 0 then
                if tonumber(v.entityId) ~= 0 and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                    EngineWorld:getWorld():updateSessionNpc(
                        v.entityId,
                        player.rakssid,
                        "",
                        v.actorName,
                        v.bodyName,
                        v.bodyId2,
                        "",
                        "",
                        0,
                        false
                    )
                end
            elseif tonumber(v.type) == 1 then
                -- vip
                if player.isVipArea and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                    EngineWorld:getWorld():updateSessionNpc(
                        v.entityId,
                        player.rakssid,
                        "",
                        v.actorName,
                        v.bodyName,
                        v.bodyId2,
                        "",
                        "",
                        0,
                        false
                    )
                end
            end
        end
    end

    if key == "expandCapacityBag:" .. tostring(rakssid) then
        local num = BirdBagConfig:getNextExpandCapacityNum(self.maxCapacityBirdNum)
        self:setMaxCapacityBirdNum(num)
    end

    player:setBirdSimulatorBag()
    return true
end

function GameBirdBag:setNests(nestIndex, createOrDelete)
    for i = 1, self.maxCarryBirdNum do
        local vec3 = NestConfig:getNestPosByIndex(nestIndex, i)
        if vec3 ~= nil then
            if createOrDelete then
                EngineWorld:setBlock(vec3, 96, 2)
            else
                EngineWorld:setBlockToAir(vec3)
            end
        end
    end
end
