LiftingSimulatorWeb = {}

LiftingSimulatorWeb.UploadLvApi = "/gameaide/api/v1/inner/segment/stage/report/{gameId}"
LiftingSimulatorWeb.GetRankByLvApi = "/gameaide/api/v1/inner/segment/rank/{gameId}/{segment}"
LiftingSimulatorWeb.GetLastRankChampionApi = "/gameaide/api/v1/inner/segment/champion/info/list/{gameId}"
LiftingSimulatorWeb.GetDecorationApi = "/decoration/api/v1/decorations/user-decoration"

LiftingSimulatorWeb.WaitUploadLvList = {}

function LiftingSimulatorWeb:UploadLv(userId, lv)
    if not lv then
        return
    end

    table.insert(self.WaitUploadLvList, {
        userId = tonumber(tostring(userId)),
        stage = lv
    })
end

function LiftingSimulatorWeb:GetRankByLv(lv, pageNo, pageSize, callback)
    local path = WebService.ServerHttpHost .. self.GetRankByLvApi
    path = string.gsub(path, "{gameId}", BaseMain:getGameType())
    path = string.gsub(path, "{segment}", tostring(lv))

    local params = {
        { "gameId", BaseMain:getGameType() },
        { "segment", lv },
        { "pageNo", pageNo },
        { "pageSize", pageSize },
    }

    WebService.asyncGet(path, params, callback)
end

function LiftingSimulatorWeb:GetLastRankChampion(callback)
    local path = WebService.ServerHttpHost .. self.GetLastRankChampionApi
    path = string.gsub(path, "{gameId}", BaseMain:getGameType())

    local params = {
        { "gameId", BaseMain:getGameType() },
    }

    WebService.asyncGet(path, params, callback)
end

function LiftingSimulatorWeb:GetDecoration(userId, callback)
    local path = WebService.ServerHttpHost .. self.GetDecorationApi

    local params = {
        { "userId", tonumber(tostring(userId)) },
        { "engineVersion", EngineVersionSetting.getEngineVersion() },
    }

    WebService.asyncGet(path, params, callback)
end

function LiftingSimulatorWeb:tryUploadLv()
    if #self.WaitUploadLvList == 0 then
        return
    end

    local path = WebService.ServerHttpHost .. self.UploadLvApi
    path = string.gsub(path, "{gameId}", BaseMain:getGameType())

    local params = {
        { "gameId", BaseMain:getGameType() },
    }

    local body = {}
    for _, info in pairs(self.WaitUploadLvList) do
        table.insert(body, {
            userId = info.userId,
            stage = info.stage
        })
    end

    self.WaitUploadLvList = {}

    WebService.asyncPost(path, params, body, function (info, code)
        if code == 1 then
            LogUtil.log("LiftingSimulatorWeb:tryUploadLv suc " .. inspect.inspect(body))
        else
            LogUtil.log("LiftingSimulatorWeb:tryUploadLv error " .. inspect.inspect(body))
        end
    end)
end


