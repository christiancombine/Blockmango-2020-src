require "Match"

LogicListener = {}

function LogicListener:init()
    PlaceItemBuildingEvent.registerCallBack(self.onPlaceItemBuilding)
    PlaceItemHouseEvent.registerCallBack(self.onPlaceItemHouse)
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    GetRanchShortcutCostEvent.registerCallBack(self.onGetRanchShortcutCost)
    CanRanchStorageSaveItemsEvent.registerCallBack(self.onCanRanchStorageSaveItems)
    GetRanchBuildItemCostEvent.registerCallBack(self.onGetRanchBuildItemCost)
    GetRanchBuildingSpeedUpCostEvent.registerCallBack(self.onGetRanchBuildingSpeedUpCost)
    GetRanchBuildQueueUnlockCostEvent.registerCallBack(self.onGetRanchBuildQueueUnlockCost)
    RemoveItemBuildingEvent.registerCallBack(self.onRemoveItemBuilding)
    GetRanchTaskItemsCostEvent.registerCallBack(self.onGetRanchTaskItemsCost)
end

function LogicListener.onPlaceItemBuilding(userId, actorId, actorType, vec3, yaw, startPos, endPos)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(userId)
    if manorInfo == nil then
        return false
    end

    local isInArea = true

    for i = startPos.x, endPos.x do
        local pos = VectorUtil.newVector3(i, startPos.y, startPos.z)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.z, endPos.z do
        local pos = VectorUtil.newVector3(startPos.x, startPos.y, i)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.z, endPos.z do
        local pos = VectorUtil.newVector3(endPos.x, startPos.y, i)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.x, endPos.x do
        local pos = VectorUtil.newVector3(i, startPos.y, endPos.z)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    if isInArea == false then
        return false
    end

    for i = startPos.x, endPos.x - 1 do
        for k = startPos.y - 1, startPos.y do
            for j = startPos.z, endPos.z - 1 do
                local blockPos = VectorUtil.newVector3i(i, k, j)
                local blockId = EngineWorld:getBlockId(blockPos)
                if blockId == 60 or blockId == 255 then
                    --HostApi.log("=== LuaLog: LogicListener.onPlaceItemBuilding : Field or Roads in the area from startPos to endPos")
                    return false
                end
            end
        end
    end

    -- 判断与房子的重合
    -- 房子的offset = 0，0，0 可以放置
    if player.house ~= nil then
        local houseOffset = player.house.house.offset
        if houseOffset.x ~= 0 or houseOffset.y ~= 0 or houseOffset.z ~= 0 then
            local houseStartPos = ManorConfig:getVec3ByOffset(player.manorIndex, houseOffset)
            if houseStartPos == nil then
                houseStartPos = VectorUtil.newVector3(0, 0, 0)
            end
            local houseInfo = HouseConfig:getHouseInfoByLevel(player.house.houseLevel)
            if houseInfo ~= nil then
                local length = houseInfo.length
                local width = houseInfo.width
                local height = houseInfo.height

                local houseEndPosX = houseStartPos.x + length
                local houseEndPosY = houseStartPos.y + height
                local houseEndPosZ = houseStartPos.z + width

                local isInHouse = false

                local minX = math.min(houseStartPos.x, houseEndPosX) + 1
                local minY = math.min(houseStartPos.y, houseEndPosY)
                local minZ = math.min(houseStartPos.z, houseEndPosZ) + 1

                local maxX = math.max(houseStartPos.x, houseEndPosX) - 1
                local maxY = math.max(houseStartPos.y, houseEndPosY)
                local maxZ = math.max(houseStartPos.z, houseEndPosZ) - 1

                local startPos2 = VectorUtil.newVector3(startPos.x, startPos.y, endPos.z)
                local startPos3 = VectorUtil.newVector3(endPos.x, startPos.y, startPos.z)

                if startPos.x >= minX and startPos.x <= maxX
                and startPos.y >= minY and startPos.y <= maxY
                and startPos.z >= minZ and startPos.z <= maxZ then
                    isInHouse = true
                end

                if startPos2.x >= minX and startPos2.x <= maxX
                and startPos2.y >= minY and startPos2.y <= maxY
                and startPos2.z >= minZ and startPos2.z <= maxZ then
                    isInHouse = true
                end

                if startPos3.x >= minX and startPos3.x <= maxX
                and startPos3.y >= minY and startPos3.y <= maxY
                and startPos3.z >= minZ and startPos3.z <= maxZ then
                    isInHouse = true
                end

                if endPos.x >= minX and endPos.x <= maxX
                and endPos.y >= minY and endPos.y <= maxY
                and endPos.z >= minZ and endPos.z <= maxZ then
                    isInHouse = true
                end

                if isInHouse == true then
                    return false
                end

            end
        end
    end

    local newVec3i = VectorUtil.newVector3i(vec3.x, vec3.y - 1, vec3.z)
    local blockId = EngineWorld:getBlockId(newVec3i)
    if blockId ~= 2 then
        return false
    end

    local offset = ManorConfig:getOffset3ByVec3(player.manorIndex, vec3)
    if offset == nil then
        return false
    end

    if actorType == 1 or actorType == 2 then
        -- 养殖 和 生产
        local buildingInfo = BuildingConfig:getBuildingInfoByActorId(actorId)
        if buildingInfo == nil then
            return false
        end

        local prosperity = BuildingConfig:getProsperityById(buildingInfo.id)
        player.building.prosperity = player.building.prosperity + prosperity

        local ranchInfo = player:initRanchInfo()
        player:getRanch():setInfo(ranchInfo)

        local entityId = EngineWorld:getWorld():addBuildNpc(vec3, yaw, userId, actorId)

        local _buildingInfo = player.building.building[buildingInfo.id]

        local curQueueNum = buildingInfo.initQueueNum
        if _buildingInfo ~= nil then
            curQueueNum = _buildingInfo.curQueueNum
        end

        local config = {
            id = buildingInfo.id,
            entityId = entityId,
            actorId = actorId,
            offset = offset,
            yaw = yaw,
            type = buildingInfo.type,
            curQueueNum = curQueueNum,
            maxQueueNum = buildingInfo.maxQueueNum,
            productCapacity = buildingInfo.productCapacity
        }
        
        local buildingData = player.building.building[buildingInfo.id]

        if buildingData == nil then
            player.building:initBuilding(config)
            player.building:initAllQueueStatus(config.id, config.curQueueNum, config.maxQueueNum)
        else
            local queues = buildingData.queue

            local products = {}
            for i, v in pairs(buildingData.product) do
                local p = {}
                p.itemId = v.itemId
                p.num = v.num
                table.insert(products, p)
            end

            player.building:initBuilding(config)
            player.building:initAllQueueStatus(config.id, config.curQueueNum, config.maxQueueNum)

            player.building:addQueues(buildingInfo.id, queues)
            player.building:addProducts(buildingInfo.id, products)
        end

        local ranchBuilding = player.building:getBuildingInfoById(config.id)
        player.building:initRanchBuildingQueue(ranchBuilding)

        return true
    end

    if actorType == 3 then
        -- 仓库
        player.wareHouse:setWareHouseOffset(offset, yaw)
        player.wareHouse:setWareHousePos(userId)

        local ranchInfo = player:initRanchInfo()
        player:getRanch():setInfo(ranchInfo)

        return true
    end

    return false
