require "config.ItemsConfig"
GameBuilding = class()

function GameBuilding:ctor(manorIndex)
    self.manorIndex = manorIndex
    self.prosperity = 0
    self.building = {}
end

function GameBuilding:initBuilding(config)
    local id = config.id
    self.building[id] = {}
    self.building[id].id = config.id
    self.building[id].entityId = config.entityId
    self.building[id].isInWorld = false
    self.building[id].type = config.type
    self.building[id].offset = config.offset
    self.building[id].yaw = config.yaw
    self.building[id].actorId = config.actorId
    self.building[id].curQueueNum = config.curQueueNum
    self.building[id].maxQueueNum = config.maxQueueNum
    self.building[id].productCapacity = config.productCapacity
    self.building[id].queue = {}
    self.building[id].product = {}
end

function GameBuilding:removeAllBuildingFromWorld()
    for i, v in pairs(self.building) do
        if v.entityId ~= 0 then
            EngineWorld:removeEntity(v.entityId)
        end
    end
end

function GameBuilding:getBuildingInfoById(id)
    if self.building[id] ~= nil then
        return self.building[id]
    end

    return nil
end

function GameBuilding:getBuildingInfoByEntityId(entityId)
    for i, v in pairs(self.building) do
        if v.entityId == entityId then
            return v
        end
    end

    return nil
end

function GameBuilding:initDataFromDB(data, userId)
    self:initBuildingFromDB(data.buildingInfo or {}, userId)
end

function GameBuilding:initBuildingFromDB(buildingInfo, userId)
    for i, v in pairs(buildingInfo) do
        self:initBuildingInfoFromDB(v.id, v.actorId, v.yaw, v.type, v.curQNum, v.maxQNum, v.pCapacity, v.inWorld)
        self:initBuildingOffsetFromDB(v.id, v.offset)
        self:initBuildingProductFromDB(v.id, v.product)
        self:initBuildingQueueFromDB(v.id, v.queue)
        self:setBuildingPos(v.id, userId)
    end
end

function GameBuilding:initBuildingInfoFromDB(id, actorId, yaw, type, curQueueNum, maxQueueNum, productCapacity, isInWorld)
    if self.building[id] == nil then
        self.building[id] = {}
    end

    self.building[id].id = id
    self.building[id].actorId = actorId
    self.building[id].entityId = 0
    self.building[id].isInWorld = false
    if isInWorld == 1 then
        self.building[id].isInWorld = true
    end
    self.building[id].type = type
    self.building[id].offset = VectorUtil.newVector3(0, 0, 0)
    self.building[id].yaw = yaw
    self.building[id].curQueueNum = curQueueNum
    self.building[id].maxQueueNum = maxQueueNum
    self.building[id].productCapacity = productCapacity
    self.building[id].queue = {}
    self.building[id].product = {}
end

function GameBuilding:initBuildingOffsetFromDB(id, offset)
    if self.building[id] == false then
        return false
    end

    self.building[id].offset = VectorUtil.newVector3(offset.x, offset.y, offset.z)
end

function GameBuilding:initBuildingProductFromDB(id, products)
    self:addProducts(id, products)
end

function GameBuilding:initBuildingQueueFromDB(id, queues)
    if self.building[id] == nil then
        return false
    end

    local tempQueues = {}
    for i, v in pairs(queues) do
        local queue = {}
        queue.queueId = i
        queue.unlockStatus = v.status
        queue.productId = v.pId
        queue.createTime = v.cTime
        queue.startTime = v.sTime
        queue.needTime = v.nTime
        table.insert(tempQueues, queue)
    end

    self:addQueues(id, tempQueues)
end

function GameBuilding:addQueues(id, queues)
    for i, v in pairs(queues) do
        if self.building[id].queue[i] == nil then
            self.building[id].queue[i] = {}
        end
        self.building[id].queue[i].queueId = v.queueId
        self.building[id].queue[i].unlockStatus = v.unlockStatus
        self.building[id].queue[i].productId = v.productId
        self.building[id].queue[i].createTime = v.createTime
        self.building[id].queue[i].startTime = v.startTime
        self.building[id].queue[i].needTime = v.needTime
        self.building[id].queue[i].isWorking = false
    end
end

