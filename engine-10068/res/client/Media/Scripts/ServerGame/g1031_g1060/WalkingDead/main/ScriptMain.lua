package.path = package.path ..';..\\?.lua';

require "Header"

ScriptMain = {}

function ScriptMain.init()
    ScriptMain:initListener()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setMaxInventorySize(21)
    HostApi.setUseMovingEnderChest(true)
    HostApi.setCanEatWhenever(true)
    HostApi.setShowGunEffectWithSingle(true)
    ServerHelper.putBoolPrefs("IsCreatureCollision", false)
    ServerHelper.putIntPrefs("HurtProtectTime", 0)
end

function ScriptMain:initListener()
    GameListener:init()
    PlayerListener:init()
    ChestListener:init()
    BlockListener:init()
    MonsterListener:init()
    DBDataListener:init()
    AIListener:init()
    FormulationListener:init()
    TitleListener:init()
    StoreListener:init()
    ActorListener:init()
    ItemListener:init()
    CustomListener:init()
end

ScriptMain:init()