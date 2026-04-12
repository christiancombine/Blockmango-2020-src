GameTokenSkin = {}

function GameTokenSkin:onButtonClick(player, itemId)
    if not player.property.skinList[tostring(itemId)] then
        self:onBuySkin(player, itemId, true)
    elseif player.property.skinList[tostring(itemId)] and player.property.skinId ~= itemId then
        self:onEquipSkin(player, itemId)
    elseif player.property.skinList[tostring(itemId)] and player.property.skinId == itemId then
        self:onEquipSkin(player, -1)
    end
end

function GameTokenSkin:onBuySkin(player, id, needMoney)
    if not player then
        return
    end
    local config = TokenSkinConfig:getSkinConfig(id)
    if not config then
        return
    end

    --判断性别
    if player:getSex() ~= config.sex then
        return
    end
    if player.property.skinList[tostring(id)] then
        return
    end

    LogUtil.log("GameTokenSkin:onBuySkin player == "..tostring(player.userId) .."  id == ".. tostring(id))
    if needMoney then

        if config.moneyType == Define.MoneyType.DiamondGolds1 or config.moneyType == Define.MoneyType.DiamondGolds2 then
            PayHelper.payDiamondsSync(player.userId, config.id, config.price, function(player)
                self:onBuySkinSuccess(player, id)
            end, nil)
        end


        if config.moneyType == Define.MoneyType.Token then
            if player.property.championShipCoin >= config.price then
                player.property:changeToken(-config.price)
                self:onBuySkinSuccess(player, id)
            else
                HostApi.sendCommonTip(player.rakssid, "token_not_enough")
            end

        end


        -- if player:getDiamond() < config.price then
        --     ReportUtil.reportBuySkinId(player, id, false)
        --     return
        -- end
        -- PayHelper.payDiamondsSync(player.userId, config.id, config.price, function(player)
        --     self:onBuySkinSuccess(player, id)
        -- end, nil)
    else
        self:onBuySkinSuccess(player, id)
    end

end

function GameTokenSkin:onEquipSkin(player, id)
    if not player then
        return
    end

    if player:isBeGrab() then
        return
    end

    LogUtil.log("GameTokenSkin:onEquipSkin player == "..tostring(player.userId) .."  id == ".. tostring(id))
    player:equipSkin(id)
end


function GameTokenSkin:onBuySkinSuccess(player, id)
    player.property.skinList[tostring(id)] = 1

    --通知玩家购买成功
    local data = DataBuilder.new()
                            :addParam("Type", Define.PayStoreItem.Skin)
                            :addParam("Param", id)
                            :getData()
    PacketSender:sendLuaCommonData(player.rakssid, "BuySuccess", data)

    --购买后立刻装备
    self:onEquipSkin(player, id)

    ReportUtil.reportBuySkinId(player, id, true)

    LogUtil.log("GameTokenSkin:onBuySkinSuccess player == "..tostring(player.userId) .."  id == ".. tostring(id))
end

return GameTokenSkin