---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "util.DbUtil"
require "util.ReportUtil"
require "process.PersonalProcess"
require "process.TeamProcess"
require "process.PvpProcess"
require "process.ChickenProcess"
require "data.GameTeam"
require "manager.SupplyManager"
require "manager.SkillManager"
require "manager.DeBuffManager"
require "manager.EnterRoomManager"

GameMatch = {}

GameMatch.gameType = ""
GameMatch.Process = nil

function GameMatch:initMatch()
    self:setProcess()
    GameTimeTask:start()
    SupplyManager:init()
end

function GameMatch:setProcess()
    if self.gameType == "g1043" or self.gameType == "g3043" then
        self.Process = TeamProcess
        TeamManager:InitTeams()
    elseif self.gameType == "g1044" or self.gameType == "g3044" then
        self.Process = PersonalProcess
    elseif self.gameType == "g1045" or self.gameType == "g3045" then
        self.Process = PvpProcess
    elseif self.gameType == "g1053" then
        self.Process = ChickenProcess
    end
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self.Process:onTick(ticks)
    SkillManager:onTick(ticks)
    DeBuffManager:onTick(ticks)
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
end

function GameMatch:onPlayerQuit(player)
    GameMatch.Process:onPlayerQuit(player)
end

return GameMatch