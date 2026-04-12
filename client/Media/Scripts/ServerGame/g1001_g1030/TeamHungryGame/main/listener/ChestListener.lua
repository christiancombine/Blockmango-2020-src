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
        InventoryUtil:fillItem(inv, GameConfig.inventory[1])
        self.hasResetChest[sign] = true
    end

end

return ChestListener
--endregion
