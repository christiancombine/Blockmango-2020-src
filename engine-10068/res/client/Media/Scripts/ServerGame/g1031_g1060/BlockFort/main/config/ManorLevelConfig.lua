ManorLevelConfig = {}
ManorLevelConfig.manorLevel = {}

function ManorLevelConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.offsetStartX = tonumber(v.offsetStartX)
        data.offsetStartY = tonumber(v.offsetStartY)
        data.offsetStartZ = tonumber(v.offsetStartZ)
        data.offsetEndX = tonumber(v.offsetEndX)
        data.offsetEndY = tonumber(v.offsetEndY)
        data.offsetEndZ = tonumber(v.offsetEndZ)
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.repairBlockId = tonumber(v.repairBlockId)
        data.repairBlockMeta = tonumber(v.repairBlockMeta)
        data.repairBlockId2 = tonumber(v.repairBlockId2)
        data.repairBlockMeta2 = tonumber(v.repairBlockMeta2)
        self.manorLevel[data.level] = data
    end
end

function ManorLevelConfig:getOffsetPosByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getOffsetPosByLevel: manorLevel.csv can not find level =", level)
        return nil, nil
    end

    local manorLevel = self.manorLevel[tonumber(level)]

    local minX = math.min(manorLevel.offsetStartX, manorLevel.offsetEndX)
    local maxX = math.max(manorLevel.offsetStartX, manorLevel.offsetEndX)
    local minY = math.min(manorLevel.offsetStartY, manorLevel.offsetEndY)
    local maxY = math.max(manorLevel.offsetStartY, manorLevel.offsetEndY)
    local minZ = math.min(manorLevel.offsetStartZ, manorLevel.offsetEndZ)
    local maxZ = math.max(manorLevel.offsetStartZ, manorLevel.offsetEndZ)

    return VectorUtil.newVector3(minX, minY, minZ), VectorUtil.newVector3(maxX, maxY, maxZ)
end

function ManorLevelConfig:getRepairBlockIdByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockId
    else
        return nil
    end
end

function ManorLevelConfig:getRepairBlockMetaByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockMeta
    else
        return nil
    end
end

function ManorLevelConfig:getRepairBlockId2ByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockId2
    else
        return nil
    end
end

function ManorLevelConfig:getRepairBlockMeta2ByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockMeta2
    else
        return nil
    end
end

function ManorLevelConfig:getMoneyTypeByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getMoneyTypeByLevel: manorLevel.csv can not find level =", level)
        return 1
    end

    return self.manorLevel[tonumber(level)].moneyType
end

function ManorLevelConfig:getNextPriceByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getNextPriceByLevel: manorLevel.csv can not find level =", level)
        return 0
    end

    return self.manorLevel[tonumber(level)].price
end

function ManorLevelConfig:getMaxLevel()
    return #self.manorLevel
end

return ManorLevelConfig