function GameBuilding:initFirstAnimalBuilding(userId)
    local buildingInfo = BuildingConfig:getFirstAnimalBuildingInfo()
    local offset = BuildingConfig.firstAnimalBuildingPos
    local yaw = BuildingConfig.firstAnimalBuildingYaw
    local vec3 = ManorConfig:getVec3ByOffset(self.manorIndex, offset)
    if vec3 == nil then
        HostApi.log("=== LuaLog: [error] GameBuilding:initFirstAnimalBuilding: can not get the position of building")
        return false
    end

    local prosperity = BuildingConfig:getProsperityById(buildingInfo.id)
    self.prosperity = self.prosperity + prosperity

    local entityId = EngineWorld:getWorld():addBuildNpc(vec3, yaw, userId, buildingInfo.actorId)
    local config = {
        id = buildingInfo.id,
        entityId = entityId,
        actorId = buildingInfo.actorId,
        type = buildingInfo.type,
        offset = offset,
        yaw = yaw,
        curQueueNum = buildingInfo.initQueueNum,
        maxQueueNum = buildingInfo.maxQueueNum,
        productCapacity = buildingInfo.productCapacity
    }

    self:initBuilding(config)
    self:initAllQueueStatus(config.id, config.curQueueNum, config.maxQueueNum)

    local ranchBuilding = self:getBuildingInfoById(config.id)
    self:initRanchBuildingQueue(ranchBuilding)
end

function GameBuilding:prepareDataSaveToDB()
    local data = {
        buildingInfo = self:prepareBuildingInfoDataToDB()
    }
    --HostApi.log("GameBuilding:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameBuilding:prepareBuildingInfoDataToDB()
    local data = {}
    for i, v in pairs(self.building) do
        local item = {}
        item.id = v.id
        item.actorId = v.actorId
        item.type = v.type
        item.inWorld = 0
        if v.isInWorld == true then
            item.inWorld = 1
        end
        item.curQNum = v.curQueueNum
        item.maxQNum = v.maxQueueNum
        item.pCapacity = v.productCapacity
        item.offset = self:prepareBuildingOffsetDataToDB(v.offset)
        item.yaw = v.yaw
        item.queue = self:prepareBuildingQueueDataToDB(v.queue)
        item.product = self:prepareBuildingProductDataToDB(v.product)
        table.insert(data, item)
    end
    return data
end

function GameBuilding:prepareBuildingOffsetDataToDB(data)
    local offset = {
        x = 0,
        y = 0,
        z = 0
    }

    if data == nil then
        return offset
    end

    offset.x = data.x
    offset.y = data.y
    offset.z = data.z
    return offset
end

function GameBuilding:prepareBuildingQueueDataToDB(data)
    local queues = {}
    if data == nil then
        return queues
    end

    for i, v in pairs(data) do
        local queue = {}
        queue.status = v.unlockStatus
        queue.pId = v.productId
        queue.cTime = v.createTime
        queue.sTime = v.startTime
        queue.nTime = v.needTime
        table.insert(queues, queue)
    end
    return queues
end

function GameBuilding:prepareBuildingProductDataToDB(data)
    local products = {}
    if data == nil then
        return products
    end

    for i, v in pairs(data) do
        local product = {}
        product.itemId = v.itemId
        product.num = v.num
        table.insert(products, product)
    end
    return products
end

function GameBuilding:setBuildingPos(id, userId)
    if self.building[id] == nil then
        return false
    end

    local offset = self.building[id].offset
    if offset.x == 0 and offset.y == 0 and offset.z == 0 then
        return false
    end

    local vec3 = ManorConfig:getVec3ByOffset(self.manorIndex, offset)
    if vec3 == nil then
        HostApi.log("=== LuaLog: [error] GameBuilding:setBuildingPos: can not get the position of building["..id.."]")
        return false
    end

    local yaw = self.building[id].yaw
    local actorId = self.building[id].actorId

    local prosperity = BuildingConfig:getProsperityById(id)
    self.prosperity = self.prosperity + prosperity

    local entityId = EngineWorld:getWorld():addBuildNpc(vec3, yaw, userId, actorId)

    self.building[id].entityId = entityId

    local ranchBuilding = self:getBuildingInfoById(id)
    self:initRanchBuildingQueue(ranchBuilding)
end

function GameBuilding:removeBuilding(id)
    if self.building[id] == nil then
        return false
    end

    self.building[id].offset = VectorUtil.newVector3(0, 0, 0)

    if self.building[id].entityId ~= 0 then
        EngineWorld:removeEntity(self.building[id].entityId)
    end

    self.building[id].entityId = 0
end

function GameBuilding:initRanchBuilding(buildingType)
    local builds = {}

    local tempBuilds = {}
    for i, v in pairs(BuildingConfig.building) do
        if v.type == buildingType then
            table.insert(tempBuilds, v)
        end
    end

    table.sort(tempBuilds, function(a, b)
        if a.playerLevel ~= b.playerLevel then
            return a.playerLevel < b.playerLevel
        end

        return a.id < b.id
    end)

    for i, v in pairs(tempBuilds) do
        local buildingInfo = RanchBuild.new()
        buildingInfo.id = v.id
        buildingInfo.price = v.price
        buildingInfo.num = 0
        buildingInfo.maxNum = 1
        buildingInfo.needLevel = v.playerLevel
        for _, m in pairs(self.building) do
            if m.id == v.id then
                buildingInfo.num = 1
            end
        end
        table.insert(builds, buildingInfo)
    end
    return builds
end

function GameBuilding:getNextQueueStartTime(id, queueId)
    local nextQueueStartTime = os.time()
    local startTime = self:getQueueStartTime(id, queueId)
    local needTime = self:getQueueNeedTime(id, queueId)
    if startTime ~= nil and needTime ~= nil then
        nextQueueStartTime = startTime + needTime
    end
    return nextQueueStartTime
end

function GameBuilding:updateQueueStateById(id, queueId, userId, isCostDiamond)
    local productId = self:getQueueProductId(id, queueId)
    if productId ~= nil and productId ~= 0 then
        local player = PlayerManager:getPlayerByUserId(userId)
        --if player ~= nil then
        --    local exp = ItemsConfig:getExpByItemId(productId)
        --    player:addExp(exp)
        --end
        self:addProduct(id, tonumber(productId), 1)
    end

    local nextQueueStartTime = self:getNextQueueStartTime(id, queueId)
    if isCostDiamond then
        nextQueueStartTime = os.time()
    end
    self:resetQueueStatusById(id, queueId)
    self:startFactoryQueueWorking(id, nextQueueStartTime)
    self:startFarmingQueueWorking(id)
    local buildingInfo = self:getBuildingInfoById(id)
    self:initRanchBuildingQueue(buildingInfo)
end

function GameBuilding:hasFactoryQueueWorking(id)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].type ~= BuildingConfig.TYPE_FACTORY then
        return false
    end

    local isWorking = false

    for i, v in pairs(self.building[id].queue) do
        if v.startTime ~= 0 then
            isWorking = true
        end
    end

    return isWorking
