ActiveSkillConfig = {}

local Config = {}

function ActiveSkillConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.SkillId = tonumber(vConfig.SkillId) or 0
        data.Name = tostring(vConfig.Name) or ""
        data.Desc = tostring(vConfig.Desc) or ""
        data.Type = tonumber(vConfig.Type) or 1
        data.Icon = tostring(vConfig.Icon) or ""
        data.TargetType = tonumber(vConfig.TargetType) or 0
        data.InitCd = tonumber(vConfig.InitCd)  or 0
        data.Cd = tonumber(vConfig.Cd) or 0
        data.GuideTime = tonumber(vConfig.GuideTime) or 0
        data.DelayTime = tonumber(vConfig.DelayTime) or 0
        data.Num = tonumber(vConfig.Num) or ""
        data.duration = tonumber(vConfig.duration) or -1
        data.AuraRange = tonumber(vConfig.AuraRange) or 0
        data.AuraEffect = tostring(vConfig.AuraEffect) or ""
        data.BuffId = tonumber(vConfig.BuffId) or 0
        Config[data.SkillId] = data
    end
end

function ActiveSkillConfig:getConfigById(id)
    return Config[id]
end

ActiveSkillConfig:init(FileUtil.getConfigFromCsv("setting/ActiveSkill.csv"))