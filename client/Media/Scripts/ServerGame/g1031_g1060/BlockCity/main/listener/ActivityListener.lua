require "manager.TigerLotteryManager"

ActivityListener = {}

function ActivityListener:init()
    BlockCityOpenTigerLotteryEvent:registerCallBack(self.onBlockCityOpenTigerLottery)
    BlockCitySelectTigerLotteryEvent:registerCallBack(self.onBlockCitySelectTigerLottery)
    BlockCityOperationEvent:registerCallBack(self.onBlockCityOperation)
    BlockCityBuyGoodsEvent:registerCallBack(self.onBlockCityBuyGoods)
    BlockCityChangeEditModeEvent:registerCallBack(self.onBlockCityChangeEditMode)
    RotateOperationEvent:registerCallBack(self.onRotateOperation)
    BlockCityBuyLackingItemsEvent:registerCallBack(self.onBlockCityBuyLackingItems)
    BlockCityManorBuyOperationEvent:registerCallBack(self.onBlockCityManorBuyOperation)
    BlockCityChoosePropEvent:registerCallBack(self.onBlockCityChooseProp)
    BlockCityGetFreeFlyEvent:registerCallBack(self.onBlockCityGetFreeFly)
    BlockCityHouseShopBuyGoodsEvent:registerCallBack(self.onBlockCityHouseShopBuyGoods)
    BlockCityChangeToolStatusEvent:registerCallBack(self.onBlockCityChangeToolStatus)
    MoveElevatorEvent:registerCallBack(self.onMoveElevator)
    BlockCityTeleportToShopEvent:registerCallBack(self.onBlockCityTeleportToShop)
    BlockCityOperateArchiveDataEvent:registerCallBack(self.onBlockCityOperateArchiveData)
    BlockCityUnlockNewArchiveEvent:registerCallBack(self.onBlockCityUnlockNewArchive)
    BlockCityChangeArchiveNameEvent:registerCallBack(self.onBlockCityChangeArchiveName)
    BlockCityTossBackEvent:registerCallBack(self.onBlockCityTossBack)
    BlockCityPortalEvent:registerCallBack(self.onBlockCityTeleportGate)
    BlockCityToRaceOrNotEvent:registerCallBack(self.onBlockCityToRaceOrNot)
end

