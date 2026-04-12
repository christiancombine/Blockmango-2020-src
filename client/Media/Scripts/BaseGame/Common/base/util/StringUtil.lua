---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 12:04
---
StringUtil = {}

function StringUtil.split(str, reps, keep_blank, to_number)
    local result = {}
    if str == nil or reps == nil then
        return result
    end

    keep_blank = keep_blank or false
    if keep_blank then
        local startIndex = 1
        while true do
            local lastIndex = string.find(str, reps, startIndex)
            if not lastIndex then
                table.insert(result, string.sub(str, startIndex, string.len(str)))
                break
            end
            table.insert(result, string.sub(str, startIndex, lastIndex - 1))
            startIndex = lastIndex + string.len(reps)
        end
    else
        string.gsub(str, '[^' .. reps .. ']+', function(w)
            table.insert(result, w)
        end)
    end

    to_number = to_number or false
    if to_number then
        for i = 1, #result do
            result[i] = tonumber(result[i])
        end
    end

    return result
end

local function v2s(value, sp, restLevel)
    local typ = type(value)
    if typ=="string" then
        return '\"' .. value .. '\"'
    elseif typ~="table" then
        return tostring(value)
    end
    restLevel = restLevel - 1
    if restLevel<0 then
        return "..."
    end
    local nsp = sp .. "  "
    local tb = {}
    local idxs = {}
    for k, v in ipairs(tb) do
        idxs[k] = true
        tb[#tb+1] = "[" .. k .. "] = " .. v2s(v, nsp, restLevel)
    end
    for k, v in pairs(value) do
        if not idxs[k] then
            k = (type(k)=="string") and ('\"'..k..'\"') or tostring(k)
            tb[#tb+1] = "[" .. k .. "] = " .. v2s(v, nsp, restLevel)
        end
    end
    if not tb[1] then
        return "{}"
    end
    nsp = "\n" .. nsp
    return "{"..nsp .. table.concat(tb, ","..nsp) .. "\n" .. sp .. "}";
end

function StringUtil.v2s(value, maxLevel)
    return v2s(value, "", maxLevel or 3)
end


return StringUtil