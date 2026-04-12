require "ui.layout.GUIDressContent"

local UIMain = class("UIMain", IGUILayout)

function UIMain:onLoad()
    self.btnShop = self:getChildWindow("TowerDefenseHallMain-shop", GUIType.Button)
    self.btnBook = self:getChildWindow("TowerDefenseHallMain-book", GUIType.Button)
    self.stTeamTip = self:getChildWindow("TowerDefenseHallMain-TeamTip", GUIType.StaticText)
    self.stTeamTip:SetVisible(false)
    self.stTeamTip:SetText(Lang:getString("gui_TowerDefenseHallMain-Team-Tip"))
    self:initRegister()
    self:hideTeamList()
end

function UIMain:initRegister()
    self.btnShop:registerEvent(GUIEvent.ButtonClick, function()
        GUIShop:showShop()
    end)

    self.btnBook:registerEvent(GUIEvent.ButtonClick, function()
        GUIBook:showBook()
    end)

    self:registerEvent(ClickSkinActorEvent, function(skinId)
        self:showDressDialog(skinId)
    end
    )
end

function UIMain:hideTeamList()
    GUIManager:getWindowByName("PlayerInfo-Health"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("PlayerInfo-FoodSaturation"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("Main-Player-Exp"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("Main-ItemBarBg"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("Main-VisibleBar"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("ToolBar-Team-List"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("Main-MoveState"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("MainControl-sneak"):SetArea({ 1, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 })
    GUIManager:getWindowByName("ToolBar", GUIType.Layout):SetProperty("StretchType", "NineGrid")
    GUIManager:getWindowByName("ToolBar", GUIType.Layout):SetBackImage("set:tower_defense_main.json image:toolbar_bg")
    GUIManager:getWindowByName("ToolBar-Currency-Diamond", GUIType.Layout):SetBackImage("set:tower_defense_main.json image:recourse_bg")
end

function UIMain:showDressDialog(skinId)
    local skinConfig = SkinStoreConfig:getConfigById(skinId)
    if not skinConfig then
        return
    end

    GameAnalytics:design(1, { "H_skin_ID", skinId })

    local uiType = 1
    if GamePlayer:Instance().equipSkinId == skinId then
        uiType = 3
    elseif GamePlayer:Instance().skinList[tostring(skinId)] and GamePlayer:Instance().skinList[tostring(skinId)].isHave == true then
        uiType = 2
    end
    DressContent:setCurrencyInfo(PayHelper.getMoneyIcon(skinConfig.moneyType), uiType, skinConfig.price)
    local listener = function()
        if uiType == 1 then
            if skinConfig.moneyType == Define.Money.DiamondsGold then
                PayHelper.checkMoney(MoneyType.Diamonds, skinConfig.price, function(isEnough)
                    if isEnough then
                        GamePacketSender:sendBuySkinItem(skinId)
                        self.isNotCloseTip = false
                    else
                        GameAnalytics:design(0, { "H_skin_ID_buy", skinId })
                        self.isNotCloseTip = true
                    end
                end)
                return self.isNotCloseTip
            end
        elseif uiType == 2 then
            GamePacketSender:sendChangeSkin(skinId)
        elseif uiType == 3 then
            --正在使用该装扮
        end
    end
    CustomDialog.builder()
                .setHideLeftButton()
                .setRightText(Lang:getString("gui_TowerDefenseDress-sure"))
                .setTitleText(Lang:getString("gui_TowerDefenseDress-tip"))
                .setContentView(DressContent)
                .setRightClickListener(listener)
                .show()
end

function UIMain:showTeamTip(userId, isShow)
    if userId ~= tostring(GamePlayer:Instance().userId) then
        return
    end
    self.stTeamTip:SetVisible(isShow)
end

return UIMain