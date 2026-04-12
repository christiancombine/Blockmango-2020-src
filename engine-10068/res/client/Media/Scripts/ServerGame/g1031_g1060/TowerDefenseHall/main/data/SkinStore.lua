SkinStore = {}

function SkinStore:init()
    --TODO
end

function SkinStore:onBuyPaySkin(player, skinId)
    if not player then
        return
    end

    local config = SkinStoreConfig:getConfigById(skinId)
    if not config then
        return
    end

    if player.skinList and player.skinList[tostring(skinId)] ~= nil then
        return
    end

    if config.moneyType == Define.Money.HallGold then
        if player.Wallet:costMoney(Define.Money.HallGold, config.price) then
            self:onBuyPaySkinSuccess(player, skinId)
        end
    elseif config.moneyType == Define.Money.DiamondsGold then
        PayHelper.payDiamondsSync(player.userId, config.id, config.price, function(player)
            self:onBuyPaySkinSuccess(player, skinId)
        end, nil)
    end
end

function SkinStore:onBuyPaySkinSuccess(player, skinId)
    player:onGetNewSkin(skinId)
    --通知玩家购买成功
    ReportUtil.reportBuySkin(player, skinId)
end

return SkinStore