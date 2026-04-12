
GameSeason = {}
GameSeason.UserSeasonInfoCache = {}

function GameSeason:getUserSeasonInfo(userId, isLast, retryTime)
    WebService:GetUserSeasonInfo(userId, isLast, function(data)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        if not data then
            if retryTime > 0 then
                GameSeason:getUserSeasonInfo(userId, isLast, retryTime - 1)
            end
            return
        end
        if isLast then
            GameSeason:onGetUserLastSeasonInfo(player, data)
        else
            GameSeason:onGetUserCurrentSeasonInfo(player, data)
        end
    end)
end

function GameSeason:onGetUserCurrentSeasonInfo(player, data)
    local honorId = data.segment
    local rank = data.rank
    local honor = data.integral
    local days = math.floor(data.timeRemains / 86400)
    GameSeason.UserSeasonInfoCache[tostring(player.userId)] = {
        honorId = honorId,
        rank = rank,
        honor = honor
    }
    GamePacketSender:sendCurrentSeasonInfo(player.rakssid, honorId, rank, honor, days)
    if data.needReward == 1 then
        GameSeason:getUserSeasonInfo(player.userId, true, 3)
    end
end

function GameSeason:onGetUserLastSeasonInfo(player, data)
    local honorId = data.segment
    local rank = data.rank
    local honor = data.integral
    GamePacketSender:sendLastSeasonInfo(player.rakssid, honorId, rank, honor)
    WebService:UpdateUserSeasonReward(player.userId)
end

function GameSeason:onPlayerQuit(player)
    GameSeason.UserSeasonInfoCache[tostring(player.userId)] = nil
end

return GameSeason