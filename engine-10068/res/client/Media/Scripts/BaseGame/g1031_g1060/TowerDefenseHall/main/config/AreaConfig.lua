AreaConfig = {}

AreaConfig.Config = {}
AreaConfig.Type = {
    Tower = 1,
    Team = 2,
    Skin = 3,
    Level = 4
}

function AreaConfig:init(Areas)
    for _, area in pairs(Areas) do
        local item = {}
        item.Id = tonumber(area.Id) or 0
        item.type = tonumber(area.type) or 0
        item.configId = tonumber(area.configId) or 0
        item.actor = tostring(area.actor) or ""
        item.yaw = tonumber(area.yaw) or 0
        item.Area = self:getArea(area.areaRange)
        item.areaEffect = tostring(area.areaEffect)
        item.scale = StringUtil.split(area.scale, ",")
        item.pYaw = tonumber(area.pYaw) or 0
        item.scale1 = tonumber(area.scale1) or 0
        item.scale2 = tonumber(area.scale2) or 0
        item.scale3 = tonumber(area.scale3) or 0
        item.scale4 = tonumber(area.scale4) or 0
        item.centrePos, item.pos1, item.pos2, item.pos3, item.pos4 = self:getPoint(StringUtil.split(area.centrePos or "", ","),
                StringUtil.split(area.pos1 or "", ","),
                StringUtil.split(area.pos2 or "", ","),
                StringUtil.split(area.pos3 or "", ","),
                StringUtil.split(area.pos4 or "", ","))
        self.Config[item.Id] = item
    end
    AreaManager:initArea(self.Config)
end

function AreaConfig:getArea(areaRange)
    local area = IArea.new()
    local spawnPosStr = StringUtil.split(areaRange, "#")
    if #spawnPosStr ~= 2 then
        area:init(0, 0, 0, 0, 0, 0)
        return area
    end
    local pos1 = StringUtil.split(spawnPosStr[1], ",")
    local pos2 = StringUtil.split(spawnPosStr[2], ",")
    area:init(tonumber(pos1[1]), tonumber(pos2[1]), tonumber(pos1[2]), tonumber(pos2[2]), tonumber(pos1[3]), tonumber(pos2[3]))
    return area
end

function AreaConfig:getPoint(centrePos, pos1, pos2, pos3, pos4)
    --设置特效坐标默认值
    local centrePoint = VectorUtil.newVector3(0, 0, 0)
    if #centrePos == 3 then
        centrePoint = VectorUtil.newVector3(centrePos[1], centrePos[2], centrePos[3])
    end
    --设置特性坐标默认值
    local point1 = centrePoint
    local point2 = centrePoint
    local point3 = centrePoint
    local point4 = centrePoint
    if #pos1 == 3 then
        point1 = VectorUtil.newVector3(pos1[1], pos1[2], pos1[3])
    end
    if #pos2 == 3 then
        point2 = VectorUtil.newVector3(pos2[1], pos2[2], pos2[3])
    end
    if #pos3 == 3 then
        point3 = VectorUtil.newVector3(pos3[1], pos3[2], pos3[3])
    end
    if #pos4 == 3 then
        point4 = VectorUtil.newVector3(pos4[1], pos4[2], pos4[3])
    end
    return centrePoint, point1, point2, point3, point4
end

function AreaConfig:getConfigById(id)
    return self.Config[id]
end

AreaConfig:init(FileUtil.getConfigFromCsv("setting/Area.csv"))