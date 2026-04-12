TokenFitnessEquipConfig = {}

TokenFitnessEquipConfig.Equips = MemoryPool.newTable()

function TokenFitnessEquipConfig:loadConfig()

    local temp = FileUtil.getConfigFromCsv("setting/TokenFitnessEquip.csv", 2)

    for _, Value in pairs(temp) do
        table.insert(self.Equips, {
            Id = tonumber(Value.n_id),
            Name = Value.s_name,
            Desc = Value.s_desc,
            Actor = Value.s_actor,
            Icon = Value.s_icon,
            Efficiency = tonumber(Value.n_efficiency),
            MoneyType = tonumber(Value.n_moneyType),
            Price = tonumber(Value.n_price),
            sortId = tonumber(Value.n_sortId) or 0,
            payConfigId = tonumber(Value.n_payConfigId) or 0,
            Status = ItemBuyStatus.Lock, --购买状态
            Effect = Value.s_effect or "#"
        })
    end
end

function TokenFitnessEquipConfig:getFitnessEquips()
    return self.Equips
end

function TokenFitnessEquipConfig:getFitnessEquipById(Id)
    for _, Value in pairs(self.Equips) do
        if Value.Id==Id then
            return Value
        end
    end
    return nil
end