local UIBulletin = class("UIBulletin", IGUILayout)

function UIBulletin:onLoad()
    self:onLoadInitUI()
end

UIBulletin.tabType =
{
    ActivityTab = 1,
    SystemTab = 2,
}

function UIBulletin:onLoadInitUI()
    HostApi.log("UIBulletin onLoadInitUI")
    self.btnActivity = self:getChildWindow("SkyBlockBulletin-Activity-Bulletin", GUIType.RadioButton)
    self.btnSystem = self:getChildWindow("SkyBlockBulletin-System-Bulletin", GUIType.RadioButton)
    self.activityRedImg = self:getChildWindow("SkyBlockBulletin-Activity-Red", GUIType.StaticImage)
    self.systemRedImg = self:getChildWindow("SkyBlockBulletin-System-Red", GUIType.StaticImage)
    self.activityRedImg:SetVisible(false)
    self.systemRedImg:SetVisible(false)

    self.gleActivityImg = self:getChildWindow("SkyBlockBulletin-Activity-Tab_Triangle", GUIType.StaticImage)
    self.gleSystemImg = self:getChildWindow("SkyBlockBulletin-System-Tab_Triangle", GUIType.StaticImage)
    self.gleActivityImg:SetVisible(false)
    self.gleSystemImg:SetVisible(false)

    self.btnClose = self:getChildWindow("SkyBlockBulletin-BtnClose", GUIType.Button)
    self.btnClose:registerEvent(GUIEvent.ButtonClick, function()
        self:onClosePanel()
    end)

    self.btnActivity:SetText(Lang:getString("gui_activity_bulletin"))
    self.btnActivity:registerEvent(GUIEvent.RadioStateChanged, function()
        self:onMainRadioChanged(self.btnActivity, UIBulletin.tabType.ActivityTab)
    end)

    self.btnSystem:SetText(Lang:getString("gui_system_bulletin"))
    self.btnSystem:registerEvent(GUIEvent.RadioStateChanged, function()
        self:onMainRadioChanged(self.btnSystem, UIBulletin.tabType.SystemTab)
    end)


    self.llLeftTab = self:getChildWindow("SkyBlockBulletin-Left-Tab", GUIType.Layout)
    self.LeftTabListView = IGUIListView.new("SkyBlockBulletin-left-ListView", self.llLeftTab)
    self.LeftTabListView:setItemSpace(12)


    self.llDetail = self:getChildWindow("SkyBlockBulletin-Content", GUIType.Layout)
    self.detailListView = IGUIListView.new("SkyBlockBulletin-Detail-ListView", self.llDetail)
    self.silder = self:getChildWindow("SkyBlockBulletin-Slider", GUIType.Slider)

    self.detailListView.root:registerEvent(GUIEvent.TouchDown, function()
        self:onDetailListView()
    end)

    self.detailListView.root:registerEvent(GUIEvent.TouchMove, function()
        self:onDetailListView()
    end)

    self.detailListView.root:registerEvent(GUIEvent.TouchUp, function()
        self:onDetailListView()
    end)

    self.selectTabId = UIBulletin.tabType.ActivityTab
    self.selectGroupId = 1
    self.activityTabIndex = 1
    self.systemTabIndex = 1

    self.bulletinData = {}
    self:registerDataCallBack("BulletinData", function(data)
        self:onBulletinUpdate(data)
    end, JsonBuilderProcessor)

end

function UIBulletin:onDetailListView()
    local sildHight = 10
    for _, item in pairs(self.detailListView.items) do
        sildHight = item.root:GetPixelSize().y - self.detailListView.root:GetPixelSize().y
        break
    end

    if sildHight <= 0 then
        sildHight = 10
    else
        local offset = self.detailListView.root:GetScrollOffset()
        local progres = -offset / sildHight
        print("sildHight == "..sildHight .."   offset == "..offset.." progres == "..progres)
        self.silder:SetProgress(progres)
    end
end

function UIBulletin:onMainRadioChanged(radio, tabType)
    print("tabType == "..tabType)
    self.gleActivityImg:SetVisible(false)
    self.gleSystemImg:SetVisible(false)
    if radio:IsSelected() then
        self.selectTabId = tabType


        local cfgList = BulletinConfig:getTabDataByTabId(tabType)
        self.LeftTabListView:clearItems()
        for i, tab in pairs(cfgList) do
            local cur_radio = UIBulletinTabItem.new("SkyBlockBulletinTabItem.json")
            cur_radio.root:SetArea({ 0, 0 }, { 0, 0 }, { 0.91, 0 }, { 0, 65 })
            cur_radio:setData(tab)
            self.LeftTabListView:addItem(cur_radio)
        end
        self:updateAllRedPoint()

        if self.selectTabId == UIBulletin.tabType.ActivityTab then
            self.gleActivityImg:SetVisible(true)
            self:onSelectRadio(self.activityTabIndex)
        end

        if self.selectTabId == UIBulletin.tabType.SystemTab then  
            self.gleSystemImg:SetVisible(true)
            self:onSelectRadio(self.systemTabIndex)
        end
    end

