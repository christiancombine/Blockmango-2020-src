
local function Split(input, delimiter)
    input = tostring(input)
    delimiter = tostring(delimiter)
    if (delimiter == '') then return false end
    local pos, arr = 0, {}
    for st, sp in function() return string.find(input, delimiter, pos, true) end do
        table.insert(arr, string.sub(input, pos, st - 1))
        pos = sp + 1
    end
    table.insert(arr, string.sub(input, pos))
    return arr
end

PersonResourceConfig = {}
PersonResourceConfig.ConfigInfo = {}
function PersonResourceConfig:init(configs)
    for count, info in pairs(configs) do
        local args = {}
        for key, val in pairs(info) do
            local keyTab = Split(key, ',')
            local valTab = Split(val, ',')
            for index = 1, #keyTab do
                args[keyTab[index]] = tonumber(valTab[index])
            end
        end
        table.insert(self.ConfigInfo, args)
    end
end

function PersonResourceConfig:getSpeedAndCeiling(count)
    for _, val in pairs(self.ConfigInfo) do
        if val.Count == count then
            return val.Speed, val.Ceiling
        end
    end
end

return PersonResourceConfig