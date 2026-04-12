FitnessEquipConfig = {}

FitnessEquipConfig.settings = {}

function FitnessEquipConfig:init(config)

    --print("FitnessEquipConfig:init(config)")
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
        table.insert(FitnessEquipConfig.settings, data)
    end

    table.sort(FitnessEquipConfig.settings, function(a, b)
        return a.sortId < b.sortId
    end)

    --print(inspect.inspect(FitnessEquipConfig.settings))
    --print(json.encode(FitnessEquipConfig.settings))
end

function FitnessEquipConfig:getEfficiency(id)
    for _, setting in pairs(FitnessEquipConfig.settings) do
        if setting.id == id then
            return setting.efficiency
        end
    end

    for _, setting in pairs(TokenFitnessEquipConfig.settings) do
        if setting.id == id then
            return setting.efficiency
        end
    end
    
    return 1.0
end

function FitnessEquipConfig:getFirstFitnessEquipId()
    --print("FitnessEquipConfig:getFirstFitnessEquipId " .. FitnessEquipConfig.settings[1].id)
    return FitnessEquipConfig.settings[1].id
end

function FitnessEquipConfig:isValidId(id)
    if id == nil then
        return false
    end

    for _, setting in pairs(FitnessEquipConfig.settings) do
        if setting.id == id then
            return true
        end
    end

    return false
end

function FitnessEquipConfig:getFitnessEquipConfig(id)
    for _, setting in pairs(FitnessEquipConfig.settings) do
        if setting.id == id then
            return setting
        end
    end
    --print("FitnessEquipConfig:getFitnessEquipConfig return nil!")
    return nil
end

function FitnessEquipConfig:getFitnessEquipSortId(id)
    local config = FitnessEquipConfig:getFitnessEquipConfig(id)
    if not config then
        return 0
    end
    return config.sortId
end

function FitnessEquipConfig:getPayFitnessEquipConfig(id)
    local config = self:getFitnessEquipConfig(id)
    if not config then
        return nil
    end
    return PayFitnessEquipConfig:getPayFitnessEquipConfig(config.payConfigId)
end

function FitnessEquipConfig:isPayFitnessEquip(id)
    local config = self:getFitnessEquipConfig(id)
    if not config then
        return false
    end
    return config.payConfigId ~= 0
end

function FitnessEquipConfig:getNextSortFitnessEquipId(id)
    local key = #(FitnessEquipConfig.settings)
    for k, Value in pairs(FitnessEquipConfig.settings) do
        if Value.id == id then
            key  = k
            break
        end
    end

    if FitnessEquipConfig.settings[key+1] then
        return FitnessEquipConfig.settings[key+1]
    else
        return FitnessEquipConfig.settings[1]
    end

end

function FitnessEquipConfig:getNextNeedSelectById(id)
    local key = 1
    for k, Value in ipairs(FitnessEquipConfig.settings) do
        if Value.id == id then
            if Value.moneyType == Define.MoneyType.DiamondGolds1 or Value.moneyType == Define.MoneyType.DiamondGolds2 then
                return Value
            end

            key = k
            break
        end
    end


    for k, Value in ipairs(FitnessEquipConfig.settings) do
        if k > key and Value.moneyType ~= Define.MoneyType.DiamondGolds1 and Value.moneyType ~= Define.MoneyType.DiamondGolds2 then
            key = k
            break
        end
    end

    if FitnessEquipConfig.settings[key] then
        return FitnessEquipConfig.settings[key]
    end

    return FitnessEquipConfig.settings[1]
end

return FitnessEquipConfig
