AdvancedConfig = {}

AdvancedConfig.settings = {}

function AdvancedConfig:init(config)
    --print("AdvancedConfig:init(config)")
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.gradeId = tonumber(vConfig.n_gradeId) or 0
        data.name = vConfig.s_name or "" --名称
        data.desc = vConfig.s_desc or "" --描述
        data.icon = vConfig.s_icon or "" --图标
        data.speed = tonumber(vConfig.n_speed) or 0 --移动速度系数
        data.attack = tonumber(vConfig.n_attack) or 0 --攻击系数
        data.workout = tonumber(vConfig.n_workout) or 0 --锻炼系数
        data.hpCoe = tonumber(vConfig.n_hpCoe) or 0
        data.moneyType = tonumber(vConfig.n_moneyType) or 0 --价格类型
        data.price = tonumber(vConfig.n_price) or 0 --价格
        data.unlock =  tonumber(vConfig.n_unlock) or 0 --解锁条件
        data.speedEffect = vConfig.s_speedEffect or "" --速度特效
        data.powerEffect = vConfig.s_powerEffect or "" --力量特效
        data.killEffect = vConfig.s_killEffect or "" --击杀特效
        data.level =  tonumber(vConfig.n_level) or 0 --阶数等级
        table.insert(AdvancedConfig.settings, data)
    end

    table.sort(AdvancedConfig.settings, function(a, b)
            return a.id < b.id
    end)
end

function AdvancedConfig:getAdvancedConfig(id)
    for _, setting in pairs(AdvancedConfig.settings) do
        if setting.id == id then
            return setting
        end
    end
    return nil
end

--获得击杀特效
function AdvancedConfig:getKillEffect(id)
    local config = AdvancedConfig:getAdvancedConfig(id)
    if not config then
        return nil
    end
    return tonumber(config.killEffect)
end

return AdvancedConfig
