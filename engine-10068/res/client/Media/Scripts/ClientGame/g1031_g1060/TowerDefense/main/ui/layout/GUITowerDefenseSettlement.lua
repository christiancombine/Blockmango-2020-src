require "ui.window.GUISettlementItem"
require "ui.window.GUINumberItem"

local UITowerDefenseSettlement = class("UITowerDefenseSettlement", IGUILayout)

function UITowerDefenseSettlement:onLoad()
    --StaticImage
    self.siShowround = self:getChildWindow("TowerDefenseSettlement-showround", GUIType.StaticImage)
    self.siTitle = self:getChildWindow("TowerDefenseSettlement-title", GUIType.StaticImage)
    self.siRank = self:getChildWindow("TowerDefenseSettlement-rank", GUIType.StaticImage)
    self.siName = self:getChildWindow("TowerDefenseSettlement-name", GUIType.StaticImage)
    self.siGold = self:getChildWindow("TowerDefenseSettlement-gold", GUIType.StaticImage)
    self.siKill = self:getChildWindow("TowerDefenseSettlement-kill", GUIType.StaticImage)
    self.siMiss = self:getChildWindow("TowerDefenseSettlement-miss", GUIType.StaticImage)
    self.siTopround = self:getChildWindow("TowerDefenseSettlement-topround", GUIType.StaticImage)
    self.siSign = self:getChildWindow("TowerDefenseSettlement-sign", GUIType.StaticImage)

    self.llRound = self:getChildWindow("TowerDefenseSettlement-round_num", GUIType.Layout)
    self.llAllRound = self:getChildWindow("TowerDefenseSettlement-round_all_num", GUIType.Layout)

    self.stReward = self:getChildWindow("TowerDefenseSettlement-reward-num2", GUIType.StaticText)
    self.stPlayAgain = self:getChildWindow("TowerDefenseSettlement-playagain_text", GUIType.StaticText)
    self.stLogin = self:getChildWindow("TowerDefenseSettlement-reward-num4", GUIType.StaticText)
    self.stTitle = self:getChildWindow("TowerDefenseSettlement-title_text", GUIType.StaticText)
    self.stRoundText = self:getChildWindow("TowerDefenseSettlement-round_text", GUIType.StaticText)
    self.stFinalRoundText = self:getChildWindow("TowerDefenseSettlement-finalyround", GUIType.StaticText)

    self.btBack = self:getChildWindow("TowerDefenseSettlement-bt_back", GUIType.Button)
    self.btPlayAgain = self:getChildWindow("TowerDefenseSettlement-bt_playagain", GUIType.Button)

    self.isWin = true
    local text = Lang:getString("tower.defense.round.text")
    if text ~= "tower.defense.round.text" then
        self.stRoundText:SetText(text)
    end

    self:updateLanguage()

    self:initSettlementInfoList()

    self.btBack:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickBack()
    end)

    self.btPlayAgain:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickPlayAgain()
    end)

    self:registerDataCallBack("Settlement", function(data)
        self:updateSettlementInfo(data)
    end, JsonBuilderProcessor)

    --[[self:registerDataCallBack("Reward", function(data)
        self:updateRewardInfo(data)
    end, DataBuilderProcessor)--]]

    --更新playAgain
    self:registerDataCallBack("PlayAgainStatus", function(data)
        self:updatePlayAgain(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("PlayAgain", function()
        self:hideSettlement()
    end)

    --self.root:SetAlpha(0)
end

function UITowerDefenseSettlement:updateLanguage()
    self:getChildWindow("TowerDefenseSettlement-finalyround", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-finalyround"))
    self:getChildWindow("TowerDefenseSettlement-back_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-back"))
    self:getChildWindow("TowerDefenseSettlement-reward-text1", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-time"))
    self:getChildWindow("TowerDefenseSettlement-reward-text2", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-reward"))
    self:getChildWindow("TowerDefenseSettlement-reward-text3", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-boss"))
    self:getChildWindow("TowerDefenseSettlement-reward-text4", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseSettlement-login"))
    self.stPlayAgain:SetText(Lang:getString("gui_TowerDefenseSettlement-playagain"))
    self.stTitle:SetText(Lang:getString("gui_TowerDefenseSettlement-title"))
end

function UITowerDefenseSettlement:initSettlementInfoList()
    local llSettlementInfo = self:getChildWindow("TowerDefenseSettlement-settlementitem")

    self.gvSettlementInfo = IGUIGridView.new("TowerDefenseSettlement-settlementinfoview", llSettlementInfo)
    self.gvSettlementInfo:setArea({ 0, 0 }, { 0, 60 }, { 1, 0 }, { 0, 300 })
    self.gvSettlementInfo:setConfig(0, 10, 1)
    self.gvSettlementInfo.root:SetHorizontalAlignment(HorizontalAlignment.Center)
    self.gvSettlementInfo.root:SetVerticalAlignment(VerticalAlignment.Top)

    for i = 1, 4, 1 do
        local SettlementInfo = UISettlementItem.new("TowerDefenseSettlementItem.json")
        SettlementInfo:setWidth({ 1, 0 })
        SettlementInfo:setHeight({ 0, 70 })
        SettlementInfo:initInfo(i)
        self.gvSettlementInfo:addItem(SettlementInfo)
    end
end

function UITowerDefenseSettlement:updateSettlementInfo(data)
    self:clearSettlement()
    for i, player in pairs(data) do
        self.gvSettlementInfo:getItem("index", i):updateInfo(player)
    end
end

function UITowerDefenseSettlement:clearSettlement()
    for i = 1, 4, 1  do
        self.gvSettlementInfo:getItem("index", i):initInfo(i)
    end
end

function UITowerDefenseSettlement:updateRewardInfo(data)
    local round = data:getNumberParam("round")
    GUINumberItem.setNumberToLayout(round, self.llRound)
    GUINumberItem.setNumberToLayout(data:getNumberParam("allRound"), self.llAllRound)
    self.stReward:SetText(data:getNumberParam("reward"))
    self.stLogin:SetText(data:getNumberParam("loginReward"))
    local time = data:getNumberParam("time")
    local minute = math.floor(time / 60)
    local second = math.fmod(time, 60)
    local showTime = minute .. "′ " .. second .. " ″"
    if second < 10 then
        showTime = minute .. "′ 0" .. second .. "″"
    end
    self:getChildWindow("TowerDefenseSettlement-reward-num1", GUIType.StaticText):SetText(showTime)
    self:getChildWindow("TowerDefenseSettlement-reward-num3", GUIType.StaticText):SetText(data:getNumberParam("boss"))
    self:getChildWindow("TowerDefenseSettlement-map-name", GUIType.StaticText):SetText(Define.MapName[tonumber(data:getNumberParam("checkPoint"))])
    local isWin = (data:getNumberParam("round") == data:getNumberParam("allRound") and tonumber(GUIMain.stHomeHpNum:GetText()) > 0)
    self:changeBg(isWin)

    if round < 10 then
        self.llRound:SetWidth({ 0.2, 0 })
    end

    self:hideUI()
    self:show()
    --local alpha = 0
    --[[LuaTimer:scheduleTimer(function ()
        alpha = alpha + 0.02
        self.root:SetAlpha(alpha)
    end,40,50)--]]

end

function UITowerDefenseSettlement:changeBg(isWin)
    if self.isWin == isWin then
        return
    end
    self.isWin = isWin
    local bgType = self.BgType.Win
    local titleText = "gui_TowerDefenseSettlement-title"
    if not self.isWin then
        bgType = self.BgType.Lose
        titleText = "gui_TowerDefenseSettlement-title-lose"
    end
    --换皮
    self.siTitle:SetImage(bgType.titleBg)

    self.stTitle:SetText(Lang:getString(titleText))
end

function UITowerDefenseSettlement:updatePlayAgain(data)
    local playAgainNum = data:getNumberParam("agreedNum")
    local totalNum = data:getNumberParam("totalNum")
    self.stPlayAgain:SetText(Lang:getString("gui_TowerDefenseSettlement-playagain") .. "(" .. playAgainNum .. "/" .. totalNum .. ")")
end

function UITowerDefenseSettlement:onClickBack()
    self.btPlayAgain:SetEnabled(false)
    GamePacketSender:sendRequestLogout()
end

function UITowerDefenseSettlement:onClickPlayAgain()
    GamePacketSender:sendRequestPlayAgain()
end

function UITowerDefenseSettlement:hideUI()
    GUISupport:hide()
    GUIMain:hide()
    GUIManager:hideWindowByName("Main.json")
    GUIManager:hideWindowByName("ToolBar.json")
    GUITowerDetail:hide()
    GUIPassTip:hide()
    GUIGameBook:hide()
    GUIGameBookDetail:hide()
    GUISkillDetail:hide()
    GUIShopDetail:hide()
    GUIShop:hide()
end

function UITowerDefenseSettlement:hideSettlement()
    GUIMain.stGoldNum:SetText(0)
    GUIMain.stHomeHpNum:SetText("")
    GUISupport:show()
    GUIMain:show()
    GUIManager:showWindowByName("Main.json")
    GUIManager:showWindowByName("ToolBar.json")
    self:hide()
end

UITowerDefenseSettlement.BgType = {
    Win = {
        underBg = "set:tower_defense_hall_shop.json image:background",
        background = "set:towerdefensesettlement.json image:bg",
        showround = "set:towerdefensesettlement.json image:background",
        titleBg = "set:towerdefensesettlement.json image:title_victory",
        rank = "set:towerdefensesettlement.json image:rank",
        kill = "set:towerdefensesettlement.json image:kill",
        gold = "set:towerdefensesettlement.json image:gold",
        name = "set:towerdefensesettlement.json image:name",
        topNum = "set:towerdefensesettlement.json image:TopNum",
        miss = "set:towerdefensesettlement.json image:miss",
        sign = "set:towerdefensesettlement.json image:sign_win"
    },
    Lose = {
        underBg = "set:towerdefensesettlement.json image:under_bg_lose",
        background = "set:towerdefensesettlement.json image:bg_lose",
        showround = "set:towerdefensesettlement.json image:background_lose",
        titleBg = "set:towerdefensesettlement.json image:title_fail",
        rank = "set:towerdefensesettlement.json image:rank_lose",
        kill = "set:towerdefensesettlement.json image:kill_lose",
        gold = "set:towerdefensesettlement.json image:gold_lose",
        name = "set:towerdefensesettlement.json image:name_lose",
        topNum = "set:towerdefensesettlement.json image:TopNum_lose",
        miss = "set:towerdefensesettlement.json image:miss_lose",
        sign = "set:towerdefensesettlement.json image:sign_lose"
    }
}

return UITowerDefenseSettlement

