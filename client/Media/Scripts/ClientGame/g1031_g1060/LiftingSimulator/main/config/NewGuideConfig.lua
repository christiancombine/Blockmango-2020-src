local Guides = MemoryPool.newTable()

NewGuideConfig = {}

function NewGuideConfig:loadConfig()

    local temp = FileUtil.getConfigFromCsv("setting/NewGuide.csv", 2)

    for _, Value in pairs(temp) do
        local data = 
        {
            Id = tonumber(Value.n_id) or 1,
            Desc = Value.s_desc or "",
            Img = Value.s_img or "",
            Time = tonumber(Value.n_time) or 1,
            ShowArea = Value.s_showArea or "",
            LucencyArea = Value.s_lucencyArea or "",
            LucencyArea2 = Value.s_lucencyArea2 or "",
            ArrowsArea = Value.s_arrowsArea or "",
            ArrowsImg = Value.s_arrowsImg or "",
            IsShowLucencyArea2 = tonumber(Value.n_isShowLucencyArea2) or 0 ,
            ArrowsArea2 = Value.s_arrowsArea2 or "",
            ArrowsImg2 = Value.s_arrowsImg2 or "",
        }

        table.insert(Guides, data)
    end
end

function NewGuideConfig:getGuides()
    return Guides
end

function NewGuideConfig:getGuideById(Id)
    for _, Value in pairs(Guides) do
        if Value.Id == Id then
            return Value
        end
    end

    return nil
end

function NewGuideConfig:getAreaByString(string)
    local area = StringUtil.split(string, "#")
    if #area < 4 then
        return nil
    end
    local data = {}
    data.posX = tonumber(area[1])
    data.posY = tonumber(area[2])
    data.areaX = tonumber(area[3])
    data.areaY = tonumber(area[4])
    return data
end

function NewGuideConfig:getLucencyAreaByString(string)
    local area = StringUtil.split(string, "#")
    if #area < 8 then
        return nil
    end

    local data = {}
    data.pos_x_1 = tonumber(area[1])
    data.pos_x_2 = tonumber(area[2])
    data.pos_y_1 = tonumber(area[3])
    data.pos_y_2 = tonumber(area[4])
    data.size_x_1 = tonumber(area[5])
    data.size_x_2 = tonumber(area[6])
    data.size_y_1 = tonumber(area[7])
    data.size_y_2 = tonumber(area[8])
    return data
end