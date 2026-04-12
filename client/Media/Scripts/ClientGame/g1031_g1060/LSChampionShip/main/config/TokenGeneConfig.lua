local TokenGenes = MemoryPool.newTable()

TokenGeneConfig = {}

function TokenGeneConfig:loadConfig()

    local temp = FileUtil.getConfigFromCsv("setting/TokenGene.csv", 2)

    for _, Value in pairs(temp) do
        table.insert(TokenGenes, {
            Id = tonumber(Value.n_id),
            Name = Value.s_name,
            Desc = Value.s_desc,
            Icon = Value.s_icon,
            MaxMuscle = tonumber(Value.n_geneMuscleMass),
            MoneyType = tonumber(Value.n_moneyType),
            Price = tonumber(Value.n_price),
            sortId = tonumber(Value.n_sortId) or 0,
            Status = ItemBuyStatus.Lock, --购买状态
            Effect = Value.s_effect or "#"
        })
    end

end

function TokenGeneConfig:getGenes()
    return TokenGenes
end

function TokenGeneConfig:getGeneById(Id)

    for _, Value in pairs(TokenGenes) do
        if Value.Id==Id then
            return Value
        end
    end
    return nil
end