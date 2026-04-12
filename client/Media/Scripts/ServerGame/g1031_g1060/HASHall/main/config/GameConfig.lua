---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
require "config.ShopConfig"
require "config.LotteryNpcConfig"
require "config.ActorsConfig"
require "config.ModeNpcConfig"
require "config.RankNpcConfig"
require "util.VideoAdHelper"

GameConfig = {}

GameConfig.initPos = {}
function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    ShopConfig:init(config.shop)
    self.todayFirstLogin = tonumber(config.todayFirstLogin) or 500

    self.lotteryOneTimesRare = config.lotteryOneTimesRare
    self.lotteryTenTimesRare = config.lotteryTenTimesRare
    self.standByTime = tonumber(config.standByTime) or 300
    self.gameTipShowTime = tonumber(config.gameTipShowTime) or 3
    self.rareItemBgImg = config.rareItemBgImg
    self.ordinaryItemBgImg = config.ordinaryItemBgImg

    --print(config.waitingTimeAd)
    VideoAdHelper:setIntervalTime(config.waitingTimeAd)

    LotteryNpcConfig:init(FileUtil.getConfigFromCsv("LotteryNpc.csv"))
    ActorsConfig:init(FileUtil.getConfigFromCsv("Actors.csv"))
    ModeNpcConfig:init(FileUtil.getConfigFromCsv("ModeNpc.csv"))
    RankNpcConfig:init(FileUtil.getConfigFromCsv("RankNpc.csv"))
    self:initRandomSeed()
end

function GameConfig:initRandomSeed()
    for i = 1, 10 do
        local rareProbability = self.lotteryTenTimesRare
        rare = rareProbability[i]
        LotteryUtil:initRandomSeed(i, {rare, 1 - rare})
    end
end

return GameConfig