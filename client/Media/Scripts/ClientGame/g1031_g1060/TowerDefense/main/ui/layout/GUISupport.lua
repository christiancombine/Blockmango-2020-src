require "ui.window.GUISupportItem"

local UISupport = class("UISupport", IGUILayout)

UISupport.Money = 0
UISupport.TeammateNum = 1

function UISupport:onLoad()
    self:updateLanguage()
    self:initSupport()
    self:initRegister()
end

function UISupport:updateLanguage()
    self:getChildWindow("TowerDefenseSupport-titlename", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseHandselGoldTitle"))
end

function UISupport:initRegister()
    self:registerDataCallBack("PlayerList", function(data)
        self:initTeammate(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("PlayAgain", function()
        self:resetTeammate()
    end)

    self.btClose:registerEvent(GUIEvent.ButtonClick, function()
        self:CloseSupport()
    end)

    self:registerEvent(AddMoneyToMateEvent, function(moneyNum)
        self:onAddMoneyToMate(moneyNum)
    end)
end

function UISupport:initSupport()
    local llSupport = self:getChildWindow("TowerDefenseSupport-window")

    self.btClose = self:getChildWindow("TowerDefenseSupport-CloseButton", GUIType.Button)

    self.gvSupportInfo = IGUIGridView.new("TowerDefenseSupport-supportview", llSupport)
    self.gvSupportInfo:setArea({ 0, 0 }, { 0, 65 }, { 1, -34 }, { 1, 0 })
    self.gvSupportInfo:setConfig(0, 14, 1)
    self.gvSupportInfo.root:SetHorizontalAlignment(HorizontalAlignment.Center)
    self.gvSupportInfo.root:SetVerticalAlignment(VerticalAlignment.Center)

    for i = 1, 3, 1 do
        local supportItem = UISupportItem.new("TowerDefenseSupportItem.json")
        supportItem:setWidth({ 0, 534 })
        supportItem:setHeight({ 0, 76 })
        supportItem:initInfo(i)
        self.gvSupportInfo:addItem(supportItem)
    end
end

function UISupport:initTeammate(data)
    GUIMain.btnSupport:SetVisible(true)
    local userId = data.userId
    local name = data.name
    if userId ~= tostring(GamePlayer:Instance().userId) then
        self.gvSupportInfo:getItem("index", self.TeammateNum):updateInfo(userId, name, self.TeammateNum)
        self.TeammateNum = self.TeammateNum + 1
    end
end

function UISupport:resetTeammate()
    GUIMain.btnSupport:SetVisible(false)
    self.gvSupportInfo:clearItems()
    self.TeammateNum = 1

    for i = 1, 3, 1 do
        local supportItem = UISupportItem.new("TowerDefenseSupportItem.json")
        supportItem:setWidth({ 0, 534 })
        supportItem:setHeight({ 0, 76 })
        supportItem:initInfo(i)
        self.gvSupportInfo:addItem(supportItem)
    end
end

function UISupport:showSupport()
    GameAnalytics:design(1, { "G_transaction" })
    self.Money = GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold)
    for i = 1, 3, 1 do
        self.gvSupportInfo:getItem("index", i):initItem()
    end
    self:onAddMoneyToMate(0)
    self:show()
end

function UISupport:CloseSupport()
    self:hide()
end

function UISupport:onAddMoneyToMate(moneyNum)
    self.Money = self.Money - moneyNum
    for i = 1, 3, 1 do
        self.gvSupportInfo:getItem("index", i):ChangeSupportNum(self.Money)
    end
end

return UISupport


