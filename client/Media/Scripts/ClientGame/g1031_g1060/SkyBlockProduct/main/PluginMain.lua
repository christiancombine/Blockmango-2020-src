require "listener.GameListener"
require "listener.DataListener"
require "config.GameConfig"
require "packet.GamePacketSender"

PluginMain = {}

function PluginMain:init()
    GameListener:init()
    DataListener:init()
    registerMoneyIcon(3, "set:items.json image:currency_money")
    ClientHelper.putBoolPrefs("IsUseCustomArmor", true)
    ClientHelper.putBoolPrefs("IsShowItemDurability", true)
end

PluginMain:init()

return PluginMain