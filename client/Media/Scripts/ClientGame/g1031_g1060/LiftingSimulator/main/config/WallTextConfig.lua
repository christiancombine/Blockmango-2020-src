WallTextConfig = {}

WallTextConfig.settings = {}

function WallTextConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.type = tonumber(vConfig.n_type) or 0 --类型
        data.timeLeft = tonumber(vConfig.n_timeLeft) or 0
        data.pos = StringUtil.split(vConfig.s_pos or "", ",", false, true) --位置
        data.scale = tonumber(vConfig.n_scale) or 0 --缩放
        data.yaw = tonumber(vConfig.n_yaw) or 0 --yaw
        data.pitch = tonumber(vConfig.n_pitch) or 0 --pitch
        data.color = StringUtil.split(vConfig.s_color or "", ",", false, true) --color
        table.insert(WallTextConfig.settings, data)
    end

    table.sort(WallTextConfig.settings, function(a, b)
            return a.id < b.id
    end)
end

function WallTextConfig:getWallTextConfig(index)
    for _, setting in pairs(self.settings) do
        if setting.id == index then
            return setting
        end
    end
    return nil
end

return WallTextConfig
