--region *.lua
require "messages.Messages"
require "config.ChipConfig"
require "config.SkillConfig"
require "data.GamePlayer"
require "util.DbUtil"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent,self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerBuySwitchablePropEvent.registerCallBack(self.onPlayerBuySwitchableProp)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch:isGameRunning() then
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
    DbUtil:getPlayerData(player)
    RankNpcConfig:getPlayerRankData(player)
    RealRankUtil:getPlayerRankData(player)
    GameConfig:syncMerchants(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    MsgSender.sendTopTipsToTarget(player.rakssid, 30, Messages:msgGameRule())
    return 43200
end

function PlayerListener.onPlayerRespawn(player)
    if GameConfig.isAllDay then
         return 43200
    end
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)

    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end

    if player:isInvincible() then
        return false
    end

    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    hurtValue.value = 0
    if hurtPlayer == nil or hurtFrom == nil then
        return false
    end
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if hurter == nil or attacker == nil then
        return false
    end
    if hurter:isInvincible() or attacker:isInvincible() then
        return false
    end
    if attacker.isHaveTnt == false or hurter.isHaveTnt then
        return false
    end
    hurter:becomeTntPlayer(attacker:getDisplayName())
    attacker:becomeNormalPlayer(hurter:getDisplayName())
    MsgSender.sendMsg(Messages:msgHitTNTHint(hurter:getDisplayName(), attacker:getDisplayName()))
    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerBuySwitchableProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local title = TitleConfig:getTitle(uniqueId)
    if title == nil then
        return
    end
    if player:hasTitle(title) then
        player:useTitle(title)
        player:sendTitleData()
        return
    end
    if player:buyTitle(title) then
        player:sendTitleData()
    end
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local coinId = goodsInfo.coinId
    local coinNum = goodsInfo.coinNum
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if type == 11 then
        -- skill
        local skill = SkillConfig:getSkillByItemId(goodsId)
        if skill == nil then
            return false
        end
        if player:hasSkill(skill) then
            msg.value = Messages:msgBuySkillHaveSkill()
            return false
        end
        local itemId = GameConfig:getItemIdByCoinId(coinId)
        local chip = ChipConfig:getChipByItemId(itemId)
        if chip == nil then
            return false
        end
        player:subChip(chip, coinNum)
        player:addSkill(skill)
        return true
    end
    if type == 12 then
        -- chest
        if goodsId == 749 then
            math.randomseed(tostring(os.time()):reverse():sub(1, 6))
            local random = NumberUtil.getIntPart(math.random() * 100)
            local chip = ChipConfig:randomChip(random)
            if chip ~= nil then
                player:addChip(chip)
            end
            attr.isAddGoods = false
            return true
        end
        -- chip
        local chip = ChipConfig:getChipByItemId(goodsId)
        if chip == nil then
            return false
        end
        player:addChip(chip, goodsNum)
        attr.isAddGoods = false
        return true
    end
    if type == 13 then
        -- money
        local itemId = GameConfig:getItemIdByCoinId(coinId)
        local chip = ChipConfig:getChipByItemId(itemId)
        if chip == nil then
            return false
        end
        return player:subChip(chip, coinNum)
    end
    return true
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local times = 0
    if itemId == 749 then
        times = 1
    end
    if itemId == 750 then
        times = 2
    end
    if itemId == 751 then
        times = 3
    end
    math.randomseed(tostring(os.time()):reverse():sub(1, 6))
    while times > 0 do
        local random = NumberUtil.getIntPart(math.random() * 100)
        local chip = ChipConfig:randomChip(random)
        if chip ~= nil then
            player:addChip(chip)
        end
        times = times - 1
    end
    isAddItem.value = false
    return true
end

function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)
    if GameMatch:isGameRunning() == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local skill = SkillConfig:getSkillByItemId(itemId)
    if skill == nil then
        return
    end
    MsgSender.sendBottomTipsToTarget(rakssid, 2, Messages:msgSelectSkillHint(skill.tip))
end

return PlayerListener
--endregion
