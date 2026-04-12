local RequestQueue = {}
local RequestRunningCount = 0

isLocalDb = true

local UserAttrApi = "/user/api/v1/inner/user/details"
local UserInfosApi = "/user/api/v1/inner/simple-info"
local RewardApi = "/game/api/v1/users/games/settlement"
local RewardGoldApi = "/game/api/v1/users/games/settlement/golds"
local RewardListApi = "/game/api/v1/users/inner/games/settlement/list"
local ReportListApi = "/game/api/v2/inner/users/games/reporting/list"
local ExpRuleApi = "/activity/api/v1/inner/activity/games/settlement/rule"
local UserExpApi = "/activity/api/v1/inner/activity/games/user/exp"
local SaveExpApi = "/activity/api/v1/inner/activity/games/records"
local SaveHonorApi = "/gameaide/api/v1/inner/user/segment/integral"
local UserSeasonInfoApi = "/gameaide/api/v1/inner/user/segment/info"
local UserSeasonRewardApi = "/gameaide/api/v1/inner/segment/integral/reward"
local UserHasRechargeApi = "/pay/api/v1/pay/inner/has/user/recharge"
local UserPropsApi = "/gameaide/api/v1/inner/user/game/props"
local ConsumeUserPropsApi = "/gameaide/api/v1/inner/user/game/props/times"
local PayOrderApi = "/pay/api/v2/inner/pay/users/purchase/game/props"
local UploadRankApi = "/gameaide/api/v1/inner/rank/integral/upload/{gameId}"
local MailsApi = "/gameaide/api/v1/inner/game/mails"
local MailsStatusApi = "/gameaide/api/v1/inner/game/mails/{id}/status"
local SendMsgApi = "/gameaide/api/v1/inner/msg/send"
local SendThumbUpRecordApi = "/gameaide/api/v1/inner/insert/thumbs-up-record"
local GetThumbUpRecordApi = "/gameaide/api/v1/inner/get/thumbs-up-record"
local DeleteThumbUpRecordApi = "/gameaide/api/v1/inner/delete/thumbs-up-record"
local UserGameDataApi = "/api/v2/game/data"
local SaveGameDataApi = "/api/v2/game/data"
local AppRankUserApi = "/game/api/v1/inner/games/{gameId}/uses/rank"
local AppRankAllApi = "/game/api/v1/inner/games/{gameId}/rank"
local GetDispatchApi = "/game/api/v1/inner/region/disp-cluster/{regionCode}"
local DispatchPartyApi = "/v1/dispatch-party"
local SumRechargeGCubeApi = "/pay/api/v1/inner/user/game/recharge/sum/gDiamond"
local RecordRankApi = "/gameaide/api/v1/inner/game/rank"
local ConfigFileApi = "/config/files/{fileName}"
local ExchangeConfigApi = "/activity/api/v1/inner/collect/exchange/treasurebox/timeline"
local OpenExchangeBoxApi = "/activity/api/v1/inner/collect/exchange/treasurebox/open"
local GetPlayTimeApi = "/activity/api/v1/inner/collect/exchange/playtime/get"
local SetPlayTimeApi = "/activity/api/v1/inner/collect/exchange/playtime/set"
local GetExchangePropApi = "/activity/api/v1/inner/collect/exchange/game/props"
local SetExchangePropApi = "/activity/api/v1/inner/collect/exchange/game/props"
local ConsumeExchangePropApi = "/activity/api/v1/inner/collect/exchange/game/notify"

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
        return nil, 0, ""
    end
    local success = false
    success, result = pcall(json.decode, result)
    if not success then
        LogUtil.log("[Http Response Error][url]=" .. url .. " [result]=" .. result, LogUtil.LogLevel.Error)
        return nil, 0, ""
    end
    if type(result) ~= "table" then
        return nil, 0, ""
    end
    if result.code == 1 or result.data ~= nil then
        return result.data, result.code, result.message or ""
    else
        LogUtil.log("[Http Response Error][url]=" .. url .. " [code]=" .. result.code .. " [message]=" .. (result.message or ""), LogUtil.LogLevel.Error)
    end
    return nil, result.code, result.message or ""