function ActivityListener.onBlockCityOpenTigerLottery(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local entityId = DataBuilder.new():fromData(data):getNumberParam("entityId")
    return TigerLotteryManager:OnPlayerReceive(player, entityId)
end

function ActivityListener.onBlockCitySelectTigerLottery(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local uniqueId = DataBuilder.new():fromData(data):getParam("uniqueId")
    player:choiceTigerLotteryItem(uniqueId)
end

function ActivityListener.onBlockCityOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    -- operationType: 3->buy wing, 4-> clear inventory, 5-> cash prize
    local operationType = DataBuilder.new():fromData(data):getNumberParam("operationType")
    local flyId = DataBuilder.new():fromData(data):getNumberParam("flyId")
    if operationType == 3 then -- 购买飞行
        if player:getIsCanFlying() then
            return
        end

        local id = flyId
        local flyingInfo = FlyingConfig:getFlyingDataById(id)
        if not flyingInfo then
            return
        end

        local moneyType = flyingInfo.moneyType
        local totalMoney = flyingInfo.price
        if moneyType == MoneyType.Diamond then
            player:consumeDiamonds(1200 + flyId, totalMoney, Messages.buyFly .. ":" .. id, true)
        elseif moneyType == MoneyType.Gold then
            player:consumeGolds(1200 + flyId, totalMoney, Messages.buyFly .. ":" .. id, true)
        end
        return
    end

    if operationType == 4 then  --清空道具栏物品
        player:clearInv()
        player:clearHotBar()
        return
    end

    if operationType == 5 then
        GameMatch:weekRankReward(player)
        return
    end
end

function ActivityListener.onBlockCityBuyGoods(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    --tabId: 1、圖紙 ，2、方塊 ，3、裝飾
    --goodsId: 唯一標識
    --buyType: 1、按個購買 ， 2、按組購買
    local tabId = DataBuilder.new():fromData(data):getNumberParam("tabId")
    local goodsId = DataBuilder.new():fromData(data):getNumberParam("goodsId")
    local buyType = DataBuilder.new():fromData(data):getNumberParam("buyType")
    local moneyType = DataBuilder.new():fromData(data):getNumberParam("moneyType")

    local totalMoney = 0
    local infoKey = ""
    if tabId == 1 then
        local draw = DrawingConfig:getDrawingById(goodsId)
        if draw ~= nil then
            if player.level >= draw.level then
                totalMoney = draw.price
                infoKey = Messages.buyDraw .. ":" .. goodsId .. ":" .. draw.itemId .. ":" .. 0 .. ":" .. 1

                if moneyType == MoneyType.Diamond then
                    totalMoney = math.ceil(totalMoney / 100)
                    player:consumeDiamonds(1000 + goodsId, totalMoney, infoKey, true)

                elseif moneyType == MoneyType.Gold then
                    player:consumeGolds(1000 + goodsId, totalMoney, infoKey, true)

                elseif moneyType == MoneyType.Cash then
                    local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
                    if hadEnoughCash then
                        player:subMoney(totalMoney)
                        TradeManager:judgeConsumeEntrance(player.rakssid, infoKey, moneyType)
                    end

                end
            end
        end
    end

    if tabId == 2 then
        local block = BlockConfig.block[goodsId]
        if block ~= nil then
            totalMoney = block.price
            local num = 1
            if buyType == 2 then
                totalMoney = totalMoney * 64
                num = 64
            end

            local itemId = block.blockId
            local meta = block.meta
            infoKey = Messages.buyBlock .. ":" .. itemId .. ":" .. meta .. ":" .. num
            if moneyType == MoneyType.Diamond then
                totalMoney = math.ceil(totalMoney / 100)
                player:consumeDiamonds(goodsId, totalMoney, infoKey, true)

            elseif moneyType == MoneyType.Gold then
                player:consumeGolds(goodsId, totalMoney, infoKey, true)

            elseif moneyType == MoneyType.Cash then
                local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
                if hadEnoughCash then
                    player:subMoney(totalMoney)
                    TradeManager:judgeConsumeEntrance(player.rakssid, infoKey, moneyType)
                end

            end
        end
    end

    if tabId == 3 then
        local decoration = DecorationConfig:getDecorationInfo(goodsId)
        if decoration ~= nil then
            infoKey = Messages.buyDecoration .. ":" .. goodsId .. ":" .. decoration.itemId .. ":" .. 0 .. ":" .. 1
            totalMoney = decoration.price
            if moneyType == MoneyType.Diamond then
                totalMoney = math.ceil(totalMoney / 100)
                player:consumeDiamonds(goodsId, totalMoney, infoKey, true)

            elseif moneyType == MoneyType.Gold then
                player:consumeGolds(goodsId, totalMoney, infoKey, true)

            elseif moneyType == MoneyType.Cash then
                local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
                if hadEnoughCash then
                    player:subMoney(totalMoney)
                    TradeManager:judgeConsumeEntrance(player.rakssid, infoKey, moneyType)
                end

            end
        end
    end
end

function ActivityListener.onBlockCityChangeEditMode(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local isEditMode = DataBuilder.new():fromData(data):getBoolParam("isEditMode")
    player:toggleEditMode()
end

function ActivityListener.onRotateOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end
    local manorInfo = SceneManager:getUserManorInfoByIndex(player.curManorIndex)
    if not manorInfo then
        return false
    end
    local master = PlayerManager:getPlayerByUserId(manorInfo.userId)
    if not master then
        return false
    end

    --type: 1->decoration  ||  2->schematic
    --direction: 1->turn left   ||  2->turn right
    local type = DataBuilder.new():fromData(data):getNumberParam("type")
    local direction = DataBuilder.new():fromData(data):getNumberParam("direction")
    local id = DataBuilder.new():fromData(data):getNumberParam("id")

    if type == 1 then
        player:rotateDecoration(direction, master)
    elseif type == 2 then
        player:rotateSchematic(direction, master)
    end
end

function ActivityListener.onBlockCityBuyLackingItems(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local lackItems = player.lackItems
    if not lackItems then
        return
    end

    -- buyType: 1-> buy one. 2-> buy group
    local buyType = DataBuilder.new():fromData(data):getNumberParam("buyType")
    local moneyType = DataBuilder.new():fromData(data):getNumberParam("moneyType")

    local itemType = lackItems.type
    local totalMoney = lackItems.moneyPrice
    local num = 1
    if itemType == 2 then --方块
        if buyType == 2 then
            totalMoney = totalMoney * 64
            num = num * 64
        end
    end

    local goodsId = 0

    if itemType == 1 then
        goodsId = 10001000 + lackItems.drawingId
    end

    if itemType == 2 then
        local blockInfo = BlockConfig:getBlockInfo(lackItems.blockId, lackItems.masterMeta)
        if blockInfo then
            goodsId = 10000000 + blockInfo.id
        end
    end

    if itemType == 3 then
        goodsId = 10000000 + lackItems.actorId
    end

    player:setIsWaitForPayItems(true)
    local infoKey = Messages.buyLackItems .. ":" .. num
    if moneyType == MoneyType.Diamond then
        totalMoney = math.ceil(totalMoney / 100)
        player:consumeDiamonds(goodsId, totalMoney, infoKey, true)

    elseif moneyType == MoneyType.Gold then
        player:consumeGolds(goodsId, totalMoney, infoKey, true)

    elseif moneyType == MoneyType.Cash then
        local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
        if hadEnoughCash then
            player:subMoney(totalMoney)
            TradeManager:judgeConsumeEntrance(player.rakssid, infoKey, moneyType)
        else
            player:setIsWaitForPayItems(false)
        end
    end
end

function ActivityListener.onBlockCityManorBuyOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    -- operationType: 1-> delete manor , 2-> upgrade
    local operationType = DataBuilder.new():fromData(data):getNumberParam("operationType")
    local moneyType = DataBuilder.new():fromData(data):getNumberParam("moneyType")

    if operationType == 1 then -- 清空领地所有方块和装饰
        if player.curManorIndex == player.manorIndex then
            local totalMoney = ManorLevelConfig:getPriceToDeleteByLevel(player.level)
            if totalMoney == 0 then
                player:deleteEverythingInManor()
                return
            end

            if moneyType == MoneyType.Diamond then
                totalMoney = math.ceil(totalMoney / 100)
                player:consumeDiamonds(1300, totalMoney, Messages.aButtonToDeleteAll .. ":", true)

            elseif moneyType == MoneyType.Gold then
                player:consumeGolds(1300, totalMoney, Messages.aButtonToDeleteAll .. ":", true)

            end
        end
        return
    end

    if operationType == 2 then -- 升级领地
        if player.curManorIndex == player.manorIndex then
            local maxManorLevel = ManorLevelConfig:getMaxLevel()
            if player.level >= maxManorLevel then
                HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "manor_level_up_max")
                return
            end
            local totalMoney = ManorLevelConfig:getNextPriceByLevel(player.level)
            if moneyType == MoneyType.Diamond then
                totalMoney = math.ceil(totalMoney / 100)
                player:consumeDiamonds(1300 + player.level, totalMoney, Messages.upgradeManor .. ":", true)

            elseif moneyType == MoneyType.Gold then
                player:consumeGolds(1300 + player.level, totalMoney, Messages.upgradeManor .. ":", true)

            elseif moneyType == MoneyType.Cash then
                local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
                if hadEnoughCash then
                    player:subMoney(totalMoney)
                    TradeManager:judgeConsumeEntrance(player.rakssid, Messages.upgradeManor .. ":", moneyType)
                end

            end
        end
        return
    end
end

function ActivityListener.onBlockCityChooseProp(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local guideId = DataBuilder.new():fromData(data):getNumberParam("guideId")
    local propId = DataBuilder.new():fromData(data):getNumberParam("propId")

    if player.chooseTemplateId == 0 then
        player.chooseTemplateId = propId
        player:buyDrawing(propId)
    end
    player:sendStoreInfo(true)
end

function ActivityListener.onBlockCityGetFreeFly(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if player:getIsCanFlying() or player:getIsGotFreeFly() then
        return
    end

    player:setIsGotFreeFly(true)
    player:setFlyingTime(GameConfig.freeFlyTime)
    HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "gui_blockCity_free_fly_time")
end

function ActivityListener.onBlockCityHouseShopBuyGoods(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local classifyId = DataBuilder.new():fromData(data):getNumberParam("shopId")
    local itemId = DataBuilder.new():fromData(data):getNumberParam("goodsId")
    local moneyType = DataBuilder.new():fromData(data):getNumberParam("moneyType")
    local index = DataBuilder.new():fromData(data):getNumberParam("index")

    local item = ShopHouseManager:getItemInfosByItemId(itemId)
    if not item then
        return
    end

    if player:getIsWaitForPayItems() then
        return
    end

    if item.limit > 0 and player:getItemStatusInfos(itemId) ~= ShopItemStatus.NotBuy then
        --此处局限性：只是根据itemStatus~=NotBuy就证明已经存在这个商品了，就不给再买，所以当limit>1时，这个判断就不起作用。
        return
    end

    --10000 + itemId
    player:setIsWaitForPayItems(true)
    local totalMoney = item.itemPrice
    local infoKey = Messages.buyShopHouseItem .. ":" .. item.itemType .. ":" .. classifyId .. ":" .. itemId .. ":" .. index
    if moneyType == MoneyType.Diamond then
        totalMoney = (item.moneyType ~= MoneyType.Diamond and {math.ceil(totalMoney / 100)} or {totalMoney})[1]
        player:consumeDiamonds(10000 + itemId, totalMoney, infoKey, true)

    elseif moneyType == MoneyType.Gold then
        player:consumeGolds(10000 + itemId, totalMoney, infoKey, true)

    elseif moneyType == MoneyType.Cash then
        local hadEnoughCash = TradeManager:checkCash(userId, totalMoney)
        if hadEnoughCash then
            player:subMoney(totalMoney)
            TradeManager:judgeConsumeEntrance(player.rakssid, infoKey, moneyType)
        else
            player:setIsWaitForPayItems(false)
        end

    end
end

function ActivityListener.onBlockCityChangeToolStatus(userId, data)
    --goodsId = itemId
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local classifyId = DataBuilder.new():fromData(data):getNumberParam("classifyId")
    local itemId = DataBuilder.new():fromData(data):getNumberParam("itemId")
    local index = DataBuilder.new():fromData(data):getNumberParam("index")
    ShopHouseManager:syncSingleShop(player.rakssid, classifyId, itemId, true, index)
end

function ActivityListener.onMoveElevator(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local floorId = DataBuilder.new():fromData(data):getNumberParam("floorId")

    local pos = player:getPosition()
    local elevatorId = ElevatorConfig:getElevatorIdByPos(pos)
    if elevatorId == 0 then
        return
    end

    local curFloor = ElevatorConfig:getFloorIdByPosY(pos.y)
    if curFloor == floorId then
        return
    end

    local startPos = ElevatorConfig:getStartPosByFloorId(curFloor, elevatorId)
    if not startPos then
        return
    end

    local endPos = ElevatorConfig:getEndPosByFloorId(floorId, elevatorId)
    if not endPos then
        return
    end

    player:teleportPos(startPos)

    local speed = ElevatorConfig:getSpeedByFloorId(floorId)
    if curFloor > floorId then
        speed = -speed
        endPos.y = endPos.y + 1
    else
        endPos.y = endPos.y - 1
    end

    player.entityPlayerMP:setElevator(true, speed, endPos.y, floorId, elevatorId)
end

function ActivityListener.onBlockCityTeleportToShop(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    player:teleportPosWithYaw(GameConfig.superMarket.pos, GameConfig.superMarket.yaw)
end

function ActivityListener.onBlockCityOperateArchiveData(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local archiveId = DataBuilder.new():fromData(data):getNumberParam("index")
    local operateType = DataBuilder.new():fromData(data):getNumberParam("type")
    -- operateType : 1 delete, 2 save, 3 read

    archiveId = archiveId + 1
    if not player.archiveData[archiveId] then
        return
    end

    if operateType == 1 then
        player:deleteArchive(archiveId)
    end

    if operateType == 2 then
        player:saveArchiveById(archiveId)
    end

    if operateType == 3 then
        player:loadArchive(archiveId)
    end
end

function ActivityListener.onBlockCityUnlockNewArchive(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if not player:isCanUnlockArchive() then
        return
    end

    local price = ArchiveConfig:getNextPrice(player:getArchiveNum())
    local infoKey = Messages.buyArchive

    local id = 1801 + player:getArchiveNum()
    player:consumeDiamonds(id, price, infoKey, true)
end

function ActivityListener.onBlockCityChangeArchiveName(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local archiveId = DataBuilder.new():fromData(data):getNumberParam("index")
    local archiveName = DataBuilder.new():fromData(data):getParam("name")
    archiveId = archiveId + 1
    if not player.archiveData[archiveId] then
        return
    end

    player:changeArchiveName(archiveId, archiveName)
end

function ActivityListener.onBlockCityTossBack(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local targetId = DataBuilder.new():fromData(data):getNumberParam("targetId")

    if not player:checkCanPraiseByTargetId(targetId) then
        HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "already_praise_today")
        return
    end


    if not player:checkHasPraiseNum() then
        local roseInfo = RoseConfig:getDefaultRoseInfo()
        local shopInfo = ShopHouseManager:getItemInfosByItemId(roseInfo.itemId)
        if shopInfo then
            local lackItems = {
                id = roseInfo.uniqueId,
                itemId = roseInfo.itemId,
                needNum = 1,
                haveNum = 0,
                itemMeta = -1,
                price = shopInfo.itemPrice
            }
            local lackItemsInfo = {}
            table.insert(lackItemsInfo, lackItems)
            local items = {
                price = shopInfo.itemPrice,
                type = 1,
                items = lackItemsInfo,
                priceCube = math.ceil(shopInfo.itemPrice / 100)
            }
            player:setBuyLackItems(items)
            local itemInfo = {
                type = 5,       -- 5是玫瑰花
                master = player,
                items = lackItemsInfo,
                moneyPrice = items.price,
            }
            player:setLackItems(itemInfo)
        end

        return
    end

    if player:praiseManor(targetId, 1) then
        HostApi.sendPlayingPraiseAction(player.rakssid, 3000)
    end
end

function ActivityListener.onBlockCityTeleportGate(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local uId = DataBuilder.new():fromData(data):getNumberParam("uId")
    local id = DataBuilder.new():fromData(data):getNumberParam("id")
    local playerPos = player:getPosition()

    local isInActorRegion = PublicFacilitiesConfig:isInPortalActorRegion(uId, playerPos)
    if not isInActorRegion then
        return
    end

    local portalInfo = PublicFacilitiesConfig:getPortalInfoByUidAndId(uId, id)
    if not portalInfo or portalInfo.isOpen == 0 then
        return
    end

    HostApi.sendPlaySound(player.rakssid, 326)
    player:teleportPosWithYaw(portalInfo.tpPos, portalInfo.tpYaw)
end

function ActivityListener.onBlockCityToRaceOrNot(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local inviteId = DataBuilder.new():fromData(data):getNumberParam("targetUserId")
    local isAgree = DataBuilder.new():fromData(data):getBoolParam("isAgree")

    local invitor = PlayerManager:getPlayerByUserId(inviteId)
    if not invitor then
        player.race = nil
        return
    end

    local drivers = {
        [player] = {},
        [invitor] = {},
    }

    if not isAgree then
        VehicleConfig:sendToRaceOrNotPacket(invitor.rakssid, 0, userId, false, player.name)
        for driver, _ in pairs(drivers) do
            driver.race = nil
        end
        return
    end

    local freeLines = VehicleConfig:checkFreeRaceLines()
    if not freeLines then
        for driver, _ in pairs(drivers) do
            VehicleConfig:sendRaceFailMsgPacket(driver.rakssid, RaceFailType.notFreeLines)
            driver.race = nil
        end
        return
    end

    drivers = {
        [player] = freeLines[1],
        [invitor] = freeLines[2],
    }

    for driver, info in pairs(drivers) do
        driver:closeGlide()
        driver:setEditMode(false)
        driver:teleportPosWithYaw(info.pos, info.yaw)
        local vEntityId = VehicleConfig:prepareRaceVehicle(info.pos, info.yaw)
        local competitorUId = (driver.userId == userId and {inviteId} or {userId})[1]
        local competitorName = (driver.userId == userId and {invitor.name} or {player.name})[1]
        driver:setRaceData(true, competitorUId, competitorName, false, vEntityId, false)
        VehicleConfig:sendToRaceOrNotPacket(driver.rakssid, vEntityId, competitorUId, true)
    end
end

return ActivityListener