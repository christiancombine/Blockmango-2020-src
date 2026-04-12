
GameTokenGene = {}

function GameTokenGene:onButtonClick(player, itemId)

    for _, Value in pairs(player.tokenShopGene) do
        if Value.id == itemId then
            if Value.status == Define.TokenItemBuyStatus.Unlock then
                self:onGeneBuy(player, itemId, true)
            elseif Value.status == Define.TokenItemBuyStatus.Buy then
                self:onGeneUsed(player, itemId)
            elseif Value.status == Define.TokenItemBuyStatus.Used then
                self:onGeneUnload(player, itemId)
            end
        end
    end
end

function GameTokenGene:onGeneBuy(player, itemId, needMoney)
    local gene = TokenGeneConfig:getGeneById(itemId)
    if gene == nil then
        return
    end
    if not needMoney then
        self:onBuySuccess(player, itemId)
        return
    end
    if gene.moneyType == Define.MoneyType.Token then
        if player.property.championShipCoin >= gene.price then
            player.property:changeToken(-gene.price)
            self:onBuySuccess(player, itemId)
        else
            HostApi.sendCommonTip(player.rakssid, "token_not_enough")
        end
    end

    if gene.moneyType == Define.MoneyType.DiamondGolds1 then
        PayHelper.payDiamondsSync(player.userId, gene.id, gene.price, function(player)
            self:onBuySuccess(player, itemId)
        end, nil)

    end

    if gene.moneyType == Define.MoneyType.DiamondGolds2 then
        PayHelper.payDiamondsSync(player.userId, gene.id, gene.price, function(player)
            self:onBuySuccess(player, itemId)
        end, nil)

    end
    ReportUtil.reportGeneBuy(player, itemId)
end

function GameTokenGene:onBuySuccess(player, itemId)

    local data = {}
    local used = true
    local usingGene = TokenGeneConfig:getGeneById(player.property.geneId)
    local gene = TokenGeneConfig:getGeneById(itemId)
    if usingGene == nil then
        usingGene = GeneConfig:getGeneById(player.property.geneId)
    end

    if gene == nil then
        gene = GeneConfig:getGeneById(player.property.geneId)
    end

    if usingGene and gene and usingGene.geneMuscleMass <= gene.geneMuscleMass then

        for _, Value in pairs(player.tokenShopGene) do
            if Value.status == Define.TokenItemBuyStatus.Used then
                Value.status = Define.TokenItemBuyStatus.Buy
                table.insert(data, {
                    Id = Value.id,
                    Status = Value.status
                })
            end
        end

        for _, Value in pairs(player.tokenShopGene) do
            if Value.id == itemId then
                Value.status = Define.TokenItemBuyStatus.Used
                table.insert(data, {
                    Id = Value.id,
                    Status = Value.status
                })
            end
        end

        player.property:changeGene(itemId)

    else
        used = false
        for _, Value in pairs(player.tokenShopGene) do
            if Value.id == itemId then
                Value.status = Define.TokenItemBuyStatus.Buy
                table.insert(data, {
                    Id = Value.id,
                    Status = Value.status
                })
            end
        end

    end

    for _, Value in pairs(player.tokenShopGene) do
        local gene = TokenGeneConfig:getGeneById(Value.id)
        if gene and gene.unlock == itemId and Value.status == Define.TokenItemBuyStatus.Lock and player.property.advancedLevel >= gene.advancedId then
            Value.status = Define.TokenItemBuyStatus.Unlock
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    local nextGene = TokenGeneConfig:getNextSortGeneId(gene.id)
    if gene and gene.moneyType == Define.MoneyType.DiamondGolds1 or gene.moneyType == Define.MoneyType.DiamondGolds2 then


        for _, Value in pairs(player.tokenShopGene) do

            local valueGene = TokenGeneConfig:getGeneById(Value.id)
            if valueGene and valueGene.sortId < gene.sortId and Value.status == Define.TokenItemBuyStatus.Lock then

                Value.status = Define.TokenItemBuyStatus.Unlock
                table.insert(data, {
                    Id = Value.id,
                    Status = Value.status
                })
            end
            if  Value.id == nextGene.id and Value.status == Define.TokenItemBuyStatus.Lock then

                Value.status = Define.TokenItemBuyStatus.Unlock
                table.insert(data, {
                    Id = Value.id,
                    Status = Value.status
                })
            end


        end
    end
    --print("GameTokenGene:onBuySuccess(player, itemId)"..itemId)

    TokenShopManager:senderTokenGeneInfo(player, data)
    GameTokenGene:shareDataToGene(player, data, used)

    -- local nextNeedSelect = TokenGeneConfig:getNextNeedSelectById(gene.id)
    -- if nextNeedSelect then
    --     GamePacketSender:sendShopItemSelectStatus(player.rakssid, Define.ShopKind.Gene, nextNeedSelect.id)
    -- end