end

local function handleRequest(requestFunc, queue, ...)
    if not queue then
        return true
    end
    if RequestRunningCount >= 20 then
        local request = { func = requestFunc, params = { ... } }
        table.insert(RequestQueue, request)
        return false
    end
    RequestRunningCount = RequestRunningCount + 1
    return true
end

local function handleResponse(url, result, callback, queue)
    if not callback then
        return
    end
    result = formatResult(result)
    local data, code, message = formatData(url, result)
    callback(data, code, message)
    if queue then
        RequestRunningCount = RequestRunningCount - 1
        if #RequestQueue > 0 then
            local request = RequestQueue[1]
            request.func(unpack(request.params))
            table.remove(RequestQueue, 1)
        end
    end
end

WebService = {}

function WebService:init(version, config)
    self.ver = version
    self.ServerHttpHost = config.blockymodsRewardAddr
    self.DBHttpHost = config.dbHttpServiceUrl
    self.DBHttpRetryHost = config.dbHttpServiceRetryUrl
    self:initGameId()
end

function WebService:initGameId()
    local uuid = require "base.util.uuid"
    uuid.randomseed(os.time())
    self.gameId = uuid()
end

function WebService.asyncGet(url, params, callback, queue)
    if not handleRequest(WebService.asyncGet, queue, url, params, callback, queue) then
        return
    end
    HttpRequest.asyncGet(url, params, function(result, code)
        handleResponse(url, result, callback, queue)
    end)
end

function WebService.asyncPost(url, params, body, callback, queue)
    if not handleRequest(WebService.asyncPost, queue, url, params, body, callback, queue) then
        return
    end
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    HttpRequest.asyncPost(url, params, body, function(result, code)
        handleResponse(url, result, callback, queue)
    end)
end

function WebService.asyncPut(url, params, body, callback, queue)
    if not handleRequest(WebService.asyncPut, queue, url, params, body, callback, queue) then
        return
    end
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    HttpRequest.asyncPut(url, params, body, function(result, success)
        handleResponse(url, result, callback, queue)
    end)
end

function WebService.asyncDelete(url, params, body, callback, queue)
    if not handleRequest(WebService.asyncDelete, queue, url, params, body, callback, queue) then
        return
    end
    body = body or {}
    if type(body) == "table" then
        body = json.encode(body)
    end
    HttpRequest.asyncDelete(url, params, body, function(result, success)
        handleResponse(url, result, callback, queue)
    end)
end

---获取玩家详情信息
function WebService:GetUserAttr(userId, callback)
    local path = self.ServerHttpHost .. UserAttrApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", BaseMain:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        if data ~= nil then
            local player = PlayerManager:getPlayerByUserId(data.userId)
            if player ~= nil then
                local playerName = data.nickName or player.name
                playerName = playerName:gsub(TextFormat.colorStart, "")
                playerName = playerName:gsub("\n", " ")
                player.name = playerName

                local clanName = data.clanName or ""
                clanName = clanName:gsub(TextFormat.colorStart, "")
                clanName = clanName:gsub("\n", " ")
                player.staticAttr.clanName = clanName

                player.staticAttr.role = data.role
                player.staticAttr.props = data.propsId or {}
            else
                data = nil
            end
        else
            if Platform.isWindow() then
                data = {}
            end
        end
        callback(data)
    end)
end

---获得n个玩家的简单信息
function WebService:GetUserInfos(userIds, callback)
    local path = self.ServerHttpHost .. UserInfosApi
    local params = {
        { "userIdList", table.concat(userIds, ",") }
    }
    WebService.asyncGet(path, params, function(data, code)
        callback(data)
    end)
end

