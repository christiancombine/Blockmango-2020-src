GameWareHouse = class()

function GameWareHouse:ctor(manorIndex)
    self.manorIndex = manorIndex
    self.level = 1
    self.entityId = 0
    self.wareHouse = {}
    self.wareHouseItems = {}
    self:initWareHousePos()
end

function GameWareHouse:prepareDataSaveToDB()
    local data = {
        level = self.level,
        wareHouseItems = self:prepareWareHouseItemsToDB(),
        wareHouse = self:prepareWareHouseToDB()
    }
    --HostApi.log("GameWareHouse:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameWareHouse:prepareWareHouseItemsToDB()
    local wareHouseItems = {}
    local index = 1
    for i, v in pairs(self.wareHouseItems) do
        wareHouseItems[index] = {}
        wareHouseItems[index].itemId = i
        wareHouseItems[index].num = v
        index = index + 1
    end
    return wareHouseItems
end

function GameWareHouse:prepareWareHouseToDB()
    local wareHouse = {}
    wareHouse.offset = {}
    wareHouse.offset.x = self.wareHouse.offset.x
    wareHouse.offset.y = self.wareHouse.offset.y
    wareHouse.offset.z = self.wareHouse.offset.z
    wareHouse.yaw = self.wareHouse.yaw
    return wareHouse
end

function GameWareHouse:removeWareHouseFromWorld()
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
    end
end

function GameWareHouse:initDataFromDB(data)
    self.level = data.level or 1
    self:initWareHouseItemsFromDB(data.wareHouseItems or {})
    self:initWareHouseFromDB(data.wareHouse or {})
end

function GameWareHouse:addPlayerInitItems()
    for i, v in pairs(PlayerInitItemsConfig.items) do
        self:addItem(v.itemId, v.num)
    end
end

function GameWareHouse:initWareHouseItemsFromDB(wareHouseItems)
    for i, v in pairs(wareHouseItems) do
        self:initWareHouseItem(v.itemId, v.num)
    end
end

function GameWareHouse:initWareHouseFromDB(wareHouse)
    self.wareHouse.offset = VectorUtil.newVector3(wareHouse.offset.x, wareHouse.offset.y, wareHouse.offset.z)
    self.wareHouse.yaw = wareHouse.yaw
end

function GameWareHouse:initWareHouseItem(id, num)
    if self.wareHouseItems[id] == nil then
        self.wareHouseItems[id] = num
    else
        self.wareHouseItems[id] = self.wareHouseItems[id] + num
    end
end

function GameWareHouse:initWareHousePos()
    self.wareHouse.offset = WareHouseConfig.wareHouseInitPos
    self.wareHouse.yaw = WareHouseConfig.wareHouseInitYaw
end

function GameWareHouse:setWareHouseOffset(offset, yaw)
    self.wareHouse.offset = VectorUtil.newVector3(offset.x, offset.y, offset.z)
    self.wareHouse.yaw = yaw
end

function GameWareHouse:setWareHousePos(userId)
    local vec3 = self.wareHouse.offset
    if vec3.x == 0 and vec3.y == 0 and vec3.z == 0 then
        return false
    end

    local yaw = self.wareHouse.yaw
    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    if wareHouseInfo == nil then
        HostApi.log("=== LuaLog: [error] GameWareHouse:setWareHousePos: can not find wareHouse Info by level[" .. self.level .."]")
        return false
    end

    local actorId = wareHouseInfo.actorId

    local pos = ManorConfig:getVec3ByOffset(self.manorIndex, vec3)
    if pos ~= nil then
        self.entityId = EngineWorld:getWorld():addBuildNpc(pos, yaw, userId, actorId)
    end

end

function GameWareHouse:removeWareHousePos()
    self.wareHouse.offset = VectorUtil.newVector3(0, 0, 0)
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
    end
    self.entityId = 0
end

function GameWareHouse:initRanchWareHouse()
    return WareHouseConfig:newRanchWareHouseInfo(self.level, self.wareHouseItems)
end

function GameWareHouse:upgrade(userId)
    local maxLevel = WareHouseConfig:getMaxLevel()
    local preWareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    local preActorId = preWareHouseInfo.actorId

    self.level = self.level + 1
    if self.level > maxLevel then
        self.level = maxLevel
    end

    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    local actorId = wareHouseInfo.actorId

    if preActorId == actorId then
        return false
    end

    local vec3 = self.wareHouse.offset
    if vec3.x == 0 and vec3.y == 0 and vec3.z == 0 then
        return false
    end

    local yaw = self.wareHouse.yaw
    local pos = ManorConfig:getVec3ByOffset(self.manorIndex, vec3)
    if pos ~= nil then
        EngineWorld:removeEntity(self.entityId)
        self.entityId = EngineWorld:getWorld():addBuildNpc(pos, yaw, userId, actorId)
    end

end

function GameWareHouse:isHasItems(items)
    local isEnoughItems = true

    for _, item in pairs(items) do
        if self:isHasItem(item.itemId, item.num) == false then
            isEnoughItems = false
        end
    end

    if isEnoughItems == false then
        --HostApi.log("=== LuaLog: GameWareHouse:isHasItems: Not has enough items to do someThing ...")
        return false
    end

    return true
end

function GameWareHouse:isHasItem(id, num)
    if id == 0 or num == 0 then
        return true
    end

    for i, v in pairs(self.wareHouseItems) do
        if i == id and v >= num then
            return true
        end
    end

    return false
end

function GameWareHouse:getCurrentItemsNum()
    local totalNum = 0
    for i, v in pairs(self.wareHouseItems) do
        totalNum = totalNum + v
    end

    return totalNum
end

function GameWareHouse:isEnoughCapacity(items)
    local totalNum = self:getCurrentItemsNum()
    for i, item in pairs(items) do
        totalNum = totalNum + item.num
    end

    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    if wareHouseInfo == nil then
        HostApi.log("=== LuaLog: GameWareHouse:isEnoughCapacity : can not find wareHouseInfo by level[" .. self.level .. "]")
        return false
    end

    if totalNum  >  wareHouseInfo.capacity then
        --HostApi.log("=== LuaLog: can not add Item because the wareHouse is full ... ")
        return false
    end

    return true
end

function GameWareHouse:isEnoughCapacityByNum(num)
    local totalNum = self:getCurrentItemsNum()
    totalNum = totalNum + num

    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    if wareHouseInfo == nil then
        HostApi.log("=== LuaLog: GameWareHouse:isEnoughCapacityByNum : can not find wareHouseInfo by level[" .. self.level .. "]")
        return false
    end

    if totalNum  >  wareHouseInfo.capacity then
        --HostApi.log("=== LuaLog: can not add Item because the wareHouse is full ... ")
        return false
    end

    return true
end


function GameWareHouse:addItems(items)
    for _, item in pairs(items) do
        self:addItem(item.itemId, item.num)
    end
end

function GameWareHouse:addItem(id, num)
    if id == 0 or num == 0 then
        return false
    end

    if self.wareHouseItems[id] == nil then
        self.wareHouseItems[id] = 0
    end
    self.wareHouseItems[id] =  self.wareHouseItems[id] + num

    return true
end

function GameWareHouse:subItems(items)
    for _, item in pairs(items) do
        self:subItem(item.itemId, item.num)
    end
end

function GameWareHouse:subItem(id, num)
    if id == 0 or num == 0 then
        return false
    end

    if self.wareHouseItems[id] == nil then
        return false
    end

    if self.wareHouseItems[id] == nil then
        return false
    end

    self.wareHouseItems[id] = self.wareHouseItems[id] - num
    if self.wareHouseItems[id] <= 0 then
        self.wareHouseItems[id] = nil
    end
end

function GameWareHouse:getSellItemMoney(id, num)
    if id == 0 or num == 0 then
        return 0
    end

    if self.wareHouseItems[id] == nil then
        return 0
    end

    local itemInfo = ItemsConfig:getItemInfoById(id)
    if itemInfo == nil then
        return 0
    end

    if self.wareHouseItems[id] - num >= 0 then
        return itemInfo.price * num
    end

    return self.wareHouseItems[id] * itemInfo.price
end

function GameWareHouse:getCubePriceCostByTaskItem(itemId, num)
    if num <= 0 then
        return 0
    end

    local price = ItemsConfig:getCubePriceByItemId(itemId)
    local needNum = num

    for i, v in pairs(self.wareHouseItems) do
        if i == itemId then
            needNum = num - v
        end
    end

    if needNum <= 0 then
        return 0
    end

    return needNum * price
end

function GameWareHouse:getNumByItemId(itemId)
    for i, v in pairs(self.wareHouseItems) do
        if i == itemId then
            return v
        end
    end

    return 0
end

function GameWareHouse:getCubeMoneyByItems(items)
    local totalMoney = 0
    for i, v in pairs(items) do
        local price = ItemsConfig:getCubePriceByItemId(v.itemId)
        totalMoney = totalMoney + price * v.num
    end

    for i, v in pairs(items) do
        for wItemId, wNum in pairs(self.wareHouseItems) do
            if wItemId == v.itemId and wNum > 0 then
                local price = ItemsConfig:getCubePriceByItemId(v.itemId)
                if  v.num > wNum then
                    totalMoney = totalMoney - price * wNum
                else
                    totalMoney = totalMoney - price * v.num
                end
            end
        end
    end

    return totalMoney
end

function GameWareHouse:subItemsViaUseCubeMoney(items)
    for i, v in pairs(items) do
        for wItemId, wNum in pairs(self.wareHouseItems) do
            if wItemId == v.itemId and wNum > 0 then
                if  v.num > wNum then
                    self:subItem(v.itemId, wNum)
                else
                    self:subItem(v.itemId, v.num)
                end
            end
        end
    end
end

function GameWareHouse:upgradeWareHouse(userId, upgradeMoney)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    self:upgrade(userId)
    player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, upgradeMoney)
    local ranchAchievements = player.achievement:initRanchAchievements()
    player:getRanch():setAchievements(ranchAchievements)

    local ranchWareHouse = self:initRanchWareHouse()
    player:getRanch():setStorage(ranchWareHouse)
    HostApi.sendCommonTip(player.rakssid, "ranch_tip_upgrade_successful")

    local preWareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level - 1)
    if preWareHouseInfo == nil then
        return false
    end

    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(self.level)
    if wareHouseInfo == nil then
        return false
    end

    local offset = self.wareHouse.offset

    if offset ~= nil and offset.x ~= 0 and offset.y ~= 0 and offset.z ~= 0 then
        -- wareHouse is in the world
        if preWareHouseInfo.actorId == wareHouseInfo.actorId then
            return false
        end

        EngineWorld:removeEntity(self.entityId)
        self:setWareHousePos(userId)

    else
        -- wareHouse is not in the world
        local preMappingId = preWareHouseInfo.actorId
        local preItemId = ItemsMappingConfig:getSourceIdByMappingId(preMappingId)
        if preItemId == 0 then
            return false
        end

        local mappingId = wareHouseInfo.actorId
        local itemId = ItemsMappingConfig:getSourceIdByMappingId(mappingId)
        if itemId == 0 then
            return false
        end

        if itemId == preItemId then
            return false
        end

        player:removeItem(preItemId, 1)
        player:addItem(itemId, 1, 0)
    end
end