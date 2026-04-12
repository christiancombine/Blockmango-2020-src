local function hideEngineWindow(name)
    local window = GUIManager:getWindowByName(name)
    if not window then
        return
    end
    window:SetXPosition({ 0, 6000 })
    window:SetVisible(false)
end

local function setTouchable(window, visible)
    window:SetTouchable(visible)
    local count = window:GetChildCount()
    if count == 0 then
        return
    end
    for index = 1, count do
        setTouchable(window:GetChildByIndex(index - 1), visible)
    end
end

local UIMain = class("UIMain", IGUILayout)

function UIMain:onLoad()
    self.canBuildBlockQuickly = false
    self.quicklyBuildBlockCost = 4
    self.quicklyBuildBlockNum = 2
    self.btnQuickBuild = GUIManager:getWindowByName("BedWarGameMain-QuicklyBuildBlock", GUIType.Button)

    self.btnLottery = self:getChildWindow("BedWarGameMain-Menu-Lottery", GUIType.Button)
    self.btnBlockShop = self:getChildWindow("BedWarGameMain-Menu-BlockShop", GUIType.Button)
    self.btnCustomShop = self:getChildWindow("BedWarGameMain-Menu-Custom-Shop", GUIType.Button)
    self.btnRecharge = self:getChildWindow("BedWarGameMain-Menu-Recharge", GUIType.Button)
    self.cbShortMessage = self:getChildWindow("BedWarGameMain-MenuList-ShortMessage", GUIType.CheckBox)
    self.cbMenuList = self:getChildWindow("BedWarGameMain-MenuList-Switch", GUIType.CheckBox)
    self.siRedPoint = self:getChildWindow("BedWarGameMain-MenuList-RedPoint", GUIType.StaticImage)
    self.ivBlockShopPoint = self:getChildWindow("BedWarGameMain-Menu-BlockShop-RedPoint", GUIType.StaticImage)
    self.siChallengeRedPoint = self:getChildWindow("BedWarGameMain-Menu-Challenge-RedPoint", GUIType.StaticImage)
    self.ivLicenseRedPoint = self:getChildWindow("BedWarGameMain-Menu-License-Red-Point", GUIType.StaticImage)
    self.siShortMessageMask = self:getChildWindow("BedWarGameMain-MenuList-ShortMessage-mask", GUIType.StaticImage)
    self.btnLicense = self:getChildWindow("BedWarGameMain-Menu-License", GUIType.Button)

    self:setEngineUIStyle()
    self:register()
end

function UIMain:setEngineUIStyle()
    ClientHelper.putFloatPrefs("PoleControlMaxDistance", 65)
    ClientHelper.putFloatPrefs("MainControlKeyAlphaNormal", 1)
    ClientHelper.putFloatPrefs("MainControlKeyAlphaPress", 1)
    local ivPoleArrow = GUIManager:getWindowByName("Main-PoleControl-Arrow", GUIType.StaticImage)
    ivPoleArrow:SetImage("set:pole.json image:pole_arrow")
    local wMoveState = GUIManager:getWindowByName("Main-MoveState")
    wMoveState:SetArea(wMoveState:GetXPosition(), wMoveState:GetYPosition(), { 0, 70 }, { 0, 70 })
    local wJumControls = GUIManager:getWindowByName("Main-Jump-Controls")
    wJumControls:SetArea(wJumControls:GetXPosition(), wJumControls:GetYPosition(), { 0, 138 }, { 0, 377 })
    local wQuicklyBuild = GUIManager:getWindowByName("Main-QuicklyBuildBlock")
    self.btnQuickBuild:SetArea(wQuicklyBuild:GetXPosition(), wQuicklyBuild:GetYPosition(), { 0, 70 }, { 0, 70 })
    self.btnQuickBuild:SetNormalImage("set:bedwarsmall.json image:quick_block_close")
    self.btnQuickBuild:SetPushedImage("set:bedwarsmall.json image:quick_block_close")
    wQuicklyBuild:SetArea({ 1, 0 }, { 1, 0 }, { 0, 70 }, { 0, 70 })
    local btnSkillRelease = GUIManager:getWindowByName("Main-Skill-Release-btn", GUIType.Button)
    btnSkillRelease:SetNormalImage("set:bedwarsmall.json image:free_skill")
    btnSkillRelease:SetPushedImage("set:bedwarsmall.json image:free_skill")
    btnSkillRelease:SetArea(btnSkillRelease:GetXPosition(), btnSkillRelease:GetYPosition(), { 0, 120 }, { 0, 120 })
    local ivSneak = GUIManager:getWindowByName("Main-MoveState-Sneak", GUIType.StaticImage)
    ivSneak:SetImage("set:move_state.json image:sneak_circle")
    local btnFly = GUIManager:getWindowByName("Main-Fly", GUIType.Button)
    btnFly:SetNormalImage("set:bedwarsmall.json image:fly")
    btnFly:SetPushedImage("set:bedwarsmall.json image:fly")
    btnFly:SetArea(btnFly:GetXPosition(), btnFly:GetYPosition(), { 0, 70 }, { 0, 70 })
    hideEngineWindow("Main-Player-Exp")
    hideEngineWindow("Main-Chat-Message")

    local bgWatch = GUIManager:getWindowByName("Main-Watch-Battle-Bg", GUIType.StaticImage)
    bgWatch:SetProperty("StretchOffset", "60 10 0 0")
    bgWatch:SetImage("set:bedwar_toolbar.json image:bg_onWatch")
    GUIManager:getWindowByName("Main-Watch-Battle-Tip"):SetHeight({ 0, 49 })
    GUIManager:getWindowByName("Main-Watch-Battle-Tip"):SetWidth({ 0, 250 })
    GUIManager:getWindowByName("Main-Watch-Battle-Text"):SetArea({ 0, 50 }, { 0, 0 }, { 1, -50 }, { 1, 0 })
    GUIManager:getWindowByName("Main-Watch-Battle-Text"):SetHorizontalAlignment(HorizontalAlignment.Left)
