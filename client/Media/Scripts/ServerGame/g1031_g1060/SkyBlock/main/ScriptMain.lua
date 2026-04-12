--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.DBDataListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.ChestListener"
require "listener.CustomListener"
require "listener.RankListener"
require "packet.GamePacketSender"
require "events.GameEvents"
require "web.SkyBlockWeb"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    HostApi.setLavaDamageSource()
    GameListener:init()
    PlayerListener:init()
    DBDataListener:init()
    BlockListener:init()
    LogicListener:init()
    ChestListener:init()
    CustomListener:init()
    RankListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setGunFireIsNeedBullet(false)
    HostApi.setBreakBlockSoon(false)
    HostApi.setHideArmor(true)
    HostApi.setMaxChangeInChunk(512)
    ServerHelper.putBoolPrefs("SendSystemsChat", false)
    ServerHelper.putBoolPrefs("IsUseCustomArmor", true)
    ServerHelper.putBoolPrefs("NeedSyncSchematicPlaceBlock", true)
    ServerHelper.putBoolPrefs("BlockCustomMeta", true)
    PlayerManager.IsMessages = false
    HostApi.setSwordBreakBlock(true)
    DBManager:disableAutoCache()
end

ScriptMain:init()