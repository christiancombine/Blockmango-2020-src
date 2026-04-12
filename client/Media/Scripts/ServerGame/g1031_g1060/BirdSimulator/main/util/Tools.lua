
Tools = {}

function Tools.CastToVector3(_x, _y, _z)
    local x = tonumber(_x) or 0
    local y = tonumber(_y) or 0
    local z = tonumber(_z) or 0
    return VectorUtil.newVector3(x, y, z)
end

function Tools.CastToVector3i(_x, _y, _z)
    local x = tonumber(_x) or 0
    local y = tonumber(_y) or 0
    local z = tonumber(_z) or 0
    return VectorUtil.newVector3i(x, y, z)
end

function Tools.ConcatString(tab)
    return table.concat(tab)
end

function Tools.SplitNumber(str, reps)
    reps = reps or '#'
    local tab = StringUtil.split(str, reps)
    local result = {}
    for index = 1, #tab do
        table.insert(result, tonumber(tab[index]))
    end
    return result
end

function Tools:formatNumberThousands(num)
    local formatted = tostring(num)
    local k
    while true do
        formatted, k = string.gsub(formatted, "^(-?%d+)(%d%d%d)", '%1,%2')
        if k == 0 then
            break
        end
    end
    return formatted
end