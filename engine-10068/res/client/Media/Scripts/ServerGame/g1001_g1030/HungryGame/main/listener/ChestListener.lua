--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "config.GameConfig"
ChestListener = {}
ChestListener.hasResetChest = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(function (vec3, inv, rakssid) self:onChestOpen(vec3, inv, rakssid) end)
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    local sign ="x".. tostring(vec3.x).."y"..tostring(vec3.y).."z"..tostring(vec3.z)
    if(self.hasResetChest[sign] == nil) then
        inv:clearChest()
        self:fillItem(inv)
        self.hasResetChest[sign] = true
    end

end

function ChestListener:fillItem(inv)
  local nextSlot = 0
   nextSlot = self:fillItemByType(inv, nextSlot, GameConfig.inventory[1].food,
GameConfig.inventory[1].foodNumRange[1], GameConfig.inventory[1].foodNumRange[2])

    nextSlot = self:fillItemByType(inv, nextSlot, GameConfig.inventory[1].equip,
GameConfig.inventory[1].equipNumRange[1], GameConfig.inventory[1].equipNumRange[2])

nextSlot = self:fillItemByType(inv, nextSlot, GameConfig.inventory[1].weapon,
GameConfig.inventory[1].weaponNumRange[1], GameConfig.inventory[1].weaponNumRange[2])

    nextSlot = self:fillItemByType(inv, nextSlot, GameConfig.inventory[1].materials,
GameConfig.inventory[1].materialsNumRange[1], GameConfig.inventory[1].materialsNumRange[2])
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
--endregion
