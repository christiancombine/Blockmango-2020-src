
local define = require "define"

local utils = {}

function utils.posId(x, y, z)
    return table.concat({x, y, z}, ":")
end

function utils.checkInRegion(region, x, y, z)
    if x < region[1] or x > region[4] then
        return false
    end

    if y < region[2] or y > region[5] then
        return false
    end

    if z < region[3] or z > region[6] then
        return false
    end

    return true
end

function utils.week_rank_id(site)
    return string.format("%s.%s.%s", define.RANK_WEEK_FLAG, site, DateUtil.getYearWeek())
end

function utils.hist_rank_id(site)
    return string.format("%s.%s", define.RANK_HIST_FLAG, site)
end

function utils:get_site_from_key(key)
	local keys = StringUtil.split(key, ".")
	if keys and #keys >= 3 then
		return keys[3]
	end
	return "site"
end

function utils.print_table(root)
	assert(type(root) == "table")
	local cache = {  [root] = "." }
	local ret = ""
	
	local function _new_line(level)
		local ret = ""
		ret = ret.."\n"
		for i = 1, level do
			ret = ret.."\t"
		end
		return ret
	end
	
	local function _format(value)
		if (type(value) == "string") then
			return "\""..value.."\""
		else
			return tostring(value)
		end
	end

	local function _enter_level(t, level, name)
		local ret = ""
		local _keycache = {}
		for k, v in ipairs(t) do
			ret = ret.._new_line(level + 1)
			if (cache[v]) then
					ret = ret.."["..tostring(k).."]".." = "..cache[v]..","
			elseif (type(v) == "table") then
					local newname = name.."."..tostring(k)
					cache[v] = newname
					ret = ret.."["..tostring(k).."]".." = ".."{"
					ret = ret.._enter_level(v, level + 1, newname)
					ret = ret.._new_line(level + 1)
					ret = ret.."},"
					
			else
					if (type(v) == "string") then
						ret = ret.."["..tostring(k).."]".." = ".."\""..v.."\""..","
					else
						ret = ret.."["..tostring(k).."]".." = "..tostring(v)..","
					end
			end
			_keycache[k] = true
		end
		
		for k, v in pairs(t) do
			if (not _keycache[k]) then
				ret = ret.._new_line(level + 1)
				if (cache[v]) then
					ret = ret.."[".._format(k).."]".." = "..cache[v]..","
				elseif (type(v) == "table") then
					local newname = name.."."..tostring(k)
					cache[v] = newname
					ret = ret.."[".._format(k).."]".." = ".."{"
					ret = ret.._enter_level(v, level + 1, newname)
					ret = ret.._new_line(level + 1)
					ret = ret.."},"
				else
					if (type(v) == "string") then
						ret = ret.."[".._format(k).."]".." = ".."\""..v.."\""..","
					else
						ret = ret.."[".._format(k).."]".." = "..tostring(v)..","
					end
				end
			end
		end
		return ret
	end
	
	ret = ret .. "{"	
	ret = ret .. _enter_level(root, 0, "")
	ret = ret .. "\n}\n"
	print(ret)
end

function utils.log(...)
    local args = {}
    for _, arg in ipairs({...}) do
        table.insert(args, tostring(arg))
    end

    print(string.format("[DEBUG] %s", table.concat(args, " ")))
end

return utils