end

function GameBuilding:startFactoryQueueWorking(id, nextQueueStartTime)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].type ~= BuildingConfig.TYPE_FACTORY then
        return false
    end

    if self:isEnoughProductCapacityByNum(id, 1) == false then
        return false
    end

    if self:hasFactoryQueueWorking(id) == true then
        return false
    end

    local nextQueueId = self:getFactoryPriorityQueue(id)
    if next(nextQueueId) ~= nil then
        self:updateQueueStartTime(id, nextQueueId[1].queueId, nextQueueStartTime)
    end
end

function GameBuilding:startFarmingQueueWorking(id)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].type ~= BuildingConfig.TYPE_FARMING then
        return false
    end

    local nextQueueId = self:getFarmingPriorityQueue(id)
    if nextQueueId ~= nil and next(nextQueueId) ~= nil then
        self:updateQueueStartTime(id, nextQueueId[1].queueId, os.time())
    end
end

function GameBuilding:getFactoryPriorityQueue(id)
    if self.building[id] == nil then
        return nil
    end

    local tempQueues = {}
    for i, v in pairs(self.building[id].queue) do
        if v.createTime ~= 0 then
            table.insert(tempQueues, v)
        end
    end

    table.sort(tempQueues, function(a, b)
        if a.createTime ~= b.createTime then
            return a.createTime < b.createTime
        end
        return a.queueId < b.queueId
    end)

    return tempQueues
end

