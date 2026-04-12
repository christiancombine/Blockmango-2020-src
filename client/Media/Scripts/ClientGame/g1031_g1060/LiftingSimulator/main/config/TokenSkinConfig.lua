local TokenSkinItem = MemoryPool.newTable()

TokenSkinConfig = {}

function TokenSkinConfig:loadConfig()
    local playerSex = PlayerManager:getClientPlayer():getSex()
    local temp = FileUtil.getConfigFromCsv("setting/TokenSkin.csv", 2)
    for _, Value in pairs(temp) do
        local data = {
            Id = tonumber(Value.n_id),
            sortId = tonumber(Value.n_sortId) or 0,
            Name = Value.s_name,
            Desc = Value.s_desc,
            Icon = Value.s_icon,
            MoneyType = tonumber(Value.n_moneyType),
            Price = tonumber(Value.n_price),
            --Type = tonumber(Value.n_type),
            sex = tonumber(Value.n_sex) or 1,
            Status = TokenItemBuyStatus.Unlock,
        }
        if playerSex == data.sex then
            table.insert(TokenSkinItem,data)
        end
    end
end

function TokenSkinConfig:getSkins()
    return TokenSkinItem
end


function TokenSkinConfig:getSkinById(Id)
    for _, Value in pairs(TokenSkinItem) do
        if Value.Id == Id then
            return Value
        end
    end
    return nil
end