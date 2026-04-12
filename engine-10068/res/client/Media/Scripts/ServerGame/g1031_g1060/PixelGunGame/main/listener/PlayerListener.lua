--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "settlement.TeamSettlement"
require "settlement.PersonalSettlement"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)
    BaseListener.registerCallBack(PlayerDieTipEvent, function()
        return false
    end)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerDropItemEvent.registerCallBack(function()
        return false
    end)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerChangeItemInHandEvent.registerCallBack(self.onPlayerChangeItemInHand)
    PlayerReviewPlayerEvent.registerCallBack(self.onPlayerReviewPlayer)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    PlayerOpenGlideEvent.registerCallBack(self.onPlayerOpenGlide)

    PlayerSelect1v1BoxEvent:registerCallBack(self.onPlayerSelect1v1Box)
    PlayerBackPixelGunHallEvent:registerCallBack(self.onPlayerBackHall)
    PlayerPlayAgainEvent:registerCallBack(self.onPlayerPlayAgain)
    Player1v1RevengeEvent:registerCallBack(self.onPlayer1v1Revenge)
    ReceiveChickenWeaponEvent:registerCallBack(self.onReceiveChickenWeapon)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local players = PlayerManager:getPlayerCount()
    if players == GameConfig.maxPlayers then
        return GameOverCode.PlayerIsEnough
    end
    if not GameMatch.Process:ifPlayerCanLogin() then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    if player == nil then
        return
    end
    GameMatch:onPlayerQuit(player)
    local site = SiteConfig:getSitesById(player.siteId)
    if site == nil then
        return
    end
    if site.playernum == 0 then
        site.process:reset()
    else
        site:canNotRevenge()
    end
end

function PlayerListener.onPlayerReady(player)
    player.isReady = true
    GameMatch.Process:assignTeam(player)
    DbUtil:getPlayerData(player)
    if GameMatch.gameType ~= "g1053" then
        HostApi.sendPlaySound(player.rakssid, 10037)
    end
    GameMatch.Process:setName()
    HostApi.sendSetMoveState(player.rakssid, 2)
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    if GameMatch.Process:isGameOver(player) then
        return -1
    end

    if not GameMatch.Process:canRespawn() then
        if player.respawnTaskKey then
            GameMatch.Process:onPlayerRespawn(player)
            return 43200
        end
        return -1
    end
    GameMatch.Process:onPlayerRespawn(player)
    return 43200
end

