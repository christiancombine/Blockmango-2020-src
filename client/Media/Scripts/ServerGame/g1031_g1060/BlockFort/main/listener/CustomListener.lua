CustomListener = {}

function CustomListener:init()
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
    ConsumeGoldsEvent.registerCallBack(self.onConsumeGolds)
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
        player:setIsWaitForPay(false)
        return false
    end
    CustomListener.judgeConsumeEntrance(player, extra, 1)
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
        player:setIsWaitForPay(false)
        return false
    end
    CustomListener.judgeConsumeEntrance(player, extra, 2)
end

function CustomListener.judgeConsumeEntrance(player, extra, moneyType)
    local extras = StringUtil.split(extra, ":")
    if extras[1] == Messages.upgradeManor then
        player:updateSelfManor()
    end

    if extras[1] == Messages.buyItem then
        --extras[1-6] : message, id, meta, num, itemId, limit
        if tonumber(extras[6]) > 0 then
            player:addLimitItem(extras[2], extras[4])
        end

        player:addBagItem(extras[2], extras[4])--(id, num)
        HostApi.notifyGetItem(player.rakssid, extras[5], extras[3], extras[4])--(rakssid, itemId, itemMeta, itemNum)
        player:syncStoreInfo(extras[2])
        player:setIsWaitForPay(false)
    end
end
