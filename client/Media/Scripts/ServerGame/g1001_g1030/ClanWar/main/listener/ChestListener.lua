--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

ChestListener = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(self.onChestOpen)
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
end

function ChestListener.onChestOpen(vec3, inv, rakssid)
    GameMatch:onChestOpen(vec3, inv, rakssid)
end

function ChestListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local isInFlagArea, flagArea = player.team:locInFlagArea(vec3)
    if isInFlagArea and flagArea ~= nil then
        return false
    end

    return true
end

return ChestListener
--endregion