function PlayerListener.onPlayerMove(Player, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(Player)
    if player == nil then
        return false
    end
    if y <= GameConfig.deathLineY and GameMatch.Process:isRunning(player.siteId) then
        player:subHealth(99999)
    end
    if not player.isLife then
        return true
    end
    player:onMove(x, y, z)
    if GameMatch.Process:isRunning(player.siteId) then
        SupplyManager:onPlayerMove(player, x, y, z)
    end
    if not player.isParachute and GameMatch.Process:startFly() then
        if player.changeWing == false then
            player:changeClothes("custom_wing", 105301)
            player.changeWing = true
        end

        AirplaneConfig:ifOpenParachute(player:getPosition().y, player)
    end

    if GameMatch.Process:hasPoisonArea() and player.isLife then
        local poison = GameMatch.Process:getPoisonArea()
        local inArea = poison:inArea(VectorUtil.newVector3(x, y, z))
        if not inArea and not player.isInPoison then
            player.isInPoison = true
            player.killByPoison = true
            GamePacketSender:sendPixelGunShowPoisonMask(player.rakssid, true)
        elseif inArea and player.isInPoison then
            player.isInPoison = false
            player.killByPoison = false
            GamePacketSender:sendPixelGunShowPoisonMask(player.rakssid, false)
        end
    end
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hurter == nil then
        return true
    end

    if GameMatch.Process:isRunning(hurter.siteId) == false then
        return false
    end

    if damageType == "outOfWorld" then
        hurtValue.value = 10000
        return true
    end

    if damageType == "fall" or damageType == "inWall" or damageType == "generic" then
        return false
    end

    if hurter:isInvincible() or hurter.isFlyInvincible then
        return false
    end

    if damageType == "onFire" or damageType == "magic" then
        hurter.damageType = damageType
        return true
    end

    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if attacker == nil then
        return true
    end

    if hurter:getTeamId() == attacker:getTeamId() then
        return false
    end

    if damageType == "explosion.player" or damageType == "explosion" then
        hurtValue.value = math.max(hurtValue.value * 1.3, 10)
    end

    if damageType == "player.gun.headshot" then
        attacker.isHeadshot = true
        hurtValue.value = hurtValue.value + attacker.addHeadShot
    else
        if damageType == "player" then -- no gun
            hurtValue.value = PropConfig:getCurHandDamage(attacker)
        end
        attacker.isHeadshot = false
    end
    hurtValue.value = hurtValue.value + attacker.addDamage
    attacker:onUsedItem(hurter, hurtValue)
    hurtValue.value = hurter:subDefense(hurtValue.value)
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end
    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if killer == nil then
        killer = PlayerManager:getPlayerByUserId(dier.onSkillDamageUid)
    end
    if killer == nil then
        if dier.damageType == "onFire" then
            killer = PlayerManager:getPlayerByUserId(dier.onFireDamageUid)
        end
        if dier.damageType == "magic" then
            killer = PlayerManager:getPlayerByUserId(dier.onPosionDamageUid)
        end
    end
    if killer == nil then
        killer = PlayerManager:getPlayerByUserId(dier.onPosionDamageUid)
    end
    if killer == nil then
        killer = PlayerManager:getPlayerByUserId(dier.onFireDamageUid)
    end
    dier.damageType = ""
    dier.onSkillDamageUid = 0
    dier.onPosionDamageUid = 0
    dier.onFireDamageUid = 0

    if killer ~= nil then
        killer:onKill(dier)
        GameMatch.Process:onPlayerKill(killer)
    end
    GameMatch.Process:onPlayerDie(killer, dier)
    return true
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum)
    return false
end

function PlayerListener.onPlayerSelect1v1Box(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if GameMatch.Process:isSelect(player.siteId) == false then
        return
    end
    local index = DataBuilder.new():fromData(data):getNumberParam("index")
    player.chest = tonumber(index)
    GameMatch.Process:updateChest(player.siteId)
end

function PlayerListener.onPlayerBackHall(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    EnterRoomManager:addEnterRoomQueue(player, GameConfig.hallGameType, "")
end

function PlayerListener.onPlayerPlayAgain(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    MsgSender.sendTopTipsToTarget(player.rakssid, 3600, Messages:msgWaitMatching())
    GameMatch.Process:onPlayerPlayAgain(player)
end

function PlayerListener.onPlayer1v1Revenge(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    player.revenge = true
    local site = SiteConfig:getSitesById(player.siteId)
    if site ~= nil then
        site:updateRevenge()
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerChangeItemInHand(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    player:onEquipItem(itemId)
end

function PlayerListener.onPlayerReviewPlayer(rakssid1, rakssid2)
    local player1 = PlayerManager:getPlayerByRakssid(rakssid1)
    local player2 = PlayerManager:getPlayerByRakssid(rakssid2)
    if player1 == nil or player2 == nil then
        return
    end
    if player1:getTeamId() ~= player2:getTeamId() then
        player1.entityPlayerMP:changeNamePerspective(player2.rakssid, true)
        player1:setShowName(GameMatch.Process:buildShowName(player1, false), player2.rakssid)
    else
        player1.entityPlayerMP:changeNamePerspective(player2.rakssid, false)
        player1:setShowName(GameMatch.Process:buildShowName(player1, true), player2.rakssid)
    end
end

function PlayerListener.onWatchAdvertFinished(rakssid, type, params, code)
    if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if type == Define.AdType.Game then
        player:doDoubleReward()
    end
end

function PlayerListener.onPlayerOpenGlide(userId, isOpenGlide)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if not isOpenGlide then
        player:resetClothes()
        player.isFlyInvincible = false
        --player:changeClothes("custom_glasses", 9999)
        HostApi.changePlayerPerspece(player.rakssid, 0)
    else
        player.isFlyInvincible = true
        player:onOpenParachute()
    end
end

function PlayerListener.onReceiveChickenWeapon(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    SupplyManager:playerReceiveWeapon(player.userId)
end

return PlayerListener
--endregion
