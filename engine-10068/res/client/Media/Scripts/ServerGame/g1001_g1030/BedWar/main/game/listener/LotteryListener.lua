LotteryListener = {}

function LotteryListener:init()
    LotteryRefreshEvent:registerCallBack(self.onPlayerRequestLotteryRefresh)
    LotteryEvent:registerCallBack(self.onPlayerRequestLottery)
    CloseVideoAdTipEvent:registerCallBack(self.onCloseVideoAdTip)
    LotteryReplaceEvent:registerCallBack(self.onLotteryReplace)
    LotteryEffectEndEvent:registerCallBack(self.onLotteryEffectEnd)
end

function LotteryListener.onPlayerRequestLotteryRefresh(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    GameLottery:RefreshLotteryPool(userId)
end

function LotteryListener.onPlayerRequestLottery(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local seq = DataBuilder.new():fromData(data):getNumberParam("seq")
    GameLottery:onPlayerLottery(userId, seq)
end

function LotteryListener.onCloseVideoAdTip(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local Type = builder:getNumberParam("Type")
    local isClickSure = builder:getBoolParam("IsClickSure")

    if Type == Define.AdType.UpdateFeedback then
        ReportUtil.reportUpdateFeedback(player, isClickSure)
        player.updateTipsVersion = GameConfig.updateTipsVersion
    end

end

function LotteryListener.onLotteryReplace(userId, data)
    local builder = DataBuilder.new():fromData(data)
    local isReplace = builder:getBoolParam("isReplace")
    GameLottery:onLotteryReplace(userId, isReplace)
end

function LotteryListener.onLotteryEffectEnd(userId)
    GameLottery:onLotteryEffectEnd(userId)
end