end

function UIMain:register()
    self.btnQuickBuild:registerEvent(GUIEvent.ButtonClick, function()
        SoundUtil.playSound(SoundType.Click)
        self:clickQuicklyBuildBlockBtn()
    end)

    self:registerDataCallBack("QuicklyBuildBlock", function(data)
        self.quicklyBuildBlockCost = data:getNumberParam("Cost") or 3
        self.quicklyBuildBlockNum = data:getNumberParam("Num") or 4
    end, DataBuilderProcessor)

    self:registerDataCallBack("BuySuccessQBB", function()
        self.canBuildBlockQuickly = true
        self:setQuicklyBuildBlockBtn(true)
    end)

    ------------
    self.btnRecharge:registerEvent(GUIEvent.ButtonClick, function()
        ReportUtil.reportClickFirstRecharge(GUISumRechargeLayout.currentId)
        GUISumRechargeLayout:show()
    end)
    self.btnLicense:registerEvent(GUIEvent.ButtonClick, function()
        GameAnalytics:design( 1, { "G_License" })
        GUILicense:show()
        CommonPacketSender:sendPlayerOpenLicense()
    end)

    self:registerDataCallBack("ShowSumRecharge", function(data)
        local id = data:getParam("Id")
        local config = SumRechargeConfig:getSumRecharge(id)
        if config then
            self.btnRecharge:SetVisible(true)
        else
            self.btnRecharge:SetVisible(false)
        end
    end, DataBuilderProcessor)

    self.cbShortMessage:registerEvent(GUIEvent.CheckStateChanged, function()
        if self.cbShortMessage:GetChecked() then
            GUICommandList:show(true)
        else
            GUICommandList:hide()
        end
    end)

    self.btnLottery:registerEvent(GUIEvent.ButtonClick, function()
        self:onLotteryBtn()
        GameAnalytics:design(1, { "ui_luckybox" })
    end)

    self.btnBlockShop:registerEvent(GUIEvent.ButtonClick, function()
        GameAnalytics:design(1, { "G_blockshop" })
        GUIBlockShop:show()
    end)

    self.btnCustomShop:registerEvent(GUIEvent.ButtonClick, function()
        GUICustomShop:show(true)
        GameAnalytics:design(1, { "ui_customshop" })
    end)

    self.cbMenuList:registerEvent(GUIEvent.CheckStateChanged, function()
        self:showMenuList(self.cbMenuList:GetChecked())
    end)

    self:getChildWindow("BedWarGameMain-Menu-Challenge", GUIEvent.Button):registerEvent(GUIEvent.ButtonClick, function()
        GameAnalytics:design(1, { "G_Treasure" })
        GUITreasureChallenge:onDataChange(TreasureChallenge.rewardData)
        GUITreasureChallenge:show(true)
    end)
