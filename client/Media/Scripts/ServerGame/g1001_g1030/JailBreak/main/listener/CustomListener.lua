---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 11:51
---
require "util.VideoAdHelper"

CustomListener = {}

function CustomListener:init()
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    CloseVideoAdTipEvent:registerCallBack(self.onCloseVideoAdTip)
end

function CustomListener.onWatchAdvertFinished(rakssid, type, params, code)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if code ~= 1 then
        if type == Define.AdType.RespawnAd then
            RespawnHelper.sendRespawnCountdown(player, 1)
        end
        return
    end
    if type == Define.AdType.UnlockVehicle then
        local entityId = DataBuilder.new():fromData(params):getNumberParam("EntityId")
        VideoAdHelper:doUnlockVehicleReward(player, entityId)
        return
    end
    if type == Define.AdType.RespawnAd then
        VideoAdHelper:doRespawnVideoAdReward(player)
        return
    end
    if type == Define.AdType.Shop then
        local itemId = DataBuilder.new():fromData(params):getNumberParam("ItemId")
        VideoAdHelper:doShopVideoAdReward(player, itemId)
        return
    end
end

function CustomListener.onCloseVideoAdTip(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local type = DataBuilder.new():fromData(data):getNumberParam("Type")
    if type == Define.AdType.RespawnAd then
        RespawnHelper.sendRespawnCountdown(player, 1)
        return
    end
end

return CustomListener
--endregion
