FunctionalAreaConfig = {}

FunctionalAreaConfig.settings = {}

function FunctionalAreaConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.AreaId = tonumber(vConfig.n_AreaId) or 0 --区域id
        data.AreaType = tonumber(vConfig.n_AreaType) or 0 --区域类型
        data.AreaRange = FunctionalAreaConfig:parseAreaPosition(vConfig.n_AreaRange) or "" --区域范围
        data.AreaLanguage = tostring(vConfig.n_AreaLanguage) or "" --区域多语言
        table.insert(self.settings, data)
    end
end

function FunctionalAreaConfig:parseAreaPosition(AreaRange)
    local Ranges = StringUtil.split(AreaRange, "##")
    local Regions = {}

    for k, Range in pairs(Ranges) do
        local Region = {}

        for i, Positions in pairs(StringUtil.split(Range, "&")) do
            local Pos = StringUtil.split(Positions, ",")

            local Point = {}
            Point.x = tonumber(Pos[1])
            Point.z = tonumber(Pos[2])

            Region[i] = Point
        end
        table.insert(Regions, k, Region)
    end

    return Regions
end

function FunctionalAreaConfig:getAllAreaInfo()
    return self.settings
end