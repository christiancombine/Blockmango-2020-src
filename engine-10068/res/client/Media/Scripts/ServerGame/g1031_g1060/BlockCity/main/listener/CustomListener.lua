CustomListener = {}

function CustomListener:init()
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
    ConsumeGoldsEvent.registerCallBack(self.onConsumeGolds)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
end

function CustomListener.onConsumeDiamonds(rakssid, isSuccess, message, extra, payId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        print("CustomListener.onConsumeDiamonds : player is nil  rakssid = ", tostring(rakssid))
        EngineUtil.disposePlatformOrder(0, payId, false)
        return false
    end

    if isSuccess == false then
        print("CustomListener.onConsumeDiamonds : message :", message)
        player:setIsWaitForPayItems(false)
        return false
    end
    --CustomListener.judgeConsumeEntrance(player, extra, 1)
    TradeManager:judgeConsumeEntrance(player.rakssid, extra, MoneyType.Diamond)
end

function CustomListener.onConsumeGolds(rakssid, isSuccess, message, extra, payId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        print("CustomListener.onConsumeGolds : player is nil  rakssid = ", tostring(rakssid))
        EngineUtil.disposePlatformOrder(0, payId, false)
        return false
    end

    if isSuccess == false then
        print("CustomListener.onConsumeGolds : message :", message)
        player:setIsWaitForPayItems(false)
        return false
	end
    --CustomListener.judgeConsumeEntrance(player, extra, 2)
    TradeManager:judgeConsumeEntrance(player.rakssid, extra, MoneyType.Gold)
end

--[=[
function CustomListener.judgeConsumeEntrance(player, extra, moneyType)
    --local extras = StringUtil.split(extra, ":")
    --if extras[1] == Messages.aButtonToDeleteAll then
    --    player:deleteEverythingInManor()
    --    return
    --end
    --
    --if extras[1] == Messages.upgradeManor then
    --    player:updateSelfManor()
    --    return
    --end
    --
    --if extras[1] == Messages.buyFly then
    --    local flyingInfo = FlyingConfig:getFlyingDataById(tonumber(extras[2]))
    --    if not flyingInfo then
    --        return
    --    end
    --
    --    player:setFlyingTime(flyingInfo.time)
    --    HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "gui_fly_buy_success")
    --    player:buyFlyingAnalytics(flyingInfo.day)
    --    return
    --end
    --
    --if extras[1] == Messages.buyDraw then
    --    player:buyDrawing(extras[2])
    --    HostApi.notifyGetItem(player.rakssid, extras[3], extras[4], extras[5])
    --    player:buyDrawAnalytics(extras[3], moneyType)
    --    return
    --end
    --
    --if extras[1] == Messages.buyBlock then
    --    player:addBlock(extras[2], extras[3], extras[4])
    --    HostApi.notifyGetItem(player.rakssid, extras[2], extras[3], extras[4])
    --    player:buyBlocksAnalytics(extras[2], extras[3], extras[4], moneyType)
    --    return
    --end
    --
    --if extras[1] == Messages.buyDecoration then
    --    player:addDecoration(extras[2], 1)
    --    HostApi.notifyGetItem(player.rakssid, extras[3], extras[4], extras[5])
    --    player:buyDecorationAnalytics(extras[2], moneyType)
    --    return
    --end
    --
    --if extras[1] == Messages.buyTigerLottery then
    --    print("buyTigerLottery success...")
    --    return
    --end
    --
    --if extras[1] == Messages.buyLackItems then
    --    player:setIsWaitForPayItems(false)
    --    local lackItems = player.lackItems
    --    if not lackItems then
    --        return
    --    end
    --    local num = tonumber(extras[2])
    --    local master = lackItems.master
    --    SceneManager:addPersonalGoods(master, player, lackItems, true, num)
    --    local itemType = tonumber(lackItems.type)
    --    local id = 0
    --    local meta = 0
    --
    --    if itemType == 1 then
    --        id = lackItems.drawingId
    --    end
    --
    --    if itemType == 2 then
    --        id = lackItems.blockId
    --        meta = lackItems.meta
    --    end
    --
    --    if itemType == 3 then
    --        id = lackItems.actorId
    --    end
    --
    --    player:buyLackItemsAnalytics(id, meta, moneyType)
    --    return
    --end
    --
    --if extras[1] == Messages.buyShopHouseItem then
    --    --Messages.buyShopHouseItem .. ":" .. item.itemType .. ":" .. classifyId .. ":" .. itemId .. ":" .. index
    --    local itemId = tonumber(extras[4])
    --    local classifyId = tonumber(extras[3])
    --    local index = tonumber(extras[5])
    --
    --    if classifyId == ShopItemType.Wings then
    --        local itemInfo = ShopHouseManager:getItemInfoByClassifyIdAndItemId(ShopItemType.Wings, itemId)
    --        if itemInfo then
    --            HostApi.notifyGetGoods(player.rakssid, itemInfo.icon, 1)
    --        end
    --    else
    --        HostApi.notifyGetItem(player.rakssid, itemId, 0, 1)
    --    end
    --
    --    player:setIsWaitForPayItems(false)
    --
    --    if classifyId == ShopItemType.Flower then
    --        player:addRoseNum(itemId, 1, true)
    --    end
    --    ShopHouseManager:syncSingleShop(player.rakssid, classifyId, itemId, false, index)
    --    return
    --end
    --
    --if extras[1] == Messages.buyArchive then
    --    player:unlockNewArchive()
    --    return
    --end
end
--]=]

function CustomListener.onWatchAdvertFinished(rakssid, type, params, code)
    if code ~= 1 then
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if type == 104701 then
        VideoAdHelper:doGetTigerAdReward(player, params)
        return
    end

    if type == 104702 then
        VideoAdHelper:doGetFlyingAdReward(player)
        return
    end

end


return CustomListener