end

function LogicListener.onPlaceItemHouse(userId, fileName, startPos, endPos)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(userId)
    if manorInfo == nil then
        return false
    end

    local isInArea = true

    for i = startPos.x, endPos.x do
        local pos = VectorUtil.newVector3(i, startPos.y + 1, startPos.z)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.z, endPos.z do
        local pos = VectorUtil.newVector3(startPos.x, startPos.y + 1, i)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.z, endPos.z do
        local pos = VectorUtil.newVector3(endPos.x, startPos.y + 1, i)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    for i = startPos.x, endPos.x do
        local pos = VectorUtil.newVector3(i, startPos.y + 1, endPos.z)
        if manorInfo:isInUnlockedArea(pos) == false then
            isInArea = false
            break
        end
    end

    if isInArea == false then
        return false
    end

    for i = startPos.x, endPos.x - 1 do
        for k = startPos.y, startPos.y + 1 do
            for j = startPos.z, endPos.z - 1 do
                local blockPos = VectorUtil.newVector3i(i, k, j)
                local blockId = EngineWorld:getBlockId(blockPos)
                if blockId == 60 or blockId == 255 then
                    return false
                end
            end
        end
    end

    local blockPos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    local blockId = EngineWorld:getBlockId(blockPos)
    if blockId ~= 2 then
        return false
    end

    local pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    local offset = ManorConfig:getOffset3iByVec3(player.manorIndex, pos)

    player.house:setHouseOffset(offset)
    player.house:setHouseToWorld()

    local ranchInfo = player:initRanchInfo()
    player:getRanch():setInfo(ranchInfo)

    return true
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

