---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 15:36
---
DbUtil = {}

function DbUtil:getPlayerData(player)
    DBManager:getPlayerData(player.userId, 1)
end

function DbUtil:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    local data = {}
    data.money = player.money
    data.games = player.games
    data.integral = player.integral
    data.hasWin = player.hasWin
    data.titles = player.titles
    data.skills = player.skills
    data.chips = player.chips
    DBManager:savePlayerData(player.userId, 1, data, immediate)
end

return DbUtil