LiftingSimulatorWeb = {}

LiftingSimulatorWeb.getRankDataApi = "/gameaide/api/v1/segment/rank/{gameId}/{userId}"
LiftingSimulatorWeb.getMyRankDataApi = "/gameaide/api/v1/segment/user/rank/{gameId}/{userId}"

function LiftingSimulatorWeb:getRankData(userId, pageNo, pageSize, callback)
    local path = LiftingSimulatorWeb.getRankDataApi
    path = string.gsub(path, "{gameId}", Game.GameType)
    path = string.gsub(path, "{userId}", tostring(userId))

    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) },
        { "pageNo", tostring(pageNo) },
        { "pageSize", tostring(pageSize) }
    }
    WebService.asyncGet(path, params, callback)
end

function LiftingSimulatorWeb:getMyRankData(userId, callback)
    local path = LiftingSimulatorWeb.getMyRankDataApi
    path = string.gsub(path, "{gameId}", Game.GameType)
    path = string.gsub(path, "{userId}", tostring(userId))

    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, callback)
end