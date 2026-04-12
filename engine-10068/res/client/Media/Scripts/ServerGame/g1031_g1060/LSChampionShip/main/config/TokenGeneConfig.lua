TokenGeneConfig = {}

TokenGeneConfig.settings = {}

function TokenGeneConfig:init(config)
    --print("TokenGeneConfig:init(config)")
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.name = vConfig.s_name or "" --名称
        data.desc = vConfig.s_desc or "" --描述
        data.icon = vConfig.s_icon or "" --图标
        data.geneMuscleMass = tonumber(vConfig.n_geneMuscleMass) or 0 --肌肉量上限
        data.moneyType = tonumber(vConfig.n_moneyType) or 0
        data.price = tonumber(vConfig.n_price) or 0
        data.unlock =  tonumber(vConfig.n_unlock) or 0 --解锁条件
        data.sortId = tonumber(vConfig.n_sortId) or 0
        data.advancedId = tonumber(vConfig.n_advancedId) or 0
        table.insert(TokenGeneConfig.settings, data)
    end

    table.sort(TokenGeneConfig.settings, function(a, b)
        return a.sortId < b.sortId
    end)
end

function TokenGeneConfig:getGeneMuscleMass(id)
    for _, v in pairs(TokenGeneConfig.settings) do
        if v.id == id then
            return v.geneMuscleMass
        end
    end

    return 0.0
end

function TokenGeneConfig:getGeneById(id)
    for _, v in pairs(TokenGeneConfig.settings) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function TokenGeneConfig:getFirstGeneId()
    return TokenGeneConfig.settings[1].id
end

function TokenGeneConfig:getValidGeneId(id)
    for _, v in pairs(TokenGeneConfig.settings) do
        if v.id == id then
            return id
        end
    end

    return TokenGeneConfig:getFirstGeneId()
end

function TokenGeneConfig:getNextSortGeneId(id)
    local key = #(TokenGeneConfig.settings)
    for k, Value in pairs(TokenGeneConfig.settings) do
        if Value.id == id then
            key  = k
            break
        end
    end

    if TokenGeneConfig.settings[key+1] then
        return TokenGeneConfig.settings[key+1]
    else
        return TokenGeneConfig.settings[1]
    end

end

function TokenGeneConfig:getNextNeedSelectById(id)
    local key = 1

    for k, Value in ipairs(TokenGeneConfig.settings) do
        if Value.id == id then
            if Value.moneyType == Define.MoneyType.DiamondGolds1 or Value.moneyType == Define.MoneyType.DiamondGolds2 then
                return Value
            end

            key = k
            break
        end
    end

    for k, Value in ipairs(TokenGeneConfig.settings) do
        if k > key and Value.moneyType ~= Define.MoneyType.DiamondGolds1 and Value.moneyType ~= Define.MoneyType.DiamondGolds2 then
            key = k
            break
        end
    end

    if TokenGeneConfig.settings[key] then
        return TokenGeneConfig.settings[key]
    end

    return TokenGeneConfig.settings[1]
end

return TokenGeneConfig
