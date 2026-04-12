
TokenShopManager = {}

function TokenShopManager:onButtonClick(player, data)
    local builder = DataBuilder.new():fromData(data)
    local kind = builder:getNumberParam("Kind")
    local itemId = builder:getNumberParam("ItemId")
    if kind == Define.TokenShopKind.FitnessEquip then
        GameTokenFitnessEquip:onButtonClick(player, itemId)
    end

    if kind == Define.TokenShopKind.Gene then
        GameTokenGene:onButtonClick(player, itemId)
    end

    if kind == Define.TokenShopKind.Skin then
        GameTokenSkin:onButtonClick(player, itemId)
    end

    if kind == Define.TokenShopKind.Skill then
        GameTokenSkill:onButtonClick(player, itemId)
    end
end

function TokenShopManager:initItem(player)
    -- GameAdvanced:initAdvancedItem(player)
    GameTokenFitnessEquip:initFitnessEquipItem(player)
    GameTokenGene:initGeneItem(player)
    GameTokenSkill:initSkillItem(player)
end

function TokenShopManager:senderAllItemInfo(player)
    local data1 = {}
    for _, Value in pairs(player.tokenShopFitnessEquip) do
        if Value.status ~= Define.TokenItemBuyStatus.Lock then
            table.insert(data1, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    self:senderTokenFitnessEquipInfo(player, data1)

    local data2 = {}
    for _, Value in pairs(player.tokenShopGene) do
        if Value.status ~= Define.TokenItemBuyStatus.Lock then
            table.insert(data2, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    self:senderTokenGeneInfo(player, data2)

    -- local data3 = {}
    -- for _, Value in pairs(player.shopAdvanced) do
    --     if Value.status ~= Define.TokenItemBuyStatus.Lock then
    --         table.insert(data3, {
    --             Id = Value.id,
    --             Status = Value.status
    --         })
    --     end
    -- end
    -- self:senderTokenSkillInfo(player, data3)

    local data4 = {}
    for _, Value in pairs(player.tokenShopSkill) do
        if Value.status ~= Define.TokenItemBuyStatus.Lock then
            table.insert(data4, {
                Id = Value.id,
                Status = Value.status
            })
        end
    end
    self:senderTokenSkillInfo(player, data4)

    --
    for _, Value in pairs(player.equipmentSkill) do
        if Value.id ~= 0 then
            GamePacketSender:SendEquipSkill(player.rakssid, Value.id, Value.position, false)
        end
    end

end

function TokenShopManager:senderTokenFitnessEquipInfo(player, data)

    GamePacketSender:SendTokenShopFitnessEquip(player.rakssid, json.encode(data))
end

function TokenShopManager:senderTokenGeneInfo(player, data)
    GamePacketSender:SendTokenShopGene(player.rakssid, json.encode(data))
end

-- function TokenShopManager:senderTokenAdvancedInfo(player, data)

--     GamePacketSender:SendTokenShopAdvanced(player.rakssid, json.encode(data))
-- end

function TokenShopManager:senderTokenSkillInfo(player, data)
    GamePacketSender:SendTokenShopSkill(player.rakssid, json.encode(data))
end

return TokenShopManager