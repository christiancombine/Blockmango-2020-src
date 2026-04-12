---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "manager.EnterRoomManager"
require "messages.Messages"
require "util.DbUtil"
require "util.ReportUtil"

GameMatch = {}

GameMatch.gameType = "g1042"
GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    LuaTimer:schedule(function()
        for _, player in pairs(PlayerManager:getPlayers()) do
            player.taskController:initActiveInfo()
        end
    end, (DateUtil.getDayLastTime(nil, true) - os.time() + 1) * 1000, DateUtil.getDaySeconds() * 1000)
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    WaitUpgradeQueue:onTick()
    EnterRoomManager:tryQueueEnterRoom()
    GameMatch:checkPlayerOnline()
end

function GameMatch:checkPlayerOnline()
    local players = PlayerManager:getPlayers()
    if self.curTick % 60 == 0 then
        for _, player in pairs(players) do
            player.taskController:packingProgressData(player, Define.TaskType.GrowUp.Online, GameMatch.gameType)
        end
    end
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:onPlayerQuit(player)
    GameSeason:onPlayerQuit(player)
    EnterRoomManager:onPlayerLogout(player)
end

return GameMatch