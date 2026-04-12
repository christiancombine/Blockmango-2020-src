require "data.EventCallback"
ConsumeDiamondsManager = {}
local eventListenerMap = {}

ConsumeId = {
    BuyHouseStoreGoodsId = 1995,
    BuyGiftbagGoodsId = 1996,
    ExpandCarryBagId = 8888,
    ExpandCapacityBagId = 9999,
    DrawLuckyId = 100003,
}

function ConsumeDiamondsManager.AddConsumeSuccessEvent(key, callback, obj)
    if type(callback) ~= "function" then
        return
    end
    local func = EventCallback.new(callback, obj)
    eventListenerMap[key] = func
end

function ConsumeDiamondsManager.OnConsumeSuccess(rakssid, message, extra, payId)
    local func = eventListenerMap[extra]
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        EngineUtil.disposePlatformOrder(0, payId, false)
        return false
    end
    if type(func) == "table"  then
        local result = func:Invoke(extra, rakssid)
        eventListenerMap[extra] = nil
        if result then
            EngineUtil.disposePlatformOrder(player.userId, payId, true)
            return true
        end
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        return false
    end
    EngineUtil.disposePlatformOrder(player.userId, payId, true)
    return true
end

function ConsumeDiamondsManager.OnConsumeFail(rakssid, message, extra, payId)
    if message == "diamonds.not.enough" then
        HostApi.sendCommonTip(rakssid, "not_enough_money")
    elseif message == "pay.failed" then
        HostApi.sendCommonTip(rakssid, "pay_failure")
    elseif message == "diamondBlues.not.enough" then
        --do nothing
    else
        HostApi.sendCommonTip(rakssid, "something_error")
    end
    EngineUtil.disposePlatformOrder(0, payId, false)
    eventListenerMap[extra] = nil
    return false
end
