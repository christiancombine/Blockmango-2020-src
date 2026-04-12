GameTokenFitnessEquip = {}

function GameTokenFitnessEquip:onButtonClick(player, itemId)
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.id == itemId then 
            if Value.status == Define.TokenItemBuyStatus.Unlock then
                self:onFitnessEquipBuy(player, itemId, true)
            elseif Value.status == Define.TokenItemBuyStatus.Buy then
                self:onFitnessEquipUsed(player, itemId)
            elseif Value.status == Define.TokenItemBuyStatus.Used then
                self:onFitnessEquipUnload(player, itemId)
            end
        end
    end
end

function GameTokenFitnessEquip:onFitnessEquipBuy(player, itemId, needMoney)    
    local equip = TokenFitnessEquipConfig:getFitnessEquipConfig(itemId)
    if equip == nil then
        return
    end
    if not needMoney then
        self:onBuySuccess(player, itemId)
        return
    end
    if equip.moneyType == Define.MoneyType.Token then
        if player.property.championShipCoin >= equip.price then
            player.property:changeToken(-equip.price)
            self:onBuySuccess(player, itemId)
        else
            HostApi.sendCommonTip(player.rakssid, "token_not_enough")
        end
    end

    if equip.moneyType == Define.MoneyType.DiamondGolds1 or equip.moneyType == Define.MoneyType.DiamondGolds2 then
        PayHelper.payDiamondsSync(player.userId, equip.id, equip.price, function(player)
            self:onBuySuccess(player, itemId)
        end, nil)
    end

    ReportUtil.reportFitnessEquipBuy(player, itemId)
end

function GameTokenFitnessEquip:onBuySuccess(player, itemId)
    local data = {}
    local usedId = 0
    local unlockId = 0

    --Used to Buy
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.status == Define.TokenItemBuyStatus.Used then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    --to Used
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Used
            usedId = Value.id
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })


        end
    end
    --Lock to Unlock
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        local equip = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
        if equip and equip.unlock == itemId and Value.status == Define.TokenItemBuyStatus.Lock and player.property.advancedLevel >= equip.advancedId then
            Value.status = Define.TokenItemBuyStatus.Unlock
            unlockId = Value.id
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    table.sort(player.tokenShopFitnessEquip, function(a, b)
        return TokenFitnessEquipConfig:getFitnessEquipSortId(a.id) < TokenFitnessEquipConfig:getFitnessEquipSortId(b.id)
    end)

    local equip = TokenFitnessEquipConfig:getFitnessEquipConfig(itemId)
    player.property:changeFitnessEquipId(usedId, unlockId)

    GameTokenFitnessEquip:shareDataToFitnessEquip(player, data)
    TokenShopManager:senderTokenFitnessEquipInfo(player, data)
end

function GameTokenFitnessEquip:onFitnessEquipUsed(player, itemId)

    local data = {}
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.status == Define.TokenItemBuyStatus.Used then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Used
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    player.property:changeFitnessEquipId(itemId, nil)

    GameTokenFitnessEquip:shareDataToFitnessEquip(player, data)
    TokenShopManager:senderTokenFitnessEquipInfo(player, data)
end

function GameTokenFitnessEquip:onFitnessEquipUnload(player, itemId)

end

function GameTokenFitnessEquip:updateShopFitnessEquip(player)
    if not player then
        return
    end

    for _, setting in pairs(TokenFitnessEquipConfig.settings) do
        local find = false
        for _, data in pairs(player.tokenShopFitnessEquip) do
            if data.id == setting.id then
                find = true
                break
            end
        end

        if not find then
            table.insert(player.tokenShopFitnessEquip, {
                id = setting.id,
                status = Define.TokenItemBuyStatus.Lock
            })
        end
    end
end

function GameTokenFitnessEquip:initFitnessEquipItem(player)
    if #player.tokenShopFitnessEquip ~= 0 then
        GameTokenFitnessEquip:updateShopFitnessEquip(player)
        return
    end

    table.sort(TokenFitnessEquipConfig.settings, function(a, b)
        return a.sortId < b.sortId
    end)

    local i = 1
    local itemId = 0
    for _, Value in pairs(TokenFitnessEquipConfig.settings) do
        if i == 1 then
            table.insert(player.tokenShopFitnessEquip, {
                id = Value.id,
                status = Define.TokenItemBuyStatus.Unlock
            })
            itemId = Value.id
        else
            table.insert(player.tokenShopFitnessEquip, {
                id = Value.id,
                status = Define.TokenItemBuyStatus.Lock
            })
        end
        i = i + 1
    end

    table.sort(player.tokenShopFitnessEquip, function(a, b)
        return TokenFitnessEquipConfig:getFitnessEquipSortId(a.id) < TokenFitnessEquipConfig:getFitnessEquipSortId(b.id)
    end)

    local nextUnlockItemId = nil
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        local equip = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
        if equip and equip.unlock == itemId and Value.status == Define.TokenItemBuyStatus.Lock  and player.property.advancedLevel >= equip.advancedId then
            nextUnlockItemId = equip.id
            Value.status = Define.TokenItemBuyStatus.Unlock
        end
    end
    
    if not nextUnlockItemId then
        for _, Value in pairs(player.tokenShopFitnessEquip) do
            if Value.status == Define.TokenItemBuyStatus.Lock then
                Value.status = Define.TokenItemBuyStatus.Unlock
                break
            end
        end
    end

    -- player.property:changeFitnessEquipId(itemId, nextUnlockItemId)

