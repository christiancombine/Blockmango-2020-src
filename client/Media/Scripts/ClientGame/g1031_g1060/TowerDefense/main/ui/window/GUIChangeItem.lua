UIChangeItem = class("UIBookItem", IGUIWindow)

UIChangeItem.itemId = 0

function UIChangeItem:onLoad()
    self.siBackground = self:getChildWindow(0, GUIType.StaticImage)
    self.siItem = self:getChildWindow(1, GUIType.StaticImage)
    self.stStars = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(2, GUIType.Layout))
    self.stName = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(3, GUIType.StaticImage))
    self.btSelect = self:getChildWindow(4)

    self.btSelect:registerEvent(GUIEvent.ButtonClick, function()
        GUITowerDefenseChangeTower:onClickItem(self.itemId)
    end)
end

function UIChangeItem:initInfo(ItemId)
    self.itemId = ItemId
    local config = TowerConfig:getConfigById(tonumber(ItemId))
    self.siBackground:SetImage(IconMapConfig:getStarsBackground(config.StarLevel))
    self.siItem:SetImage(config.IconName)

    self.stName:SetText(Lang:getString(config.Name))
    self.stStars:SetText(config.StarLevel)
end

return UIChangeItem