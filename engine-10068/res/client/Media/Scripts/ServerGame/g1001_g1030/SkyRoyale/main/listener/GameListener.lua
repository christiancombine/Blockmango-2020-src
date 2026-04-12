--region *.lua
require "config.GameConfig"
require "config.RespawnConfig"
require "config.ChestConfig"
require "config.ShopConfig"
require "config.BlockConfig"
require "config.NewIslandConfig"
require "config.TeamConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    local initPos = VectorUtil.newVector3i(TeamConfig:getTeam(1).initPosX,
            TeamConfig:getTeam(1).initPosY, TeamConfig:getTeam(1).initPosZ)
    GameServer:setInitPos(initPos)
    EngineWorld:stopWorldTime()
    RespawnConfig:prepareRespawnGoods()
    ChestConfig:prepareChest()
    ShopConfig:prepareShop()
    BlockConfig:prepareBlockHardness()
    NewIslandConfig:generateIslandWall()
    if GameConfig.isKeepInventory == 1 then
        HostApi.setEntityItemLife(GameConfig.entityItemLifeTime * 20)
    end
end

return GameListener
--endregion
