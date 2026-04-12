GameConfig = {}

local function randomMap()
    local config = FileUtil.getConfigFromYml("map")
    local mapId = config.mapId or 0
    if mapId <= 0 then
        mapId = HostApi.random("map", 1, #config.maps + 1)
    end
    local selectMap
    for index, map in pairs(config.maps) do
        if index == mapId then
            selectMap = map
            break
        end
    end
    if selectMap == nil then
        selectMap = config.maps[1]
    end
    GameConfig.mapId = selectMap.id
    GameConfig.waitPos = VectorUtil.newVector3(selectMap.waitPos[1], selectMap.waitPos[2], selectMap.waitPos[3])
    GameConfig.waitYaw = selectMap.waitPos[4] or 0
    GameConfig.watchPoint = selectMap.watchPoint
    if selectMap.areaStartPos and selectMap.areaEndPos then
        GameConfig.MovementArea = IArea.new()
        local startPos = VectorUtil.newVector3(selectMap.areaStartPos[1], selectMap.areaStartPos[2], selectMap.areaStartPos[3])
        local endPos = VectorUtil.newVector3(selectMap.areaEndPos[1], selectMap.areaEndPos[2], selectMap.areaEndPos[3])
        GameConfig.MovementArea:initByPos(startPos, endPos)
    end
end

function GameConfig:init(config)
    require "game.config.MoneyConfig"
    require "game.config.BridgeConfig"
    require "game.config.SkillConfig"
    require "game.config.AppPropConfig"
    require "game.config.PropBlockConfig"
    require "game.config.HotSpringConfig"
    require "game.config.RuneSetting"
    require "game.config.RewardLevelConfig"
    require "game.config.LotteryPricesConfig"
    require "game.config.LotteryRewardConfig"
    require "game.config.RefreshPricesConfig"
    require "game.config.ScoreConfig"
    require "game.config.AIConfig"
    require "game.config.AIPlayerConfig"
    require "game.config.AIActionConfig"
    require "game.config.AITeamConfig"
    require "game.config.AIDeadConfig"
    require "game.config.BlockHardnessConfig"
    require "game.config.RespawnConfig"
    require "common.config.PrivilegeConfig"
    require "common.config.RewardConfig"
    require "common.config.DressConfig"
    require "common.config.ChestRewardConfig"
    require "common.config.ChestConfig"

    if GameServer:isPrivateParty() then
        self.startPlayers = tonumber(config.partyStartPlayers or "5")
    else
        self.startPlayers = tonumber(config.startPlayers)
    end
    self.teamPlayers = tonumber(config.teamPlayers) or 4
    randomMap()

    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.waitingPlayerTime2 = tonumber(config.waitingPlayerTime2)
    self.prepareTime = tonumber(config.prepareTime)
    self.openLotterySwitch = tonumber(config.openLotterySwitch) or 0
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.deathLineY = tonumber(config.deathLineY) or -256
    self.notGameOver = tonumber(config.notGameOver) or 1
    self.isGameOver = tonumber(config.isGameOver) or 1
    self.playerWin = tonumber(config.playerWin) or 1
    self.playerLos = tonumber(config.playerLos) or 1
    self.quitSubHonor = tonumber(config.quitSubHonor) or -10
    self.fire_time = tonumber(config.fire_time) or 2

    self.standTime = tonumber(config.standTime)

    self.autoAddHp = config.autoAddHp
    self.goldApple = config.goldApple
    self.status = config.status
    self.helmetMap = config.helmetMap
    self.coloursHelmet = config.coloursHelmet
    self.explosionSize = tonumber(config.arrowExplosionSize)
    self.explosionDamageFactor = tonumber(config.arrowExplosionDamageFactor)

    self.grenadeExplosionSize = tonumber(config.grenadeExplosionSize)
    self.grenadeExplosionDamageFactor = tonumber(config.grenadeExplosionDamageFactor)
    self.grenadeExplosionRepelDistanceFactor = tonumber(config.grenadeExplosionRepelDistanceFactor)
    self.critExtraHurt = tonumber(config.critExtraHurt) or 0
    self.critExtraHurtPro = tonumber(config.critExtraHurtPro) or 0
    self.swordFirePro = tonumber(config.swordFirePro) or 0

    self.runLimitCheck = tonumber(config.runLimitCheck) or 0
    self.sprintLimitCheck = tonumber(config.sprintLimitCheck) or 0

    self.updateTipsVersion = tonumber(config.updateTipsVersion) or 0

    self.aiDamageCoefficient = tonumber(config.aiDamageCoefficient) or 0.5

    self.worldTime = tonumber(config.worldTime)

    self.outOfAreaHurt = tonumber(config.outOfAreaHurt) or 5

    self.quicklyBuildBlockCost = config.quicklyBuildBlockCost or 5
    self.quicklyBuildBlockNum = config.quicklyBuildBlockNum or 3

    self.checkAIStuckTime = config.checkAIStuckTime or 30
    self.checkAIStuckDistance = config.checkAIStuckDistance or 10

    self.invincibleTime = tonumber(config.invincibleTime) or 3 --复活无敌时间
    self.InvincibleEffect = tonumber(config.InvincibleEffect) or "130_wudi.effect" --复活无敌时间

    self.taskGiveUpCD = tonumber(config.taskGiveUpCD or "0")
    self.runeMapping = config.RuneMapping

    self.challengeAdProgress = tonumber(config.challengeAdProgress or 10)

    self.buildScore = config.buildScore or { 1, 20, 5, 10 }

    ItemID.WitchPotion = config.WitchPotionId or 599
    ItemID.BackHomeReel = config.BackHomeReelId or 598
    ItemID.IronPuppet = config.IronPuppetId or 597
    ItemID.SnowBallInsect = config.SnowBallInsect or 596

    HostApi.setSwordBreakBlock(true)
    HostApi.setToolDurable(false)

    MoneyConfig:initCoinMapping(config.coinMapping)
    BridgeConfig:init(config.bridge)
end

function GameConfig:initConfig()
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/RewardSetting.csv", 3))
    SkillConfig:init(FileUtil.getConfigFromCsv("SkillConfig.csv"))
    BuffConfig:init(FileUtil.getConfigFromCsv("setting/BuffConfig.csv"))
    AppPropConfig:init(FileUtil.getConfigFromCsv("AppProps.csv"))
    PropBlockConfig:init(FileUtil.getConfigFromCsv("PropBlock.csv"))
    HotSpringConfig:init(FileUtil.getConfigFromCsv("HotSpring.csv"))
    RuneSetting:init(FileUtil.getConfigFromCsv("setting/RuneSetting.csv", 3))
    DressConfig:initConfig(FileUtil.getConfigFromCsv("setting/DressSetting.csv", 3))
    ChestRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/BoxRewardSetting.csv", 3))
    ChestConfig:initConfig(FileUtil.getConfigFromCsv("setting/BoxSetting.csv", 3))
    RewardLevelConfig:init(FileUtil.getConfigFromCsv("setting/RewardLevel.csv"))
    LotteryPricesConfig:init(FileUtil.getConfigFromCsv("setting/LotteryPrices.csv"))
    LotteryRewardConfig:init(FileUtil.getConfigFromCsv("setting/LotteryRewardSetting.csv", 3))
    RefreshPricesConfig:init(FileUtil.getConfigFromCsv("setting/RefreshPrices.csv"))
    AIConfig:init(FileUtil.getConfigFromCsv("AIConfig.csv"))
    AIPlayerConfig:init()
    AIActionConfig:init(FileUtil.getConfigFromCsv("AiActionConfig.csv"))
    AITeamConfig:init(FileUtil.getConfigFromCsv("AITeamConfig.csv"))
    AIDeadConfig:init(FileUtil.getConfigFromCsv("AIDeadConfig.csv"))

end

function GameConfig:getStatusTime(index)
    return self.status[index]
end

return GameConfig