end

function UIMain:clickQuicklyBuildBlockBtn()
    if self.canBuildBlockQuickly then
        self:setQuicklyBuildBlockBtn(ClientHelper.getIntPrefs("QuicklyBuildBlockNum") == 1)
    else
        CustomDialog.builder()
                    .setHideLeftButton()
                    .setTitleText("gui_dialog_tip_title_quickly_build_block")
                    .setContentText(string.format(Lang:getString("gui_dialog_tip_content_quickly_build_block"), tonumber(self.quicklyBuildBlockNum)))
                    .setRightText("gui_dialog_tip_btn_sure")
                    .setExtraInfo("set:app_shop.json image:app_shop_diamonds_mods", tonumber(self.quicklyBuildBlockCost))
                    .setLeftText("gui_dialog_tip_btn_cancel")
                    .setRightClickListener(function()
            LuaTimer:schedule(function()
                PayHelper.checkMoney(Define.Money.DIAMOND, self.quicklyBuildBlockCost, function(canBuy)
                    if canBuy then
                        GamePacketSender:sendBuyQuicklyBuildBlock()
                    end
                end)
            end, 100)
        end)        .show()
    end
end

function UIMain:setQuicklyBuildBlockBtn(status)
    if status and self.canBuildBlockQuickly then
        ClientHelper.putIntPrefs("QuicklyBuildBlockNum", self.quicklyBuildBlockNum)
        self.btnQuickBuild:SetNormalImage("set:bedwarsmall.json image:quick_block_open")
        self.btnQuickBuild:SetPushedImage("set:bedwarsmall.json image:quick_block_open")
    else
        ClientHelper.putIntPrefs("QuicklyBuildBlockNum", 1)
        self.btnQuickBuild:SetNormalImage("set:bedwarsmall.json image:quick_block_close")
        self.btnQuickBuild:SetPushedImage("set:bedwarsmall.json image:quick_block_close")
    end
end

function UIMain:onTick()
    self.btnQuickBuild:SetVisible(not Game:getPlayer():isWatchMode())
end

function UIMain:showMenuList(check)
    local layout = self:getChildWindow("BedWarGameMain-MenuList", GUIType.Layout)
    self.siRedPoint:SetVisible(false)
    LuaTimer:cancel(self.listKey or 0)
    if check then
        self.listKey = LuaTimer:scheduleTimer(function()
            local curWidth = math.floor(layout:GetPixelSize().x)
            if curWidth == 0 then
                LuaTimer:cancel(self.listKey or 0)
                setTouchable(layout, false)
            else
                if curWidth - 3 > 0 then
                    layout:SetWidth({ 0, curWidth - 20 })
                else
                    layout:SetWidth({ 0, 0 })
                end
            end
        end, 10)
    else
        setTouchable(layout, true)
        self.listKey = LuaTimer:scheduleTimer(function()
            local curWidth = math.floor(layout:GetPixelSize().x)
            if curWidth == 354 then
                LuaTimer:cancel(self.listKey or 0)
            else
                if curWidth + 3 < 354 then
                    layout:SetWidth({ 0, curWidth + 20 })
                else
                    layout:SetWidth({ 0, 354 })
                end
            end
        end, 10)
    end
end

function UIMain:onLotteryBtn()
    GUILottery:show()
    SoundUtil.playSound(SoundConfig.openLottery)
end

function UIMain:onCommandCD()
    self.cbShortMessage:SetTouchable(false)
    self.siShortMessageMask:SetVisible(true)
    self.siShortMessageMask:setCooldownAnim(true, 1000)

    LuaTimer:schedule(function()
        self.cbShortMessage:SetTouchable(true)
        self.siShortMessageMask:SetVisible(false)
    end, 1000)
end

function UIMain:hideList()
    ---开始游戏，玩家传送到自己的岛X秒之后，如果玩家没有把图标列表收起来，则自动收起来，并且收缩按钮有一个红点提示，点击按钮之后红点消失·
    if not self.listKey then
        self.listKey = LuaTimer:schedule(function()
            if not self.cbMenuList:GetChecked() then
                self.cbMenuList:SetChecked(true)
            end
            self.siRedPoint:SetVisible(true)
        end, GameConfig.mainTabRedPoint * 1000)
    end
end

return UIMain