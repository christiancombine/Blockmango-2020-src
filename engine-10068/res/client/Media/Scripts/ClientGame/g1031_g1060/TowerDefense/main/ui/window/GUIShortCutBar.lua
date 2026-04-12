require "ui.layout.GUIBuyHallGoldContent"
UIShortCutBar = class("UIShortCutBar", IGUIWindow)

UIShortCutBar.Status = Define.ShortCutBarStatus.Lock
UIShortCutBar.towerId = 0
UIShortCutBar.towerPrice = 0
UIShortCutBar.payItemId = 0
UIShortCutBar.isBarLock = true
UIShortCutBar.isNotCloseTip = false

function UIShortCutBar:onLoad()
    self.llTowerBg = self:getChildWindow(0, GUIType.Layout)
    self.btAdd = self:getChildWindow(0, GUIType.Button, self:getChildWindow(0, GUIType.Layout, self.llTowerBg))
    self.btUnlock = self:getChildWindow(1, GUIType.Button, self:getChildWindow(0, GUIType.Layout, self.llTowerBg))
    self.siMoney = self:getChildWindow(0, GUIType.StaticImage, self.btUnlock)
    self.stPrice = self:getChildWindow(1, GUIType.StaticText, self.btUnlock)

    self.btUnlock:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickUnlock()
    end)

    self.btAdd:registerEvent(GUIEvent.ButtonClick, function()
        GUITowerDefenseChangeTower:showPage(self)
    end)

    self.llTowerBg:registerEvent(GUIEvent.TouchUp, function()
        GUITowerDefenseChangeTower:showPage(self)
    end)

    self:registerDataCallBack("ShortCutBarSetting", function(data)
        self:initShortCutBarSetting(data)
    end, JsonBuilderProcessor)
end

function UIShortCutBar:onClickUnlock(towerId)
    local config = PayStoreConfig:getConfigById(self.payItemId)
    local listener = function()
        if config.moneyType == Define.Money.DiamondsGold then
            PayHelper.checkMoney(MoneyType.Diamonds, config.price, function(isEnough)
                if isEnough then
                    self:onMoneyEnough(self.payItemId, towerId)
                    self.isNotCloseTip = false
                else
                    self.isNotCloseTip = true
                end
            end)
        elseif config.moneyType == Define.Money.HallGold then
            if GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold) >= config.price then
                self:onMoneyEnough(self.payItemId, towerId)
                self.isNotCloseTip = false
            else
                self:onMoneyNotEnough(self.payItemId, config.price - GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
                self.isNotCloseTip = true
            end
        end
        return self.isNotCloseTip
    end

    local moneyIcon
    if config.moneyType == Define.Money.DiamondsGold then
        moneyIcon = PayHelper.getMoneyIcon(config.moneyType)
    elseif config.moneyType == Define.Money.HallGold then
        moneyIcon = "set:tower_defense_hall_main.json image:hall_money"
    end

    CustomDialog.builder()
                .setTitleText(Lang:getString("gui_title_shortcutbar"))
                .setContentText(Lang:getString("gui_tip_shortcutbar"))
                .setRightText(Lang:getString("gui_TowerDefenseDress-sure"))
                .setRightClickListener(listener)
                .setHideLeftButton()
                .setExtraInfo(moneyIcon, config.price)
                .show()
end

function UIShortCutBar:onMoneyEnough(itemId, towerId)
    GamePacketSender:sendBuyPayItem(itemId, 1)
    self.towerId = towerId
end

function UIShortCutBar:onMoneyNotEnough(itemId, needMoneyNum)
    local DiamondsNum = math.ceil(needMoneyNum / GamePlayer:Instance().ExchangeRate)

    BuyHallGoldContent:setCurrencyInfo(needMoneyNum, DiamondsNum, DiamondsNum * GamePlayer:Instance().ExchangeRate)
    local listener = function()
        GamePacketSender:sendExchangeHallGold(itemId, DiamondsNum)
    end

    CustomDialog.builder()
                .setTitleText(Lang:getString("lack_of_money"))
                .setContentView(BuyHallGoldContent)
                .setRightText(Lang:getString("gui_TowerDefenseDress-sure"))
                .setRightClickListener(listener)
                .setHideLeftButton()
                .show()
end

function UIShortCutBar:init(itemId)
    local config = PayStoreConfig:getConfigById(tonumber(itemId))

    self.payItemId = itemId
    self.towerPrice = config.BuildPrice
    self.stPrice:SetText(config.price)

    if config.moneyType == Define.Money.DiamondsGold then
        self.siMoney:SetImage(PayHelper.getMoneyIcon(config.moneyType))
    elseif config.moneyType == Define.Money.HallGold then
        self.siMoney:SetImage("set:tower_defense_hall_main.json image:hall_money")
    end

    self.Item = UITowerItem.new("TowerDefenseShortCutItem.json")
    self.Item:setArea({ 0.025, 0 }, { 0.0435, 0 }, { 0.532, 0 }, { 0.913, 0 })
    self.Item.root:SetVisible(false)
    self.root:AddChildWindow(self.Item.root)
end

function UIShortCutBar:initShortCutBarSetting(data)
    if data == nil then
        return
    end
    local towerId = data[tostring(self.payItemId)]
    if towerId ~= nil and towerId ~= 0 then
        self.towerId = towerId
        self:updateShortCutBar(Define.ShortCutBarStatus.HaveTower)
        self.Item:init(towerId, self.payItemId)
        self:updateStatus(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))
    end
end

function UIShortCutBar:setUnlock()
    self.isBarLock = false
end

function UIShortCutBar:updateShortCutBar(Status)
    if Status == Define.ShortCutBarStatus.NotTower then
        if self.towerId ~= 0 then
            self:onChoseTower(self.towerId)
            return
        end
    end

    if Status == Define.ShortCutBarStatus.Lock then
        self.btAdd:SetVisible(false)
        self.btUnlock:SetVisible(true)
    elseif Status == Define.ShortCutBarStatus.NotTower then
        self.btAdd:SetVisible(true)
        self.btUnlock:SetVisible(false)
        self.btAdd:SetNormalImage("set:tower_defense_main.json image:add_bt")
        self.btAdd:SetPushedImage("set:tower_defense_main.json image:add_bt")
        self.llTowerBg:SetBackImage("set:tower_defense_main.json image:shortcutbarnull_bg")
    elseif Status == Define.ShortCutBarStatus.HaveTower then
        self.btAdd:SetVisible(true)
        self.btUnlock:SetVisible(false)
        self.btAdd:SetNormalImage("set:tower_defense_main.json image:change_bt")
        self.btAdd:SetPushedImage("set:tower_defense_main.json image:change_bt")
        self.Item.root:SetVisible(true)
        GamePacketSender:sendSetShortCurBar(self.payItemId, self.towerId)
    end
end

function UIShortCutBar:isSelect(towerId)
    if self.Item then
        self.Item:isSelect(towerId)
    end
end

function UIShortCutBar:onChoseTower(towerId)
    self.towerId = towerId
    self:updateShortCutBar(Define.ShortCutBarStatus.HaveTower)
    self.Item:updateItem(towerId)
    GUIMain:updateSelectTowerItem(towerId)
    self:updateStatus(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))
    GamePlayer:Instance():checkAllTowerItem()
end

function UIShortCutBar:updateStatus(money)
    if self.Item then
        self.Item:updateStatus(money)
    end
end

return UIShortCutBar
