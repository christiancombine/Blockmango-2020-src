AIManager = {}
local AIPlayers = {}

function AIManager:addAI(ai)
    table.insert(AIPlayers, ai)
end

function AIManager:subAI(ai)
    for index, player in pairs(AIPlayers) do
        if tostring(player.rakssid) == tostring(ai.rakssid) then
            table.remove(AIPlayers, index)
            return
        end
    end
end

function AIManager:getAIs()
    return AIPlayers
end

function AIManager:getAICount()
    return #AIPlayers
end

function AIManager:getAIByRakssid(rakssid)
    for _, player in pairs(AIPlayers) do
        if player.rakssid == rakssid then
            return player
        end
    end
    return nil
end

function AIManager:getAIsByTeamId(teamId)
    local teamList = {}
    for _, player in pairs(AIPlayers) do
        if player.luaPlayer.teamId == teamId then
            table.insert(teamList, player)
        end
    end
    return teamList
end

function AIManager:onGameOver()
    for _, AI in pairs(AIPlayers) do
        AI:removeBtTree()
    end
    self.AIPlayers = {}
end

function AIManager:isAI(rakssid)
    if tonumber(tostring(rakssid)) <= 100 then
        --AI return
        return true
    end
    return false
end

function AIManager:isAIByUserId(userId)
    if self:getAIByUserId(userId) ~= nil then
        return true
    end
    return false
end

function AIManager:getAIByUserId(userId)
    for _, player in pairs(AIPlayers) do
        if player.userId == userId then
            return player
        end
    end
    return nil
end

function AIManager:getRealPlayerCount()
    return PlayerManager:getPlayerCount() - #AIPlayers
end