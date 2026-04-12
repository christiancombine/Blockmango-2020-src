LSChampionShipWeb = {}

LSChampionShipWeb.UploadScoreApi = "/gameaide/api/v1/inner/segment/stage/integral/report/{gameId}"

LSChampionShipWeb.WaitUploadScoreList = {}

function LSChampionShipWeb:UploadScore(userId, score, num)

    if not score or not num then
        return
    end

    table.insert(self.WaitUploadScoreList, {
        userId = tonumber(tostring(userId)),
        integral = score,
        killNum = num
    })
end

function LSChampionShipWeb:tryUploadScore()
    if #self.WaitUploadScoreList == 0 then
        return
    end

    local path = WebService.ServerHttpHost .. self.UploadScoreApi
    path = string.gsub(path, "{gameId}", "g1055")

    local params = {
        { "gameId", "g1055" },
    }

    local body = {}
    for _, info in pairs(self.WaitUploadScoreList) do
        table.insert(body, {
            userId = info.userId,
            integral = info.integral,
            killNum = info.killNum
        })
    end

    self.WaitUploadScoreList = {}

    WebService.asyncPost(path, params, body, function (info, code)
        if code == 1 then
            LogUtil.log("LiftingSimulatorWeb:tryUploadScore suc " .. inspect.inspect(body))
        else
            LogUtil.log("LiftingSimulatorWeb:tryUploadScore error " .. inspect.inspect(body))
        end
    end)
end


