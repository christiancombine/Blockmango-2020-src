PayFitnessEquipConfig = {}

PayFitnessEquipConfig.settings = {}

function PayFitnessEquipConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.unlockAdvancedLevel = tonumber(vConfig.n_unlockAdvancedLevel) or 0 --解锁需要的进阶等级
        data.invailedAdvancedLevel = tonumber(vConfig.n_invailedAdvancedLevel) or 0
        data.efficiencyPercentage = tonumber(vConfig.n_efficiencyPercentage) or 0 --锻炼肌肉量百分比
        data.efficiencyFix = tonumber(vConfig.n_efficiencyFix) or 0 --锻炼肌肉量固定值
        data.efficiencyFixHuge = tonumber(vConfig.n_efficiencyFixHuge) or 0 --无限肌肉锻炼肌肉量固定值
        table.insert(PayFitnessEquipConfig.settings, data)
    end
end

function PayFitnessEquipConfig:getPayFitnessEquipConfig(id)
    for _, setting in pairs(self.settings) do
        if setting.id == id then
            return setting
        end
    end
    return nil
end

return PayFitnessEquipConfig