function GameBuilding:getFarmingPriorityQueue(id)
    if self.building[id] == nil then
        return nil
    end

    local tempQueues = {}
    for i, v in pairs(self.building[id].queue) do
        if v.createTime ~= 0 and v.isWorking == false then
            table.insert(tempQueues, v)
        end
    end

    table.sort(tempQueues, function(a, b)
        if a.createTime ~= b.createTime then
            return a.createTime < b.createTime
        end
        return a.queueId < b.queueId
    end)

    return tempQueues
end

function GameBuilding:getRanchBuildingProducts(id)
    local products = {}
    local buildingProduct = self:getBuildingProductById(id)
    for _, v in pairs(buildingProduct) do
        local p = RanchCommon.new()
        p.itemId = v.itemId
        p.num = v.num
        table.insert(products, p)
    end

    return products
end

function GameBuilding:getRanchBuildingQueues(id, queues, productCapacity, type)
    local data = {}
    if queues == nil then
        return data
    end

    local curProductNum = self:getProductNumInCapacity(id)
    local remainProductNum = productCapacity - curProductNum

    for i, v in pairs(queues) do
        local queue = ProductQueue.new()
        queue.queueId = i
        queue.productId = v.productId
        local timeLeft =  v.needTime - (os.time() - v.startTime)
        if timeLeft <= 0 then
            timeLeft = 0
        end
        queue.timeLeft = timeLeft * 1000
        queue.needTime = v.needTime * 1000
        queue.unlockPrice = 0
        queue.currencyType = 0
        queue.state = BuildingConfig.QUEUE_LEISURE
        if v.productId ~= 0 then

            if type == BuildingConfig.TYPE_FACTORY then
                queue.state = BuildingConfig.QUEUE_WORKING
                if v.queueId ~= self:getFactoryPriorityQueue(id)[1].queueId then
                    -- 不是第一优先级不能生产
                    queue.state = BuildingConfig.QUEUE_WAIT
                    self:updateQueueStartTime(id, v.queueId, 0)
                    queue.timeLeft = v.needTime * 1000
                end

                if self:isEnoughProductCapacityByNum(id, 1) == false then
                    --容量满了不能生产
                    queue.state = BuildingConfig.QUEUE_WAIT
                    self:updateQueueStartTime(id, v.queueId, 0)
                    queue.timeLeft = v.needTime * 1000
                end

                if self.building[id].isInWorld == false then
                    -- 如果建筑不在地图上，不生产
                    queue.state = BuildingConfig.QUEUE_WAIT
                    queue.timeLeft = v.needTime * 1000
                end

            elseif type == BuildingConfig.TYPE_FARMING then
                -- 生产前判断容量和在生产的队列

                queue.state = BuildingConfig.QUEUE_WAIT
                if v.isWorking == true then
                    queue.state = BuildingConfig.QUEUE_WORKING
                end

                local workingNum = self:getQueueWorkingStateNum(id)
                local remainProductQueueNum = remainProductNum - workingNum
                if remainProductQueueNum > 0 then

                    local tempQueues = self:getFarmingPriorityQueue(id)
                    if tempQueues ~= nil and tempQueues[1] ~= nil then
                        if v.queueId == tempQueues[1].queueId then
                            queue.state = BuildingConfig.QUEUE_WORKING
                            self:updateQueueWorkingState(id, v.queueId, true)
                        end
                    end
                end

                if self.building[id].isInWorld == false then
                    queue.state = BuildingConfig.QUEUE_WAIT
                    queue.timeLeft = v.needTime * 1000
                    self:updateQueueWorkingState(id, v.queueId, false)
                end

                if queue.state == BuildingConfig.QUEUE_WAIT and self.building[id].isInWorld == true then
                    self:updateQueueStartTime(id, v.queueId, 0)
                    queue.timeLeft = v.needTime * 1000
                end
            end
        end

        if v.unlockStatus == 0 then
            queue.state = BuildingConfig.QUEUE_LOCK
            local queueUnlockMoney = BuildingConfig:getQueuesUnlockMoneyById(id)
            if queueUnlockMoney ~= nil then
                queue.unlockPrice = queueUnlockMoney[i].money
                queue.currencyType = queueUnlockMoney[i].moneyType
            end
        end
        table.insert(data, queue)
    end

    return data
end

