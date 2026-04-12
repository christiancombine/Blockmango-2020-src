require "config.ManorConfig"
require "config.ManorLevelConfig"
require "config.CannonConfig"
require "config.BlockConfig"
require "config.DecorationConfig"
require "config.HardnessConfig"
require "config.TigerLotteryConfig"
require "config.CheckInConfig"
require "config.TigerLotteryItems"
require "config.WingsConfig"
require "config.DrawingConfig"
require "config.MailBoxConfig"
require "config.VehicleConfig"
require "config.TagConfig"
require "config.PublicFacilitiesConfig"
require "config.GuideConfig"
require "config.FlyingConfig"
require "config.ShopHouseConfig"
require "config.PraiseRewardConfig"
require "config.RoseConfig"
require "config.ElevatorConfig"
require "config.WeekRewardConfig"
require "config.ArchiveConfig"
require "config.CDRewardConfig"
require "config.CurrencyExConfig"

GameConfig = {}
GameConfig.initPos = {}

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.manorReleaseTime = tonumber(config.manorReleaseTime) or 120
    GameConfig.inviteShowTime = tonumber(config.inviteShowTime) or 20
    GameConfig.inGuidePlaceTemplatePreId = tonumber(config.inGuidePlaceTemplatePreId) or 0
    GameConfig.inGuidePlaceTemplateId = tonumber(config.inGuidePlaceTemplateId) or 0
    GameConfig.inGuideLastId = tonumber(config.inGuideLastId) or 0
    GameConfig.GuideDemoDrawId = tonumber(config.GuideDemoDrawId) or 914
    GameConfig.GuideDemoDrawRPos90 = VectorUtil.newVector3(config.GuideDemoDrawRPos90[1], config.GuideDemoDrawRPos90[2], config.GuideDemoDrawRPos90[3])
    GameConfig.GuideDemoDrawRPos180 = VectorUtil.newVector3(config.GuideDemoDrawRPos180[1], config.GuideDemoDrawRPos180[2], config.GuideDemoDrawRPos180[3])
    GameConfig.GuideDemoDrawRPos270 = VectorUtil.newVector3(config.GuideDemoDrawRPos270[1], config.GuideDemoDrawRPos270[2], config.GuideDemoDrawRPos270[3])
    GameConfig.GuidePlaceDemoDrawEventId = tonumber(config.GuidePlaceDemoDrawEventId) or 0
    GameConfig.GuideSuspendTeleportRPos = VectorUtil.newVector3(config.GuideSuspendTeleportRPos[1], config.GuideSuspendTeleportRPos[2], config.GuideSuspendTeleportRPos[3])
    GameConfig.defaultName = config.defaultName or ""
    GameConfig.groundName = config.groundName or ""
    GameConfig.maxArchiveNum = tonumber(config.maxArchiveNum) or 5
    GameConfig.freeArchiveNum = tonumber(config.freeArchiveNum) or 1
    GameConfig.archiveName = config.archiveName or ""
    GameConfig.freeFlyTime = tonumber(config.freeFlyTime) or 600
    GameConfig.roseId = tonumber(config.roseId) or 0
    GameConfig.roseNum = tonumber(config.roseNum) or 5
    GameConfig.superMarket = {
        pos = VectorUtil.newVector3(config.shopPosWithYaw[1], config.shopPosWithYaw[2], config.shopPosWithYaw[3]),
        yaw = config.shopPosWithYaw[4],
    }
    GameConfig.freeR_Currency = tonumber(config.freeR_Currency) or 3000
    GameConfig.ADRefreshTime = config.ADRefreshTime
    GameConfig.flyingAdReward = tonumber(config.flyingAdReward or 5)
    ManorConfig:init(FileUtil.getConfigFromCsv("manor.csv"))
    ManorConfig:initDoorTip(FileUtil.getConfigFromCsv("doorTip.csv"))
    ManorLevelConfig:init(FileUtil.getConfigFromCsv("manorLevel.csv"))
    BlockConfig:initBlock(FileUtil.getConfigFromCsv("block.csv"))
    BlockConfig:initSimplifyBlock(FileUtil.getConfigFromCsv("blockSimplify.csv"))
    DecorationConfig:init(FileUtil.getConfigFromCsv("decoration.csv"))
    CannonConfig:init(FileUtil.getConfigFromCsv("cannon.csv"))
    TigerLotteryConfig:init(FileUtil.getConfigFromCsv("tigerLottery.csv"))
    VehicleConfig:init(FileUtil.getConfigFromCsv("vehicle.csv"))
    HardnessConfig:init(FileUtil.getConfigFromCsv("hardness.csv"))
    CheckInConfig:init(FileUtil.getConfigFromCsv("checkIn.csv"))
    TigerLotteryItems:init(FileUtil.getConfigFromCsv("tigerLotteryItems.csv"))
    WingsConfig:init(FileUtil.getConfigFromCsv("wing.csv"))
    DrawingConfig:initDrawing(FileUtil.getConfigFromCsv("drawing.csv"))
    DrawingConfig:initDrawingData(FileUtil.getConfigFromCsv("drawingData.csv"))
    MailBoxConfig:init(FileUtil.getConfigFromCsv("mailBox.csv"))
    TagConfig:init(FileUtil.getConfigFromCsv("tag.csv"))
    FlyingConfig:init(FileUtil.getConfigFromCsv("flying.csv"))
    GuideConfig:initGuide(FileUtil.getConfigFromCsv("setting/GuideSetting.csv", 3))
    GuideConfig:initCondition(FileUtil.getConfigFromCsv("setting/ConditionSetting.csv", 3))
    GuideConfig:initEvent(FileUtil.getConfigFromCsv("setting/EventSetting.csv", 3))
    ShopHouseConfig:initShopHouses(FileUtil.getConfigFromCsv("shopHouseConfig.csv"))
    ShopHouseConfig:initShopHouseItems(FileUtil.getConfigFromCsv("shopHouseItems.csv"))
    PraiseRewardConfig:init(FileUtil.getConfigFromCsv("praiseReward.csv"))
    RoseConfig:init(FileUtil.getConfigFromCsv("rose.csv"))
    ElevatorConfig:init(FileUtil.getConfigFromCsv("elevator.csv"))
    WeekRewardConfig:init(FileUtil.getConfigFromCsv("weekReward.csv"))
    ArchiveConfig:init(FileUtil.getConfigFromCsv("archive.csv"))
    CDRewardConfig:init(FileUtil.getConfigFromCsv("CDReward.csv"))
    CurrencyExConfig:init(FileUtil.getConfigFromCsv("currencyEx.csv"))
    PublicFacilitiesConfig:initPublicChairs(FileUtil.getConfigFromCsv("publicChairs.csv"))
    PublicFacilitiesConfig:initElevatorActor(FileUtil.getConfigFromCsv("elevatorActor.csv"))
    PublicFacilitiesConfig:initPortalActor(FileUtil.getConfigFromCsv("setting/portalActor.csv", 3))
    PublicFacilitiesConfig:initPortal(FileUtil.getConfigFromCsv("setting/portal.csv", 3))
    VehicleConfig:initRaceVehicleInfo(config.RaceInitInfo)
end

return GameConfig
