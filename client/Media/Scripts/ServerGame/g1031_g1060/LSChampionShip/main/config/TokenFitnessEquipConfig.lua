TokenFitnessEquipConfig = {}

TokenFitnessEquipConfig.settings = {}

function TokenFitnessEquipConfig:init(config)

    --print("TokenFitnessEquipConfig:init(config)")
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.name = vConfig.s_name or "" --名称
        data.desc = vConfig.s_desc or "" --描述
        data.actor = vConfig.s_actor or "" --actor
        data.icon = vConfig.s_icon or "" --图标
        data.efficiency = tonumber(vConfig.n_efficiency) or 0 --锻炼效率
        data.moneyType = tonumber(vConfig.n_moneyType) or 0
        data.price = tonumber(vConfig.n_price) or 0
        data.unlock =  tonumber(vConfig.n_unlock) or 0 --解锁条件
        data.weight = tonumber(vConfig.n_weight) or 0 --组队基础肌肉增益
        data.payConfigId = tonumber(vConfig.n_payConfigId) or 0 --payConfigId
        data.sortId = tonumber(vConfig.n_sortId) or 0
        data.advancedId = tonumber(vConfig.n_advancedId) or 0
        table.insert(TokenFitnessEquipConfig.settings, data)
    end

    table.sort(TokenFitnessEquipConfig.settings, function(a, b)
        return a.sortId < b.sortId
    end)

    --print(inspect.inspect(TokenFitnessEquipConfig.settings))
    --print(json.encode(TokenFitnessEquipConfig.settings))
end

function TokenFitnessEquipConfig:getEfficiency(id)
    for _, setting in pairs(TokenFitnessEquipConfig.settings) do
        if setting.id == id then
            return setting.efficiency
        end
    end

    return 1.0
end

function TokenFitnessEquipConfig:getFirstFitnessEquipId()
    --print("TokenFitnessEquipConfig:getFirstFitnessEquipId " .. TokenFitnessEquipConfig.settings[1].id)
    return TokenFitnessEquipConfig.settings[1].id
end

function TokenFitnessEquipConfig:isValidId(id)
    if id == nil then
        return false
    end

    for _, setting in pairs(TokenFitnessEquipConfig.settings) do
        if setting.id == id then
            return true
        end
    end

    return false
end

function TokenFitnessEquipConfig:getFitnessEquipConfig(id)
    for _, setting in pairs(TokenFitnessEquipConfig.settings) do
        if setting.id == id then
            return setting
        end
    end
    --print("TokenFitnessEquipConfig:getFitnessEquipConfig return nil!")
    return nil
end

function TokenFitnessEquipConfig:getFitnessEquipSortId(id)
    local config = TokenFitnessEquipConfig:getFitnessEquipConfig(id)
    if not config then
        return 0
    end
    return config.sortId
end

function TokenFitnessEquipConfig:getPayFitnessEquipConfig(id)
    local config = self:getFitnessEquipConfig(id)
    if not config then
        return nil
    end
    return PayFitnessEquipConfig:getPayFitnessEquipConfig(config.payConfigId)
end

function TokenFitnessEquipConfig:isPayFitnessEquip(id)
    local config = self:getFitnessEquipConfig(id)
    if not config then
        return false
    end
    return config.payConfigId ~= 0
end

return TokenFitnessEquipConfig