end

function GameTokenGene:onGeneUsed(player, itemId)

    local data = {}
    for _, Value in pairs(player.tokenShopGene) do
        if Value.status == Define.TokenItemBuyStatus.Used then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    for _, Value in pairs(player.tokenShopGene) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Used
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    player.property:changeGene(itemId)
    TokenShopManager:senderTokenGeneInfo(player, data)
    GameTokenGene:shareDataToGene(player, data, true)

end

function GameTokenGene:onGeneUnload(player, itemId)

end

function GameTokenGene:initGeneItem(player)
    if #player.tokenShopGene ~= 0 then
        self:updateShopGene(player)
        return
    end
    table.sort(TokenGeneConfig.settings, function(a, b)
        return a.id < b.id
    end)

    local itemId = 0
    local i = 1
    for _, Value in pairs(TokenGeneConfig.settings) do
        if i == 1 then
            table.insert(player.tokenShopGene, {
                id = Value.id,
                status = Define.TokenItemBuyStatus.Unlock
            })
            itemId = Value.id
        else
            table.insert(player.tokenShopGene, {
                id = Value.id,
                status = Define.TokenItemBuyStatus.Lock
            })
        end
        i = i + 1
    end

    for _, Value in pairs(player.tokenShopGene) do
        local gene = TokenGeneConfig:getGeneById(Value.id)
        if gene and gene.unlock == itemId and Value.status == Define.TokenItemBuyStatus.Lock  and player.property.advancedLevel >= gene.advancedId then
            Value.status = Define.TokenItemBuyStatus.Unlock
        end
    end
    player.property:changeGene(itemId)
end

function GameTokenGene:updateShopGene(player)

    if not player then
        return
    end
    for _, setting in pairs(TokenGeneConfig.settings) do

        local find = false
        for _, Value in pairs(player.tokenShopGene) do
            if Value.id == setting.id then
                find = true
                break
            end
        end

        if find == false then
            local preGene = TokenGeneConfig:getGeneById(setting.unlock)
            if preGene then
                local flag = false
                for _, Value in pairs(player.tokenShopGene) do
                    if Value.id == preGene.id and (Value.status == Define.TokenItemBuyStatus.Buy or Value.status == Define.TokenItemBuyStatus.Used )then
                        flag = true
                        break
                    end
                end

                if flag == true  then
                    table.insert(player.tokenShopGene, {
                        id = setting.id,
                        status = Define.TokenItemBuyStatus.Unlock
                    })

                else
                    table.insert(player.tokenShopGene, {
                        id = setting.id,
                        status = Define.TokenItemBuyStatus.Lock
                    })
                end
            else
                table.insert(player.tokenShopGene, {
                    id = setting.id,
                    status = Define.TokenItemBuyStatus.Unlock
                })
            end
        end

    end

end

