---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 10:16
---
DateUtil = {}

function DateUtil.getYearWeek(time)
    return os.date("%Y", time) .. DateUtil.getWeeksOfYear(time)
end

function DateUtil.getWeeksOfYear(time)
    if time == nil then
        time = os.time()
    end
    local firstYearTime = os.time({ day = 1, month = 1, year = os.date("%Y", time) })
    local firstYearDate = os.date("*t", firstYearTime)
    local date = os.date("*t", time)
    local start_wday = firstYearDate.wday - 1
    local yday = date.yday
    if (start_wday + yday) % 7 == 0 then
        return (start_wday + yday) / 7
    else
        return NumberUtil.getIntPart((start_wday + yday) / 7) + 1
    end
end

function DateUtil.getWeekDay(time)
    if time == nil then
        time = os.time()
    end
    return os.date("*t", time).wday
end

function DateUtil.getYearDay(time)
    if time == nil then
        time = os.time()
    end
    return os.date("%Y", time) .. os.date("*t", time).yday
end

function DateUtil.getDate(time)
    if time == nil then
        time = os.time()
    end
    return os.date("%Y-%m-%d", time)
end

function DateUtil.getDateString(time)
    if time == nil then
        time = os.time()
    end
    return os.date("%Y-%m-%d %X", time)
end

function DateUtil.getWeekLastTime(time, real)
    time = DateUtil.getDayLastTime(time, true)
    local date = os.date("*t", time)
    local wday = date.wday
    time = time + (7 - wday + 1) * 86400
    if real then
        return time
    end
    return time + 86400
end

function DateUtil.getDayLastTime(time, real)
    if time == nil then
        time = os.time()
    end
    local date = os.date("*t", time + 86400)
    time = os.time({ year = date.year, month = date.month, day = date.day, hour = 0, minute = 0, second = 0})
    if real then
        return time
    end
    return time + 86400
end

function DateUtil.getWeekSeconds()
    return 604800
end

function DateUtil.getDaySeconds()
    return 86400
end

local function checkTime(text, append, format, pattern, split)
    split = split or ":"
    if string.find(format, pattern) ~= nil then
        if #text ~= 0 then
            text = text .. split
        end
        if append < 10 then
            text = text .. "0" .. append
        else
            text = text .. append
        end
    end
    return text
end

---获取格式化后的时间 默认(HH:mm:ss)
---@param seconds 秒数
---@param format 格式化规则
function DateUtil.getFormatTime(seconds, format)
    format = format or "HH:mm:ss"
    local text = ""
    local hour = math.floor(seconds / 3600)
    local minute = math.floor((seconds % 3600) / 60)
    local second = seconds % 60
    text = checkTime(text, hour, format, "HH")
    text = checkTime(text, minute, format, "mm")
    text = checkTime(text, second, format, "ss")
    return text
end

---获取北京跟当前服务器时间戳的差值
local function getBeiJingDiffTime()
    return os.time(os.date("*t")) - os.time(os.date("!*t")) - 28800
end

---根据date获取北京时间戳
---@param date table 某个北京时间日期的table
---eg: { year = 2020, month = 1, day = 20, hour = 1, min = 1, sec = 0 }
function DateUtil.date2BeiJingTime(date)
    date.isdst = false
    return os.time(date) + getBeiJingDiffTime()
end

return DateUtil