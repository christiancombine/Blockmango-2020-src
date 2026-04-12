require "ui.window.GUITowerItem"
require "ui.window.GUISkillItem"
require "ui.window.GUIShortCutBar"
require "ui.window.GUIBuyGuideItem"
require "ui.window.GUITowerPlayerInfo"
require "ui.layout.GUIBuyGameGoldContent"

local UIMain = class("UIMain", IGUILayout)
UIMain.current_tower_id = 0
UIMain.broadcastQueue = {}
UIMain.teamInfo = {}
UIMain.skillSkipInfo = {}

function UIMain:onLoad()
    self.subAlpha = true
    self.borderAlpha = 1
    self.showScreenIcon = false

    self.stBroadcastText = self:getChildWindow("TowerDefenseMain-broadcastText", GUIType.StaticText)
    self.broadcastTextDefaultXPos = self.stBroadcastText:GetXPosition()
    self.stBroadcastTextBound = self:getChildWindow("TowerDefenseMain-broadcastTextBound", GUIType.Layout)

    self.stGoldNum = self:getChildWindow("TowerDefenseMain-gold_num", GUIType.StaticText)
    self.stHomeHpNum = self:getChildWindow("TowerDefenseMain-hp_num", GUIType.StaticText)
    self.stGetGoldNum = self:getChildWindow("TowerDefenseMain-hovertips_text", GUIType.StaticText)

    self.pbtHomeHp = self:getChildWindow("TowerDefenseMain-hp_progress", GUIType.ProgressBar)

    self.llBroadcast = self:getChildWindow("TowerDefenseMain-broadcast", GUIType.Layout)
    self.llBuyGuide = self:getChildWindow("TowerDefenseMain-buyguide", GUIType.Layout)
    self.llHudBox = self:getChildWindow("TowerDefenseMain-Hudbox", GUIType.Layout)

    self.llTeamInfo = self:getChildWindow("TowerDefenseMain-team-list", GUIType.Layout)
    self.gvTeamInfo = IGUIGridView.new("TowerDefenseMain-teaminfoview", self.llTeamInfo)
    self.gvTeamInfo.root:SetTouchable(false)
    self.gvTeamInfo:setConfig(0, 3, 1)

    self.btnSupport = self:getChildWindow("TowerDefenseMain-support_bt", GUIType.Button)
    self.btnBook = self:getChildWindow("TowerDefenseMain-book", GUIType.Button)
    self.btnShop = self:getChildWindow("TowerDefenseMain-shop", GUIType.Button)
    self.btnAddMoney = self:getChildWindow("TowerDefenseMain-gold_add", GUIType.Button)

    self.ivScreenEffect = self:getChildWindow("TowerDefenseMain-Screen-Effect", GUIType.StaticImage)
    self.hoverTips = self:getChildWindow("TowerDefenseMain-hovertips_bg", GUIType.StaticImage)

    self.hoverTips:SetVisible(false)
    self.ivScreenEffect:SetVisible(false)

    self.ltChatBox = GUIManager:getWindowByName("Main-Chat-Message")
    self.ltChatBox:SetArea({ 0, 10 }, { 0, 230 }, { 0, 369 }, { 0.22, 0 })


    self.ivScreenIcon = self:getChildWindow("TowerDefenseMain-Screen-Icon", GUIType.StaticImage)
    self.ivScreenIcon:SetVisible(false)
    self:initTowerList()
    self:initRegister()
    self:hideTeamList()
    self:initSkillList()
    self:initBuyGuideList()
    self:initShortCutBar()

    self.btnSupport:SetVisible(false)

    local size = self.root:GetPixelSize()
    if size.y / size.x > 0.5625 then
        self.llHudBox:SetWidth({ 0.586, 0 })
        size = self.llHudBox:GetPixelSize()
        self.llHudBox:SetHeight({ 0, size.x / 3 })
    end

    --Effect Switch
    self.openMonster = true
    self.openAura = true
    self.openBuff = true
    self.openBoom = true
    self.openFloat = true
    self.openMonsterBtn = self:getChildWindow("TowerDefenseMain-Effect-Switch-Monster-Btn", GUIType.Button)
    self.openMonsterBtn:registerEvent(GUIEvent.ButtonClick, function()
        if self.openMonster then
            self.openMonster = false
            self.openMonsterBtn:SetText("关")
        else
            self.openMonster = true
            self.openMonsterBtn:SetText("开")
        end
    end)

    self.openAuraBtn = self:getChildWindow("TowerDefenseMain-Effect-Switch-Aura-Btn", GUIType.Button)
    self.openAuraBtn:registerEvent(GUIEvent.ButtonClick, function()
        if self.openAura then
            self.openAura = false
            self.openAuraBtn:SetText("关")
            for _, tower in pairs(LuaEntityManager:getTowers()) do
                tower:removeAuraEffect()
                tower:removeOwnEffect()
            end
        else
            self.openAura = true
            self.openAuraBtn:SetText("开")
            for _, tower in pairs(LuaEntityManager:getTowers()) do
                tower:addAuraEffect()
                tower:addOwnEffect()
            end
        end
    end)

    self.openBuffBtn = self:getChildWindow("TowerDefenseMain-Effect-Switch-Buff-Btn", GUIType.Button)
    self.openBuffBtn:registerEvent(GUIEvent.ButtonClick, function()
        if self.openBuff then
            self.openBuff = false
            self.openBuffBtn:SetText("关")
        else
            self.openBuff = true
            self.openBuffBtn:SetText("开")
        end
    end)

    self.openBoomBtn = self:getChildWindow("TowerDefenseMain-Effect-Switch-Boom-Btn", GUIType.Button)
    self.openBoomBtn:registerEvent(GUIEvent.ButtonClick, function()
        if self.openBoom then
            self.openBoom = false
            self.openBoomBtn:SetText("关")
        else
            self.openBoom = true
            self.openBoomBtn:SetText("开")
        end
    end)

    self.openFloatBtn = self:getChildWindow("TowerDefenseMain-Effect-Switch-Float-Btn", GUIType.Button)
    self.openFloatBtn:registerEvent(GUIEvent.ButtonClick, function()
        if self.openFloat then
            self.openFloat = false
            self.openFloatBtn:SetText("关")
        else
            self.openFloat = true
            self.openFloatBtn:SetText("开")
        end
    end)
