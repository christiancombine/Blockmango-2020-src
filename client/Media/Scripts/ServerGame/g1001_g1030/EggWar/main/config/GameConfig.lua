--GameConfig.lua
require "config.MerchantConfig"
require "config.TeamConfig"
require "config.MoneyConfig"
require "config.BridgeConfig"
require "config.TeleportConfig"
require "config.ShopConfig"
require "config.RespawnConfig"
require "config.BlockConfig"
require "config.SkillConfig"
require "config.KeepItemConfig"
require "config.AppPropConfig"
require "config.CenterMerchantConfig"

GameConfig = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.maxLevel = 0
GameConfig.updateSuccess = ""
GameConfig.updateFailed = ""
GameConfig.updateSound = 0
GameConfig.standUpTime = 0

GameConfig.sppItems = {}
GameConfig.clzItems = {}

function GameConfig:init(config)

    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.maxLevel = tonumber(config.maxLevel)
    self.updateSuccess = tostring(config.updateSuccess)
    self.updateFailed = tostring(config.updateFailed)
    self.updateSound = tonumber(config.updateSound)
    self.standTime = tonumber(config.standTime)
    self.enchantmentEquip = config.enchantmentEquip
    self.enchantmentEffect = config.enchantmentEffect
    self.enchantmentConsume = tonumber(config.enchantmentConsume)
    self.enchantmentOpenTime = tonumber(config.enchantmentOpenTime)
    self.enchantmentQuickMoney = tonumber(config.enchantmentQuickMoney)

    local sppi = 0
    for i, v in pairs(config.sppItems) do
        sppi = sppi + 1
        self.sppItems[sppi] = {}
        self.sppItems[sppi].id = tonumber(i)
        self.sppItems[sppi].num = tonumber(v)
    end

    for i, v in pairs(config.clzItems) do
        self.clzItems[i] = {}
        local clzi = 0
        for si, sv in pairs(v) do
            clzi = clzi + 1
            self.clzItems[i][clzi] = {}
            self.clzItems[i][clzi].id = tonumber(si)
            self.clzItems[i][clzi].num = tonumber(sv)
        end
    end

    TeamConfig:initTeams(config.teams)
    MerchantConfig:initGoods(config.storeInfo.goods)
    MerchantConfig:initShopTruck(config.shopTruck)
    MoneyConfig:init(config.money)
    MoneyConfig:initCoinMapping(config.coinMapping)
    BridgeConfig:init(config.bridge)
    TeleportConfig:init(config.teleport)
    ShopConfig:init(config.shop)
    RespawnConfig:init(config.respawn)
    BlockConfig:init(config.block)
    CenterMerchantConfig:init(config.centerMerchant)

    SkillConfig:init(FileUtil.getConfigFromCsv("SkillConfig.csv"))
    KeepItemConfig:init(FileUtil.getConfigFromCsv("KeepItem.csv"))
    AppPropConfig:init(FileUtil.getConfigFromCsv("AppProps.csv"))
    EnchantMentNpcConfig:init(FileUtil.getConfigFromCsv("EnchantmentNpc.csv"))

end

function GameConfig:canEnchantment(id)
    for _, item in pairs(self.enchantmentEquip) do
        if tonumber(item) == id then
            return true
        end
    end
    return false
end

function GameConfig:getEnchantmentEffect(index)
    for idx, item in pairs(self.enchantmentEffect) do
        if index == idx then
            return item.id, item.level
        end
    end
    return 0, 0
end

function GameConfig:getEnchantmentEffectDes(index)
    for idx, item in pairs(self.enchantmentEffect) do
        if index == idx then
            return item.title, item.des
        end
    end
    return "", ""
end

return GameConfig