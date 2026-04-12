--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "config.ChestConfig"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerCloseContainerEvent.registerCallBack(self.onPlayerCloseContainer)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    local callback = function()
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            HostApi.updateMemberLeftAndKill(v.rakssid, GameMatch:getLifePlayer(), v.kills)
        end
    end
    return GameOverCode.Success, player, 1, callback
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        HostApi.updateMemberLeftAndKill(v.rakssid, GameMatch:getLifePlayer(), v.kills)
    end
end

function PlayerListener.onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return GameMatch.hasStartGame
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if dier ~= nil then
        local inv = InventoryUtil:getPlayerInventoryInfo(dier)
        local pos = VectorUtil.toBlockVector3(dier:getPosition().x, dier:getPosition().y, dier:getPosition().z)
        ChestConfig:setPlayerChest(pos, inv)
        EngineWorld:setBlock(pos, BlockID.CHEST)
        dier:clearInv()
        dier:onDie()

        if killPlayer ~= nil then
            local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
            if killer ~= nil then
                MsgSender.sendMsg(IMessages:msgPlayerKillPlayer(dier.name, killer.name))
                killer:onKill()
            end
        end
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            HostApi.updateMemberLeftAndKill(v.rakssid, GameMatch:getLifePlayer(), v.kills)
        end
    end
    GameMatch:ifGameOverByPlayer()
    return true
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    return player ~= nil
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if goodsId == 10000 then
        attr.isAddGoods = false
        player:addCurrency(goodsNum)
    end
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerCloseContainer(rakssid, vec3)
    return false
end

return PlayerListener
--endregion
