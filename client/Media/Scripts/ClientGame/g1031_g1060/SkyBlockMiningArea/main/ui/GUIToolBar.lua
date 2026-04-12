local UIToolBar = class("UIToolBar", IGUILayout)
UIToolBar.ClickType = 
{
    CHRISTMAS = 1,
}

function UIToolBar:onLoad()
	print("UIToolBar:onLoad()")
	self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetNormalImage("set:party.json image:myParty")
	self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetPushedImage("set:party.json image:myParty")
	self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetText(Lang:getString("gui_skill_cast"))
	self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetTextColor(Color.WHITE)
    self:getChildWindow("ArmorPanel-SkyBlock-Switch-Effect", GUIType.StaticImage):SetImage("set:skyblock_friend.json image:red_dot")
    self:getChildWindow("Main-SkyBlock-BarEffect", GUIType.StaticImage):SetImage("set:skyblock_friend.json image:red_dot")

	self.llMoney = self:getChildWindow("ToolBar-Currency-Money", GUIType.Layout)
    self.llMoney:SetArea({ -0.207031, 0 }, { 0, 0 }, { 0.122657, 0 }, { 0.777778, 0})

    self:getChildWindow("Main-Skill-Btn-Effect", GUIType.StaticImage):SetVisible(true)
    self.dressBox = self:getChildWindow("ArmorPanel-Dress-Switch", GUIType.CheckBox)
    self.dressBox:registerEvent(GUIEvent.CheckStateChanged, function()
        self:onCheckStateChanged()
    end)
    self.ivArmorRed = self:getChildWindow("ArmorPanel-SkyBlock-Switch-Effect", GUIType.StaticImage)
    self.ivMainBarRed = self:getChildWindow("Main-SkyBlock-BarEffect", GUIType.StaticImage)
    self.ivArmorRed:SetVisible(false)
    self.ivMainBarRed:SetVisible(false)
    
    self.addMoneyBtn = self:getChildWindow("ToolBar-Currency-Money-Add", GUIType.Button)
    self.addMoneyBtn:SetVisible(true)
    self.addMoneyBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddMoneyButton()
    end)
    
    self.ChristmasBtn = self:getChildWindow("SkyBlockMiningAreaLuaToolBar-Christmas-Btn", GUIType.Button)
    self.ChristmasBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickButton(UIToolBar.ClickType.CHRISTMAS)
    end)

    self.ivRichRed = self:getChildWindow("SkyBlockMiningAreaLuaToolBar-Rich-Red-Point", GUIType.StaticImage)
    self.ivRichRed:SetVisible(false)
    self.richBtn = self:getChildWindow("SkyBlockMiningAreaLuaToolBar-Rich-Btn", GUIType.Button)
    self.richBtn:SetText(Lang:getString("gui_toolbar_rich_btn_text"))
    self.richBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onRichBtnClick()
    end)

    self:registerDataCallBack("SwitchShowRedPoint", function(data)
        self:onSwitchShowRedPoint(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("RichShowRedPoint", function(data)
        self:onRichBtnShowRedPoint(data)
    end, JsonBuilderProcessor)
end

function UIToolBar:onClickButton(clickType)
    if clickType == UIToolBar.ClickType.CHRISTMAS then
        GUIChristmas:showPanelByIndex(GUIChristmas.tabType.buyGiftTab)
        local data =
        {
            type = 5
        }
        GamePacketSender:sendCommonClick(data)
        return
    end
end

function UIToolBar:onClickAddMoneyButton()
    GUIBuyMoneyTips:onShowLackMoney()
end

function UIToolBar:SetToolBarStatus(isShow)
    if isShow then
        GUIManager:showWindowByName("ToolBar.json")
    else
        GUIManager:hideWindowByName("ToolBar.json")
    end
end

function UIToolBar:onTick()
    local last_time = GameConfig.christmasFinishTime - os.time()
    if last_time <= 0 then
        if self.ChristmasBtn:IsVisible() then
            self.ChristmasBtn:SetVisible(false)
        end
    end
    if PlayerManager:getClientPlayer().Player:hasEntityFeature("TreasureHint") then
        PlayerManager:getClientPlayer().Player:clearEntityFeature("TreasureHint")
    end
end

function UIToolBar:onCheckStateChanged()
    if self.ivArmorRed:IsVisible() or self.ivMainBarRed:IsVisible() then
        self:setRedStatus(false)
        GamePacketSender:sendRedSwitchClick()
    end
end

function UIToolBar:onSwitchShowRedPoint(curTable)
    if not self:isLoad() then
        return
    end
    print("onSwitchShowRedPoint")
    if curTable then
        self:setRedStatus(curTable.RedIsShow)
    end 
end

function UIToolBar:setRedStatus(isShow)
    self.ivArmorRed:SetVisible(isShow)
    self.ivMainBarRed:SetVisible(isShow)
end

function UIToolBar:onRichBtnClick()
    if self.ivRichRed:IsVisible() then
        self.ivRichRed:SetVisible(false)
    end
    GamePacketSender:sendRedRichBtnClick()
end

function UIToolBar:onRichBtnShowRedPoint(curTable)
    if not self:isLoad() then
        return
    end
    print("onRichBtnShowRedPoint")
    if curTable then
        self.ivRichRed:SetVisible(curTable.RedIsShow)
    end 
end

GUIToolBar = UIToolBar.new("SkyBlockMiningAreaLuaToolBar.json", GUIManager:getToolBar())
GUIToolBar:show()