---给一个玩家结算平台奖励
function WebService:GetReward(userId, data, callback)
    local path = self.ServerHttpHost .. RewardApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", RewardManager:getGameType() }
    }
    WebService.asyncPost(path, params, data, function(data, code)
        if data ~= nil then
            local player = PlayerManager:getPlayerByUserId(data.userId)
            if player ~= nil then
                player.gold = data.golds or 0
                player.available = data.available or 0
                player.hasGet = data.hasGet or 0
                player.adSwitch = data.adSwitch or 0
            end
        end
        callback(data)
    end)
end

---给一个玩家结算平台奖励{自定义金币数量}
function WebService:GetGoldReward(userId, golds, callback)
    local path = self.ServerHttpHost .. RewardGoldApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", RewardManager:getGameType() },
        { "golds", tostring(golds) }
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        if data ~= nil then
            local player = PlayerManager:getPlayerByUserId(data.userId)
            if player ~= nil then
                player.gold = data.golds or 0
                player.available = data.available or 0
                player.hasGet = data.hasGet or 0
            end
        end
        callback(data)
    end)
end

---给n个玩家结算平台奖励
function WebService:GetRewardList(data, callback)
    local path = self.ServerHttpHost .. RewardListApi
    WebService.asyncPost(path, {}, data, function(data, code)
        if data ~= nil then
            for _, reward in pairs(data) do
                local player = PlayerManager:getPlayerByUserId(reward.userId)
                if player ~= nil then
                    player.gold = reward.golds or 0
                    player.available = reward.available or 0
                    player.hasGet = reward.hasGet or 0
                    player.adSwitch = reward.adSwitch or 0
                end
            end
        end
        callback(data)
    end)
end

---上报n个玩家游戏数据
function WebService:GetReportList(data)
    HostApi.log("[GetReportList]: " .. json.encode(data))
    local path = self.ServerHttpHost .. ReportListApi
    WebService.asyncPost(path, {}, data)
end

---获得经验获取规则，主要用于平台做活动
function WebService:GetExpRule()
    local path = self.ServerHttpHost .. ExpRuleApi
    local params = {
        { "gameId", BaseMain:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        if data ~= nil then
            ExpRule:initRule(data)
        else
            ExpRule:disable()
        end
    end)
end

---获取玩家当前的经验信息，主要用于平台做活动
function WebService:GetUserExp(userId)
    local path = self.ServerHttpHost .. UserExpApi
    local params = {
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, function(data, code)
        if data ~= nil then
            local player = PlayerManager:getPlayerByUserId(data.userId)
            if player ~= nil then
                UserExpManager:addExpCache(data.userId, data.level, data.experience, data.expGetToday)
            end
        end
    end)
end

---给玩家添加经验，主要用于平台做活动
function WebService:SaveUsersExp(data)
    local path = self.ServerHttpHost .. SaveExpApi
    WebService.asyncPost(path, {}, data)
end

---给n个玩家添加段位分，主要用于段位匹配
function WebService:SaveUsersHonor(data)
    local path = self.ServerHttpHost .. SaveHonorApi
    WebService.asyncPost(path, {}, data)
end

---获取玩家赛季的段位信息
function WebService:GetUserSeasonInfo(userId, isLast, callback)
    local path = self.ServerHttpHost .. UserSeasonInfoApi
    local lastSeason
    if isLast then
        lastSeason = "1"
    else
        lastSeason = "0"
    end
    local params = {
        { "userId", tostring(userId) },
        { "gameId", BaseMain:getGameType() },
        { "lastSeason", lastSeason }
    }
    WebService.asyncGet(path, params, function(data, code)
        callback(data)
    end)
end

---更新玩家结算奖励的状态，主要用与赛季结算奖励
function WebService:UpdateUserSeasonReward(userId)
    local path = self.ServerHttpHost .. UserSeasonRewardApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", BaseMain:getGameType() }
    }
    WebService.asyncPost(path, params, "{}")
end

---查询玩家是否有过消费订单(记录)
function WebService:HasRecharge(userId, callback)
    local path = self.ServerHttpHost .. UserHasRechargeApi
    local params = {
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, function(data, code)
        local status = 0
        if data ~= nil then
            status = data.status
        end
        callback(status)
    end)
end

