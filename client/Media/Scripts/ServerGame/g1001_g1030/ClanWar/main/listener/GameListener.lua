--region *.lua
require "web.ClanWarWeb"
require "config.TeamConfig"
require "messages.Messages"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    GameServer:setInitPos(GameMatch.Teams[1].initPos)
    EngineWorld:stopWorldTime()
    ClanWarWeb:init(ver, config.rankAddr, config.rewardAddr, config.propAddr, config.gameId)
end

return GameListener
--endregion
