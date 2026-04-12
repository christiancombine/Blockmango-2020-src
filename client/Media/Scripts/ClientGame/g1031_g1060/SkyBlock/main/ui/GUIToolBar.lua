local UIToolBar = class("UIToolBar", IGUILayout)
UIToolBar.LoveNumEnum = 
{
	mySelf = 1,
	otherPlayer = 2,
}

UIToolBar.ClickType = 
{
	SIGN_IN = 1,
	RECIVE_PRI = 2,
    PARTY = 3,
    BULLETIN = 4,
    CHRISTMAS = 5,
}

function UIToolBar:onLoad()
	print("UIToolBar:onLoad()")
	self.loveNumType = 1

	self.loveIcon = self:getChildWindow("SkyBlockLuaToolBar-Heart-Icon", GUIType.StaticImage)
    self.loveNum = self:getChildWindow("SkyBlockLuaToolBar-Heart-Num")
    self.friendBtn = self:getChildWindow("SkyBlockLuaToolBar-Friend-Btn", GUIType.Button)
    self.give_laout = self:getChildWindow("SkyBlockLuaToolBar-Party-Love-Num", GUIType.Layout)

    self.SignInBtn = self:getChildWindow("SkyBlockLuaToolBar-SignIn-Btn", GUIType.Button)
    self.SignInBtn:SetVisible(false)
    self.SignInBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIToolBar.ClickType.SIGN_IN)
	end)

    self.PartyBtn = self:getChildWindow("SkyBlockLuaToolBar-Party-Btn", GUIType.Button)
    self.PartyBtn:SetVisible(true)
    self.PartyBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickButton(UIToolBar.ClickType.PARTY)
    end)

    self.partyRedImg = self:getChildWindow("SkyBlockLuaToolBar-Party-Red", GUIType.StaticImage)
    self.partyRedImg:SetVisible(false)

    self.friendBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onOpenFriendPanel()
    end)
    self.friendRed = self:getChildWindow("SkyBlockLuaToolBar-Friend-Red", GUIType.StaticImage)
    self.friendRed:SetVisible(false)

    self.btnBulletin = self:getChildWindow("SkyBlockLuaToolBar-Bulletin-Btn", GUIType.Button)
    self.btnBulletin:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickButton(UIToolBar.ClickType.BULLETIN)
    end)

    self.bulletinRedImg = self:getChildWindow("SkyBlockLuaToolBar-Bullentin-Red", GUIType.StaticImage)
    self.bulletinRedImg:SetVisible(false)

    self.ReviveNewPriBtn = self:getChildWindow("SkyBlockLuaToolBar-RecivePri-Btn", GUIType.Button)
    self.ReviveNewPriBtn:SetVisible(false)
    self.ReviveNewPriBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIToolBar.ClickType.RECIVE_PRI)
	end)

    self.ChristmasBtn = self:getChildWindow("SkyBlockLuaToolBar-Christmas-Btn", GUIType.Button)
    self.ChristmasBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickButton(UIToolBar.ClickType.CHRISTMAS)
    end)

    self.llMoney = self:getChildWindow("ToolBar-Currency-Money", GUIType.Layout)
    self.llMoney:SetArea({ -0.207031, 0 }, { 0, 0 }, { 0.122657, 0 }, { 0.777778, 0})
    
    self.addMoneyBtn = self:getChildWindow("ToolBar-Currency-Money-Add", GUIType.Button)
    self.addMoneyBtn:SetVisible(true)
    self.addMoneyBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddMoneyButton()
    end)

    self.ivRichRed = self:getChildWindow("SkyBlockLuaToolBar-Rich-Red-Point", GUIType.StaticImage)
    self.ivRichRed:SetVisible(false)
    self.richBtn = self:getChildWindow("SkyBlockLuaToolBar-Rich-Btn", GUIType.Button)
    self.richBtn:SetText(Lang:getString("gui_toolbar_rich_btn_text"))
    self.richBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onRichBtnClick()
    end)

    self.give_laout:registerEvent(GUIEvent.TouchUp, function()
        self:onGiveLoveClick()
    end)

    self:registerDataCallBack("SignInData", function(data)
        self:onSignInDataStatus(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("ReviveNewPri", function(data)
        self:onReviveNewPriStatus(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("SwitchShowRedPoint", function(data)
        self:onSwitchShowRedPoint(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("RichShowRedPoint", function(data)
        self:onRichBtnShowRedPoint(data)
    end, JsonBuilderProcessor)

    self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetNormalImage("set:party.json image:myParty")
    self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetPushedImage("set:party.json image:myParty")
    self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetText(Lang:getString("gui_skill_cast"))
    self:getChildWindow("Main-Skill-Release-btn", GUIType.Button):SetTextColor(Color.WHITE)
    self:getChildWindow("Main-Skill-Btn-Effect", GUIType.StaticImage):SetVisible(true)
    self:getChildWindow("ArmorPanel-SkyBlock-Switch-Effect", GUIType.StaticImage):SetImage("set:skyblock_friend.json image:red_dot")
    self:getChildWindow("Main-SkyBlock-BarEffect", GUIType.StaticImage):SetImage("set:skyblock_friend.json image:red_dot")

    self.dressBox = self:getChildWindow("ArmorPanel-Dress-Switch", GUIType.CheckBox)
    self.dressBox:registerEvent(GUIEvent.CheckStateChanged, function()
        self:onCheckStateChanged()
    end)
    self.ivArmorRed = self:getChildWindow("ArmorPanel-SkyBlock-Switch-Effect", GUIType.StaticImage)
    self.ivMainBarRed = self:getChildWindow("Main-SkyBlock-BarEffect", GUIType.StaticImage)
    self.ivArmorRed:SetVisible(false)
    self.ivMainBarRed:SetVisible(false)

end

function UIToolBar:isMainLayout()
    return true
end

function UIToolBar:onOpenFriendPanel()
	HostApi.log("onOpenFriendPanel")
    RootGuiLayout.Instance():showMainControl()
	GUIFriend:show()
    MultiBuildData:getAllNeedList(false)
    -- GUIFriend.myFriendBtn:SetSelected(true)
	GUIFriend:updateCurGridView()
end


function UIToolBar:setLoveNum(loveNum, loveNumType)
	self.loveNumType = loveNumType
	self.loveNum:SetText(loveNum)

	if self.loveNumType == self.LoveNumEnum.mySelf then
		self.loveIcon:SetImage("set:party.json image:heart")
	else
		self.loveIcon:SetImage("set:party.json image:red_pollex")
	end
end

function UIToolBar:onGiveLoveClick()
	if GUIParty.lastGiveLoveNum <= 0 then
		GUITipDialog:onShowTipDialog(Lang:getString("gui_give_love_is_max"), GUITipDialog.TipType.COMMON)
		return
	end

	if GUIParty.level < GameConfig.giveLoveNumLevel then
		GUITipDialog:onShowTipDialog(Lang:getString("gui_level_too_low"), GUITipDialog.TipType.COMMON)
		return
	end

	GamePacketSender:sendOnToolBarGiveLove()
end

function UIToolBar:onSignInDataStatus(curTable)
	if not self:isLoad() then
        return
    end
    if curTable then
    	self.SignInBtn:SetVisible(curTable.IconShow)
    end
end

function UIToolBar:onReviveNewPriStatus(curTable)
    if not self:isLoad() then
        return
    end
    if curTable then
    	self.ReviveNewPriBtn:SetVisible(curTable.isShowIcon)
    end
end

function UIToolBar:onClickButton(clickType)
	if clickType == UIToolBar.ClickType.SIGN_IN then
        RootGuiLayout.Instance():showMainControl()
        RootGuiLayout.Instance():showSkyBlockSignIn()
		return
	end

	if clickType == UIToolBar.ClickType.RECIVE_PRI then
        RootGuiLayout.Instance():showMainControl()
        RootGuiLayout.Instance():showSkyBlocReciveNewPri()
		return
	end

    if clickType == UIToolBar.ClickType.PARTY then
        self:SetPartyRedPoint(false)
        RootGuiLayout.Instance():showMainControl()
        GUIParty:show()
        return
    end

    if clickType == UIToolBar.ClickType.BULLETIN then
        RootGuiLayout.Instance():showMainControl()
        GUIBulletin:showPanel()
        return
    end

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

function UIToolBar:SetFriendRedPoint(isShow)
    self.friendRed:SetVisible(isShow)
end

function UIToolBar:SetPartyRedPoint(isShow)
    self.partyRedImg:SetVisible(isShow)
end

function UIToolBar:SetBulletinRedPoint(isShow)
    self.bulletinRedImg:SetVisible(isShow)
end

function UIToolBar:SetToolBarStatus(isShow)
    if isShow then
        GUIManager:showWindowByName("ToolBar.json")
    else
        GUIManager:hideWindowByName("ToolBar.json")
    end
end

function UIToolBar:onClickAddMoneyButton()
    GUIBuyMoneyTips:onShowLackMoney()
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

GUIToolBar = UIToolBar.new("SkyBlockLuaToolBar.json", GUIManager:getToolBar())
GUIToolBar:show()