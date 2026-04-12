--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "config.GameConfig"
require "config.XTools"
require "data.XGameTips"
require "data.XGameRank"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerDropItemEvent.registerCallBack(function() return false end)

    PlayerLongHitEntityEvent.registerCallBack(self.onPlayerLongHitEntity)

    PlayerBuyCustomPropEvent.registerCallBack(self.onBuyCustomProp)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyNpcCommodity)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerGetRankData)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerByAppShopItem)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangWeapon)
end

function PlayerListener.onPlayerGetRankData(rakssId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player ~= nil then
        XGameRank:sendRealRankData(player)
    end
end

function PlayerListener.onPlayerByAppShopItem(rakssId, gropId, itemId, msgType, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player ~= nil then
        isAddItem.value = false
        local result = player:OnBuyAppShopItem(itemId)
        msgType.value = result and 1036004 or 1036005
        return result
    end
    return false
end

function PlayerListener.onBuyCustomProp(rakssId, itemId, msgType)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    local res = XPlayerShopItems:HasNormalShopItem(itemId)
    if player ~= nil and res then
        local result = player:OnBuyNormalItemFromPlayerShop(itemId)
        msgType.value = result and 0 or 1036009
        return result
    end
    if player ~= nil then
        player:OnUpgradeWeapon(itemId)
        return true
    end
    return false
end

function PlayerListener.onPlayerBuyNpcCommodity(raskId, _, commodityInfo)
    local player = PlayerManager:getPlayerByRakssid(raskId)
    if player ~= nil then
        return player:OnBuyNpcCommodity(commodityInfo.goodsId)
    end
    return false
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
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    player:onPlayerRespawn()
    GameManager:onPlayerRespawn(player)
    return 43200
end

function PlayerListener.onChangWeapon(rasskid, itemid, hitValue)
    local player = PlayerManager:getPlayerByRakssid(rasskid)
    if player ~= nil then
        player:getSetHoldWeapon(itemid);
    end
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch.hasStartGame == false then
        return false
    end
    if damageType == "fall" then
        return false
    end
    if damageType == "outOfWorld" then
        return true
    end
    local attackPlayer = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    local beAccackPlayer = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if damageType == "explosion.player" or damageType == "explosion" then
        hurtValue.value = 0
        if attackPlayer ~= nil and beAccackPlayer ~= nil then         
            if attackPlayer.bindTeamId ~= beAccackPlayer.bindTeamId then
                if attackPlayer.HoldingWeaponId == 46 then
                    hurtValue.value = GameConfig.TntHitVlaue
                end
            end
        end
        return true
    end

    if attackPlayer ~= nil then
        local itemId = attackPlayer:getHeldItemId()
        if itemId == ItemID.FISHING_ROD then
            hurtValue.value = GameConfig.fishingRodDamage
        end
    end

    if damageType == "inFire" and beAccackPlayer ~= nil then
        hurtValue.value = GameConfig.magmahurtValue or 1
        return true
    end
    if attackPlayer ~= nil then
        local itemId = attackPlayer:getSetHoldWeapon()
        local hit = XPlayerShopItems:getWeaponHitById(itemId)
        if type(hit) == 'number' and hit > 0 then
            hurtValue.value = hit
        end
    end


    if beAccackPlayer and attackPlayer then
        if beAccackPlayer.bindTeamId ~= attackPlayer.bindTeamId then
            beAccackPlayer:OnBeAttack(attackPlayer, hurtValue.value)
            attackPlayer:OnAttack(hurtValue.value)
            return true
        else
            return false
        end
    end
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local die_player = PlayerManager:getPlayerByPlayerMP(diePlayer)
    local kill_player = PlayerManager:getPlayerByPlayerMP(killPlayer)

    if kill_player and die_player then
        local killTeam, killName = kill_player:getTeamNameAndPlayerName()
        local beKillTeam, beKillNamme = die_player:getTeamNameAndPlayerName()
        local money = GameConfig.killPlayerAddMoney
        if killName and killName and beKillTeam and beKillNamme and money then
            XGameTips:sendTips(1036001, killTeam, killName, beKillTeam, beKillNamme, money)
        end
        kill_player:onKillPlayer(die_player)
    end
    if die_player ~= nil then
        die_player:OnPlayerDeath(iskillByPlayer, killPlayer)
    end
    return true
end

function PlayerListener.onPlayerLongHitEntity(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local car = GameManager:getCarByEntityId(entityId)
    if car ~= nil then
        car:onPlayerLongHit(player)
    end
end

return PlayerListener
--endregion
