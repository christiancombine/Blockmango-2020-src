
require "Match"

ChestListener = {}
ChestListener.chest = {}

function ChestListener:init()
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
    ChestOpenEvent.registerCallBack(function (vec3, inv, rakssid) self:onChestOpen(vec3, inv, rakssid) end)
end

function ChestListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if GameMatch:isGameRunning() == false then
        return false
    end
    if type == Define.ItemType.Armor then
        player:onGetItem(itemId, itemMeta)
    else
        if itemId == Define.HotPotato then
            return false
        end
        player:onDropItem(itemId, itemMeta)
    end
    return true
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    BoxManager:setInv(vec3, inv)
    return true
end

return ChestListener