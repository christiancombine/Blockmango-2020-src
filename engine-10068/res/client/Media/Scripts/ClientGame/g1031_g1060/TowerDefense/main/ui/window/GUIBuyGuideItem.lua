UIBuyGuideItem = class("UIBuyGuideItem", IGUIWindow)

UIBuyGuideItem.towerId = 0
UIBuyGuideItem.towerPrice = 0

function UIBuyGuideItem:onLoad()
    self.siEffect = self:getChildWindow(0, GUIType.StaticImage)
    self.llList = self:getChildWindow(1, GUIType.Layout)

    self.siTowerBg_1 = self:getChildWindow(0, GUIType.StaticImage, self.llList)
    self.siTowerBg_2 = self:getChildWindow(1, GUIType.StaticImage, self.llList)
    self.siTowerBg_3 = self:getChildWindow(2, GUIType.StaticImage, self.llList)

    self.siTowerIcon_1 = self:getChildWindow(0, GUIType.StaticImage, self.siTowerBg_1)
    self.siTowerIcon_2 = self:getChildWindow(0, GUIType.StaticImage, self.siTowerBg_2)
    self.siTowerIcon_3 = self:getChildWindow(0, GUIType.StaticImage, self.siTowerBg_3)

    self.siResultBg = self:getChildWindow(2, GUIType.StaticImage)
    self.siResultIcon = self:getChildWindow(0, GUIType.StaticImage, self.siResultBg)

    self.btUnLock = self:getChildWindow(3, GUIType.Button)

    self.btUnLock:registerEvent(GUIEvent.ButtonClick, function()
        local itemId = TowerStoreConfig:getItemIdByTowerId(self.towerId)
        GUIShopDetail:showDetailPage(itemId, true)
    end)
end

function UIBuyGuideItem:init(towerId, i)
    self.towerId = towerId
    self.index = i
    local config = TowerConfig:getConfigById(tonumber(towerId))

    self.siTowerBg_1:SetImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))
    self.siTowerBg_2:SetImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))
    self.siTowerBg_3:SetImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))

    self.siTowerIcon_1:SetImage(config.FormulaImage1)
    self.siTowerIcon_2:SetImage(config.FormulaImage2)
    self.siTowerIcon_3:SetImage(config.FormulaImage3)

    self.siResultBg:SetImage(IconMapConfig:getStarsBackground(config.StarLevel))
    self.siResultIcon:SetImage(config.IconName)
end

return UIBuyGuideItem