---获取玩家购买过并且正在生效的道具信息，自定义google道具，需要花费现金购买的道具
function WebService:GetUserProps(userId, gameType, callback)
    local path = self.ServerHttpHost .. UserPropsApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", gameType or BaseMain:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        callback(data or {})
    end)
end

---消耗玩家道具(google自定义道具)次数
function WebService:ConsumeProps(userId, gameType, propsId, callback)
    local path = self.ServerHttpHost .. ConsumeUserPropsApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", gameType or BaseMain:getGameType() },
        { "propsId", propsId }
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        callback(data)
    end)
end

function WebService:CheckMails(userId, gameType, callback)
    local path = self.ServerHttpHost .. MailsApi
    local params = {
        { "userId", tostring(userId) },
        { "gameType", gameType }
    }
    WebService.asyncGet(path, params, function(data, code)
        local items = {}
        if data ~= nil then
            for _, mail in pairs(data) do
                if mail.id ~= nil then
                    local item = {}
                    item.id = mail.id
                    item.title = mail.title
                    item.content = mail.content
                    item.fromUser = mail.fromUser
                    item.toUser = mail.toUser
                    item.status = mail.status
                    item.mailType = mail.mailType
                    table.insert(items, item)
                end
            end
        end
        callback(items)
    end)
end

function WebService:SendMails(fromUser, toUser, mailType, title, content, gameType)
    local path = self.ServerHttpHost .. MailsApi
    local content_json = json.encode(content)
    local data = {
        fromUser = tostring(fromUser),
        toUser = tostring(toUser),
        mailType = mailType,
        title = title,
        content = content_json,
        gameType = gameType,
    }
    WebService.asyncPost(path, {}, data)
end

function WebService:UpdateMails(id, status, callback)
    local path = self.ServerHttpHost .. MailsStatusApi
    path = string.gsub(path, "{id}", id)
    local params = {
        { "status", status }
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        callback(data, code)
    end)
end

---向某人/某游戏发送消息，可以跨服务器
function WebService:SendMsg(targets, content, msgType, scope, gameType)
    local path = self.ServerHttpHost .. SendMsgApi
    local data = {
        content = content,
        gameType = gameType,
        msgType = msgType,
        scope = scope, -- 'all', 'game', 'user'
        targets = targets,
    }
    WebService.asyncPost(path, {}, data)
end

function WebService:SendThumbUpRecord(userId, targetId, status)
    local path = self.ServerHttpHost .. SendThumbUpRecordApi
    local data = {
        activeUserId = tostring(userId),
        gameId = BaseMain:getGameType(),
        passiveUserId = tostring(targetId),
        status = status,
    }
    WebService.asyncPost(path, {}, data)
end

function WebService:GetThumbUpRecord(userId, pageNo, pageSize)
    local path = self.ServerHttpHost .. GetThumbUpRecordApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", BaseMain:getGameType() },
        { "pageNo", pageNo },
        { "pageSize", pageSize }
    }
    WebService.asyncGet(path, params)
end

function WebService:DeleteThumbUpRecord(messageId)
    local path = self.ServerHttpHost .. DeleteThumbUpRecordApi
    local params = {
        { "id", messageId },
    }
    WebService.asyncPost(path, params, "{}")
end

