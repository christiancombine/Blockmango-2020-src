require "ui.window.GUIShopItem"

local UIShop = class("UIShop", IGUILayout)

function UIShop:onLoad()
    self.btnBack = self:getChildWindow("TowerDefenseHallShop-page_back", GUIType.Button)
    self.siGameMoneyImage = self:getChildWindow("TowerDefenseHallShop-page_money_1_icon", GUIType.StaticImage)
    self.stGameMoneyNum = self:getChildWindow("TowerDefenseHallShop-page_money_1_text", GUIType.StaticText)
    self.siDiamondsImage = self:getChildWindow("TowerDefenseHallShop-page_money_2_icon", GUIType.StaticImage)
    self.stDiamondsNum = self:getChildWindow("TowerDefenseHallShop-page_money_2_text", GUIType.StaticText)

    self:initItemList()
    self:initRegister()

    self:getChildWindow("TowerDefenseHallShop-page_title_text", GUIType.StaticText):SetText(Lang:getString("gui_shoptitle"))
end

function UIShop:initRegister()
    self.btnBack:registerEvent(GUIEvent.ButtonClick, function()
        self:close()
    end)

    self:registerEvent(UpdateMoneyEvent, function()
        self:updateMoneyNum()
    end)

    self:registerEvent(GetNewTowerEvent, function()
        self:updateItemList()
    end)

    self:registerEvent(ClickSelfTowerEvent, function(towerId)
        self:showItemDetail(towerId)
    end)

    self:registerDataCallBack("BuySuccess", function(data)
        self:onGetBuySuccess(data)
    end, DataBuilderProcessor)
end

function UIShop:updateMoneyNum()
    self.stGameMoneyNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
    self.stDiamondsNum:SetText(GamePlayer:Instance().Wallet:getDiamondsGolds())
end

function UIShop:showShop()
    GameAnalytics:design(1, { "H_shop" })
    self:updateItemList()
    self.stDiamondsNum:SetText(GamePlayer:Instance().Wallet:getDiamondsGolds())
    self.stGameMoneyNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
    self:show()
end

function UIShop:close()
    self:hide()
end

function UIShop:initItemList()
    local llItemList = self:getChildWindow("TowerDefenseHallShop-goodslist")
    self.itemList = IGUIGridView.new("TowerDefenseHallShop-Items", llItemList)
    self.itemList:setArea({ 0, 13 }, { 0, 14 }, { 1, -26 }, { 1, -28 })
    local width = self.itemList:getWidth()
    local itemWidth = (width - 13 * 6) / 7
    self.itemList:setConfig(13, 13, 7)
    self.itemList.root:SetHorizontalAlignment(HorizontalAlignment.Left)
    self.itemList.root:SetVerticalAlignment(VerticalAlignment.Top)

    local shopConfig = TowerStoreConfig:getShopConfigSeq()
    if shopConfig ~= nil then
        for _, config in pairs(shopConfig) do
            local shopItem = UIShopItem.new("TowerDefenseHallShopItem.json")
            shopItem:setWidth({ 0, itemWidth })
            shopItem:setHeight({ 0, itemWidth * 200 / 149 })  -- 200 / 149 为Item长宽比
            shopItem:initInfo(config.id)
            self.itemList:addItem(shopItem)
        end
    end
end

function UIShop:updateItemList()
    local shopConfig = TowerStoreConfig:getShopConfigSeq()
    local towerList = GamePlayer:Instance():getOwnedTowerList()
    for _, config in pairs(shopConfig) do
        local item = self.itemList:getItem("itemId", config.id)
        if towerList[config.towerId] ~= nil then
            item:updateStatus(towerList[config.towerId].isHave)
            if item.isHave == true then
                self.itemList:removeItem(item)
                self.itemList:addItem(item)
            end
        end
    end
end

function UIShop:showItemDetail(towerId)
    local shopConfig = TowerStoreConfig:getAllShopConfig()
    for _, config in pairs(shopConfig) do
        if towerId == config.towerId then
            GUIShopDetail:showDetailPage(config.id)
        end
    end
end

function UIShop:onGetBuySuccess(data)
    self.stDiamondsNum:SetText(GamePlayer:Instance().Wallet:getDiamondsGolds())
    self.stGameMoneyNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
end

return UIShop