TowerConfig = {}

local Config = {}
local StarList = {}
local ShowSeq = {}
local FreeList = {}

function TowerConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.Name = tostring(vConfig.Name) or "tower"
        data.isFree = tonumber(vConfig.isFree) or "0"
        data.CoinId = tonumber(vConfig.CoinId) or 4
        data.BuildPrice = tonumber(vConfig.BuildPrice) or 10
        data.SellPrice = tonumber(vConfig.SellPrice) or 10
        data.StarLevel = tonumber(vConfig.StarLevel) or 1
        data.ClassId = tonumber(vConfig.ClassId) or 0
        data.ActorName = tostring(vConfig.ActorName) or ""
        data.IconName = tostring(vConfig.IconName) or ""
        data.UnKnowIconName = tostring(vConfig.UnKnowIconName) or ""
        data.skillIds = StringUtil.split(vConfig.SkillIds or "0", "#")
        data.AttackRangeEffect = tostring(vConfig.AttackRangeEffect) or ""
        data.AttackType = tonumber(vConfig.AttackType) or 1
        data.AttackTypeName = tostring(vConfig.AttackTypeName) or ""
        data.AttackTypeInfo = tostring(vConfig.AttackTypeInfo) or ""
        data.Attack = tonumber(vConfig.Attack) or 0
        data.AttackSpeed = tonumber(vConfig.AttackSpeed) * 1000 or 1
        data.AttackSpeedA = tonumber(vConfig.AttackSpeedA) or 1
        data.AttackSpeedB = tonumber(vConfig.AttackSpeedB) or 1
        data.MinAttackSpeed = tonumber(vConfig.MinAttackSpeed) * 1000 or 0
        data.AttackTime = tonumber(vConfig.AttackTime) * 1000 or 0
        data.bulletYawY = tonumber(vConfig.bulletYawY) or 0
        data.AttackRange = tonumber(vConfig.AttackRange) or 0
        data.AttackRangeLang = tostring(vConfig.AttackRangeLang) or ""
        data.BulletId = tonumber(vConfig.BulletId) or 0
        data.scatterCount = tonumber(vConfig.scatterCount) or 0
        data.ejectionsCount = tonumber(vConfig.ejectionsCount) or 0
        data.FormulaImage1 = tostring(vConfig.FormulaImage1) or "#"
        data.FormulaImage2 = tostring(vConfig.FormulaImage2) or "#"
        data.FormulaImage3 = tostring(vConfig.FormulaImage3) or "#"
        data.ShowSeq = tostring(vConfig.showSeq) or 0
        Config[data.Id] = data
        StarList[tostring(data.StarLevel)] = StarList[tostring(data.StarLevel)] or {}
        ShowSeq[tonumber(data.ShowSeq)] = data
        table.insert(StarList[tostring(data.StarLevel)], data)
        if data.isFree == 1 then
            FreeList[data.Id] = data
        end
    end
end

function TowerConfig:getConfigById(id)
    return Config[id]
end

function TowerConfig:getAllConfig()
    return Config
end

function TowerConfig:getShowSeq()
    return ShowSeq
end

function TowerConfig:getFreeTowerList()
    return FreeList
end

TowerConfig:init(FileUtil.getConfigFromCsv("setting/Tower.csv"))