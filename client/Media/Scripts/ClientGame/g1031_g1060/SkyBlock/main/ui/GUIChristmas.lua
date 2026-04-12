require "ui.GUIChirstmasGiftItem"
require "ui.GUISkyBlockItemIcon"
local UIChristmas = class("UIChristmas", IGUILayout)

function UIChristmas:onLoad()
    self:onLoadInitUI()
end

UIChristmas.tabType =
{
    buyGiftTab = 1,
    openGiftTab = 2,
}

UIChristmas.openType =
{
    NONE = 0,
    MONEY_OPEN = 1,
    LOOK_AD = 2,
    DIAMOND_OPEN  = 3,
}

UIChristmas.openBtnType =
{
    LEFT_BTN = 1,
    RIGHT_BTN = 2
}

function UIChristmas:onLoadInitUI()
    HostApi.log("UIChristmas onLoadInitUI")
    self.selectTabId = UIChristmas.tabType.buyGiftTab
    self.selfOpenIndex = 1
    self.selectOpenData = {}
    self.giftNumList = {}
    self.openReward = {}


    self.buyGiftTab = self:getChildWindow("SkyBlockChristmas-Buy-Gift-Tab", GUIType.RadioButton)
    self.buyGiftTab:SetText(Lang:getString("gui_buy_christmas_gift"))
    self.buyGiftTab:registerEvent(GUIEvent.RadioStateChanged, function()
        self:onRadioStateChanged(self.buyGiftTab, UIChristmas.tabType.buyGiftTab)
    end)

    self.openGiftTab = self:getChildWindow("SkyBlockChristmas-Open-Gift-Tab", GUIType.RadioButton)
    self.openGiftTab:SetText(Lang:getString("gui_open_christmas_gift"))
    self.openGiftTab:registerEvent(GUIEvent.RadioStateChanged, function()
        self:onRadioStateChanged(self.openGiftTab, UIChristmas.tabType.openGiftTab)
    end)

    self.closeBtn = self:getChildWindow("SkyBlockChristmas-BtnClose", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClosePanel()
    end)

    self.llBuyGift = self:getChildWindow("SkyBlockChristmas-Buy-Gift-Panel", GUIType.Layout)
    self.llBuyGift:SetVisible(false)
    self.llOpenGift = self:getChildWindow("SkyBlockChristmas-Open-Gift-Panel", GUIType.Layout)
    self.llOpenGift:SetVisible(false)

    self.buySmallImg = self:getChildWindow("SkyBlockChristmas-Buy-Gift-Small", GUIType.StaticImage)
    self.openSmallImg = self:getChildWindow("SkyBlockChristmas-Open-Gift-Small", GUIType.StaticImage)
    self.buySmallImg:SetVisible(false)
    self.openSmallImg:SetVisible(false)

    self.llBuyGiftLaout = self:getChildWindow("SkyBlockChristmas-Buy-Gift-List", GUIType.Layout)
    self.buyItemGridView = IGUIGridView.new("SkyBlockChristmas-Buy-Item-grid-view", self.llBuyGiftLaout)
    self.buyItemGridView:setConfig(25, 0, 2)
    self.buyItemGridView.root:SetMoveAble(false)


    self.openSmallIcon = self:getChildWindow("SkyBlockChristmas-Gift-Small-Icon", GUIType.StaticImage)
    self.openBigImg = self:getChildWindow("SkyBlockChristmas-Big-Icon", GUIType.StaticImage)
    
    self.toLeftBtn = self:getChildWindow("SkyBlockChristmas-To-Left-Btn", GUIType.Button)
    self.toLeftBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:toLeftBtnEvent()
    end)    

    self.toRightBtn = self:getChildWindow("SkyBlockChristmas-To-Right-Btn", GUIType.Button)
    self.toRightBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:toRightBtnEvent()
    end)
    self.stGiftName = self:getChildWindow("SkyBlockChristmas-Gift-Name")

    self.llOpenLeft = self:getChildWindow("SkyBlockChristmas-Left-Open-Panel", GUIType.Layout)
    self.llOpenLeft:SetVisible(false)
    self.openLeftBtn = self:getChildWindow("SkyBlockChristmas-Left-Open-Btn", GUIType.Button)
    self.openLeftBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:openLeftBtnEvent()
    end)
    self.stLeftOpenText = self:getChildWindow("SkyBlockChristmas-Left-Price-Num")
    self.leftOpenPriceIcon = self:getChildWindow("SkyBlockChristmas-Left-Price-Icon", GUIType.StaticImage)

    self.llBuyGiftBg = self:getChildWindow("SkyBlockChristmas-Buy-Gift-Bg", GUIType.StaticImage)
    self.llOpenGiftBg = self:getChildWindow("SkyBlockChristmas-Open-Gift-Bg", GUIType.StaticImage)
    self.llBuyGiftBg:SetVisible(false)
    self.llOpenGiftBg:SetVisible(false)

    
    self.llOpenRight = self:getChildWindow("SkyBlockChristmas-Right-Open-Panel", GUIType.Layout)
    self.llOpenRight:SetVisible(false)
    self.openRightBtn = self:getChildWindow("SkyBlockChristmas-Right-Open-Btn", GUIType.Button)
    self.openRightBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:openRightBtnEvent()
    end)
    self.stRightOpenText = self:getChildWindow("SkyBlockChristmas-Right-Price-Num")
    self.rightOpenPriceIcon = self:getChildWindow("SkyBlockChristmas-Right-Price-Icon", GUIType.StaticImage)
    
    self.stMayHaveText = self:getChildWindow("SkyBlockChristmas-May-Have-Text")
    self.stMayHaveText:SetText(Lang:getString("gui_open-may-have"))
    self.llMayHave = self:getChildWindow("SkyBlockChristmas-Reward-List", GUIType.Layout)
    self.mayHaveGridView = IGUIGridView.new("SkyBlockChristmas-open-may-have-grid-view", self.llMayHave)
    self.mayHaveGridView:setConfig(5, 5, 5)

    self.llRewardHide = self:getChildWindow("SkyBlockChristmas-Reward-Hide-Layout")
    self.llRewardHide:SetVisible(true)

    self.stGiftNum = self:getChildWindow("SkyBlockChristmas-My-Have-Num")

    self.llRewardResult = self:getChildWindow("SkyBlockChristmas-Reward-Result-Panel", GUIType.Layout)
    self.resultbigBgImg = self:getChildWindow("SkyBlockChristmas-Result-Big-Bg", GUIType.StaticImage)
    self.resultbigBgImg:registerEvent(GUIEvent.TouchUp, function()
        self.llRewardResult:SetVisible(false)
        self.llRewardHide:SetVisible(true)
    end)

    self.resultbigBgImg = self:getChildWindow("SkyBlockChristmas-Result-Ok-Btn", GUIType.Button)
    self.resultbigBgImg:registerEvent(GUIEvent.ButtonClick, function()
        self.llRewardResult:SetVisible(false)
        self.llRewardHide:SetVisible(true)
    end)
    self.llRewardResult:SetVisible(false)

    self.llRewardIcon = self:getChildWindow("SkyBlockChristmas-Result-Icon", GUIType.Layout)
    self.rewardIconGridView = IGUIGridView.new("SkyBlockChristmas-open-reward_icon-grid-view", self.llRewardIcon)
    self.rewardIconGridView:setConfig(0, 0, 1)
    self.rewardIconItem = UISkyBlockItemIcon.new("SkyBlockItemIcon.json")
    self.rewardIconItem:setWidth({ 1, 0 })
    self.rewardIconItem:setHeight({ 1, 0 })
    self.rewardIconGridView:addItem(self.rewardIconItem)
    self.rewardIconGridView.root:SetMoveAble(false)

    self.stLastTime = self:getChildWindow("SkyBlockChristmas-Last-Time")
    self.stLastTimeText = self:getChildWindow("SkyBlockChristmas-Last-Time-Text")
    self.stLastTimeText:SetText(Lang:getString("gui_gift_last_time"))
    self.stInvHaveText = self:getChildWindow("SkyBlockChristmas-My-Have-Num-Text")
    self.stInvHaveText:SetText(Lang:getString("gui_inv_have_gift_num"))
    self.stRewardText = self:getChildWindow("SkyBlockChristmas-Reward-Text")
    self.stRewardText:SetText(Lang:getString("gui_sky_block_pri_reward_text"))

    self.btnProbability = self:getChildWindow("SkyBlockChristmas-Probability", GUIType.Button)
    if Game:isChina() then
        self.btnProbability:SetVisible(true)
        self.btnProbability:registerEvent(GUIEvent.ButtonClick, function()
            ProbabilityDialog.show("g1048_1")
        end)
    else
        self.btnProbability:SetVisible(false)
    end

    self:registerDataCallBack("SendGiftNumList", function(data)
        self:onGiftNumListUpdate(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("SendOpenGiftReward", function(data)
        self:onOpenGiftReward(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("ChristUseBoxItem", function(data)
        print("ChristUseBoxItem tabType == " .. data.index)
        self:showPanelByItem(data)
    end, JsonBuilderProcessor)

    self:upDatePanel()
    self:selectTabByIndex(self.selectTabId)
    self:changeOpenGift()
end

function UIChristmas:onGiftNumListUpdate(curTable)
    if curTable then
        self.giftNumList = curTable
    end
    self:updateCurGiftNum()
end

function UIChristmas:upDatePanel()
    self.buyItemGridView:clearItems()
    local cfg = ChristmasConfig:getGiftdata()
    for i, v in pairs(cfg) do
        local curItem = UIChirstmasGiftItem.new("SkyBlockChristmasBuyItem.json")
        curItem:setItemData(v)
        curItem:setWidth({ 0.48, 0 })
        curItem:setHeight({ 1, 0 })
        self.buyItemGridView:addItem(curItem)
    end
end

function UIChristmas:onRadioStateChanged(radio, tabType)
    print("UIChristmas tabType == "..tabType)
    self.buySmallImg:SetVisible(false)
    self.openSmallImg:SetVisible(false)

    if radio:IsSelected() then
        self.selectTabId = tabType
        if self.selectTabId == UIChristmas.tabType.buyGiftTab then
            self.buySmallImg:SetVisible(true)
            self.llBuyGift:SetVisible(true)
            self.llOpenGift:SetVisible(false)
            self.llBuyGiftBg:SetVisible(true)
            self.llOpenGiftBg:SetVisible(false)
        end

        if self.selectTabId == UIChristmas.tabType.openGiftTab then
            self.openSmallImg:SetVisible(true)
            self.llBuyGift:SetVisible(false)
            self.llOpenGift:SetVisible(true)
            self.llBuyGiftBg:SetVisible(false)
            self.llOpenGiftBg:SetVisible(true)
            GamePacketSender:sendGetGiftNum()
        end
    end
end

function UIChristmas:onClosePanel()
    self:hide()
    GUISkyBlockMoneyDeatil:hide()
    GUIToolBar:SetToolBarStatus(true)
end

function UIChristmas:selectTabByIndex(index)
    if GameConfig:checkIsInChiristmasTime() == false then
        index = UIChristmas.tabType.openGiftTab
    end

    if index == UIChristmas.tabType.buyGiftTab then
        self.buyGiftTab:SetSelected(true)
        return
    end

    if index == UIChristmas.tabType.openGiftTab then
        self.openGiftTab:SetSelected(true)
        return
    end
end

function UIChristmas:toLeftBtnEvent()
    self.selfOpenIndex =  self.selfOpenIndex - 1
    if self.selfOpenIndex < 1 then
        self.selfOpenIndex = ChristmasConfig:getGiftDataSize()
    end
    self:changeOpenGift() 
end

function UIChristmas:toRightBtnEvent()
    self.selfOpenIndex =  self.selfOpenIndex + 1
    if self.selfOpenIndex > ChristmasConfig:getGiftDataSize() then
        self.selfOpenIndex = 1
    end
    self:changeOpenGift() 
end

function UIChristmas:openLeftBtnEvent()
    if not self:checkCanOpenGift() then
        return
    end

    if self.selectOpenData.OpenType == UIChristmas.openType.LOOK_AD then
        local str = "OpenGift#"..tostring(self.selectOpenData.GiftType)
        ClientHelper.onWatchAd(1004803, str)
        return
    end

    if self.selectOpenData.OpenType == UIChristmas.openType.MONEY_OPEN then
        GameConfig:checkMoney(MoneyType.Currency, self.selectOpenData.OpenMoneyNum, function(isCan)
            if isCan then
                local data = 
                {
                    giftType = self.selectOpenData.GiftType,
                    openType = UIChristmas.openBtnType.LEFT_BTN
                }
                GamePacketSender:sendOpenGift(data)
            end
        end)
        return
    end
end

function UIChristmas:openRightBtnEvent()
    if not self:checkCanOpenGift() then
        return
    end

    GameConfig:checkMoney(MoneyType.Diamonds, self.selectOpenData.OpenDiamondNum, function(isCan)
        if isCan then
            local data = 
            {
                giftType = self.selectOpenData.GiftType,
                openType = UIChristmas.openBtnType.RIGHT_BTN
            }
            GamePacketSender:sendOpenGift(data)
        end
    end)
end

function UIChristmas:checkCanOpenGift()
    local cur_num = self:getGiftNumByItemId(self.selectOpenData.ItemId)
    if cur_num <= 0 then
        GUITipDialog:onShowTipDialog(Lang:getString("gui_not_have_cur_gift"), GUITipDialog.TipType.COMMON, "",  function(isSure)
            if isSure then
                self:selectTabByIndex(UIChristmas.tabType.buyGiftTab)
            end
        end)

        return false
    end

    local player = Game:getPlayer()
    local inv = player:getInventory()
    if not inv:isCanAddItem(self.selectOpenData.ItemId, 0, 1) then
        GUITipDialog:onShowTipDialog(Lang:getString("gui_inventory_not_enough"), GUITipDialog.TipType.COMMON)
        return false
    end

    return true
end

function UIChristmas:changeOpenGift()
    local cfg = ChristmasConfig:getGiftdataBytype(self.selfOpenIndex)
    if cfg == nil then
        return
    end
    self.selectOpenData = cfg

    self.stGiftName:SetText(Lang:getString(tostring(self.selectOpenData.Name)))
    self.openBigImg:SetImage(self.selectOpenData.BigImg)
    self.openSmallIcon:SetImage(self.selectOpenData.Img)

    self.llOpenRight:SetVisible(true)
    self.llOpenLeft:SetVisible(false)
    self.llOpenRight:SetArea({ 0.606138, 0 }, { 1.37, 0 }, { 0.475702, 0 }, { 0.2, 0 })

    if cfg.OpenType == UIChristmas.openType.MONEY_OPEN then
        self.llOpenLeft:SetVisible(true)
        self.leftOpenPriceIcon:SetVisible(true)
        self.stLeftOpenText:SetText(tostring(cfg.OpenMoneyNum))
        self.leftOpenPriceIcon:SetImage(PayHelper.getMoneyIcon(MoneyType.Currency))
    end

    if cfg.OpenType == UIChristmas.openType.LOOK_AD then
        self.llOpenLeft:SetVisible(true)
        self.stLeftOpenText:SetText(Lang:getString("gui_look_ad-open-gift"))
        self.leftOpenPriceIcon:SetVisible(false)
    end

    if cfg.OpenType == UIChristmas.openType.NONE then
        self.llOpenRight:SetArea({ 0.306384, 0 }, { 1.37, 0 }, { 0.475702, 0 }, { 0.2, 0 })
    end

    self.stRightOpenText:SetText(tostring(cfg.OpenDiamondNum))
    self.rightOpenPriceIcon:SetImage(PayHelper.getMoneyIcon(MoneyType.Diamonds))

    self:upDateMayHavePanel()
    self:updateCurGiftNum()
end

function UIChristmas:updateCurGiftNum()
    local cfg = ChristmasConfig:getGiftdataByIndex(self.selfOpenIndex)
    if cfg == nil then
        return
    end

    local cur_num = self:getGiftNumByItemId(cfg.ItemId)
    local str = "x"..tostring(cur_num)
    self.stGiftNum:SetText(str)
end

function UIChristmas:getGiftNumByItemId(itemId)
    for _, v in pairs(self.giftNumList) do
        if tonumber(itemId) == tonumber(v.ItemId) then
            return v.Num
        end
    end
    return 0
end

function UIChristmas:upDateMayHavePanel()
    local cfg = ChristmasConfig:getGiftdataBytype(self.selfOpenIndex)
    if cfg == nil then
        return
    end

    self.mayHaveGridView:clearItems()
    for i, v in pairs(cfg.Reward) do
        local cue_item_cfg = GiftRewardConfig:getRewardById(v)
        if cue_item_cfg then
            local curItem = UISkyBlockItemIcon.new("SkyBlockItemIcon.json")
            local cur_data = GameConfig:getItemIconData()
            cur_data.ItemId = cue_item_cfg.ItemId
            cur_data.Meta = cue_item_cfg.Meta
            cur_data.Num = cue_item_cfg.Num
            cur_data.Icon = cue_item_cfg.Icon
            cur_data.Name = cue_item_cfg.Name
            cur_data.Detail = cue_item_cfg.Detail

            curItem:setWidth({ 0, 70 })
            curItem:setHeight({ 0, 70 })
            curItem:setItemData(cur_data)
            self.mayHaveGridView:addItem(curItem)
        end
    end
end

function UIChristmas:onOpenGiftReward(curTable)
    if curTable then
        self.openReward = curTable
    end
    local data = GameConfig:getItemIconData()
    data.ItemId = curTable.ItemId
    data.Meta = curTable.Meta
    data.Num = curTable.Num
    data.Icon = curTable.Icon
    data.Name = curTable.Name
    data.Detail = curTable.Detail
    data.DataType = Define.ItemIcon.Gift_reward

    self.llRewardHide:SetVisible(false)
    self.llRewardResult:SetVisible(true)
    self.rewardIconItem:setItemData(data)
end


function UIChristmas:showPanelByIndex(index)
    RootGuiLayout.Instance():showMainControl()
    GamePacketSender:sendGetGiftNum()
    GUIToolBar:SetToolBarStatus(false)
    self:show()
    self:selectTabByIndex(index)
    GUISkyBlockMoneyDeatil:show()
end

function UIChristmas:showPanelByItem(data)
    self.selfOpenIndex = 1
    if data.itemId then
        local cfg = ChristmasConfig:getGiftdataByItemId(data.itemId)
        if cfg then
            self.selfOpenIndex = cfg.Id or 1 
        end
    end
    self:showPanelByIndex(data.index)
    self:changeOpenGift() 
end

function UIChristmas:getFormatLastTime(seconds)
    local format = "dd:HH:mm:ss"
    local text = ""
    local day = math.floor(seconds / 3600 / 24)
    local hour = math.floor(seconds % (3600 * 24) / 3600)
    local minute = math.floor((seconds % 3600) / 60)
    local second = seconds % 60
    text = self:checkTime(text, day, format, "dd")
    text = self:checkTime(text, hour, format, "HH", "D ")
    text = self:checkTime(text, minute, format, "mm")
    text = self:checkTime(text, second, format, "ss")
    return text
end
--
function UIChristmas:checkTime(text, append, format, pattern, split)
    split = split or ":"
    if string.find(format, pattern) ~= nil then
        if #text ~= 0 then
            text = text .. split
        end
        if append < 10 then
            text = text .. "0" .. append
        else
            text = text .. append
        end
    end
    return text
end

function UIChristmas:onTick()
    local last_time = GameConfig.christmasFinishTime - os.time()
    if last_time <= 0 then
        if self.buyGiftTab:IsVisible() then
            self.buyGiftTab:SetVisible(false)
        end

        if not self.openGiftTab:IsSelected() then
            self.openGiftTab:SetSelected(true)
        end

        last_time = 0
    end 

    if GameConfig.isShowChristmasTime == 1 then
        self.stLastTime:SetVisible(true)
        self.stLastTimeText:SetVisible(true)
        local timeText = self:getFormatLastTime(last_time)
        self.stLastTime:SetText(timeText)
    else
        self.stLastTime:SetVisible(false)
        self.stLastTimeText:SetVisible(false)
    end
end

GUIChristmas = UIChristmas.new("SkyBlockChristmas.json")
GUIChristmas:hide()