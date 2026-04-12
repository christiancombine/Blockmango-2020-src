UIBulletinTabItem = class("UIBulletinTabItem", IGUIWindow)

function UIBulletinTabItem:onLoad()

    print("UIBulletinTabItem:onLoad()")
    self.radioBtn = self:getChildWindow(0 , GUIType.RadioButton)
    self.radioBtn:registerEvent(GUIEvent.RadioStateChanged, function()
        self:onRadioStateChangedEvent()
    end)
    self.radioText = self:getChildWindow(1 , GUIType.StaticText)
    self.radioRedImg = self:getChildWindow(2 , GUIType.StaticImage)
    self.radioRedImg:SetVisible(false)
    self.data = {}
end

function UIBulletinTabItem:onRadioStateChangedEvent()
    if self.radioBtn:IsSelected() then
        GUIBulletin:onSelectRadio(self.data.GroupId)
    end
end

function UIBulletinTabItem:setData(data)
    self.data = data
    self.radioText:SetText(Lang:getString(self.data.GroupName))
end
