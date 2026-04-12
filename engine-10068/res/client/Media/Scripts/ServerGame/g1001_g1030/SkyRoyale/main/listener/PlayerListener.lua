--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "config.SnowBallConfig"
require "config.BridgeConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerBuyRespawnResultEvent.registerCallBack(self.onBuyRespawnResult)
    PlayerCloseContainerEvent.registerCallBack(self.onPlayerCloseContainer)
    PlayerBuyEnchantmentPropEvent.registerCallBack(self.onPlayerBuyEnchantmentProp)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerUpdateRealTimeRank)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    player:changeMaxHealth(GameConfig.playerHp)
    GameMatch:sendRealRankData(player)
    if player:isVisitor() == false then
        player:getDBDataRequire()
    end
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch.hasStartGame == false then
        return false
    end

    if hurtFrom ~= nil and hurtPlayer ~= nil then
        local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
        local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)
        if hurter ~= nil and fromer ~= nil then
            if hurter.team.id == fromer.team.id then
                return false
            end
        end
    end

    if damageType == "fall" then
        return false
    end

    if damageType == "outOfWorld" then
        return true
    end

    if damageType == "thrown" then
        hurtValue.value = SnowBallConfig.damage
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    if diePlayer == nil then
        return true
    end

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier ~= nil then
        if GameConfig.isKeepInventory == 1 then
            --local inv = dier:getInventory()
            --local index = 1
            --for i = 1, 40 do
            --    local info = inv:getItemStackInfo(i - 1)
            --    if info.id ~= 0 then
            --        dier.tempInventory[index] = info
            --        index = index + 1
            --    end
            --end
            dier:clearInv()
        end

        dier.deathTimes = dier.deathTimes + 1
        --dier.team.deathTimes = dier.team.deathTimes + 1
        GameMatch:sendRealRankData(dier)
        GameMatch:onPlayerDie(dier)

        if killPlayer ~= nil then
            local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
            if killer ~= nil then
                MsgSender.sendMsg(IMessages:msgPlayerKillPlayer(dier.name, killer.name))
                killer:onKill()
                --killer.team.kills = killer.team.kills + 1
                GameMatch:sendRealRankData(killer)
            end
        end
    end

    return true
end

function PlayerListener.onBuyRespawnResult(rakssid, code)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        GameMatch:ifGameOverByPlayer()
        return
    end
    if code ~= 1 then
        player:onDie()
        GameMatch:ifGameOverByPlayer()
    else
        MsgSender.sendMsg(IMessages:msgRespawn(player:getDisplayName()))
        HostApi.broadCastPlayerLifeStatus(player.userId, player.isLife)
        RespawnManager:removeSchedule(player.userId)
    end
end

function PlayerListener.onPlayerRespawn(player)
    player:addInitItems()
    player:changeMaxHealth(GameConfig.playerHp)
    player:teleportPosWithYaw(player.team:getRespawnPos(), 0)
    player.realLife = true
    player.isLife = true
    HostApi.broadCastPlayerLifeStatus(player.userId, player.isLife)
    return 43200
end

function PlayerListener.onPlayerCloseContainer(rakssid, vec3)
    return false
end

function PlayerListener.onPlayerBuyEnchantmentProp(rakssid, id, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local enchantmentItem = EnchantmentShop:findEnchantmentById(id)
    if enchantmentItem == nil then
        return
    end

    if player.enchantmentBuyItems[enchantmentItem.id] == nil then
        player.enchantmentBuyItems[enchantmentItem.id] = enchantmentItem.id
        player:sendEnchantmentData()
        player:addEnchantmentGoodsItem(enchantmentItem)
        player:subMoney(enchantmentItem.price)
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerUpdateRealTimeRank(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    GameMatch:sendRealRankData(player)
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return false
    end

    return true
end

function PlayerListener.onPlayerUseCommonItem(rakssId, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    if itemId >= 467 and itemId <= 471 then
        local pos = player:getPosition()
        BridgeConfig:buildBridge(itemId, player:getYaw(), VectorUtil.toBlockVector3(pos.x, pos.y - 1, pos.z))
    end
end

return PlayerListener
--endregion
