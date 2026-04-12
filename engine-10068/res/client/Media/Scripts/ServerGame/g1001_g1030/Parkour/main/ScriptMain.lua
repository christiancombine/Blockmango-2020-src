package.path = package.path ..';..\\?.lua';

require "config.GameConfig"

local define = require "define"
local listener = require "listener"

listener:init()

BaseMain:setGameType(define.gameType)
HostApi.setNeedFoodStats(false)
ReportManager:setRankType(ReportManager.RankType.Max)
DBManager:addMustLoadSubKey(1)
