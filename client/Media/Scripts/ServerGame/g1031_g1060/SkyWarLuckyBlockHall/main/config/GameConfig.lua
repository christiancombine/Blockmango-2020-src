---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.FuncNpcConfig"
require "config.AdvertConfig"
require "config.RewardConfig"
require "config.JobConfig"
require "config.JobPondConfig"

GameConfig = {}
GameConfig.initPos = {}
GameConfig.standByTime = 0
function GameConfig:init(config)
    self.initExp = tonumber(config.initExp) or 0
    self.pondRefreshPrice = config.pondRefreshPrice
    self.standByTime = tonumber(config.standByTime) or 200
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameMatch.gameType = config.gameType or "g1058"
    FuncNpcConfig:initConfig(FileUtil.getConfigFromCsv("FuncNpc.csv"))
    AdvertConfig:initConfig(FileUtil.getConfigFromCsv("AdvertSetting.csv"))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/RewardConfig.csv", 3))
    JobConfig:init(FileUtil.getConfigFromCsv("setting/JobConfig.csv", 3))
    JobPondConfig:init(FileUtil.getConfigFromCsv("JobPondConfig.csv"))
end

return GameConfig