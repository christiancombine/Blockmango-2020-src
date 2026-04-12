UIShopItem = class("UIShopItem", IGUIWindow)

UIShopItem.itemId = 0

function UIShopItem:onLoad()
    self.siBackground = self:getChildWindow(0, GUIType.StaticImage)
    self.siItem = self:getChildWindow(1, GUIType.StaticImage)
    self.stStars = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(2, GUIType.Layout))
    self.stName = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(3, GUIType.StaticImage))
    self.siPriceBg = self:getChildWindow(4, GUIType.StaticImage)
    self.siMoneyIcon = self:getChildWindow(0, GUIType.StaticImage, self.siPriceBg)
    self.stPrice = self:getChildWindow(1, GUIType.StaticText, self.siPriceBg)
    self.siOwned = self:getChildWindow(5, GUIType.StaticImage)
    self.stOwned = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(5, GUIType.StaticImage))
    self.btSelect = self:getChildWindow(6, GUIType.Button)

    self.btSelect:registerEvent(GUIEvent.ButtonClick, function()
        GUIShopDetail:showDetailPage(self.itemId, false)
    end)
end

function UIShopItem:initInfo(ItemId)
    self.itemId = ItemId
    local config = TowerStoreConfig:getConfigById(tonumber(ItemId))
    local towerConfig = TowerConfig:getConfigById(tonumber(config.towerId))

    if config.moneyType == Define.Money.DiamondsGold then
        self.siMoneyIcon:SetImage(PayHelper.getMoneyIcon(config.moneyType))
    elseif config.moneyType == Define.Money.HallGold then
        self.siMoneyIcon:SetImage("set:tower_defense_hall_main.json image:hall_money")
    end

    self.siBackground:SetImage(IconMapConfig:getStarsBackground(towerConfig.StarLevel))
    self.siItem:SetImage(towerConfig.IconName)

    self.stPrice:SetText(config.price)
    self.stName:SetText(Lang:getString(towerConfig.Name))
    self.stStars:SetText(towerConfig.StarLevel)
    self.stOwned:SetText(Lang:getString("gui_own"))
    self.siOwned:SetVisible(false)
end

function UIShopItem:updateStatus(isHave)
    if isHave then
        self.siOwned:SetVisible(true)
        self.siPriceBg:SetVisible(false)
    else
        self.siOwned:SetVisible(false)
        self.siPriceBg:SetVisible(true)
    end
end

return UIShopItem