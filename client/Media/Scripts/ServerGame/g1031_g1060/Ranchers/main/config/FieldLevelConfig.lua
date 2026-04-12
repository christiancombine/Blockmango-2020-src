FieldLevelConfig = {}
FieldLevelConfig.field = {}

function FieldLevelConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.fieldLevel = tonumber(v.fieldLevel)
        data.playerLevel = tonumber(v.playerLevel)
        data.maxFieldNum = tonumber(v.maxFieldNum)
        data.money = tonumber(v.money)
        self.field[data.fieldLevel] = data
    end
end

function FieldLevelConfig:getFieldInfoByFiledLevel(fieldLevel)
    if self.field[fieldLevel] ~= nil then
        return self.field[fieldLevel]
    end

    return nil
end

function FieldLevelConfig:getMaxFieldLevel()
    return #self.field
end

function FieldLevelConfig:getMaxFieldNumByFieldLevel(fieldLevel)
    if self.field[fieldLevel] == nil then
        return self.field[#self.field].maxFieldNum
    end

    return self.field[fieldLevel].maxFieldNum
end

function FieldLevelConfig:getFieldLevelByCurFieldNum(curFieldNum)
    for i, v in pairs(self.field) do
        if v.maxFieldNum > curFieldNum then
            -- 这里获取的是下一个田地等级(例如 1级田地数上限是5个，当前拥有5个，那么返回的是下一级:2级)
            return v.fieldLevel
        end
    end

    return self:getMaxFieldLevel()
end

function FieldLevelConfig:getMoneyByFieldLevel(fieldLevel)
    if self.field[fieldLevel] == nil then
        return 0
    end

    return self.field[fieldLevel].money
end

function FieldLevelConfig:getFieldLevelByPlayerLevel(playerLevel)
    for i, v in pairs(self.field) do
        if v.playerLevel > playerLevel then
            -- 这里获取的田地等级和上面的 FieldLevelConfig:getFieldLevelByCurFieldNum(curFieldNum) 不一样,这个需要 -1
            return v.fieldLevel - 1
        end
    end

    return self:getMaxFieldLevel()
end

function FieldLevelConfig:getMaxFieldNumByPlayerLevel(playerLevel)
    local fieldLevel = self:getFieldLevelByPlayerLevel(playerLevel)
    return self:getMaxFieldNumByFieldLevel(fieldLevel)
end

return FieldLevelConfig