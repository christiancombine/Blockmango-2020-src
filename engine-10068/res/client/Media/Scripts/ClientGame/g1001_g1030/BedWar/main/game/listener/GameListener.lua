local function updateShowMoreGame()
    if not Game:isEnableIndie() then
        ---主包不需要，只有分包回大厅需要展示更多游戏
        return
    end
    local showMoreGameDate = ClientHelper.getStringForKey("showMoreGameDate", "")
    if showMoreGameDate == DateUtil.getYearDay() then
        ---今天已经展示过了，不需要再展示了
        return
    end
    ClientHelper.putBoolForKey("canShowMoreGame", true)
end

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameReadyEvent, self.onGameReady)
    BaseListener.registerCallBack(GameActionTriggerEvent, self.onGameActionTriggerEvent)
    BaseListener.registerCallBack(GameMainTickEvent, function()
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            player:onUpdate()
        end
    end)

    CommonDataEvents:registerCallBack("BedWarToolBarVisible", function(data)
        BedWarToolBarEvent:invoke(data)
    end, DataBuilderProcessor)

    CommonDataEvents:registerCallBack("MapId", function(mapId)
        GameConfig.mapId = tonumber(mapId)
        if ResourcePointConfig and PublicResourceConfig then
            PublicResourceConfig:init(FileUtil.getConfigFromCsv("setting/PublicResource.csv"))
            ResourcePointConfig:init(FileUtil.getConfigFromCsv("setting/ResourcePoint.csv"))
        end
    end)

    CommonDataEvents:registerCallBack("GameStart", self.onGameStart)
    CommonDataEvents:registerCallBack("BuyKeepItem", self.onBuyKeepItem, DataBuilderProcessor)
    CommonDataEvents:registerCallBack("BuyRespawn", self.onBuyRespawn, DataBuilderProcessor)
    CommonDataEvents:registerCallBack("synWatcherResource", self.onSynResource, JsonBuilderProcessor)
end

function GameListener.onGameReady()
    BaseGameConfig:init()
    GameConfig:loadConfig()
    CustomSceneConfig:gameReady()
    require "common.data.ClientRecharge"
    require "game.data.ClientTeamResource"
    require "game.ui.GUIFactory"
    if Game.isOpenGM and Game:isOpenGM() then
        require "game.data.GMBedWar"
        require "common.data.GMBedWarCommon"
    end
    updateShowMoreGame()
    TreasureChallenge:init()
end

function GameListener.onGameActionTriggerEvent(type)
    if type == 20 or type == 21 then
        GUIResult:onShare(false)
        GUIResult8P:onShare(false)
        GUISettlementSingle:onShare(false)
    end
end

function GameListener.onGameStart()
    isGameStart = true
    --GUIBattleInfo:onGameStart()
    GUIBedWarToolBar:onGameStart()
    ClientStore:onGameStart()
    ResourcePointConfig:onGameStart()
end

function GameListener.onBuyKeepItem(data)
    local time = data:getNumberParam("time")
    local price = data:getNumberParam("price")
    local timer = LuaTimer:schedule(function()
        CustomDialog.hide()
    end, time * 1000)
    local havePrivilege = Privilege.checkHavePrivilege(105)
    CustomDialog.builder()
    if havePrivilege then
        CustomDialog.setContentText(Lang:getString("gui_dialog_tip_keep_item_and_revive_sure"))
                    .setLeftText("gui_bed_war_anon_revive")
                    .setRightText("gui_dialog_tip_revive_privilege_sure")
    else
        CustomDialog.setContentText(Lang:getString("gui_dialog_tip_keep_item_msg"))
                    .setLeftText("gui_dialog_tip_keep_item_cancel")
                    .setRightText("gui_dialog_tip_keep_item_sure")
    end
    CustomDialog.setExtraInfo(PayHelper.getMoneyIcon(Define.Money.DIAMOND), tostring(price))
                .setLeftClickListener(function()
        LuaTimer:cancel(timer)
        GamePacketSender:sendBuyKeepItem(false)
    end)        .setCloseClickListener(function()
        LuaTimer:cancel(timer)
        GamePacketSender:sendBuyKeepItem(false)
    end)
                .setRightClickListener(function()
        LuaTimer:cancel(timer)
        local canBuyKeepItem = true
        PayHelper.checkMoney(Define.Money.DIAMOND, price, function(canBuy)
            if canBuy then
                GamePacketSender:sendBuyKeepItem(true)
            end
            canBuyKeepItem = canBuy
        end)
        return not canBuyKeepItem
    end)        .show()
end

function GameListener.onBuyRespawn(data)
    local time = data:getNumberParam("time")
    local price = data:getNumberParam("price")
    local timer = LuaTimer:schedule(function()
        CustomDialog.hide()
    end, time * 1000)
    CustomDialog.builder()
                .setContentText(Lang:getString("gui_dialog_tip_keep_item_and_revive_sure"))
                .setLeftText("gui_dialog_tip_revive_cancel")
                .setRightText("gui_dialog_tip_revive_privilege_sure")
                .setExtraInfo(PayHelper.getMoneyIcon(Define.Money.DIAMOND), tostring(price))
                .setLeftClickListener(function()
        LuaTimer:cancel(timer)
        GamePacketSender:sendBuyRespawn(false)
    end)
                .setCloseClickListener(function()
        LuaTimer:cancel(timer)
        GamePacketSender:sendBuyRespawn(false)
    end)
                .setRightClickListener(function()
        LuaTimer:cancel(timer)
        local canBuySpawn = true
        PayHelper.checkMoney(Define.Money.DIAMOND, price, function(canBuy)
            if canBuy then
                GamePacketSender:sendBuyRespawn(true)
            end
            canBuySpawn = canBuy
        end)
        return not canBuySpawn
    end)        .show()
end

function GameListener.onShowSettingMenu()
    if not GUIBedWarSetting then
        return false
    end
    if GUIBedWarSetting:isShow() then
        GUIBedWarSetting:hide(true)
    else
        GUIBedWarSetting:show(true)
    end
    return false
end

function GameListener.onSynResource(data)
    ResourcePointConfig:setWatcherResource(data)
end

