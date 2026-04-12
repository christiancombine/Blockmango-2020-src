
require "game.config.LotteryConfig"
require "common.config.WatchADSwitch"

GameConfig = {}

function GameConfig:loadConfig()
    local config = FileUtil.getConfigFromYml("setting/ClientConfig.yml")
    GameConfig.useNewLottery = config.useNewLottery or 1
    GameConfig.mainTabRedPoint = config.mainTabRedPoint or 5
    GameConfig:loadGameConfig()
    WatchADSwitch:init()

end

function GameConfig:loadGameConfig()
    LotteryConfig.loadConfig()
    BuffConfig:init(FileUtil.getConfigFromCsv("setting/BuffConfig.csv"))
    require "common.config.PrivilegeConfig"
end