function LogicListener.onGetRanchShortcutCost(rakssid, areaId, isSuccess, money)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onGetRanchShortcutCost : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manor == nil then
        HostApi.log("=== LuaLog: LogicListener.onGetRanchShortcutCost: can not find manorInfo of player[".. tostring(player.userId) .."]")
        return false
    end

    local data = player.manor.waitForUnlock[areaId]
    if data == nil then
        HostApi.log("=== LuaLog: LogicListener.onGetRanchShortcutCost: can not find waitForUnlock[".. areaId .."] of player[".. tostring(player.userId) .."]")
        return false
    end

    local remainTime = data.totalTime - (os.time() - data.startTime)

    if remainTime <= 0 then
        money.value = 0
    else
        money.value = TimePaymentConfig:getPaymentByTime(remainTime)
    end

    isSuccess.value = true

end

function LogicListener.onCanRanchStorageSaveItems(rakssid, items, moneyType, price, isCanSave)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onCanRanchStorageSaveItems : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onCanRanchStorageSaveItems : can not find wareHouseInfo of player["..tostring(player.userId).."]")
        return false
    end

    local num = 0
    for i, v in pairs(items) do
        num = num + v.num
    end

    if player.wareHouse:isEnoughCapacityByNum(num) == false then
        HostApi.sendRanchWarehouseResult(player.rakssid, "gui_ranch_storage_full_operation_failure")
        return false
    end

    local totalPrice = 0
    for _, item in pairs(items) do
        for _, c_item in pairs(ItemsConfig.items) do
            if item.itemId == c_item.itemId then
                if moneyType < 3 then
                    totalPrice = totalPrice + (c_item.cubePrice * item.num)
                else
                    totalPrice = totalPrice + (c_item.price * item.num)
                end
            end
        end
    end

    isCanSave.value = true
    price.value = totalPrice
end

function LogicListener.onGetRanchBuildItemCost(rakssid, type, id, num, isSuccess, moneyType, totalPrice)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildItemCost : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    local itemId = ItemsMappingConfig:getSourceIdByMappingId(id)

    local inv = player:getInventory()
    if inv:isCanAddItem(itemId, 0, num) == false then
        HostApi.sendCommonTip(rakssid, "ranch_tip_package_full")
        return false
    end

    if type == 1 or type == 2 then
        -- 养殖商店 和 工厂商店
        local buildingInfo = BuildingConfig:getBuildingInfoById(id)
        if buildingInfo == nil then
            HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildItemCost :can not find buildingInfo by id[".. id .."]")
            return false
        end

        for i, v in pairs(player.building.building) do
            if v.id == id then
                return false
            end
        end

        moneyType.value = buildingInfo.moneyType
        if moneyType.value == 3 and player.money < totalPrice.value then
            return false
        end
        totalPrice.value = buildingInfo.price * num
        isSuccess.value = true
    end

    if type == 3 then
        -- 种植商店
        if id == 500000 then
            -- 田地
            local maxFieldNum = FieldLevelConfig:getMaxFieldNumByPlayerLevel(player.level)
            if player.currentFieldNum >= maxFieldNum then
                return false
            end

            moneyType.value = 3
            totalPrice.value = FieldLevelConfig:getMoneyByFieldLevel(player.fieldLevel) * num
            if player.money < totalPrice.value  then
                return false
            end

            isSuccess.value = true
            return true
        end

        -- 种子
        totalPrice.value = SeedLevelConfig:getMoneyBySeedId(id) * num
        if player.money < totalPrice.value then
            return false
        end

        moneyType.value = 3
        isSuccess.value = true
        return true
    end
end