---获取玩家的游戏数据，数据库
function WebService:GetUserGameData(userId, subKey, isRetry, callback)
    if isLocalDb then
        print(string.format(
            "[Local] WebService:GetUserGameData, gameType=%s, userId=%s, subKey=%s",
            tostring(DBManager:getGameType()),
            tostring(userId),
            tostring(subKey)
        ))
        local path = string.format("game_cache/%s_%s_%s.db",
            tostring(DBManager:getGameType()),
            tostring(userId),
            tostring(subKey)
        )
        local file = io.open(path, "r")
        local content
        if not file then
            content = ""
        else
            content = file:read("*a")
            file:close()
        end
        if content == "" or content == "{}" then
            callback({ __first = true })
        else
            callback(json.decode(content))
        end
        return
    end
    isRetry = isRetry or false
    local path
    if isRetry then
        path = self.DBHttpRetryHost .. UserGameDataApi
    else
        path = self.DBHttpHost .. UserGameDataApi
    end
    local params = {
        { "userId", tostring(userId) },
        { "subKey", tostring(subKey) },
        { "tableName", DBManager:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        if code == 11 then
            data = { __first = true }
        elseif code ~= 1 then
            data = nil
        end
        callback(data)
    end, true)
end

local function getCache(userId, subKey)
    local gameType = tostring(DBManager:getGameType())
    local uid = tostring(userId)
    local skey = tostring(subKey)

    local path = string.format("game_cache/%s_%s_%s.db", gameType, uid, skey)

    local file = io.open(path, "r")
    if not file then
        return {}
    end

    local content = file:read("*a")
    file:close()

    if content == "" or content == "{}" then
        return {}
    end

    local ok, data = pcall(json.decode, content)
    if not ok or type(data) ~= "table" then
        return {}
    end

    return data
end


---保存玩家的游戏数据，数据库
function WebService:SaveUserGameData(data, isRetry, callback)
    local function cleanCache(tbl)
        local result = {}
        for k, v in pairs(tbl) do
            if not (type(k) == "string" and k:match("^__")) and type(v) ~= "boolean" then
                if type(v) == "table" then
                    result[k] = cleanCache(v)
                else
                    result[k] = v
                end
            end
        end
        return result
    end

    if isLocalDb then
        local dataList = json.decode(data)
        for _, info in pairs(dataList) do
            print(string.format(
                "[Local] WebService:SaveUserGameData, gameType=%s, userId=%s, subKey=%s",
                tostring(DBManager:getGameType()),
                tostring(info.userId),
                tostring(info.subKey)
            ))

            local cache = getCache(info.userId, info.subKey)
            local path = string.format("game_cache/%s_%s_%s.db",
                tostring(DBManager:getGameType()),
                tostring(info.userId),
                tostring(info.subKey)
            )

            for k, v in pairs(info) do
                cache[k] = v
            end

            local cleanedCache = cleanCache(cache)

            local file = io.open(path, "w")
            file:write(json.encode(cleanedCache))
            file:close()
        end
        callback(1)
        return
    end

    isRetry = isRetry or false
    local path
    if isRetry then
        path = self.DBHttpRetryHost .. SaveGameDataApi
    else
        path = self.DBHttpHost .. SaveGameDataApi
    end
    local params = {
        { "tableName", DBManager:getGameType() }
    }
    WebService.asyncPost(path, params, data, function(_, code)
        if code == 3 then
            local dataList = json.decode(data)
            local infoData = {}
            for _, info in pairs(dataList) do
                local key = tostring(info.userId) .. "#" .. tostring(info.subKey)
                infoData[key] = infoData[key] or {}
                table.insert(infoData[key], info)
            end
            for key, info in pairs(infoData) do
                local keys = StringUtil.split(key, "#")
                local userId = keys[1]
                local subKey = keys[2]
                local _params = {
                    { "userId", userId },
                    { "subKey", subKey },
                    { "tableName", DBManager:getGameType() }
                }
                local _path
                if isRetry then
                    _path = self.DBHttpRetryHost .. UserGameDataApi
                else
                    _path = self.DBHttpHost .. UserGameDataApi
                end
                WebService.asyncGet(_path, _params, function(_data, _code)
                    if _code == 11 then
                        _data = {}
                    end
                    if _data == nil then
                        callback(code)
                        return
                    end
                    DBManager:replaceCache(userId, subKey, _data)
                end)
            end
            return
        end
        callback(code)
    end)
end


---更新排行榜信息，客户端可以自己读取的排行榜
function WebService:Pay(userId, propsId, currency, quantity, callback)
    local path = "http://127.0.0.1:8080" .. PayOrderApi
    local versionCode = "1"
    local packageName = "com.sandboxol.blockymods"

    local info = GameAnalyticsCache:getClientInfo(userId)
    if info ~= nil then
        versionCode = info.version_code or "1"
        packageName = info.package_name or "com.sandboxol.blockymods"
    end

    if currency == 1 then
        currency = 0
    end

    local data = {
        userId = tostring(userId),
        propsId = propsId,
        currency = currency,
        quantity = quantity,
        gameId = BaseMain:getGameType(),
        engineVersion = EngineVersionSetting.getEngineVersion(),
        appVersion = versionCode,
        packageName = packageName,
    }

    print("[PAY DEBUG] POST -> " .. tostring(path))
    print("[PAY DEBUG] BODY -> " .. json.encode(data))

    WebService.asyncPost(path, {}, data, function(response, code)
        -- protezione contro nil o dati strani
        local safeResponse = response
        if type(response) ~= "table" then
            safeResponse = {}  -- garantisce almeno una tabella vuota
        end

        local safeCode = code
        if type(code) ~= "number" then
            safeCode = -1  -- codice di errore generico
        end

        print("[PAY DEBUG] RESPONSE CODE -> " .. tostring(safeCode))
        print("[PAY DEBUG] RESPONSE DATA -> " .. tostring(safeResponse))

        -- callback sicuro
        if callback then
            callback(safeResponse)
        end
    end)
end



function WebService:GetUserAppRank(userId, type, lastWeek, callback)
    local path = self.ServerHttpHost .. AppRankUserApi
    path = string.gsub(path, "{gameId}", RewardManager:getGameType())
    local params = {
        { "userId", tostring(userId) },
        { "gameId", RewardManager:getGameType() },
        { "type", type }, -- all, month, week
        { "lastWeek", lastWeek }, -- 0 this week, 1 last week
    }
    WebService.asyncGet(path, params, function(data, code)
        if not data then
            return
        end
        callback(data)
    end)
end

function WebService:GetAllAppRank(type, pageNo, pageSize, lastWeek, callback)
    local path = self.ServerHttpHost .. AppRankAllApi
    path = string.gsub(path, "{gameId}", RewardManager:getGameType())
    local params = {
        { "userId", tostring(0) },
        { "gameId", RewardManager:getGameType() },
        { "type", type }, -- all, month, week
        { "pageNo", pageNo },
        { "pageSize", pageSize },
        { "lastWeek", lastWeek }, -- 0 this week, 1 last week
    }
    WebService.asyncGet(path, params, function(data, code)
        if data and data.pageInfo then
            callback(data.pageInfo)
        end
    end)
end

function WebService:GetDispatchUrl(regionId, callback, retryTimes)
    retryTimes = retryTimes or 3
    local path = self.ServerHttpHost .. GetDispatchApi
    path = string.gsub(path, "{regionCode}", tostring(regionId))
    local params = {
        { "engineType", "v1" }
    }
    WebService.asyncGet(path, params, function(data, code)
        if not data then
            if retryTimes > 0 then
                self:GetDispatchUrl(regionId, callback, retryTimes - 1)
                return
            end
        end
        callback(data, code)
    end)
end

function WebService:GetDispatchParty(dispatchUrl, info, callback, retryTimes)
    retryTimes = retryTimes or 3
    local path = dispatchUrl .. DispatchPartyApi
    WebService.asyncPost(path, {}, json.encode(info), function(data, code)
        if code ~= 1 and retryTimes > 0 then
            LuaTimer:schedule(function()
                self:GetDispatchParty(dispatchUrl, info, callback, retryTimes - 1)
            end, 3000)
            return
        end
        callback(data, code)
    end)
end

function WebService:GetSumRechargeGCube(userId, callback)
    local path = self.ServerHttpHost .. SumRechargeGCubeApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", DBManager:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        callback(data)
    end)
