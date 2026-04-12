require "config.SellPreciousTipConfig"
require "config.ChristmasConfig"
require "config.GiftRewardConfig"
require "config.Define"
GameConfig = {}

function GameConfig:init()
    local config = FileUtil.getConfigFromYml("setting/luaconfig")
    SellPreciousTipConfig:initConfig(FileUtil.getConfigFromCsv("setting/SellPreciousTipConfig.csv"))
    self.onceBuyMoneyCost = tonumber(config.onceBuyMoneyCost or 60)
    self.maxBuyMoneyCost = tonumber(config.maxBuyMoneyCost or 600)
    self.onceMoneyNum = tonumber(config.onceMoneyNum or 100000)
    self.christmasFinishTime = tonumber(config.christmasFinishTime or 1578499200)
    self.isShowChristmasTime = tonumber(config.isShowChristmasTime or 0)

    ChristmasConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChristmasSetting.csv"))
    GiftRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/GiftRewardSetting.csv"))
end

function GameConfig:getItemIconData()
    local data = 
    {
        Id = 0,
        ItemId = 0,
        Meta = 0,
        Num = 1,
        Icon = "",
        Name = "",
        Detail = "",
        DataType = Define.ItemIcon.Common,
    }
    return data
end

function GameConfig:checkMoney(moneyType, price, callback)
    local money = PlayerWallet.getMoneyByCoinId(moneyType)
    if money < price then

        if moneyType == MoneyType.Diamonds then
            ClientHelper.onRecharge(1)
        elseif moneyType == MoneyType.Currency then
            GUISkyBlockProductBuyMoneyTips:onShowLackMoney()
        end

        callback(false)
    else
        callback(true)
    end
end

function GameConfig:checkIsInChiristmasTime()
    local last_time = self.christmasFinishTime - os.time()

    if last_time > 0 then
        return true
    end

    return false
end
return GameConfig
