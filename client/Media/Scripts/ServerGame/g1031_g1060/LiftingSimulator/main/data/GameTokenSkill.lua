GameTokenSkill = {}

function GameTokenSkill:onButtonClick(player, itemId)

    for _, Value in pairs(player.tokenShopSkill) do
        if Value.id == itemId then
            if Value.status == Define.TokenItemBuyStatus.Unlock then
                self:onSkillBuy(player, itemId, true)
            elseif Value.status == Define.TokenItemBuyStatus.Buy then
                self:onSkillUsed(player, itemId)
            elseif Value.status == Define.TokenItemBuyStatus.Used then
                self:onSKillUnload(player, itemId)
            end
        end
    end
end

function GameTokenSkill:onSkillBuy(player, itemId, needMoney)

    local skill = TokenSkillConfig:getSkillById(itemId)
    if skill == nil then
        return
    end
    if not needMoney then
        self:onBuySuccess(player, itemId)
        return
    end
    if skill.MoneyType == Define.MoneyType.Token then
        
        if player.property.championShipCoin >= skill.Price then
            player.property:changeToken(-skill.Price)
            self:onBuySuccess(player, itemId)
        else
            HostApi.sendCommonTip(player.rakssid, "token_not_enough")
        end

    end

    if skill.MoneyType == Define.MoneyType.DiamondGolds1 then
        PayHelper.payDiamondsSync(player.userId, skill.Id, skill.Price, function(player)
            self:onBuySuccess(player, itemId)
        end, nil)
    end

    if skill.MoneyType == Define.MoneyType.DiamondGolds2 then
        PayHelper.payDiamondsSync(player.userId, skill.Id, skill.Price, function(player)
            self:onBuySuccess(player, itemId)
        end, nil)
    end

    ReportUtil.reportSkillBuy(player, itemId)
end
function GameTokenSkill:onBuySuccess(player, itemId)

    local data = {}
    for _, Value in pairs(player.tokenShopSkill) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    --for _, Value in pairs(player.tokenShopSkill) do
    --    local skill = TokenSkillConfig:getSkillById(Value.id)
    --    --print(json.encode(skill))
    --    if skill and skill.Unlock == itemId then
    --        Value.status = Define.TokenItemBuyStatus.Unlock
    --        table.insert(data, {
    --            Id = Value.id,
    --            Status = Value.status
    --        })
    --    end
    --end

    TokenShopManager:senderTokenSkillInfo(player, data)
    GameTokenSkill:shareDataToSkill(player, data)
end

function GameTokenSkill:onSkillUsed(player, itemId)

    local flag = false
    --table.sort(player.equipmentSkill,function (a,b)
    --    position = j
    --end)
    --for _, Value in pairs(player.equipmentSkill) do
    --    if Value.id == 0 and flag == false then
    --        Value.id = itemId
    --        flag = true
    --        GamePacketSender:SendEquipSkill(player.rakssid, Value.id, Value.position, false)
    --        break
    --    end
    --end

    for i = 1, 3 do
        local value = player.equipmentSkill[tostring(i)]
        if value.id == 0 and flag == false then
            value.id = itemId
            flag = true
            GamePacketSender:SendEquipSkill(player.rakssid, value.id, value.position, false)
            break
        end
    end

    if flag == false then
        HostApi.sendCommonTip(player.rakssid, "skill_enough")
        return
    end

    local data = {}
    for _, Value in pairs(player.tokenShopSkill) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Used
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end

    TokenShopManager:senderTokenSkillInfo(player, data)
    GameTokenSkill:shareDataToSkill(player, data)
    ReportUtil.reportSkillUsed(player, itemId)
end

function GameTokenSkill:onSKillUnload(player, itemId)
    for _, Value in pairs(player.equipmentSkill) do
        if Value.id == itemId  then
            Value.id = 0
            GamePacketSender:SendEquipSkill(player.rakssid, Value.id, Value.position, true)
            break
        end
    end


    local data = {}
    for _, Value in pairs(player.tokenShopSkill) do
        if Value.id == itemId then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    TokenShopManager:senderTokenSkillInfo(player, data)
    GameTokenSkill:shareDataToSkill(player, data)
end

function GameTokenSkill:initSkillItem(player)

    if #player.tokenShopSkill ~= 0 then
        self:updateSkill(player)
        return
    end

    table.sort(TokenSkillConfig.Skills, function(a, b)
        return a.Id < b.Id
    end)
    for _, Value in pairs(TokenSkillConfig.Skills) do
        table.insert(player.tokenShopSkill, {
            id = Value.Id,
            status = Define.TokenItemBuyStatus.Lock
        })
    end

    for j = 1, 3 do
        player.equipmentSkill[tostring(j)] = {
            id = 0,
            position = j
        }
    end
   -- print(json.encode(player.equipmentSkill))
end

function GameTokenSkill:updateSkill(player)

    if not player then
        return
    end

    for _, setting in pairs(TokenSkillConfig.Skills) do

        local find = false
        for _, Value in pairs(player.tokenShopSkill) do
            if  Value.id == setting.Id then
                find = true
                break
            end
        end

        if find == false then
            if player.property.advancedLevel >= setting.advancedId then
                if setting.isHave == Define.DefaultHave.Have then
                    table.insert(player.tokenShopSkill, {
                        id = setting.Id,
                        status = Define.TokenItemBuyStatus.Buy
                    })
                else
                    table.insert(player.tokenShopSkill, {
                        id = setting.Id,
                        status = Define.TokenItemBuyStatus.Unlock
                    })
                end
            else
                table.insert(player.tokenShopSkill, {
                    id = setting.Id,
                    status = Define.TokenItemBuyStatus.Lock
                })
            end
        end
    end
end

function GameTokenSkill:initAdvancedSkillItem(player, advanceId)

    local data = {}
    for _, Value in pairs(player.tokenShopSkill) do
        local config = TokenSkillConfig:getSkillById(Value.id)
        --print(json.encode(config))
        --print(json.encode(Value))
        if config and Value.status == Define.TokenItemBuyStatus.Lock and config.advancedId == advanceId
                and config.isHave == Define.DefaultHave.NotHave then
            Value.status = Define.TokenItemBuyStatus.Unlock
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end
        if config and Value.status == Define.TokenItemBuyStatus.Lock and config.advancedId == advanceId
                and config.isHave == Define.DefaultHave.Have then
            Value.status = Define.TokenItemBuyStatus.Buy
            table.insert(data, {
                Id = Value.id,
                Status = Value.status
            })
        end

    end
    --print(json.encode(data))
    TokenShopManager:senderTokenSkillInfo(player, data)
    GameTokenSkill:shareDataToSkill(player, data)
    --


end

function GameTokenSkill:shareDataToSkill(player, data)
    local info = {}
    for _, value in pairs(data) do
        for k, v in pairs(player.shopSkill) do
            if value.Id == v.id then
                v.status = value.Status
                table.insert(info, {
                    Id = value.Id,
                    Status = value.Status
                })
                break
            end
        end
    end
    ShopManager:senderSkillInfo(player, info)
end

return GameTokenSkill