function LogicListener.onGetRanchBuildingSpeedUpCost(rakssid, entityId, queueId, productId, isSuccess, money)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildingSpeedUpCost : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildingSpeedUpCost: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildingSpeedUpCost: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local queue = buildingInfo.queue[queueId]
    if queue == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildingSpeedUpCost: can not find queueInfo by ["..queueId.."]")
        return false
    end

    local startTime = queue.startTime
    local needTime = queue.needTime

    local remainTime = needTime - (os.time() - startTime)

    if remainTime <= 0 then
        money.value = 0
    else
        money.value = TimePaymentConfig:getPaymentByTime(remainTime)
    end

    isSuccess.value = true

    if player.accelerateItems ~= nil then
        for i, v in pairs(player.accelerateItems.items) do
            if v.itemId == queue.productId and v.num > 0 then
                money.value = 0
            end
        end
    end
end

function LogicListener.onGetRanchBuildQueueUnlockCost(rakssid, entityId, queueId, isSuccess, moneyType, totalPrice)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildQueueUnlockCost : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildQueueUnlockCost: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildQueueUnlockCost: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local queueIndex = player.building:getFirstLockedQueueId(buildingInfo.id)
    if queueIndex == 0 then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchBuildQueueUnlockCost: can not find queueInfo by ["..queueIndex.."]")
        return false
    end

    local money = BuildingConfig:getQueueUnlockMoneyByQueueId(buildingInfo.id, queueIndex)
    if money == nil then
        return false
    end

    moneyType.value = money.moneyType
    totalPrice.value = money.money
    isSuccess.value = true
end

function LogicListener.onRemoveItemBuilding(rakssid, actorId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onRemoveItemBuilding : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    local buildingInfo = BuildingConfig:getBuildingInfoByActorId(actorId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onRemoveItemBuilding : can not find buildingInfo by actorId[".. actorId .."]")
        return false
    end

    local type = buildingInfo.type
    if type == 1 or type == 2 then
        -- 建筑
        if player.building == nil then
            return false
        end

        for i, v in pairs(player.building.building) do
            if v.actorId == actorId then
                if v.entityId == 0 then
                    return false
                end

                local mappingId = buildingInfo.id
                local sourceId = ItemsMappingConfig:getSourceIdByMappingId(mappingId)

                local inv = player:getInventory()
                if inv:isCanAddItem(sourceId, 0, 1) == false then
                    HostApi.sendCommonTip(rakssid, "ranch_tip_package_full")
                    return false
                end

                player.building:removeBuilding(buildingInfo.id)
                if player:getItemNumById(sourceId) <= 0 then
                    player:addItem(sourceId, 1, 0)
                end

                local prosperity = BuildingConfig:getProsperityById(buildingInfo.id)
                player.building.prosperity = player.building.prosperity - prosperity

                local ranchInfo = player:initRanchInfo()
                player:getRanch():setInfo(ranchInfo)

            end
        end
        
    end

    if type == 3 then
        -- 仓库
        if player.wareHouse == nil then
            return false
        end

        if player.wareHouse.entityId == 0 then
            return false
        end


        local mappingId = buildingInfo.id
        local sourceId = ItemsMappingConfig:getSourceIdByMappingId(mappingId)

        local inv = player:getInventory()
        if inv:isCanAddItem(sourceId, 0, 1) == false then
            HostApi.sendCommonTip(rakssid, "ranch_tip_package_full")
            return false
        end

        player.wareHouse:removeWareHousePos()
        if player:getItemNumById(sourceId) <= 0 then
            player:addItem(sourceId, 1, 0)
        end

        local ranchInfo = player:initRanchInfo()
        player:getRanch():setInfo(ranchInfo)
    end

end

function LogicListener.onGetRanchTaskItemsCost(rakssid, orderId, index, isSuccess, money)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] LogicListener.onGetRanchOrderItemsCost : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        return false
    end

    if player.wareHouse == nil then
        return false
    end

    local item = player.task:getTaskItemByIndex(orderId, index)
    if item == nil then
        return false
    end

    local itemId = item.itemId
    local num = item.num

    money.value = player.wareHouse:getCubePriceCostByTaskItem(itemId, num)
    isSuccess.value = true
end

return LogicListener