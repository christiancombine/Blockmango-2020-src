GeneConfig = {}

GeneConfig.settings = {}

function GeneConfig:init(config)
    --print("GeneConfig:init(config)")
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
        table.insert(GeneConfig.settings, data)
    end

    table.sort(GeneConfig.settings, function(a, b)
        return a.sortId < b.sortId
    end)

    --LogUtil.log("GeneConfig:init ")
    --for _, vConfig in pairs(GeneConfig.settings) do
    --    LogUtil.log(vConfig.id)
    --end
end

function GeneConfig:getGeneMuscleMass(id)
    for _, v in pairs(GeneConfig.settings) do
        if v.id == id then
            return v.geneMuscleMass
        end
    end

    for _, v in pairs(TokenGeneConfig.settings) do
        if v.id == id then
            return v.geneMuscleMass
        end
    end

    return 0.0
end

function GeneConfig:getGeneById(id)
    for _, v in pairs(GeneConfig.settings) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function GeneConfig:getFirstGeneId()
    return GeneConfig.settings[1].id
end

function GeneConfig:getValidGeneId(id)
    for _, v in pairs(GeneConfig.settings) do
        if v.id == id then
            return id
        end
    end

    for _, v in pairs(TokenGeneConfig.settings) do
        if v.id == id then
            return id
        end
    end
    return GeneConfig:getFirstGeneId()
end

function GeneConfig:getNextSortGeneId(id)
    local key = #(GeneConfig.settings)
    for k, Value in pairs(GeneConfig.settings) do
        if Value.id == id then
            key  = k
            break
        end
    end

    if GeneConfig.settings[key+1] then
        return GeneConfig.settings[key+1]
    else
        return GeneConfig.settings[1]
    end

end

function GeneConfig:getNextNeedSelectById(id)
    local key = 1

    for k, Value in ipairs(GeneConfig.settings) do
        if Value.id == id then
            if Value.moneyType == Define.MoneyType.DiamondGolds1 or Value.moneyType == Define.MoneyType.DiamondGolds2 then
                return Value
            end

            key = k
            break
        end
    end

    for k, Value in ipairs(GeneConfig.settings) do
        if k > key and Value.moneyType ~= Define.MoneyType.DiamondGolds1 and Value.moneyType ~= Define.MoneyType.DiamondGolds2 then
            key = k
            break
        end
    end

    if GeneConfig.settings[key] then
        return GeneConfig.settings[key]
    end

    return GeneConfig.settings[1]
end

return GeneConfig
