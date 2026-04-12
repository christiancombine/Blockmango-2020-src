local  TokenSkills= MemoryPool.newTable()

TokenSkillConfig = {}

function TokenSkillConfig:loadConfig()

    local temp = FileUtil.getConfigFromCsv("setting/TokenSkills.csv", 2)

    for _, Value in pairs(temp) do
        table.insert(TokenSkills,{
            Id = tonumber(Value.n_id),
            Name = Value.s_name,
            Desc = Value.s_desc,
            Icon = Value.s_icon,
            Hurt  = tonumber(Value.n_hurt),
            sortId = tonumber(Value.n_sortId) or 0,
            Cd = tonumber(Value.n_cd),
            MoneyType = tonumber(Value.n_moneyType),
            Price = tonumber(Value.n_price),
            skillType = tonumber(Value.n_skillType) or 0, --技能类型
            Status = ItemBuyStatus.Lock, --购买状态
            holdItemId = tonumber(Value.n_holdItemId) or 0,
            skillTime = tonumber(Value.n_skillTime) or 0,
            delayTime = tonumber(Value.n_delayTime) or 0,
        })
    end

end

function TokenSkillConfig:getSkills()
    return TokenSkills
end

function TokenSkillConfig:getSkillById(Id)

    for _, Value in pairs(TokenSkills) do
        if Value.Id==Id then
            return Value
        end
    end
    return nil
end