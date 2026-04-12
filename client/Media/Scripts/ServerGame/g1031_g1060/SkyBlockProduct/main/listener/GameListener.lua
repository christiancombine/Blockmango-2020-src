--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "config.GameConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    DBManager:setGameType("g1048")
    PlayerManager:setMaxPlayer(100)
    ToolDurableConfig:prepareToolDurable()
end

return GameListener
--endregion