end

function UIBulletin:UpDateDetailInfo()
    local data = BulletinConfig:getDataByTabIdandGroupId(self.selectTabId, self.selectGroupId)
    if data == nil then
        return
    end

    local height = self.llDetail:GetPixelSize()
    self.detailListView:clearItems()
    local detail = UIBulletinDetailItem.new("SkyBlockBulletinDetailItem.json")
    detail.root:SetHeight({0, height.y })
    detail:setData(data)
    self.detailListView:addItem(detail)

    local hightList = {}
    for _, item in pairs(self.detailListView.items) do
        local hight = item:getNeedSetHigt()
        hightList[item.data.GroupId] = hight
    end

    self.detailListView:clearItems()
    self.detailListView.root:SetScrollOffset(0)
    local detail = UIBulletinDetailItem.new("SkyBlockBulletinDetailItem.json")
    detail.root:SetHeight({0, hightList[data.GroupId]})
    detail:setData(data)
    self.detailListView:addItem(detail)

    self.silder:SetProgress(0)
end 

function UIBulletin:onClosePanel()
    self:hide()
end

function UIBulletin:onSelectRadio(index)
    self.selectGroupId = index
    if self.selectTabId == UIBulletin.tabType.ActivityTab then
        self.activityTabIndex = index
    end

    if self.selectTabId == UIBulletin.tabType.SystemTab then
        self.systemTabIndex = index
    end

    for _, item in pairs(self.LeftTabListView.items) do
        if item.data.GroupId == index then
            item.root:SetArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 65 })
            item.radioBtn:SetSelected(true)
            if item.radioRedImg:IsVisible() then
                item.radioRedImg:SetVisible(false)
                self:onSelectRedPoint(item.data)
            end
        else
            item.root:SetArea({ 0, 0 }, { 0, 0 }, { 0.91, 0 }, { 0, 65 })
            item.radioBtn:SetSelected(false)
        end
    end

    self:UpDateDetailInfo()
end

function UIBulletin:onSelectRedPoint(data)
    local info = {
        TabId = data.TabId,
        GroupId = data.GroupId,
        UpdateTime = data.UpdateTime,
    }

    GamePacketSender:sendClickBulletinRed(json.encode(info))
end

function UIBulletin:showPanel()
    self:show()
    if self.btnActivity:IsSelected() or self.btnSystem:IsSelected() then
        return
    end

    self.btnActivity:SetSelected(true)
end

function UIBulletin:onBulletinUpdate(data)
    print("onBulletinUpdate")
    self.bulletinData = data or {}
    self:updateAllRedPoint()
end

function UIBulletin:updateAllRedPoint()
    local activeData = {}
    local systemData = {}
    for _, v in pairs(self.bulletinData) do
        if v.TabId == UIBulletin.tabType.ActivityTab then
            table.insert(activeData, v)
        end

        if v.TabId == UIBulletin.tabType.SystemTab then
            table.insert(systemData, v)
        end
    end

    self:updateActiveRedPoint(activeData)
    self:updateSystemRedPoint(systemData)
    self:updateTopRedPoint()
end

function UIBulletin:updateActiveRedPoint(activeData)
    local isShow = false
    local data = BulletinConfig:getTabDataByTabId(UIBulletin.tabType.ActivityTab)
    for _, v in pairs(data) do
        if not self:checkIsHaveClick(activeData, v) then
            isShow = true
            self:setTabRed(v)
        end
    end
    if isShow then
        self.activityRedImg:SetVisible(true)
    else
        self.activityRedImg:SetVisible(false)
    end
end

function UIBulletin:updateSystemRedPoint(systemData)
    local isShow = false
    local data = BulletinConfig:getTabDataByTabId(UIBulletin.tabType.SystemTab)
    for _, v in pairs(data) do
        if not self:checkIsHaveClick(systemData, v) then
            isShow = true
            self:setTabRed(v)
        end
    end

    if isShow then
        self.systemRedImg:SetVisible(true)
    else
        self.systemRedImg:SetVisible(false)
    end
end

function UIBulletin:updateTopRedPoint()
    if self.activityRedImg:IsVisible() or self.systemRedImg:IsVisible() then
        GUIToolBar:SetBulletinRedPoint(true)
        return
    end
    GUIToolBar:SetBulletinRedPoint(false)
end

function UIBulletin:setTabRed(data)
    for _, item in pairs(self.LeftTabListView.items) do
        if item.data.TabId == data.TabId and item.data.GroupId == data.GroupId then
            item.radioRedImg:SetVisible(true)
        end
    end
end

function UIBulletin:checkIsHaveClick(curTable, data)
    for k,v in pairs(curTable) do
        if v.TabId == data.TabId and v.GroupId == data.GroupId then
            if v.UpdateTime == data.UpdateTime then
                return true
            else
                return false
            end
        end
    end
    return false
end

GUIBulletin = UIBulletin.new("SkyBlockBulletin.json")
GUIBulletin:hide()