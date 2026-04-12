local UINewGuide = class("UINewGuide", IGUILayout)

function UINewGuide:onLoad()
    self.index = 1
    self.time = 5
    self.frameTime = 0
    self.isToolBarOpen = false

    self:initUI()
    self:initEvent()
end

function UINewGuide:initUI()

    self.ivMaskTop = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Top", GUIType.Layout)
    self.ivMaskBottom = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Bottom", GUIType.Layout)
    self.ivMaskLeft = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Left", GUIType.Layout)
    self.ivMaskRight = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Right", GUIType.Layout)

    self.ivMaskTop2 = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Top2", GUIType.Layout)
    self.ivMaskBottom2 = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Botton2", GUIType.Layout)
    self.ivMaskCreter = self:getChildWindow("LiftingSimulatorNewGuide-Mask-Center", GUIType.Layout)
    self.ivMaskTop2:SetVisible(false)
    self.ivMaskBottom2:SetVisible(false)
    self.ivMaskCreter:SetVisible(false)

    self.llCenter = self:getChildWindow("LiftingSimulatorNewGuide-Center", GUIType.Layout)
    self.ivDetail = self:getChildWindow("LiftingSimulatorNewGuide-Detail-Img", GUIType.StaticImage)
    self.tvDesc = self:getChildWindow("LiftingSimulatorNewGuide-Content-Desc", GUIType.StaticText)
    self.ivArrows = self:getChildWindow("LiftingSimulatorNewGuide-Arrows", GUIType.StaticImage)
    self.ivArrows2 = self:getChildWindow("LiftingSimulatorNewGuide-Arrows2", GUIType.StaticImage)

    self.btnNext  = self:getChildWindow("LiftingSimulatorNewGuide-Next-Button" ,GUIType.Button)
    self.btnNext:SetEnabled(false)

    self.tvTitleName = self:getChildWindow("LiftingSimulatorNewGuide-Title-Name", GUIType.StaticText)
    self.tvTitleName:SetText(Lang:getString("gui_new_guide_title_name"))
    
end

function UINewGuide:initEvent()
    self.btnNext:registerEvent(GUIEvent.ButtonClick,function ()
        self:onClickNextButton()
    end)

    self:registerEvent(S2CNewGuideProgressInfo, function(data)
        self:newGuideProgressInfo(data)
    end)

    self:registerEvent(ToolBarNewGuideClickEvent, function ()
        self:onToolBarOpenNewGuide()
    end)
end

function UINewGuide:onShow()
    self:onShowDetail()
end

function UINewGuide:onClickNextButton()
    self.index = self.index + 1
    if self.isToolBarOpen == false then
        local data = {
            index = self.index
        }
        local builder = DataBuilder.new():fromTable(data):getData()
        PacketSender:sendLuaCommonData("GuideClickNext", builder)
    end

    self:onShowDetail()
end

function UINewGuide:onShowDetail()
    local cfg = NewGuideConfig:getGuideById(self.index)
    if cfg == nil then
        self:hide()
        self.taskFinished = true
        LogUtil.log("UINewGuide taskFinished")
        return
    end

    self.time = cfg.Time
    self:setNextBtnStatus()
    self.ivDetail:SetImage(cfg.Img)
    self.tvDesc:SetText(Lang:getString(cfg.Desc))
    self:setUiArea()
end

function UINewGuide:onTick(frameTime)
    self.frameTime = self.frameTime + frameTime 
    if self.frameTime >= 1000 then
        self.frameTime = 0
        self:setNextBtnStatus()
    end
end

function UINewGuide:setNextBtnStatus()
    local tiemStr = Lang:getString("gui_new_guide_next")
    if self.time > 0 then
        tiemStr = tiemStr .. " ("..tostring(self.time)..")"
        self.btnNext:SetEnabled(false)
        self.time = self.time - 1
    else
        self.btnNext:SetEnabled(true)
    end
    self.btnNext:SetText(tiemStr)
end

