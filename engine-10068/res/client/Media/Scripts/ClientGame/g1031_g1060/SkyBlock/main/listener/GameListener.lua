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
    require "ui.GUIMain"
    require "ui.GUIParty"
    require "ui.GUIToolBar"
    require "ui.GUICreateParty"
    require "ui.GUIFriend"
    require "ui.GUIFriendItem"
    require "ui.GUIFriendApplyItem"
    require "ui.GUINearFriendItem"
    require "ui.GUIFriendBuiltItem"
    require "ui.GUIFriendBuildApplyItem"
    require "ui.GUIFriendInvitation"
    require "ui.GUIInvitationFriendItem"
    require "ui.GUIApplyParty"
    require "ui.GUISign"
    require "ui.GUISkyBlockCommonDialog"
    require "ui.GUIRank"
    require "ui.GUIRankItem"
    require "ui.GUIRankTip"
    require "ui.GUIPlayerOperation"
    require "ui.GUICenterTips"
    require "ui.GUISellPreciousTip"
    require "ui.GUISkyBlockItemDetail"
    require "ui.GUIBulletin"
    require "ui.GUIBulletinDetailItem"
    require "ui.GUIBulletinTabItem"
    require "ui.GUIChristmas"
    require "ui.GUIChristmastree"
    require "ui.GUIChristmasTip"



    require "ui.GUISkyBlockMoneyDetail"
    require "ui.GUIBuyMoneyTips"
    require "ui.GUITipDialog"
    initSkyBlockCommonDialog("SkyBlockCommonDialog.json")

    if GUISumRechargeLayout ~= nil then
        GUISumRechargeLayout:setRewardWidth(77)
    end

    if GameConfig.lightConfig then
        local threshold = GameConfig.lightConfig[1] or 0
        local saturation = GameConfig.lightConfig[2] or 0.6
        local intensity = GameConfig.lightConfig[3] or 3
        local blurMultiplier = GameConfig.bloomBlurMultiplier or 1
        local blurDeviation = GameConfig.bloomBlurDeviation or 7
        local blockLightEnhancement = GameConfig.blockLightEnhancement or 0
        HostApi.log("bloom threshold " .. threshold)
        HostApi.log("bloom saturation " .. saturation)
        HostApi.log("bloom intensity " .. intensity)
        HostApi.log("bloom blur multiplier " .. blurMultiplier)
        HostApi.log("bloom blur deviation " .. blurDeviation)
        HostApi.log("block light enhancement " .. blockLightEnhancement)
        local bm = Blockman.Instance()
        bm:setBloomThreshold(threshold)
        bm:setBloomSaturation(saturation)
        bm:setBloomIntensity(intensity)
        bm:setBloomBlurMultiplier(blurMultiplier)
        bm:setBloomBlurDeviation(blurDeviation)
        bm:setBlockLightEnhancement(blockLightEnhancement)
    end

    hideEngineWindow("Main-Watch-Battle-Text")
    hideEngineWindow("Main-Player-Exp")
    hideEngineWindow("PlayerInfo-Armor")
end

return GameListener