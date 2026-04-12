UISupportItem = class("UISupportItem", IGUIWindow)

function UISupportItem:onLoad()
    self.stName = self:getChildWindow(0, GUIType.StaticText)
    self.siMoneyBg = self:getChildWindow(1, GUIType.StaticImage)
    self.stMoney = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(1, GUIType.StaticImage))
    self.btMoneySub = self:getChildWindow(2, GUIType.Button, self:getChildWindow(1, GUIType.StaticImage))
    self.btMoneyAdd = self:getChildWindow(3, GUIType.Button, self:getChildWindow(1, GUIType.StaticImage))
    self.btHandsel = self:getChildWindow(2, GUIType.Button)

    self.moneyNum = 0
    self.supportNum = 0

    self:updateLanguage()
    self:initRegister()
end

function UISupportItem:initRegister()
    self.btMoneyAdd:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAdd()
    end)

    self.btMoneySub:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickSub()
    end)

    self.btHandsel:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickHandsel()
    end)

end

function UISupportItem:updateLanguage()
    self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(2, GUIType.Button)):SetText(Lang:getString("gui_TowerDefenseHandselButton"))
end

function UISupportItem:initInfo(i)
    self.index = i
end

function UISupportItem:updateInfo(userId, name, i)
    self.index = i
    self.stName:SetText(name)
    self.userId = userId
end

function UISupportItem:initItem()
    self.moneyNum = 0
    self.supportNum = 0
    self.stMoney:SetText(0)
end

function UISupportItem:onClickAdd()
    if self.moneyNum >= 100 then
        self.supportNum = self.supportNum + 100
        AddMoneyToMateEvent:invoke(100)
    else
        self.supportNum = self.supportNum + self.moneyNum
        AddMoneyToMateEvent:invoke(self.moneyNum)
    end
    self.stMoney:SetText(self.supportNum)
end

function UISupportItem:onClickSub()
    if self.supportNum >= 100 then
        self.supportNum = self.supportNum - 100
        AddMoneyToMateEvent:invoke(-100)
    else
        AddMoneyToMateEvent:invoke(-self.supportNum)
        self.supportNum = 0
    end
    self.stMoney:SetText(self.supportNum)
end

function UISupportItem:ChangeSupportNum(totalNum)
    self.moneyNum = totalNum
    if self.userId == nil then
        self.btMoneyAdd:SetEnabled(false)
        self.btMoneyAdd:SetNormalImage("set:tower_defense_support.json image:add_disable")
        self.btMoneySub:SetEnabled(false)
        self.btMoneySub:SetNormalImage("set:tower_defense_support.json image:sub_disable")
        self.btHandsel:SetEnabled(false)
        return
    end

    if totalNum ~= 0 then
        self.btMoneyAdd:SetEnabled(true)
        self.btMoneyAdd:SetNormalImage("set:tower_defense_support.json image:add_off")
    else
        self.btMoneyAdd:SetEnabled(false)
        self.btMoneyAdd:SetNormalImage("set:tower_defense_support.json image:add_disable")
    end

    if self.supportNum ~= 0 then
        self.btMoneySub:SetEnabled(true)
        self.btMoneySub:SetNormalImage("set:tower_defense_support.json image:sub_off")
    else
        self.btMoneySub:SetEnabled(false)
        self.btMoneySub:SetNormalImage("set:tower_defense_support.json image:sub_disable")
    end

end

function UISupportItem:onClickHandsel()
    if self.supportNum <= 0 then
        return
    end

    GamePacketSender:sendSupportTeammate(self.userId, self.supportNum)
    GUISupport:hide()
end

return UISupportItem