function UINewGuide:setUiArea()
    local cfg = NewGuideConfig:getGuideById(self.index)
    if cfg == nil then
        self:hide()
        return
    end

    local  showArea = NewGuideConfig:getAreaByString(cfg.ShowArea)
    local  arrowsArea = NewGuideConfig:getAreaByString(cfg.ArrowsArea)
    local  arrowsArea2 = NewGuideConfig:getAreaByString(cfg.ArrowsArea2)
    local  lucArea1 = NewGuideConfig:getLucencyAreaByString(cfg.LucencyArea)
    local  lucArea2 = NewGuideConfig:getLucencyAreaByString(cfg.LucencyArea2)
    if showArea == nil or lucArea1 == nil or arrowsArea == nil or lucArea2 == nil or arrowsArea2 == nil then
        return
    end

    self.llCenter:SetArea({ showArea.posX, 0 }, { showArea.posY, 0 }, { showArea.areaX, 0 }, { showArea.areaY, 0 })
    self.ivArrows:SetArea({ arrowsArea.posX, 0 }, { arrowsArea.posY, 0 }, { 0 , arrowsArea.areaX }, { 0 , arrowsArea.areaY })
    self.ivArrows:SetImage(cfg.ArrowsImg)

    self.ivArrows2:SetArea({ arrowsArea2.posX, 0 }, { arrowsArea2.posY, 0 }, { 0 , arrowsArea2.areaX }, { 0 , arrowsArea2.areaY })
    self.ivArrows2:SetImage(cfg.ArrowsImg2)

    self.ivMaskTop:SetArea({ lucArea1.pos_x_1, lucArea1.pos_x_2 }, { 0, 0 }, { lucArea1.size_x_1, lucArea1.size_x_2 }, { lucArea1.pos_y_1, lucArea1.pos_y_2 })
    self.ivMaskBottom:SetArea({ lucArea1.pos_x_1, lucArea1.pos_x_2 }, { lucArea1.pos_y_1 + lucArea1.size_y_1, lucArea1.pos_y_2 + lucArea1.size_y_2 }, { lucArea1.size_x_1, lucArea1.size_x_2 }, { 1 - lucArea1.pos_y_1 - lucArea1.size_y_1, -lucArea1.pos_y_2 - lucArea1.size_y_2 })
    self.ivMaskLeft:SetArea({ 0, 0 }, { 0, 0 }, { lucArea1.pos_x_1, lucArea1.pos_x_2 }, { 1, 0 })

    if cfg.IsShowLucencyArea2 == 1 then
        self.ivMaskTop2:SetVisible(true)
        self.ivMaskBottom2:SetVisible(true)
        self.ivMaskCreter:SetVisible(true)
        self.ivArrows2:SetVisible(true)
        self.ivMaskTop2:SetArea({ lucArea2.pos_x_1, lucArea2.pos_x_2 }, { 0, 0 }, { lucArea2.size_x_1, lucArea2.size_x_2 }, { lucArea2.pos_y_1, lucArea2.pos_y_2 })
        self.ivMaskBottom2:SetArea({ lucArea2.pos_x_1, lucArea2.pos_x_2 }, { lucArea2.pos_y_1 + lucArea2.size_y_1, lucArea2.pos_y_2 + lucArea2.size_y_2 }, { lucArea2.size_x_1, lucArea2.size_x_2 }, { 1 - lucArea2.pos_y_1 - lucArea2.size_y_1, -lucArea2.pos_y_2 - lucArea2.size_y_2 })
        self.ivMaskCreter:SetArea({ lucArea1.pos_x_1 + lucArea1.size_x_1, lucArea1.pos_x_2 + lucArea1.size_x_2 }, { 0, 0 }, { lucArea2.pos_x_1 -lucArea1.pos_x_1 - lucArea1.size_x_1, lucArea2.pos_x_2-lucArea1.pos_x_2 - lucArea1.size_x_2 }, { 1, 0 })
        self.ivMaskRight:SetArea({ lucArea2.pos_x_1 + lucArea2.size_x_1, lucArea2.pos_x_2 + lucArea2.size_x_2 }, {0, 0}, { 1 -lucArea2.pos_x_1 - lucArea2.size_x_1, -lucArea2.pos_x_2 - lucArea2.size_x_2 }, {1 , 0})
    else
        self.ivArrows2:SetVisible(false)
        self.ivMaskTop2:SetVisible(false)
        self.ivMaskBottom2:SetVisible(false)
        self.ivMaskCreter:SetVisible(false)
        self.ivMaskRight:SetArea({ lucArea1.pos_x_1 + lucArea1.size_x_1, lucArea1.pos_x_2 + lucArea1.size_x_2 }, {0, 0}, { 1 -lucArea1.pos_x_1 - lucArea1.size_x_1, -lucArea1.pos_x_2 - lucArea1.size_x_2 }, {1 , 0})
    end 
end

function UINewGuide:newGuideProgressInfo(data)
    local builder = DataBuilder.new():fromData(data)
    local index = builder:getNumberParam("index")
    self.index = index
    self.isToolBarOpen = false

    local cfg = NewGuideConfig:getGuideById(self.index)
    if cfg then
        GUIManager:addShowQueue(GUINewGuide)
    end
end

function UINewGuide:onToolBarOpenNewGuide()
    self.index = 1
    self.isToolBarOpen = true
    local cfg = NewGuideConfig:getGuideById(self.index)
    if cfg then
        RootGuiLayout.Instance():showMainControl()
        self:show()
        self:onShowDetail()
    end
end

return UINewGuide
