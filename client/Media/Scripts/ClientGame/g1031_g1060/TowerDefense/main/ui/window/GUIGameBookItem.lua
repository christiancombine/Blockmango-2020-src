UIBookItem = class("UIBookItem", IGUIWindow)

UIBookItem.itemId = 0

function UIBookItem:onLoad()
    self.siBackground = self:getChildWindow(0, GUIType.StaticImage)
    self.siItem = self:getChildWindow(1, GUIType.StaticImage)
    self.stStars = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(2, GUIType.Layout))
    self.stName = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(3, GUIType.StaticImage))
    self.siNew = self:getChildWindow(4)
    self.siShadow = self:getChildWindow(5, GUIType.StaticImage)
    self.btSelect = self:getChildWindow(6)

    self.btSelect:registerEvent(GUIEvent.ButtonClick, function()
        GUIGameBookDetail:showDetailPage(self.itemId)
    end)
end

function UIBookItem:initInfo(ItemId)
    self.itemId = ItemId
    local config = TowerConfig:getConfigById(tonumber(ItemId))
    self.siBackground:SetImage(IconMapConfig:getStarsBackground(config.StarLevel))
    self.siItem:SetImage(config.UnKnowIconName)

    self.stName:SetText(Lang:getString(config.Name))
    self.stStars:SetText(config.StarLevel)
    self.siNew:SetVisible(false)
end

function UIBookItem:updateStatus(isHave, isHaveBook, isNew)
    local config = TowerConfig:getConfigById(self.itemId)
    if isHave then
        self.siShadow:SetVisible(false)
    end
    if isHaveBook then
        self.siItem:SetImage(config.IconName)
        if isNew then
            self.siNew:SetVisible(true)
        else
            self.siNew:SetVisible(false)
        end
    end
end

return UIBookItem