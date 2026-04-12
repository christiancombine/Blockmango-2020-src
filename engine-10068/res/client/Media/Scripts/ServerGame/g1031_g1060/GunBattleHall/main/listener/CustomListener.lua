---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 11:51
---
CustomListener = {}

function CustomListener:init()
    CustomTipMsgEvent.registerCallBack(self.onCustomTipMsg)
    ConsumeTipMsgEvent.registerCallBack(self.onConsumeTipMsg)
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
    if extras[1] == "GameMode" and extras[2] ~= nil then
        local npc = GameManager:getModeNpc(tonumber(extras[2]))
        if npc then
            npc:onPlayerSelected(player)
        end
        return
    end
    if extras[1] == "BuyExGun" and extras[2] ~= nil then
        local npc = GameManager:getExGunNpc(tonumber(extras[2]))
        if npc then
            npc:onPlayerBuy(player)
        end
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
    local extras = StringUtil.split(extra, "=")
    if extras[1] == "BuyExGun" and extras[2] ~= nil then
        local npc = GameManager:getExGunNpc(tonumber(extras[2]))
        if npc then
            npc:onPlayerBuy(player)
        end
        return
    end
end

return CustomListener
--endregion
