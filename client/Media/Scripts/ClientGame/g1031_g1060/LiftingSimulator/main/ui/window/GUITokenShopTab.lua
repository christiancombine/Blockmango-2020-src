
GUITokenShopTab = class("GUITokenShopTab", IGUIWindow)

--initTab类型
TokenTabKind = {
    NotKind = 0,
    FitnessEquip = 1,
    Gene = 2,
    Skin = 3,
    Skill = 4,
}

--选中状态
TokenTabSelectStatus = {
    NotSelect = 1,
    Select = 2,
}

function GUITokenShopTab:onLoad()
    self.tabId = 0
    self.kind = TokenTabKind.NotKind
    self.selectMode = TokenTabSelectStatus.NotSelect
    self.llTabs = self:getChildWindow(0, GUIType.Layout)
    self.ivTabsBg = self:getChildWindow(0, GUIType.StaticImage, self.llTabs)
    self.ivTabsIcon = self:getChildWindow(0, GUIType.StaticImage, self.ivTabsBg)
    self.tvTabsText = self:getChildWindow(1, GUIType.StaticText, self.ivTabsBg)
end

function GUITokenShopTab:initTab(TabId, Kind, Bg, Icon, Text)
    self.tabId = TabId
    self.kind = Kind
    self.ivTabsBg:SetImage(Bg)
    self.ivTabsIcon:SetImage(Icon)
    self.tvTabsText:SetText(Lang:getString(Text))
end

function GUITokenShopTab:changeSelectStatus(Bg, SelectMode)
    self.ivTabsBg:SetImage(Bg)
    self.selectMode = SelectMode
end

return GUITokenShopTab