end

---分页获取对应key记录排行榜的数据
function WebService:GetRecordList(key, pageNo, pageSize, callback)
    local path = self.ServerHttpHost .. RecordRankApi
    local params = {
        { "key", tostring(key) },
        { "pageNo", tostring(pageNo) },
        { "pageSize", tostring(pageSize) }
    }
    WebService.asyncGet(path, params, function(data, code)
        callback(data)
    end)
end

---分页获取对应key记录排行榜的数据
function WebService:ReportOneRecord(key, value, scores, callback, maxSize)
    maxSize = maxSize or 10
    local path = self.ServerHttpHost .. RecordRankApi

    local data = {
        key = tostring(key),
        maxSize = maxSize,
        member = tostring(value),
        scores = scores,
    }

    WebService.asyncPost(path, {}, data, function(data, code)
        callback(data, code)
    end)
end

function WebService:GetPlayerIdentityCache(callback, retryTimes)
    retryTimes = retryTimes or 3
    local path = self.ServerHttpHost .. ConfigFileApi
    path = string.gsub(path, "{fileName}", "player-identity-config")
    local params = {
        { "reload", "true" }
    }
    WebService.asyncGet(path, params, function(data, code)
        if code ~= 1 and retryTimes > 0 then
            self:GetPlayerIdentityCache(callback, retryTimes - 1)
            return
        end
        callback(data)
    end)
