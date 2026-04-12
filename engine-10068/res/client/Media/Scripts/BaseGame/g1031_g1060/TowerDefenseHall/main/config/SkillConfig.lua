SkillConfig = {}

local Config = {}

function SkillConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.SkillId = tonumber(vConfig.SkillId) or 0
        data.Name = tostring(vConfig.Name) or ""
        data.Desc = tostring(vConfig.Desc) or ""
        data.Type = tonumber(vConfig.Type) or 1
        data.Icon = tostring(vConfig.Icon) or ""
        data.TargetType = tonumber(vConfig.TargetType) or 0
        data.AuraRange = tonumber(vConfig.AuraRange) or 0
        data.AuraEffect = tostring(vConfig.AuraEffect) or ""
        data.probability = tonumber(vConfig.probability) or 0
        data.triggerEffect = tostring(vConfig.triggerEffect) or ""
        data.result = tonumber(vConfig.result) or 0
        data.BuffId = tonumber(vConfig.BuffId) or 0
        Config[data.SkillId] = data
    end
end

function SkillConfig:getConfigById(id)
    return Config[id]
end

SkillConfig:init(FileUtil.getConfigFromCsv("setting/Skill.csv"))