--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerUnlockCommodityEvent.registerCallBack(self.onPlayerUnlockCommodity)
    BuildWarOpenShopEvent.registerCallBack(self.BuildWarOpenShop)
    BuildWarGradeEvent.registerCallBack(self.BuildWarGrade)
    BuildWarOpenGuessResultEvent.registerCallBack(self.BuildWarOpenGuessResult)
    BuildWarGuessVisitEvent.registerCallBack(self.BuildWarGuessVisit)
    BuildWarGuessSucEvent.registerCallBack(self.BuildWarGuessSuc)
    BuildWarShowResultEvent.registerCallBack(self.BuildWarShowResult)
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
    GameMatch:getDBDataRequire(player)
    RankNpcConfig:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    MerchantConfig:prepareStore(player.rakssid)
    return 43200
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerUnlockCommodity(rakssid, unlockedCommodity, meta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:addUnlockedCommodity(unlockedCommodity, meta)
    end
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return true
end

function PlayerListener.BuildWarOpenShop(rakssid)
    local target_merchants_id = nil
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    for k, v in pairs(MerchantConfig.merchants) do
        if player then
            if v.sp_room_id == player.at_room_id then
                target_merchants_id = v.id
                break
            end
        end
    end

    if target_merchants_id then 
        HostApi.shopOpenShopByEntityId(rakssid, target_merchants_id)
    end
end

function PlayerListener.BuildWarGrade(rakssid, grade)
    GameMatch:recordGrade(rakssid, grade)
end

function PlayerListener.BuildWarOpenGuessResult(rakssid)
    GameMatch:showBuildGuessResult(rakssid)
end

function PlayerListener.BuildWarGuessVisit(rakssid, userId)
    GameMatch:guessVisit(rakssid, userId)
end

function PlayerListener.BuildWarGuessSuc(rakssid, room_id)
    GameMatch:guessSuc(rakssid, room_id)
end

function PlayerListener.BuildWarShowResult(rakssid)
    GameMatch:showBuildWarResult(rakssid)
end


return PlayerListener
--endregion
