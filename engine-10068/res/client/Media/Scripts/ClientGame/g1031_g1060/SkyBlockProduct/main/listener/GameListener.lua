local function hideEngineWindow(name)
    local window = GUIManager:getWindowByName(name)
    if not window then
        return
    end
    window:SetXPosition({ 0, 6000 })
    window:SetVisible(false)
end

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameReadyEvent, self.onGameReady)
end

function GameListener.onGameReady()
    GameConfig:init()
    require "ui.GUISkyBlockProductItemDetail"
    require "ui.GUISkyBlockProductSellPreciousTip"
    require "ui.GUISkyBlockProductItemDetail"
    require "ui.GUIChristmas"
    require "ui.GUIToolBar"
    
    require "ui.GUISkyBlockProductMoneyDetail"
    require "ui.GUISkyBlockProductBuyMoneyTips"
    require "ui.GUITipDialog"
    
    if GUISumRechargeLayout ~= nil then
        GUISumRechargeLayout:setRewardWidth(77)
    end
    ClientHelper.putStringPrefs("RechargeGameType", "g1048")
    hideEngineWindow("Main-Watch-Battle-Text")
    hideEngineWindow("Main-Player-Exp")
    hideEngineWindow("PlayerInfo-Armor")
end

return GameListener