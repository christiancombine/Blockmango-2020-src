---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.ChestConfig"
require "config.VehicleConfig"
require "config.AirplaneConfig"
require "config.AirSupportConfig"
require "config.MapConfig"
require "config.WeaponConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.initPosArea = {}
GameConfig.coinMapping = {}

GameConfig.startPlayers = 0
GameConfig.waitingPlayerTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0
GameConfig.prepareSafeAreaTime = 0
GameConfig.safeAreaTime1 = 0
GameConfig.poisonAreaTime1 = 0
GameConfig.safeAreaTime2 = 0
GameConfig.poisonAreaTime2 = 0
GameConfig.safeAreaTime3 = 0
GameConfig.poisonAreaTime3 = 0
GameConfig.safeAreaTime4 = 0
GameConfig.poisonAreaTime4 = 0
GameConfig.horizontalSpeed = 0
GameConfig.verticalSpeed = 0
GameConfig.medicinePack = 0
GameConfig.medicinePotion = 0

function GameConfig:init(config)
    self.startPlayers = tonumber(config.startPlayers)
    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.safeAreaTime = tonumber(config.safeAreaTime)
    self.airSupportTime = tonumber(config.airSupportTime)
    self.poisonAreaTime1 = tonumber(config.poisonAreaTime1)
    self.poisonAreaTime2 = tonumber(config.poisonAreaTime2)
    self.poisonAreaTime3 = tonumber(config.poisonAreaTime3)
    self.poisonAreaTime4 = tonumber(config.poisonAreaTime4)
    self.poisonAreaTime5 = tonumber(config.poisonAreaTime5)
    self.horizontalSpeed = tonumber(config.horizontalSpeed)
    self.verticalSpeed = tonumber(config.verticalSpeed)
    self.initPosArea = config.initPosArea
    self.medicinePack = tonumber(config.medicinePack)
    self.medicinePotion = tonumber(config.medicinePotion)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    ChestConfig:init(FileUtil.getConfigFromYml("chest"))
    VehicleConfig:init(FileUtil.getConfigFromYml("vehicle"))
    AirplaneConfig:init(FileUtil.getConfigFromYml("airplane"))
    AirSupportConfig:init(FileUtil.getConfigFromYml("airsupport"))
    MapConfig:init(FileUtil.getConfigFromYml("map"))
    WeaponConfig:init(FileUtil.getConfigFromYml("weapon"))
end

return GameConfig