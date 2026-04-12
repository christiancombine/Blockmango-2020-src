require "config.InventoryConfig"
require "Match"

ChestListener = {}

function ChestListener:init()

end

function ChestListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    if type == 1 then
        return false
    end
    if GameMatch:isGameStart() == false then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if player.role == GamePlayer.ROLE_ZOMBIE then
        return false
    end
    return true
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    local sign ="x".. tostring(vec3.x).."y"..tostring(vec3.y).."z"..tostring(vec3.z)
    if(GameMatch:getCurGameScene().inventoryConfig.hasResetChest[sign] == nil) then
        inv:clearChest()
        self:fillItem(inv)
        GameMatch:getCurGameScene().inventoryConfig.hasResetChest[sign] = true
    end

end

function ChestListener:fillItem(inv)
    local nextSlot = 0
    nextSlot = self:fillItemByType(inv, nextSlot, GameMatch:getCurGameScene().inventoryConfig.food,
    GameMatch:getCurGameScene().inventoryConfig.foodNumRange[1], GameMatch:getCurGameScene().inventoryConfig.foodNumRange[2])

    nextSlot = self:fillItemByType(inv, nextSlot, GameMatch:getCurGameScene().inventoryConfig.equip,
    GameMatch:getCurGameScene().inventoryConfig.equipNumRange[1], GameMatch:getCurGameScene().inventoryConfig.equipNumRange[2])

    nextSlot = self:fillItemByType(inv, nextSlot, GameMatch:getCurGameScene().inventoryConfig.weapon,
    GameMatch:getCurGameScene().inventoryConfig.weaponNumRange[1], GameMatch:getCurGameScene().inventoryConfig.weaponNumRange[2])

    nextSlot = self:fillItemByType(inv, nextSlot, GameMatch:getCurGameScene().inventoryConfig.materials,
    GameMatch:getCurGameScene().inventoryConfig.materialsNumRange[1], GameMatch:getCurGameScene().inventoryConfig.materialsNumRange[2])
end

function ChestListener:fillItemByType(inv, slotBegin, config, rangeLow, rangeUpper)

    local itemSlot = slotBegin
    local items = InventoryUtil:randomItem(config, rangeLow, rangeUpper)
    for i, v in pairs(items) do
        inv:initByItem(itemSlot, v.id, v.num, 0)
        itemSlot =  itemSlot + 1
    end

    return itemSlot
end

function ChestListener:onTick(ticks)

end

return ChestListener