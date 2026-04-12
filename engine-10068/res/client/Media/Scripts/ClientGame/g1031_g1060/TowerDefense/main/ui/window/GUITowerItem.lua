UITowerItem = class("UITowerItem", IGUIWindow)

UITowerItem.towerId = 0
UITowerItem.towerPrice = 0

function UITowerItem:onLoad()
    self.siBg = self:getChildWindow(0, GUIType.StaticImage)
    self.siTower = self:getChildWindow(1, GUIType.StaticImage)
    self.stPrice = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(0, GUIType.Layout, self.siTower))
    self.stStars = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(1, GUIType.Layout, self.siTower))


    self.btSelect = self:getChildWindow(2, GUIType.Button)
    self.siSelectBox = self:getChildWindow(3, GUIType.StaticImage)
    self.siStarsEffect = self:getChildWindow(4, GUIType.StaticImage)

    self.btSelect:registerEvent(GUIEvent.ButtonClick, function()
        GUIMain:updateSelectTowerItem(self.towerId)
    end)

    self.siStarsEffect:SetVisible(false)
end

function UITowerItem:init(towerId, i)
    self.towerId = towerId
    self.index = i
    local config = TowerConfig:getConfigById(tonumber(towerId))
    if config ~= nil then
        self.towerPrice = config.BuildPrice
        self.siBg:SetImage(IconMapConfig:getStarsBackground(config.StarLevel))
        self.siTower:SetImage(config.IconName)
        self.stPrice:SetText(config.BuildPrice)
        self.stStars:SetText(config.StarLevel)
        self.siSelectBox:SetVisible(false)
        self.siTower:SetVisible(true)
    end
end

function UITowerItem:updateItem(towerId)
    self.towerId = towerId
    local config = TowerConfig:getConfigById(tonumber(towerId))
    if config ~= nil then
        self.towerPrice = config.BuildPrice
        self.siBg:SetImage(IconMapConfig:getStarsBackground(config.StarLevel))
        self.siTower:SetImage(config.IconName)
        self.stPrice:SetText(config.BuildPrice)
        self.stStars:SetText(config.StarLevel)
        self.siSelectBox:SetVisible(true)
        self.siTower:SetVisible(true)
    end
end

function UITowerItem:isSelect(towerId)
    self.siSelectBox:SetVisible(self.towerId == towerId)
end

function UITowerItem:updateStatus(money)
    if tonumber(self.towerPrice) > tonumber(money) then
        self.stPrice:SetTextColor(Color.RED)
    else
        self.stPrice:SetTextColor(Color.WHITE)
    end
end

return UITowerItem
