---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 11:51
---
CustomListener = {}

function CustomListener:init()
    CustomTipMsgEvent.registerCallBack(self.onCustomTipMsg)
    ConsumeTipMsgEvent.registerCallBack(self.onConsumeTipMsg)
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
    ReceiveSumRechargeStartEvent:registerCallBack(function(player, rewards)
        local inv = player:getInventory()
        local emptyCount = inv:getEmptyStackCount()
        if emptyCount < #rewards then
            return false
        end
        return true
    end)
end

function CustomListener.onCustomTipMsg(rakssid, extra, isRight)
    if isRight == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if extra == Define.AKeySell then
        player:clickSellAllOre()
        return
    end
end

function CustomListener.onConsumeTipMsg(rakssid, extra, isRight)
    if isRight == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local info = StringUtil.split(extra, "#")
    if #info == 2 then
        local extra1 = tostring(info[1])
        local extra2 = tonumber(info[2])

        if extra1 == "BuyExArea" then
            player:getExtendArea()
        elseif extra1 == Define.dareTaskTip.AddTimes then
            player:addDareTaskTimes()
            return
        elseif extra1 == Define.dareTaskTip.Refresh then
            player:refreshDareTask()
            return
        end
    end
end

function CustomListener.onConsumeDiamonds(rakssid, isSuccess, message, extra)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player.canSendCostDiamond = true

    HostApi.log("extra == "..extra)
    if isSuccess == false then
        return
    end
    
    local info = StringUtil.split(extra, "#")
    if #info == 2 then
        local data = StringUtil.split(info[2], "@")
        local data_1 = 0
        local data_2 = 0
        if #data == 2 then
            data_1 = tonumber(data[1])
            data_2 = tonumber(data[2])
        end

        local extra1 = tostring(info[1])
        local extra2 = tonumber(info[2])
    
        if extra1 == "UpdateLevel" then
            player:updateLevelSuccess()
        elseif extra1 == Define.dareTaskTip.AddTimes then
            player:addDareTaskTimesSuccess()
            return
        elseif extra1 == Define.dareTaskTip.Refresh then
            player:refreshDareTaskSuccess()
            return 
        elseif extra1 == Define.buyGood and #data == 2 then
            AppShopConfig:BuyGoodSuccess(player, data_1, data_2)
            return 
        elseif extra1 == Define.buyMoney then
            player:buyMoneySuccess(extra2)
            return
        elseif extra1 == Define.buyChirstmasGift then
            ChristmasConfig:buyChirstmasGiftSuccess(player, extra2)
            return
        elseif extra1 == Define.openGift then
            ChristmasConfig:openChirstmasGiftSuccess(player, extra2, Define.DrawGiftType.Diamond)
            return
        end
    end
end

return CustomListener
--endregion