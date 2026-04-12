TokenSkinConfig = {}

TokenSkinConfig.settings = {}

function TokenSkinConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.type = tonumber(vConfig.n_type) or 0 --道具类型
        data.price = tonumber(vConfig.n_price) or 0 --价格
        data.name = vConfig.s_name or "" --道具名称
        data.desc = vConfig.s_desc or "" --描述
        data.moneyType = tonumber(vConfig.n_moneyType) or 0 --价格类型
        data.iocn = vConfig.s_iocn or "" --图片
        data.actor = vConfig.s_actor or "" --模型
        data.sex = tonumber(vConfig.n_sex) or 1 --性别
        --data.default = tonumber(vConfig.n_default) or 0 --是否是默认皮肤
        -- data.advancedId = tonumber(vConfig.n_advancedId) or 0
        table.insert(TokenSkinConfig.settings, data)
    end

    table.sort(self.settings, function(a, b)
        return a.id < b.id
    end)
    
end

function TokenSkinConfig:getSkinConfig(id)
    for _, setting in pairs(self.settings) do
        if setting.id == id then
            return setting
        end
    end
    return nil
end

function TokenSkinConfig:getDefaultSkin(player)
    if player and player:getSex() == 2 then
        return ClientConfig.girlFreeSkinId
    else
        return ClientConfig.boyFreeSkinId
    end
end

return TokenSkinConfig