end

function UIMain:initRegister()
    self:registerDataCallBack("BuildList", function(data)
        self:initTowerItems(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("PaySkillList", function(data)
        self:initSkillItems(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("BuySkillSuccess", function(data)
        self:updateSkillItems(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("Settlement", function(data)
        self:updateTeamInfo(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("Broadcast", function(data)
        self:updateBroadCastQueue(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("UseProp", function(data)
        self:onPlayerUseProp(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("HomeHp", function(data)
        self:onUpdateHomeHp(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("ShowScreenEffect", function(data)
        self:upScreenEffect(data)
    end, JsonBuilderProcessor)

    self:registerEvent(UpdateMoneyEvent, function()
        self:onUpdateGoldNum()
    end)

    self.btnSupport:registerEvent(GUIEvent.ButtonClick, function()
        GUISupport:showSupport()
    end)

    self.btnBook:registerEvent(GUIEvent.ButtonClick, function()
        GUIGameBook:showBook()
    end)

    self.btnShop:registerEvent(GUIEvent.ButtonClick, function()
        GUIShop:showShop()
    end)

    self.btnAddMoney:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddMoney()
    end)
end

function UIMain:initTowerList()
    local llTowerList = self:getChildWindow("TowerDefenseMain-towerlist")
    self.towerList = IGUIGridView.new("TowerDefenseMain-Items", llTowerList)
    self.towerList.root:SetMoveAble(false)
    self.towerList:setArea({ 0.038, 0 }, { 0, 10 }, { 1, -10 }, { 1, 0 })
    self.towerList:setConfig(11, 0, 3)
    self.towerList.root:SetHorizontalAlignment(HorizontalAlignment.Left)
    self.towerList.root:SetVerticalAlignment(VerticalAlignment.Center)
    self.towerList.root:SetClipChild(false)
end

function UIMain:initSkillList()
    local llSkillList = self:getChildWindow("TowerDefenseMain-skilllist")
    self.skillList = IGUIGridView.new("TowerDefenseMain-skills", llSkillList)
    self.skillList.root:SetMoveAble(false)
    self.skillList:setArea({ 0.04, 0 }, { 0, 0 }, { 1, -30 }, { 1, 0 })
    self.skillList:setConfig(13, 0, 5)
    self.skillList.root:SetHorizontalAlignment(HorizontalAlignment.Center)
    self.skillList.root:SetVerticalAlignment(VerticalAlignment.Center)
end

function UIMain:initSkillItems(data)
    self.skillList:clearItems()
    for _, skillId in pairs(data) do
        local Item = UISkillItem.new("TowerDefenseSkill.json")
        Item:setWidth({ 0.16, 0 })
        Item:setHeight({ 0.75, 0 })
        Item:init(skillId)
        self.skillList:addItem(Item)
        self.skillSkipInfo[skillId] = skillId
    end
end

function UIMain:initBuyGuideList()
    local llBuyGuideList = self:getChildWindow("TowerDefenseMain-buyguide")
    self.buyGuide = IGUIGridView.new("TowerDefenseMain-buyguidelist", llBuyGuideList)
    self.buyGuide.root:SetTouchable(false)
    self.buyGuide:setArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 })
    self.buyGuide:setConfig(8, 0, 1)
    self.buyGuide.root:SetHorizontalAlignment(HorizontalAlignment.Center)
    self.buyGuide.root:SetVerticalAlignment(VerticalAlignment.Center)
end

function UIMain:initShortCutBar()
    local llTowerList = self:getChildWindow("TowerDefenseMain-shotcutbar")
    self.shortCutBar1 = UIShortCutBar.new("TowerDefenseShortCutBar.json")
    self.shortCutBar1:setArea({ 0, 50 }, { 0.34, 0 }, { 0.406, 0 }, { 0.56, 0 })
    self.shortCutBar1:init(2001)
    llTowerList:AddChildWindow(self.shortCutBar1.root)

    self.shortCutBar2 = UIShortCutBar.new("TowerDefenseShortCutBar.json", llTowerList)
    self.shortCutBar2:setArea({ 0.563, 0 }, { 0.34, 0 }, { 0.406, 0 }, { 0.56, 0 })
    self.shortCutBar2:init(2002)
    llTowerList:AddChildWindow(self.shortCutBar2.root)
end

function UIMain:onGetNewPayItem(itemId, num)
    if itemId == Define.PayStoreItem.ShortcutBarOne then
        GUITowerDefenseChangeTower:hide()
        self.shortCutBar1:setUnlock()
        self.shortCutBar1:updateShortCutBar(Define.ShortCutBarStatus.NotTower)
    elseif itemId == Define.PayStoreItem.ShortcutBarTwo then
        GUITowerDefenseChangeTower:hide()
        self.shortCutBar2:setUnlock()
        self.shortCutBar2:updateShortCutBar(Define.ShortCutBarStatus.NotTower)
    elseif itemId == Define.PayStoreItem.GameGold then
        LuaTimer:cancel(self.hoverTipsLuaTimer)
        self.stGetGoldNum:SetText("+" .. num * GamePlayer.Instance().gameMoneyRate)
        self.hoverTips:SetVisible(true)
        GamePlayer.Instance().exchangeLimit = GamePlayer.Instance().exchangeLimit - num * GamePlayer.Instance().gameMoneyRate
        self.hoverTipsLuaTimer = LuaTimer:schedule(function()
            self.hoverTips:SetVisible(false)
        end, 1000)
    end
end

function UIMain:updateSkillItems(data)
    local skillItem = self.skillList:getItem("skillId", data.skillId)
    skillItem:onBuySuccess(data)
end

function UIMain:onTick(frameTime)
    if self.showScreenIcon then
        self:upScreenIconAlpha()
    end
end

function UIMain:upScreenIconAlpha()
    if self.subAlpha then
        self.borderAlpha = self.borderAlpha - 0.01
        if self.borderAlpha <= 0.5 then
            self.subAlpha = false
        end
    else
        self.borderAlpha = self.borderAlpha + 0.01
        if self.borderAlpha >= 1 then
            self.subAlpha = true
        end
    end
    self.ivScreenIcon:SetAlpha(self.borderAlpha)
end

function UIMain:upScreenEffect(data)
    if data.isShow == false then
        return
    end
    local id = tonumber(data.propsId)
    local propConfig = PropsConfig:getConfigById(id)
    if propConfig.screenEffect ~= "#" then
        self.ivScreenEffect:SetVisible(true)
        self.ivScreenEffect:SetEffectName(propConfig.screenEffect)
        LuaTimer:scheduleTimer(function()
            self.ivScreenEffect:SetVisible(false)
        end, propConfig.effectDuration * 1000, 1)
    end

    if propConfig.screenEffectIcon ~= "#" then
        self.ivScreenIcon:SetVisible(true)
        self.showScreenIcon = true
        self.ivScreenIcon:SetImage(propConfig.screenEffectIcon)
        LuaTimer:scheduleTimer(function()
            self.ivScreenIcon:SetVisible(false)
            self.showScreenIcon = false
        end, propConfig.effectDuration * 1000, 1)
    end
end

function UIMain:onClickSkillItem(skillId, price, offPrice, isFree, isOff)
    self.skillList:getItem("skillId", skillId)
    GameAnalytics:design(1, { "G_choicebuff ", skillId })

    if self.skillSkipInfo[skillId] ~= true then
        GUISkillDetail:showDetailPage(skillId, price, offPrice, isFree, isOff)
    else
        local SkillConfig = SkillStoreConfig:getConfigById(skillId)
        local skillPrice = 0
        if not isFree and isOff then
            skillPrice = offPrice
        elseif not isFree then
            skillPrice = SkillConfig.price
        end
        PayHelper.checkMoney(MoneyType.Diamonds, skillPrice, function(isEnough)
            if isEnough then
                GamePacketSender:sendBuySkill(skillId)
            end
        end)
    end
end

function UIMain:updateTeamInfo(data)
    self.teamInfo = data
    self:showTeamInfo()
end

function UIMain:showTeamInfo()
    --GUIManager:getWindowByName("Main-Chat-Message"):SetVisible(false)
    self.gvTeamInfo:clearItems()
    for i, player in pairs(self.teamInfo) do
        local playerInfo = UITowerPlayerInfo.new("TowerDefensePlayerInfo.json")
        playerInfo.root:SetHorizontalAlignment(HorizontalAlignment.Right)
        if player.userId == tostring(GamePlayer:Instance().userId) then
            playerInfo:setWidth({ 1, 0 })
            playerInfo:setHeight({ 0, 28 })
            playerInfo:updateInfo(player, true)
        else
            playerInfo:setWidth({ 1, -30 })
            playerInfo:setHeight({ 0, 22 })
            playerInfo:updateInfo(player, false)
        end
        self.gvTeamInfo:addItem(playerInfo)
    end
end

function UIMain:initTowerItems(data)
    self.initTowers = {}
    self.towerList:clearItems()
    for i, towerId in pairs(data) do
        self.initTowers[tostring(towerId)] = 1
        local Item = UITowerItem.new("TowerDefenseItem.json")
        Item:setWidth({ 0.29, 0 })
        Item:setHeight({ 0.81, 0 })
        Item:init(towerId, i)
        self.towerList:addItem(Item)
        if i == 1 then
            self.current_tower_id = towerId
            Item.siSelectBox:SetVisible(true)
        end
    end
end

function UIMain:isInInitTowers(towerId)
    if self.initTowers[tostring(towerId)] ~= nil or self.shortCutBar1.Item.towerId == towerId or self.shortCutBar2.Item.towerId == towerId then
        return true
    end
    return false
end

function UIMain:checkBuyGuideList(towerList)
    if towerList ~= nil then
        --检查有无新增
        for _, towerId_1 in pairs(towerList) do
            if self.buyGuide:getItem("towerId", towerId_1) == nil then
                self:updateBuyGuideList(towerList)
                return
            end
        end
        --检查有无减少
        for index = 1, self.buyGuide:getItemCount() do
            local Item = self.buyGuide:getItem("index", index)
            local isHave = false
            for _, towerId in pairs(towerList) do
                if towerId == Item.towerId then
                    isHave = true
                    break
                end
            end
            if isHave == false then
                self:updateBuyGuideList(towerList)
                return
            end
        end

    else
        self.buyGuide:clearItems()
    end
end

function UIMain:updateBuyGuideList(towerList)
    self.buyGuide:clearItems()
    for index, towerId in pairs(towerList) do
        local Item = UIBuyGuideItem.new("TowerDefenseBuyGuide.json")
        Item:setWidth({ 0, 369 })
        Item:setHeight({ 0, 72 })
        Item:init(towerId, index)
        self.buyGuide:addItem(Item)
    end
end

function UIMain:updateSelectTowerItem(towerId)
    local num = self.towerList:getItemCount()
    self.current_tower_id = towerId
    for i = 1, num, 1 do
        local tower = self.towerList:getItem("index", i)
        if tower then
            tower:isSelect(self.current_tower_id)
        end
    end
    self.shortCutBar1:isSelect(self.current_tower_id)
    self.shortCutBar2:isSelect(self.current_tower_id)
end

function UIMain:getCurrentTowerId()
    return self.current_tower_id
end

function UIMain:updateBroadCastQueue(data)
    local text = data:getParam("text")
    table.insert(self.broadcastQueue, text)
    self:updateBroadCast()
end

function UIMain:updateBroadCast()
    if not self.llBroadcast:IsVisible() and self.broadcastQueue[1] ~= nil then
        self.llBroadcast:SetVisible(true)
        local text = table.remove(self.broadcastQueue, 1)
        self.stBroadcastText:SetText(text)

        local broadcastTextBound = self.stBroadcastTextBound:GetWidth()[2]
        local textLength = self.stBroadcastText:GetFont():GetTextExtent(text, 1)
        if textLength > broadcastTextBound then
            local decrement = (textLength - broadcastTextBound) / 400
            LuaTimer:scheduleTimer(function()
                local xPos = self.stBroadcastText:GetXPosition()
                LuaTimer:scheduleTimer(function()
                    xPos[2] = xPos[2] - decrement
                    self.stBroadcastText:SetXPosition(xPos)
                end, 10, 400)
            end, 990, 1)
        end

        LuaTimer:scheduleTimer(function()
            self.llBroadcast:SetVisible(false)
            self.stBroadcastText:SetXPosition(self.broadcastTextDefaultXPos)
            self:updateBroadCast()
        end, 5500, 1)
    end
end

function UIMain:onUpdateHomeHp(data)
    local homeHp = data:getNumberParam("homeHP")
    local initHp = data:getNumberParam("initHp")
    if initHp < homeHp then
        self.pbtHomeHp:SetProgress(1)
        self.stHomeHpNum:SetText((initHp) .. " + " .. (homeHp - initHp))
        self.stHomeHpNum:SetTextBoader({ 1, 0.55, 0, 1 })
    else
        self.stHomeHpNum:SetTextBoader({ 0.5, 0, 0, 1 })
        self.stHomeHpNum:SetText(homeHp)
        self.pbtHomeHp:SetProgress(homeHp / initHp)
    end
end

function UIMain:onUpdateGoldNum(goldNum)
    self.stGoldNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))

    local num = self.towerList:getItemCount()
    for i = 1, num, 1 do
        local tower = self.towerList:getItem("index", i)
        if tower then
            tower:updateStatus(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))
        end
    end
    self.shortCutBar1:updateStatus(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))
    self.shortCutBar2:updateStatus(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.GameGold))
end

function UIMain:onPlayerUseProp(data)
    local playerName = data:getParam("playerName")
    local propStr = data:getParam("propStr")
    local propName = Lang:getString(propStr)
    local text = playerName .. "\t" .. propName
    local str = Lang:getTipString(1056002, text)
    table.insert(self.broadcastQueue, str)
    self:updateBroadCast()
end

local function hideEngineWindow(name)
    local window = GUIManager:getWindowByName(name)
    if not window then
        return
    end
    window:SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    window:SetVisible(false)
end

function UIMain:hideTeamList()
    GUIManager:getWindowByName("Main-Fly"):SetHorizontalAlignment(HorizontalAlignment.Right)
    GUIManager:getWindowByName("Main-Fly"):SetVerticalAlignment(VerticalAlignment.Bottom)
    GUIManager:getWindowByName("Main-Fly"):SetArea({ -0.17, 0 }, { -0.075, 0 }, { 0, 54 }, { 0, 54 })

    hideEngineWindow("PlayerInfo-Health")
    hideEngineWindow("PlayerInfo-FoodSaturation")
    hideEngineWindow("Main-Player-Exp")
    hideEngineWindow("Main-ItemBarBg")
    hideEngineWindow("Main-MoveState")
    hideEngineWindow("Main-VisibleBar")
    hideEngineWindow("MainControl-sneak")
    hideEngineWindow("ToolBar-Team-List")
    GUIManager:getWindowByName("ToolBar", GUIType.Layout):SetProperty("StretchType", "NineGrid")
    GUIManager:getWindowByName("ToolBar", GUIType.Layout):SetBackImage("set:tower_defense_main.json image:toolbar_bg")
    GUIManager:getWindowByName("ToolBar-Currency-Diamond", GUIType.Layout):SetBackImage("set:tower_defense_main.json image:recourse_bg")
    ---首充主界面按钮位置
    GUIManager:getWindowByName("TDSumRechargeMain-Icon", GUIType.Button):SetArea({ 0, -123 }, { 0, 186 }, { 0, 93 }, { 0, 56 })
end

function UIMain:onAddMessage(windowName)
    local item = GUIManager:getWindowByName(windowName)
    if not item then
        return
    end
    item:SetText("▢FF20CEFF" .. tostring(item:GetText()))
    item:SetArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 30 })
    item:SetTouchable(true)
    item:SetProperty("TextWordWrap", "false")
    item:SetProperty("TextAutoOmission", "true")
    item:SetProperty("TextBorder", "true")
    item:SetProperty("TextBorderColor", "0 0 0 1")
end

function UIMain:onGetChat(message)
    local str = "▢FF00BFFF" .. "<" .. tostring(GamePlayer:Instance():getName()) .. "> " .. "▢FFFFFFFF" .. Lang:getString(tostring(message))
    DynamicCastHelper.dynamicCastGuiChat(RootGuiLayout.Instance():getLayout(6)):addChatMessage(str, true)
    DynamicCastHelper.dynamicCastGuiMainControl(RootGuiLayout.Instance():getLayout(0)):addChatMessage(str, true)
end

function UIMain:onClickAddMoney()

    local listener = function()
        return BuyGameGoldContent:onClickBuy()
    end
    BuyGameGoldContent:setCurrencyInfo()

    CustomDialog.builder()
                .setTitleText(Lang:getString("exchange_money"))
                .setContentView(BuyGameGoldContent)
                .setRightText(Lang:getString("gui_TowerDefense-buy"))
                .setRightClickListener(listener)
                .setHideLeftButton()
                .show()
end

return UIMain