---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 11:51
---
CustomListener = {}

function CustomListener:init()
    CustomTipMsgEvent.registerCallBack(self.onCustomTipMsg)
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    CommonReceiveRewardEvent:registerCallBack(self.onCommonReceiveReward)
end

function CustomListener.onCustomTipMsg(rakssid, extra, isRight)
    if isRight == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local extras = StringUtil.split(extra, "=")
    if #extras ~= 2 then
        return
    end

    if extras[1] == "ExtraPortal" then
        local npc = GameManager:getNpcByEntityId(tonumber(extras[2]))
        if npc then
            npc:onPlayerSelected(player)
        end
        return
    end

    if extras[1] == "Single" then
        player:unlockOccupation(extra)
        return
    end

    if extras[1] == "Perpetual" then
        player:unlockOccupation(extra)
        return
    end

    if extras[1] == "FreeUse" then
        local npc = GameManager:getNpcByEntityId(tonumber(extras[2]))
        if not npc then
            return
        end
        if player.freeOccId == 0 then
            return
        end
        if npc:selectOccupation(player) then
            player.freeOccId = 0
        else
            MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
        end
    end

    if extras[1] == "Occupation" then
        local npc = GameManager:getNpcByEntityId(tonumber(extras[2]))
        if npc and npc:selectOccupation(player) then
            return
        end
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
    end

end

function CustomListener.onConsumeDiamonds(rakssid, isSuccess, message, extra, payId)
    if isSuccess == false then
        EngineUtil.disposePlatformOrder(0, payId, false)
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        EngineUtil.disposePlatformOrder(0, payId, false)
        return
    end

    local extras = StringUtil.split(extra, "=")
    if #extras ~= 2 then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        return
    end

    local npc = GameManager:getNpcByEntityId(tonumber(extras[2]))
    if npc == nil then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    if npc.curSelectNum >= npc.selectedMaxNum then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    if extras[1] == "Single" then
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
        npc:selectOccupation(player)
        return
    end

    if extras[1] == "Perpetual" then
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
        player:addsupOccIds(npc.config.id)
        npc:selectOccupation(player)
        return
    end

end

function CustomListener.onWatchAdvertFinished(rakssid, type, params, code)
    if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if type == Define.AdType.Money then
        VideoAdHelper:doGetMoneyAdReward(player)
        return
    end
    if type == Define.AdType.Building then
        VideoAdHelper:doGetBuildingAdReward(player)
        return
    end
    if type == Define.AdType.AppShop then
        local itemId = DataBuilder.new():fromData(params):getNumberParam("ItemId")
        if itemId == BlockID.TNT then
            VideoAdHelper:doGetTNTAdReward(player)
        end
        if itemId == BlockID.LADDER then
            VideoAdHelper:doGetLadderAdReward(player)
        end
        return
    end
end

function CustomListener.onCommonReceiveReward(player, propsType, propsId, propsCount, expireTime)
    if propsType == "Privilege" then
        local occId = OccupationConfig:getOccupationIdByCommonItemId(propsId) or 0
        if occId ~= 0 then
            player.privilege[tostring(occId)] = player.privilege[tostring(occId)] or os.time()
            player.privilege[tostring(occId)] = player.privilege[tostring(occId)] + expireTime
            player:syncPrivilegeData()
        end
    end
end

return CustomListener
--endregion
