local ClientHttpHost = "http://dev.mods.sandboxol.cn"
local RequestSession = 0
local RequestCache = {}

local function requestSession()
    RequestSession = RequestSession + 1
    return tostring(RequestSession)
end

WebService = {}

local function formatResult(result)
    if type(result) ~= "string" or string.len(result) == 0 then
        return "{}"
    end
    if string.sub(result, 1, 1) == '{' and string.find(result, "\"code\":") ~= nil then
        return result
    end
    LogUtil.log("[Error] http request failed: " .. result, LogUtil.LogLevel.Error)
    return "{}"
end

local function formatData(url, result)
    if result == "{}" then
        return nil, 0
    end
    local success = false
    success, result = pcall(json.decode, string.gsub(result, "▢", ""))
    if not success then
        LogUtil.log("[Http Response Error] [url]=" .. url .. " [result]=" .. result, LogUtil.LogLevel.Error)
        return nil, 0
    end
    if type(result) ~= "table" then
        return nil, 0
    end
    if result.code == 1 or result.data ~= nil then
        return result.data, result.code
    else
        LogUtil.log("[Http Response Error] [url]=" .. url .. " [code]=" .. result.code .. " [message]=" .. (result.message or ""), LogUtil.LogLevel.Error)
    end
    return nil, result.code
end

function WebService:init()
    ClientHttpHost = CGame.Instance():getBaseUrl()
end

function WebService:destroy()
    RequestCache = {}
end

function WebService.asyncGet(url, params, callback)
    url = ClientHttpHost .. url
    local session = requestSession()
    RequestCache[session] = true
    ClientHttpRequest.asyncGet(url, params, function(result, success)
        if not RequestCache[session] then
            return
        end
        RequestCache[session] = nil
        if not callback then
            return
        end
        result = formatResult(result)
        local data, code = formatData(url, result)
        callback(data, code)
    end)
end

function WebService.asyncPost(url, params, body, callback)
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    url = ClientHttpHost .. url
    local session = requestSession()
    RequestCache[session] = true
    ClientHttpRequest.asyncPost(url, params, body, function(result, success)
        if not RequestCache[session] then
            return
        end
        RequestCache[session] = nil
        if not callback then
            return
        end
        result = formatResult(result)
        local data, code = formatData(url, result)
        callback(data, code)
    end)
end

function WebService.asyncPut(url, params, body, callback)
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    url = ClientHttpHost .. url
    local session = requestSession()
    RequestCache[session] = true
    ClientHttpRequest.asyncPut(url, params, body, function(result, success)
        if not RequestCache[session] then
            return
        end
        RequestCache[session] = nil
        if not callback then
            return
        end
        result = formatResult(result)
        local data, code = formatData(url, result)
        callback(data, code)
    end)
end

function WebService.asyncDelete(url, params, body, callback)
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    url = ClientHttpHost .. url
    ClientHttpRequest.asyncDelete(url, params, body, function(result, success)
        local session = requestSession()
        RequestCache[session] = true
        if not RequestCache[session] then
            return
        end
        RequestCache[session] = nil
        if not callback then
            return
        end
        result = formatResult(result)
        local data, code = formatData(url, result)
        callback(data, code)
    end)
end

function WebService.friendOperation(type, userId)
    ClientHttpRequest.friendOperation(type, userId)
end

return WebService