function GameBuilding:getRanchBuildingRecipes(id)
    local recipes = {}
    local data = BuildingConfig:getProductInfoById(id)
    if data ~= nil then
        for i, v in pairs(data) do
            local recipe = ProductRecipe.new()
            recipe.productId = v
            recipe.needLevel = 0
            recipe.needTime = 0
            recipe.items = {}
            for _, item in pairs(ItemsConfig.items) do
                if v == item.itemId then
                    recipe.needLevel = item.level
                    recipe.needTime = item.needTime * 1000
                end
            end

            local ranchCommonItems = {}
            local recipeItems = ItemsConfig:getRecipeItemsById(v)
            if recipeItems ~= nil then
                for _, recipeItem in pairs(recipeItems) do
                    local ranchCommonItem = RanchCommon.new()
                    ranchCommonItem.itemId = recipeItem.itemId
                    ranchCommonItem.num = recipeItem.num
                    table.insert(ranchCommonItems, ranchCommonItem)
                end
            end

            recipe.items = ranchCommonItems
            table.insert(recipes, recipe)
        end
    end
    return recipes
end

function GameBuilding:initRanchBuildingQueue(config)
    if config == nil then
        return false
    end

    local products = self:getRanchBuildingProducts(config.id)
    local queues = self:getRanchBuildingQueues(config.id, config.queue, config.productCapacity, config.type)
    local recipes = self:getRanchBuildingRecipes(config.id)

    local unlockMoney = 0
    local moneyType = 3
    local data = BuildingConfig:getQueueUnlockMoneyByQueueId(config.id, config.curQueueNum + 1)
    if data ~= nil then
        unlockMoney = data.money
        moneyType = data.moneyType
    end

    EngineWorld:getWorld():updateBuildNpc(config.entityId, config.maxQueueNum, config.productCapacity, unlockMoney, moneyType, products, queues, recipes)
end

function GameBuilding:addCurQueueNum(id)
    if self.building[id] == nil then
        return false
    end

    self.building[id].curQueueNum = self.building[id].curQueueNum + 1
    if self.building[id].curQueueNum > self.building[id].maxQueueNum then
        self.building[id].curQueueNum = self.building[id].maxQueueNum
    end
end

function GameBuilding:unlockQueueById(id, queueId)
    if self.building[id] == nil then
        return false
    end

    self:addCurQueueNum(id)
    self:initQueueStatusById(id, queueId)
    local buildingInfo = self:getBuildingInfoById(id)
    self:initRanchBuildingQueue(buildingInfo)
end

function GameBuilding:initQueueStatusById(id, queueId)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].queue[queueId] == nil then
        self.building[id].queue[queueId] = {}
    end
    self.building[id].queue[queueId].queueId = queueId
    self.building[id].queue[queueId].unlockStatus = BuildingConfig.STATE_UNLOCK
    self.building[id].queue[queueId].productId = 0
    self.building[id].queue[queueId].createTime = 0
    self.building[id].queue[queueId].startTime = 0
    self.building[id].queue[queueId].needTime = 0
    self.building[id].queue[queueId].isWorking = false
end

function GameBuilding:resetQueueStatusById(id, queueId)
    if self.building[id] == nil then
        return false
    end

    table.remove(self.building[id].queue, queueId)

    local queue = {}
    queue.queueId = id
    queue.unlockStatus = BuildingConfig.STATE_UNLOCK
    queue.productId = 0
    queue.createTime = 0
    queue.startTime = 0
    queue.needTime = 0
    queue.isWorking = false
    table.insert(self.building[id].queue, queue)

    for i, v in pairs(self.building[id].queue) do
        v.queueId = i
    end

end

function GameBuilding:initAllQueueStatus(id, curQueueNum, maxQueueNum)
    if self.building[id] == nil then
        return false
    end
    self:initQueueStatusToUnlockByCurQueueNum(id, curQueueNum)
end

function GameBuilding:initQueueStatusToUnlockByCurQueueNum(id, curQueueNum)
    if self.building[id] == nil then
        return false
    end

    for i = 1, curQueueNum do
        self:initQueueStatusById(id, i)
    end
end

function GameBuilding:updateQueueStartTime(id, queueId, startTime)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].queue[queueId] == nil then
        return false
    end
    self.building[id].queue[queueId].startTime = startTime
end

function GameBuilding:updateQueueWorkingState(id, queueId, isWorking)
    if self.building[id] == nil then
        return false
    end

    if self.building[id].queue[queueId] == nil then
        return false
    end
    self.building[id].queue[queueId].isWorking = isWorking
end

