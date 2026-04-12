---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.XTeamConfig"
require "config.XTools"
require "config.XMoneyConfig"
require "config.XInitItemConfig"
require "config.XNpcShopConfig"
require "config.XNpcShopGoodsConfig"
require "config.XRespawnConfig"
require "config.XAppShopConfig"
require "config.XTipsConfig"
require "config.XActorConfig"
require "config.XPlayerShopItems"
require "config.XRailRouteConfig"
require "config.XCarBuffConfig"
require "config.XCarConfig"
require "config.XSkillConfig"
require "config.XPrivilegeConfig"
require "config.XCannonConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.prepareTime = 0
GameConfig.prestartTime = 12
GameConfig.gameTime = 0

GameConfig.startPlayers = 0

GameConfig.attackAddMoneyPars = 0
GameConfig.physicalStrength = false
GameConfig.GetMoneyFromSuccessfulCart = 0
GameConfig.closeServerTime = 10

function GameConfig:init(config)

    GameConfig.initPos = XTools:CastToVector(config.playerInitPosX, config.playerInitPosY, config.playerInitPosZ)

    self.prepareTime = tonumber(config.prepareTime) or 10
    self.prestartTime = tonumber(config.prestartTime) or 12
    self.gameTime = tonumber(config.gameTime) or 60
    self.startPlayers = tonumber(config.startPlayers) or 1
    self.closeServerTime = tonumber(config.closeServerTime) or 10
    self.attackAddMoneyPars = tonumber(config.attackAddMoneyPars) or 1
    self.physicalStrength = config.physicalStrength or false
    self.playerShopName = config.playerShopName
    self.basicHealth = tonumber(config.basicHealth) or 20
    self.addMoneyFromCart = tonumber(config.addMoneyFromCart) or 0
    self.playerActionTime = tonumber(config.playerActionWaitTime) or 0
    self.playExplosionWaitTime = tonumber(config.playExplosionWaitTime) or 0
    self.magmahurtValue = tonumber(config.magmahurtValue) or 1
    self.fishingRodDamage = tonumber(config.fishingRodDamage) or 2
    self.killPlayerAddMoney = tonumber(config.killPlayerAddMoney) or 0
    self.TntHitVlaue = tonumber(config.TntHitVlaue) or 20
    self.LongClockCarTime = tonumber(config.LongClockCarTime) or 20
   
    local callback = self.physicalStrength and function()
        HostApi.setNeedFoodStats(true)
        HostApi.setFoodHeal(true)
    end or function()
        HostApi.setNeedFoodStats(false)
        HostApi.setFoodHeal(false)
    end
    if type(callback) == 'function' then
        callback()
    end
    XMoneyConfig:OnInit(FileUtil.getConfigFromCsv("XMoneyConfig.csv"))
    XTeamConfig:OnInit(FileUtil.getConfigFromCsv("XTeamConfig.csv"))
    XInitItemConfig:OnInit(FileUtil.getConfigFromCsv("XInitItemConfig.csv"))
    XNpcShopConfig:OnInit(FileUtil.getConfigFromCsv("XNpcShopConfig.csv"))
    XNpcShopGoodsConfig:OnInit(FileUtil.getConfigFromCsv("XNpcShopGoodsConfig.csv"))
    XRespawnConfig:OnInit(FileUtil.getConfigFromCsv("XRespawnConfig.csv"))
    XAppShopPrivilege:OnInit(FileUtil.getConfigFromCsv("XAppShopPrivilege.csv"))
    XAppShopConfig:OnInit(FileUtil.getConfigFromCsv("XAppShopConfig.csv"))
    XTipsConfig:OnInit(FileUtil.getConfigFromCsv("XTipsConfig.csv"))
    XActorConfig:OnInit(FileUtil.getConfigFromCsv("XActorConfig.csv"))
    XPlayerShopItems:OnInit(FileUtil.getConfigFromCsv("XPlayerShopItems.csv"), self.playerShopName)
    XRailRouteConfig:OnInit(FileUtil.getConfigFromCsv("XRailRouteConfig.csv"))
    XCarBuffConfig:OnInit(FileUtil.getConfigFromCsv("XCarBuffConfig.csv"))
    XCarConfig:OnInit(FileUtil.getConfigFromCsv("XCarConfig.csv"))
    XSkillConfig:OnInit(FileUtil.getConfigFromCsv("XSkillConfig.csv"))
    XPrivilegeConfig:OnInit(FileUtil.getConfigFromCsv("XPrivilegeConfig.csv"))
    XCannonConfig:OnInit(FileUtil.getConfigFromCsv("XCannonConfig.csv"))
    AppProps:init("XAppProps.csv")

    XCannonConfig:prepareCannons()
end

return GameConfig