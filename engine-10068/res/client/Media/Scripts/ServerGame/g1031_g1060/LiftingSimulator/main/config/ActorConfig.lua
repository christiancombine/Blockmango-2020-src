ActorConfig = {}

ActorConfig.settings = {}

function ActorConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.birthPos = vConfig.s_birthPos or "" --出生位置
        data.yaw = tonumber(vConfig.n_yaw) or 0 --朝向
        data.actor = vConfig.s_actor or "" --模型
        data.weight = vConfig.n_weight or "0" --重量
        data.scale = tonumber(vConfig.n_scale) or 1 --缩放
        data.textScale = tonumber(vConfig.n_textScale) or 1 --字体缩放
        data.hp = tonumber(vConfig.n_hp) or 1 --血量
        table.insert(ActorConfig.settings, data)
    end
end

return ActorConfig