function GameTokenGene:initAdvancedGeneItem(player)

    table.sort(player.tokenShopGene, function(a, b)
        return a.id < b.id
    end)

    local itemId = -1
    local i = 1
    local data = {}
    -- local flag = false

    -- for _, Value in pairs(player.tokenShopGene) do
    --     local config = TokenGeneConfig:getGeneById(Value.id)
    --     if (Value.status == Define.TokenItemBuyStatus.Buy or Value.status == Define.TokenItemBuyStatus.Used)
    --             and (config.moneyType == Define.MoneyType.DiamondGolds2 or config.moneyType == Define.MoneyType.DiamondGolds1) then
    --         flag = true
    --         itemId = Value.id
    --     end
    -- end

    -- if flag == false then

        for _, Value in pairs(player.tokenShopGene) do
            local config = TokenGeneConfig:getGeneById(Value.id)
            if config then
                if i == 1 then
                    Value.status = Define.TokenItemBuyStatus.Buy
                    itemId = Value.id
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                elseif Value.status ~= Define.TokenItemBuyStatus.Lock and config.moneyType ~= Define.MoneyType.DiamondGolds1
                        and config.moneyType ~= Define.MoneyType.DiamondGolds2 then
                    Value.status = Define.TokenItemBuyStatus.Unlock
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                elseif Value.status == Define.TokenItemBuyStatus.Used and (config.moneyType == Define.MoneyType.DiamondGolds1
                        or config.moneyType == Define.MoneyType.DiamondGolds2) then
                    Value.status = Define.TokenItemBuyStatus.Buy
                    table.insert(data, {
                        Id = Value.id,
                        Status = Value.status
                    })
                end
                i = i + 1
            end
        end

    -- else

    --     for _, Value in pairs(player.tokenShopGene) do

    --         local config = TokenGeneConfig:getGeneById(Value.id)

    --         if Value.id == itemId then
    --             Value.status = Define.TokenItemBuyStatus.Used
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif i == 1 then
    --             Value.status = Define.TokenItemBuyStatus.Buy
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif Value.status ~= Define.TokenItemBuyStatus.Lock and config.moneyType ~= Define.MoneyType.DiamondGolds1
    --                 and config.moneyType ~= Define.MoneyType.DiamondGolds2 then
    --             Value.status = Define.TokenItemBuyStatus.Unlock
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         elseif Value.status == Define.TokenItemBuyStatus.Used and (config.moneyType == Define.MoneyType.DiamondGolds1
    --                 or config.moneyType == Define.MoneyType.DiamondGolds2) then
    --             Value.status = Define.TokenItemBuyStatus.Buy
    --             table.insert(data, {
    --                 Id = Value.id,
    --                 Status = Value.status
    --             })
    --         end

    --         i = i + 1
    --     end

    -- end

    if itemId ~= -1 then
        player.property:changeGene(itemId)
    end
    TokenShopManager:senderTokenGeneInfo(player, data)
    GameTokenGene:shareDataToGene(player, data, true)

end

function GameTokenGene:shareDataToGene(player, data, isUsed)
    local info = {}
    if isUsed then
        for k, v in pairs(player.shopGene) do
            if v.status == Define.TokenItemBuyStatus.Used then
                v.status = Define.TokenItemBuyStatus.Buy
                table.insert(info, {
                    Id = v.id,
                    Status = v.status
                })
            end
        end
    end

    for _, value in pairs(data) do
        for k, v in pairs(player.shopGene) do
            if value.Id == v.id then
                v.status = value.Status
                table.insert(info, {
                    Id = value.Id,
                    Status = value.Status
                })
            end

            if value.Status == Define.TokenItemBuyStatus.Buy or value.Status == Define.TokenItemBuyStatus.Used then
                local gene = GeneConfig:getGeneById(v.id)
                if gene and gene.unlock == value.Id and v.status == Define.ItemBuyStatus.Lock then
                    v.status = Define.ItemBuyStatus.Unlock
                    table.insert(info, {
                        Id = v.id,
                        Status = v.status
                    })
                end
            end
        end
    end
    ShopManager:senderGeneInfo(player, info)
end


return GameTokenGene