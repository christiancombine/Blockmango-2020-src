require "ui.window.GUIBookItem"

local UIBook = class("UIBook", IGUILayout)

function UIBook:onLoad()
    self.btnBack = self:getChildWindow("TowerDefenseBook-page_back", GUIType.Button)
    self:initItemList()
    self:initRegister()
    self:getChildWindow("TowerDefenseBook-page_title_text", GUIType.StaticText):SetText(Lang:getString("gui_booktitle"))
end

function UIBook:initRegister()
    self.btnBack:registerEvent(GUIEvent.ButtonClick, function()
        self:close()
    end)
end

function UIBook:showBook()
    GameAnalytics:design(1, { "H_book" })
    self:updateItemList()
    self:show()
end

function UIBook:close()
    self:hide()
end

function UIBook:initItemList()
    local llBookList = self:getChildWindow("TowerDefenseBook-goodslist")
    self.itemList = IGUIGridView.new("TowerDefenseBook-Items", llBookList)
    self.itemList:setArea({ 0, 12 }, { 0, 14 }, { 1, -24 }, { 1, -28 })
    local width = self.itemList:getWidth()
    local itemWidth = (width - 12 * 6) / 7
    self.itemList:setConfig(12, 14, 7)
    self.itemList.root:SetHorizontalAlignment(HorizontalAlignment.Left)
    self.itemList.root:SetVerticalAlignment(VerticalAlignment.Top)

    local bookConfig = TowerConfig:getShowSeq()
    if bookConfig ~= nil then
        for _, config in pairs(bookConfig) do
            local bookInfo = UIBookItem.new("TowerDefenseBookItem.json")
            bookInfo:setWidth({ 0, itemWidth })
            bookInfo:setHeight({ 0, itemWidth * 171 / 154 }) -- 171 / 154 为Item长宽比
            bookInfo:initInfo(config.Id)
            bookInfo.root:SetTouchable(false)
            self.itemList:addItem(bookInfo)
        end
    end
end

function UIBook:updateItemList()
    local bookConfig = TowerConfig:getShowSeq()
    local towerList = GamePlayer:Instance():getOwnedTowerList()
    for _, config in pairs(bookConfig) do
        if towerList[config.Id] ~= nil then
            local item = self.itemList:getItem("itemId", towerList[config.Id].towerId)
            item:updateStatus(towerList[config.Id].isHave, towerList[config.Id].isHaveBook, towerList[config.Id].isNew)
        end
    end
end

function UIBook:hideItemNewIcon(itemId)
    self.itemList:getItem("itemId", itemId):updateStatus(false)
end

return UIBook