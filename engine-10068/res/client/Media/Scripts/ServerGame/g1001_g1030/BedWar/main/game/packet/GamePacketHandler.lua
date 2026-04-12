local handler = class("GamePacketHandler", EventObservable)

function handler:init()
    self:registerDataCallBack("SettlementGood", function(fromUserId, toUserId)
        SettlementManager:sendGood(fromUserId, toUserId)
    end)
    self:registerDataCallBack("BuyQBB", self.onPlayerBuyQuicklyBuildBlock)
end

function handler.onPlayerBuyQuicklyBuildBlock(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        local moneyType = Define.Money.DIAMOND
        local price = GameConfig.quicklyBuildBlockCost
        if MoneyUtil:checkHasMoney(player, moneyType, price) then
            MoneyUtil:consumeMoney(player, 30000, moneyType, price, function()
                GamePacketSender:sendQuicklyBuildBlockBuySuccess(player.rakssid)
            end)
        end
    end
end

handler:init()