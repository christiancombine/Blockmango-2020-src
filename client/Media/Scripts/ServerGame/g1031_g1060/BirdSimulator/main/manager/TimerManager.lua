require "data.EventCallback"

TimerManager = {}

local eventTimerList = {}
local currentTime = os.time()
local eventListenerId = 0

-- 添加间隔调用的事件  每秒都会调用  可以设置调用多少次 nil表示一直调用
function TimerManager.AddIntervalListener(name, number, callback, obj)
    if type(callback) ~= 'function' then
        return
    end
    local cor = nil
    local func = EventCallback.new(callback, obj)
    if type(func) ~= 'table' then
        return
    end
    if number == nil then
        cor = coroutine.create(function(...)
            while true do
                func:Invoke(...)
                coroutine.yield()
            end
        end)
    elseif type(number) == 'number' and number > 0 then
        cor = coroutine.create(function(...)
            for index = 1, number do
                func:Invoke(...)
                coroutine.yield()
            end
        end)
    end
    if type(cor) == 'thread' then
        eventTimerList[name] = cor
    end
end

function TimerManager.AddDurationEvent(name, time, count, callback, obj)
    if type(callback) ~= 'function' then
        return
    end
    local func = EventCallback.new(callback, obj)
    local cor = coroutine.create(function(...)
        for x = 1, count do
            for y = 1, time - 1 do          
                coroutine.yield()
            end
            func:Invoke(...)
            coroutine.yield()
        end
    end)
    if type(cor) == 'thread' then
        eventTimerList[name] = cor
    end
end

--  添加延时调用事件  多少秒后调用
function TimerManager.AddDelayListener(name, time, callback, obj)
    if type(time) ~= 'number' and type(callback) ~= 'function' and time < 0 then
        return
    end
    if time == 0 then callback() return end
    local func = EventCallback.new(callback, obj)
    local cor = coroutine.create(function(...)
        for index = 1, time do
            coroutine.yield()
        end
        func:Invoke(...)
    end)
    if type(cor) == 'thread' then
        eventTimerList[name] = cor
    end
end

function TimerManager.Update()
    for id, cor in pairs(eventTimerList) do
        coroutine.resume(cor, id)
        if coroutine.status(cor) == 'dead' then
            eventTimerList[id] = nil
        end
    end
end

function TimerManager.IsListenerExist(id)
    if eventTimerList[id] ~= nil then
        return true
    end

    return false
end

function TimerManager.RemoveListenerById(id)
    if eventTimerList[id] ~= nil then
        eventTimerList[id] = nil
    end
end

function TimerManager.ClearAllListener()
    eventTimerList = {}
end
