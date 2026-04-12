
require "Match"
require "config.SceneManager"


BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockCropsPlaceEvent.registerCallBack(self.onBlockCropsPlace)
    BlockCropsUpdateEvent.registerCallBack(self.onBlockCropsUpdate)
    BlockCropsPickEvent.registerCallBack(self.onBlockCropsPick)
    BlockLeavesBreakEvent.registerCallBack(self.onBlockLeavesBreak)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(player.userId)
    if manorInfo == nil then
        return false
    end

    if manorInfo:isInManor(vec3) then
        if blockId == 60 then

            local inv = player:getInventory()
            if inv:isCanAddItem(blockId, 0, 1) == false then
                HostApi.sendCommonTip(rakssid, "ranch_tip_package_full")
                return false
            end

            player:addItem(60, 1, 0)

            EngineWorld:setBlock(vec3, 2)

            local fieldOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
            if fieldOffset ~= nil then
                player.field:breakField(fieldOffset)
            end

            vec3.y = vec3.y + 1
            EngineWorld:setBlockToAir(vec3)

            local cropsOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
            if cropsOffset ~= nil then
                player.field:breakCrops(cropsOffset)
            end

            return false
        end

        if blockId == 255 then
            local roadOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
            if roadOffset ~= nil then
                player.field:breakRoad(roadOffset)
            end

            return true
        end

        if blockId >= 900 then
            return true
        end

    end

    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(player.userId)
    if manorInfo == nil then
        return false
    end

    if manorInfo:isInUnlockedArea(toPlacePos) then
        if player.house ~= nil then
            local houseOffset = player.house.house.offset
            if houseOffset.x ~= 0 or houseOffset.y ~= 0 or houseOffset.z ~= 0 then
                local houseStartPos = ManorConfig:getVec3ByOffset(player.manorIndex, houseOffset)
                local houseInfo = HouseConfig:getHouseInfoByLevel(player.house.houseLevel)
                if houseInfo ~= nil and houseStartPos ~= nil then
                    local length = houseInfo.length
                    local width = houseInfo.width

                    local houseEndPosX = houseStartPos.x + length
                    local houseEndPosZ = houseStartPos.z + width

                    local minX = math.min(houseStartPos.x, houseEndPosX) + 1
                    local minZ = math.min(houseStartPos.z, houseEndPosZ) + 1

                    local maxX = math.max(houseStartPos.x, houseEndPosX) - 1
                    local maxZ = math.max(houseStartPos.z, houseEndPosZ) - 1
                    if toPlacePos.x >= minX and toPlacePos.x <= maxX
                            and toPlacePos.z >= minZ and toPlacePos.z <= maxZ then
                        return false
                    end
                end
            end
        end

        if itemId == 60 then
            toPlacePos.y = toPlacePos.y - 1
            local id = EngineWorld:getBlockId(toPlacePos)
            if id == 2 then
                EngineWorld:setBlock(toPlacePos, 60)

                player:removeItem(60, 1)

                local fieldOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, toPlacePos)
                if fieldOffset ~= nil then
                    player.field:placeField(fieldOffset)
                end
            end

            return false
        end

        if itemId == 255 then
            toPlacePos.y = toPlacePos.y - 1
            local id = EngineWorld:getBlockId(toPlacePos)
            if id == 2 then
                toPlacePos.y = toPlacePos.y + 1
                local roadOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, toPlacePos)
                if roadOffset ~= nil then
                    player.field:placeRoad(roadOffset)

                    player.achievement:addCount(AchievementConfig.TYPE_MANOR, AchievementConfig.TYPE_MANOR_ROAD_NUM, 0, 1)

                    local ranchAchievements = player.achievement:initRanchAchievements()
                    player:getRanch():setAchievements(ranchAchievements)
                end

                return true
            end
            return false
        end

        if itemId >= 900 then
            return true
        end

    end

    return false
end

function BlockListener.onBlockCropsPlace(userId, blockId, vec3, residueHarvestNum)
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

    if manorInfo:isInManor(vec3) then
        local cropsOffset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
        if cropsOffset ~= nil then

            player.field:placeCrops(cropsOffset, blockId, residueHarvestNum)

            local mappingId = ItemsMappingConfig:getMappingIdBySourceId(blockId)
            player.achievement:addCount(AchievementConfig.TYPE_PLANT, AchievementConfig.TYPE_PLANT_PLANT, mappingId, 1)

            local ranchAchievements = player.achievement:initRanchAchievements()
            player:getRanch():setAchievements(ranchAchievements)

        end
        return true
    end

    return false
end

function BlockListener.onBlockCropsUpdate(userId, blockId, curState, curStateTime, fruitNum, vec3, residueHarvestNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    local offset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
    local crops ={}
    crops.offset = offset
    crops.blockId = blockId
    crops.state = curState
    crops.curStateTime = curStateTime
    crops.residueHarvestNum = residueHarvestNum
    crops.lastUpdateTime = os.time()

    player.field:updateCrops(crops)
end

function BlockListener.onBlockCropsPick(userId, blockId, productionId, quantity, vec3, stage, residueHarvestNum)

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

    if manorInfo:isInManor(vec3) then
        if player.wareHouse:isEnoughCapacityByNum(quantity) == false then
            HostApi.sendRanchWarehouseResult(player.rakssid, "gui_ranch_storage_full_operation_failure")
            return false
        end

        local offset = ManorConfig:getOffset3iByVec3(player.manorIndex, vec3)
        if offset ~= nil then

            player.wareHouse:addItem(productionId, quantity)

            local items = {}
            local item = RanchCommon.new()
            item.itemId = productionId
            item.num = quantity
            items[1] = item
            HostApi.sendRanchGain(player.rakssid, items)

            -- add player's exp
            local itemId = ItemsMappingConfig:getMappingIdBySourceId(blockId)
            local exp = ItemsConfig:getExpByItemId(itemId)
            player:addExp(exp)

            -- add player's achievement
            player.achievement:addCount(AchievementConfig.TYPE_PLANT, AchievementConfig.TYPE_PLANT_PICK, productionId, quantity)

            local ranchAchievements = player.achievement:initRanchAchievements()
            player:getRanch():setAchievements(ranchAchievements)


            local wareHouse = player.wareHouse:initRanchWareHouse()
            player:getRanch():setStorage(wareHouse)

            if residueHarvestNum == 0 then
                player.field:breakCrops(offset)
            else
                local crops = {}
                crops.offset = offset
                crops.blockId = blockId
                crops.state = stage
                crops.curStateTime = 0
                crops.lastUpdateTime = os.time()
                crops.residueHarvestNum = residueHarvestNum
                player.field:updateCrops(crops)
            end

            return true
        end
    end

    return false
end

function BlockListener.onBlockLeavesBreak(blockId)
    return false
end

return BlockListener
--endregion
