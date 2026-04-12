EffectConfig = {}

EffectConfig.settings = MemoryPool.newTable()

function EffectConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.name = vConfig.s_name or "" --特效名字
        data.effectPos = StringUtil.split(vConfig.s_effectPos or "", ",") --特效位置
        data.effectYaw = tonumber(vConfig.n_effectYaw) or 0 --特效朝向
        data.liveTime = tonumber(vConfig.n_liveTime) or 0 --特效存在时间
        data.scale = tonumber(vConfig.n_scale) or 0 --特效缩放
        data.type = tonumber(vConfig.n_type) or 0
        data.attach = vConfig.s_attach or ""
        data.attachPos = StringUtil.split(vConfig.s_attachPos, ",")
        data.attachAngle = StringUtil.split(vConfig.s_attachAngle, ",")
        data.attachScale = StringUtil.split(vConfig.s_attachScale, ",")
        table.insert(EffectConfig.settings, data)
    end

    --print("EffectConfig:init " .. inspect.inspect(self.settings))
end

function EffectConfig:getEffectConfig(id)
    for _, v in pairs(self.settings) do
        if v.id == id then
            return v
        end
    end

    --print("EffectConfig:getEffectConfig " .. id)

    return nil
end

return EffectConfig