end

function GameTokenFitnessEquip:initAdvancedFitnessEquipItem(player)
    local i = 1
    local itemId = -1
    local data = {}
    -- local flag = false

    table.sort(player.tokenShopFitnessEquip, function(a, b)
        return TokenFitnessEquipConfig:getFitnessEquipSortId(a.id) < TokenFitnessEquipConfig:getFitnessEquipSortId(b.id)
    end)

    -- for _, Value in pairs(player.tokenShopFitnessEquip) do
    --     local config = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
    --     if (Value.status == Define.ItemBuyStatus.Buy or Value.status == Define.ItemBuyStatus.Used)
    --             and (config.moneyType == Define.MoneyType.DiamondGolds2 or config.moneyType == Define.MoneyType.DiamondGolds1)
    --             and flag == false then
    --         flag = true
    --         itemId = Value.id
    --     end

    --     if (Value.status == Define.ItemBuyStatus.Buy or Value.status == Define.ItemBuyStatus.Used)
    --             and TokenFitnessEquipConfig:isPayFitnessEquip(Value.id) then
    --         flag = true
    --         itemId = Value.id
    --     end

    --     --������������
    --     if TokenFitnessEquipConfig:isPayFitnessEquip(Value.id) then
    --         local payConfig = TokenFitnessEquipConfig:getPayFitnessEquipConfig(Value.id)
    --         if payConfig and player.property.advancedLevel >= payConfig.unlockAdvancedLevel and Value.status == Define.ItemBuyStatus.Lock then
    --             Value.status = Define.ItemBuyStatus.Unlock
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         end
    --     end
    -- end

    -- if flag == false then
        for _, Value in pairs(player.tokenShopFitnessEquip) do
            local config = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
            if config then
                if i == 1 then
                    Value.status = Define.ItemBuyStatus.Buy
                    itemId = Value.id
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                elseif Value.status ~= Define.ItemBuyStatus.Lock and
                        (config.moneyType ~= Define.MoneyType.DiamondGolds1 and config.moneyType ~= Define.MoneyType.DiamondGolds2) then
                    Value.status = Define.ItemBuyStatus.Unlock
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                elseif Value.status == Define.ItemBuyStatus.Used  and
                        (config.moneyType == Define.MoneyType.DiamondGolds1 or config.moneyType == Define.MoneyType.DiamondGolds2) then
                    Value.status = Define.ItemBuyStatus.Buy
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                end
                i = i + 1
            end

        end
    -- else
    --     for _, Value in pairs(player.tokenShopFitnessEquip) do
    --         local config = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
    --         if Value.id == itemId then
    --             Value.status = Define.ItemBuyStatus.Used
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif i == 1 then
    --             Value.status = Define.ItemBuyStatus.Buy
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif Value.status ~= Define.ItemBuyStatus.Lock and
    --                 (config.moneyType ~= Define.MoneyType.DiamondGolds1 and config.moneyType ~= Define.MoneyType.DiamondGolds2) then
    --             Value.status = Define.ItemBuyStatus.Unlock
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif Value.status == Define.ItemBuyStatus.Used  and
    --                 (config.moneyType == Define.MoneyType.DiamondGolds1 or config.moneyType == Define.MoneyType.DiamondGolds2) then
    --             Value.status = Define.ItemBuyStatus.Buy
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         end
    --         i = i + 1
    --     end
    -- end

    local nextUnlockItemId = nil
    for _, Value in pairs(player.shopFitnessEquip) do
        local equip = TokenFitnessEquipConfig:getFitnessEquipConfig(Value.id)
        if equip and equip.unlock == itemId then
            nextUnlockItemId = equip.id
        end
    end

    -- if itemId ~= -1 then
    --     player.property:changeFitnessEquipId(itemId, nextUnlockItemId)
    -- end

    GameTokenFitnessEquip:shareDataToFitnessEquip(player, data)
    TokenShopManager:senderTokenFitnessEquipInfo(player, data)
end

function GameTokenFitnessEquip:shareDataToFitnessEquip(player, data)
    local info = {}

    for k, v in pairs(player.shopFitnessEquip) do
        if v.status == Define.TokenItemBuyStatus.Used then
            v.status = Define.TokenItemBuyStatus.Buy
            table.insert(info, {
                Id = v.id,
                Status = v.status
            })
        end
    end

    for _, value in pairs(data) do
        for k, v in pairs(player.shopFitnessEquip) do
            if value.Id == v.id then
                v.status = value.Status
                table.insert(info, {
                    Id = value.Id,
                    Status = value.Status
                })
            end
            if value.Status == Define.TokenItemBuyStatus.Buy or value.Status == Define.TokenItemBuyStatus.Used then
                local equip = FitnessEquipConfig:getFitnessEquipConfig(v.id)
                if equip and equip.unlock == value.Id and v.status == Define.ItemBuyStatus.Lock then
                    v.status = Define.ItemBuyStatus.Unlock
                    table.insert(info, {
                        Id = v.id,
                        Status = v.status
                    })
                end
            end
        end
    end
    ShopManager:senderFitnessEquipInfo(player, info)
end

return GameTokenFitnessEquip