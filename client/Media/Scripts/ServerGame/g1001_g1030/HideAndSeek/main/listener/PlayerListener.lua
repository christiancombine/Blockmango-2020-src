--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerClickChangeActorEvent.registerCallBack(self.onPlayerClickChangeActor)
    PlayerChangeActorEvent.registerCallBack(self.onPlayerChangeActor)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerReviewPlayerEvent.registerCallBack(self.onPlayerReviewPlayer)
    PlayerBuyActorResultEvent.registerCallBack(self.onPlayerBuyActorResult)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    if player == nil then
        return
    end
    RankNpcConfig:getPlayerRankData(player)
    HostApi.sendPlaySound(player.rakssid, 10020)
    HostApi.sendShowHideAndSeekBtnStatus(player.rakssid, false, false, false)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    player:teleInitPos()
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)

    if GameMatch:isGameStart() == false then
        return false
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)

    if GameMatch:isGameStart() then
        if damageType == "outOfWorld" and hurter ~= nil then
            hurtValue.value = 999
            return true
        end
    end

    if hurtFrom == nil then
        return true
    end

    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter == nil or fromer == nil then
        hurtValue.value = 0
        return true
    end

    if fromer.role == GamePlayer.ROLE_HIDE then
        hurtValue.value = 0
        return false
    end

    if fromer.role == hurter.role then
        hurtValue.value = 0
        return false
    end

    hurter:onHurted()

    MsgSender.sendBottomTipsToTarget(hurter.rakssid, 3, Messages:finded())

    fromer:onAttack()

    MsgSender.sendCenterTipsToTarget(fromer.rakssid, 3, Messages:findTrue(hurter:getModelName()))

    if hurter.hp == 0 then
        hurtValue.value = 999
        return true
    end

    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if dier == nil then
        return true
    end

    if iskillByPlayer and killPlayer then
        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)

        if killer == nil then
            return true
        end

        killer:onKill()

        dier:onDie()

        if dier.role == GamePlayer.ROLE_HIDE then
            HostApi.changePlayerPerspece(dier.rakssid, 0)
            HostApi.sendShowHideAndSeekBtnStatus(dier.rakssid, false, false, false)
            MsgSender.sendMsg(Messages:lastHide(GameMatch:getPlayerNumForRole(GamePlayer.ROLE_HIDE)))
        end

    else
        dier:onDie()
    end

    GameMatch:ifGameOverByPlayer()

    return true
end

function PlayerListener.onPlayerClickChangeActor(rakssid)

    if GameMatch:isCanChangeModel() == false then
        MsgSender.sendBottomTipsToTarget(rakssid, 3, Messages:changeActorByNoStart())
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    player:showChangeModel()

end

function PlayerListener.onPlayerChangeActor(rakssid)

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    player:changeModel()

end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return false
    end

    if player.role == GamePlayer.ROLE_SEEK then
        return itemId == ItemID.GOLD_HEART or itemId == ItemID.GOLD_SHOES or itemId == ItemID.GOLD_ARROW or itemId == ItemID.QUESTION_MARK
    end

    --if player.role == GamePlayer.ROLE_HIDE then
    --    return itemId == ItemID.GOLD_HEART or itemId == ItemID.GOLD_SHOES
    --end

    return false
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum)

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    if player.role == GamePlayer.ROLE_SEEK then

        if itemId == ItemID.GOLD_HEART then
            player:onUseHeart()
        end

        if itemId == ItemID.GOLD_SHOES then
            player:onUseShoes()
        end

        if itemId == ItemID.GOLD_ARROW then
            player:onUseArrow()
        end

        if itemId == ItemID.QUESTION_MARK then
            player:onUseQuestion()
        end

    end

    if player.role == GamePlayer.ROLE_HIDE then

        --if itemId == ItemID.GOLD_HEART then
        --    player:onUseHeart()
        --end
        --
        --if itemId == ItemID.GOLD_SHOES then
        --    player:onUseShoes()
        --end

    end

    player:removeItem(itemId, itemNum)

end

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)

    if GameMatch:isGameStart() == false then
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    if player.role == GamePlayer.ROLE_HIDE then
        return
    end

    if player.attackCache[tostring(entityId)] == nil then
        player.attackCache[tostring(entityId)] = 0
    end

    if os.time() - player.attackCache[tostring(entityId)] < 3 then
        return
    end

    if GameMatch:getGameLastTime() < GameConfig.invincibleTime then
        MsgSender.sendCenterTipsToTarget(rakssid, 3, Messages:findFalseLast45Second())
        return
    end

    player:onHurted()

    player.attackCache[tostring(entityId)] = os.time()

    MsgSender.sendCenterTipsToTarget(rakssid, 3, Messages:findFalse())

    if player.hp <= 0 then
        player:death()
    end

end

function PlayerListener.onPlayerReviewPlayer(rakssid1, rakssid2)

    local player1 = PlayerManager:getPlayerByRakssid(rakssid1)
    local player2 = PlayerManager:getPlayerByRakssid(rakssid2)

    if player1 ~= nil and player2 ~= nil then
        if player1.role == player2.role then
            player1:setShowName(player1:buildShowName(), player2.rakssid)
            player2:setShowName(player2:buildShowName(), player1.rakssid)
        else
            player1:setShowName(" ", player2.rakssid)
            player2:setShowName(" ", player1.rakssid)
        end

        if GameMatch:isGameStart() then
            if player1.role == GamePlayer.ROLE_HIDE and player2.role == GamePlayer.ROLE_SEEK then
                player1:changeInvisible(player2.rakssid, false)
            end
            return
        end

        if GameMatch:isChangeModel() then
            if player1.role == GamePlayer.ROLE_HIDE and player2.role == GamePlayer.ROLE_SEEK then
                player1:changeInvisible(player2.rakssid, true)
            end
            return
        end

    end

end

function PlayerListener.onPlayerBuyActorResult(rakssid, result)

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return
    end

    if result then
        player.changeModelTimes = player.changeModelTimes + 1
        player:changeActor()
    end

end

return PlayerListener
--endregion
