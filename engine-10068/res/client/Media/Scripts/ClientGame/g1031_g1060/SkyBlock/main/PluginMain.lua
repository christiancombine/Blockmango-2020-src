require "events.HandleEvents"
require "listener.GameListener"
require "listener.DataListener"
require "listener.BlockListener"
require "config.GameConfig"
require "packet.GamePacketSender"
require "web.SkyBlockWeb"
require "data.GameData"
require "data.MultiBuildData"
require "data.RankData"

PluginMain = {}

function PluginMain:init()
	GameData:init()
    RankData:init()
    GameListener:init()
    DataListener:init()
    BlockListener:init()
    registerMoneyIcon(3, "set:items.json image:currency_money")
    ClientHelper.putBoolPrefs("IsUseCustomArmor", true)
    ClientHelper.putBoolPrefs("IsShowCrafting", true)
    ClientHelper.putBoolPrefs("IsShowItemDurability", true)
    ClientHelper.putBoolPrefs("BlockCustomMeta", true)
end

PluginMain:init()

return PluginMain