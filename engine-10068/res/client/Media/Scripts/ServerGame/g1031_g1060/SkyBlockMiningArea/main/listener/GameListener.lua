--region *.lua

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
    MonsterConfig:prepareMonster()
    BlockSettingConfig:prepareBlockHardness()
    ToolDurableConfig:prepareToolDurable()
end

return GameListener
--endregion