function GameBuilding:getFirstLockedQueueId(id)
    if self.building[id] == nil then
        return 0
    end

    local index = 0
    for i, v in pairs(self.building[id].queue) do
        index = index + 1
    end

    if index + 1 > self.building[id].maxQueueNum then
        return 0
    end

    return index + 1
end

function GameBuilding:getFirstIdleQueueId(id)
    if self.building[id] == nil then
        return 0
    end

    for i, v in pairs(self.building[id].queue) do
        if v.productId == 0 and v.unlockStatus == BuildingConfig.STATE_UNLOCK then
            return i
        end
    end

    return 0
end

function GameBuilding:addWorkingQueue(id, queueId, productId, needTime)
    if self.building[id] == nil then
        return false
    end

    if queueId == 0 then
        return false
    end

    if self.building[id].queue[queueId] == nil then
        self.building[id].queue[queueId] = {}
    end

    self.building[id].queue[queueId].queueId = queueId
    self.building[id].queue[queueId].unlockStatus = BuildingConfig.STATE_UNLOCK
    self.building[id].queue[queueId].productId = productId
    self.building[id].queue[queueId].createTime = os.time()
    self.building[id].queue[queueId].startTime = os.time()
    self.building[id].queue[queueId].needTime = needTime
end

function GameBuilding:getQueueProductId(id, queueId)
    if self.building[id] == nil then
        return nil
    end

    if self.building[id].queue[queueId] == nil then
        return nil
    end

    return self.building[id].queue[queueId].productId
end

function GameBuilding:getQueueStartTime(id, queueId)
    if self.building[id] == nil then
        return nil
    end

    if self.building[id].queue[queueId] == nil then
        return nil
    end

    return self.building[id].queue[queueId].startTime
end

function GameBuilding:getQueueNeedTime(id, queueId)
    if self.building[id] == nil then
        return nil
    end

    if self.building[id].queue[queueId] == nil then
        return nil
    end

    return self.building[id].queue[queueId].needTime
end

function GameBuilding:getQueueWorkingStateNum(id)
    if self.building[id] == nil then
        return 0
    end

    local num = 0

    for i, v in pairs(self.building[id].queue) do
        if v.isWorking == true then
            num = num + 1
        end
    end

    return num
end

function GameBuilding:getBuildingProductById(id)
    if self.building[id] == nil then
        return nil
    end

    return self.building[id].product
end

function GameBuilding:setProducts(id, items)
    if self.building[id] == nil then
        return false
    end

    for i, v in pairs(items) do
        self:setProduct(id, v.id, v.num)
    end
end

function GameBuilding:setProduct(id, itemId, num)
    if self.building[id] == nil then
        return false
    end

    local p = {}
    p.itemId = itemId
    p.num = num
    table.insert(self.building[id].product, p)
end

function GameBuilding:getProductNumInCapacity(id)
    if self.building[id] == nil then
        return 0
    end

    local totalNum = 0
    for i, v in pairs(self.building[id].product) do
        totalNum = totalNum + v.num
    end

    return totalNum
end

function GameBuilding:isEnoughProductCapacityByNum(id, num)
    if self.building[id] == nil then
        HostApi.log("=== LuaLog: [error] GameBuilding:isEnoughProductCapacityByNum: can not find building["..id.."]")
        return false
    end

    local totalNum = self:getProductNumInCapacity(id)
    totalNum = totalNum + num
    if totalNum > self.building[id].productCapacity then
        --HostApi.log("=== LuaLog: GameBuilding:isEnoughProductCapacityByNum: can not add Item because product Capacity of building["..id.."] is full")
        return false
    end

    return true
end

function GameBuilding:addProducts(id, items)
    if self.building[id] == nil then
        return false
    end

    for _, v in pairs(items) do
        self:addProduct(id, v.itemId, v.num)
    end

end

function GameBuilding:addProduct(id, itemId, num)
    if itemId == 0 or num == 0 then
        return false
    end

    if self.building[id] == nil then
        return false
    end

    local product = {}
    product.itemId = itemId
    product.num = num
    table.insert(self.building[id].product, product)
end

function GameBuilding:subProduct(id, index)
    if index == 0 then
        return false
    end

    if self.building[id] == nil then
        return false
    end

    if #self.building[id].product <= 0 then
        return false
    end

    table.remove(self.building[id].product, index)
    return true
end