end

function WebService:GetExchangeConfig(callback)
    local path = self.ServerHttpHost .. ExchangeConfigApi
    WebService.asyncGet(path, {}, function(data, code)
        if data then
            callback(data)
        end
    end)
end

function WebService:OpenExchangeBox(userId, boxId, callback)
    local info = GameAnalyticsCache:getClientInfo(userId)
    local versionCode = 0
    local platform = "android"
    if info ~= nil then
        versionCode = info.version_code or "1"
        platform = info.platform or "android"
    end
    local path = self.ServerHttpHost .. OpenExchangeBoxApi
    local params = {
        { "userId", tostring(userId) },
        { "uuid", tostring(boxId) },
        { "versionCode", versionCode },
        { "deviceType", platform }
    }
    WebService.asyncPost(path, params, "{}", function(data, code, message)
        callback(data, code, message)
    end)
end

function WebService:GetPlayTime(userId, callback, retryTimes)
    retryTimes = retryTimes or 2
    local path = self.ServerHttpHost .. GetPlayTimeApi
    local params = {
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, function(data)
        if not data then
            if retryTimes > 0 then
                WebService:GetPlayTime(userId, callback, retryTimes - 1)
            end
            return
        end
        callback(data)
    end)
end

function WebService:SetPlayTime(userId, playSecond)
    local info = GameAnalyticsCache:getClientInfo(userId)
    local versionCode = 0
    local platform = "android"
    if info ~= nil then
        versionCode = info.version_code or "1"
        platform = info.platform or "android"
    end
    local path = self.ServerHttpHost .. SetPlayTimeApi
    local params = {
        { "userId", tostring(userId) },
        { "playSecond", tostring(playSecond) },
        { "versionCode", tostring(versionCode) },
        { "deviceType", tostring(platform) }
    }
    WebService.asyncPost(path, params, "{}")
end

function WebService:GetExchangeProp(userId, callback)
    local path = self.ServerHttpHost .. GetExchangePropApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", DBManager:getGameType() }
    }
    WebService.asyncGet(path, params, function(data, code)
        if data then
            callback(data)
        end
    end)
end

function WebService:SetExchangeProp(userId, propsId, propsCount, expiryTime)
    local path = self.ServerHttpHost .. SetExchangePropApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", DBManager:getGameType() },
        { "gamePropsId", tostring(propsId) },
        { "propsAmount", tostring(propsCount) },
        { "expiryDate", tostring(expiryTime) }
    }
    WebService.asyncPost(path, params, "{}")
end

function WebService:ConsumeExchangeProp(userId, propsId, callback)
    local path = self.ServerHttpHost .. ConsumeExchangePropApi
    local params = {
        { "userId", tostring(userId) },
        { "gameId", DBManager:getGameType() },
        { "gamePropsId", propsId }
    }
    WebService.asyncPost(path, params, "{}", function(data)
        if data then
            callback(data)
        end
    end)
end

--endregion
