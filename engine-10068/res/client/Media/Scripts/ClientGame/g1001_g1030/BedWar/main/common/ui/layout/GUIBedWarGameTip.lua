require "game.ui.window.GUIChatItem"
require "game.ui.window.GUINewKillBroadCastItem"
require "game.ui.window.GUICommandTip"

local levelTipType = {
    ["1008001"] = 1,
    ["1008010"] = 1,
    ["1008014"] = 1,
    ["1008019"] = 1,
    ["1008022"] = 2,
}

local function offsetTimer(itemHeight, offsetValue)
    if GUIBedWarGameTips.offSetKey ~= nil then
        LuaTimer:cancel(GUIBedWarGameTips.offSetKey)
    end
    if GUIBedWarGameTips.itemOffSet > itemHeight then
        GUIBedWarGameTips.offSetKey = LuaTimer:scheduleTimer(function()
            for _, tip in pairs(GUIBedWarGameTips.command) do
                local Y = tip.root:GetYPosition()
                tip.root:SetYPosition({ 0, Y[2] - offsetValue })
            end
            GUIBedWarGameTips.itemOffSet = GUIBedWarGameTips.itemOffSet - offsetValue
        end, 20, (GUIBedWarGameTips.itemOffSet - itemHeight) / offsetValue)
    end
end

local UIGameTips = class("UIGameTips", IGUILayout)

UIGameTips.itemOffSet = 0
UIGameTips.command = {}
UIGameTips.BroadCastList = {}

UIGameTips.SystemMapping = {
    GuideTip = "1",
    TeamTip = "2",
    GameTip = "3",
    ChatTip = "4",
}

function UIGameTips:onLoad()
    self.llOrder = self:getChildWindow("BedWarGameTips-order", GUIType.Layout)
    self.llTeamTip = self:getChildWindow("BedWarGameTips-team_tip", GUIType.Layout)
    self.llChat = self:getChildWindow("BedWarGameTips-chat", GUIType.Layout)
    self.llGameTip = self:getChildWindow("BedWarGameTips-game_tip", GUIType.Layout)
    self.llGuideTip = self:getChildWindow("BedWarGameTips-guide_tip", GUIType.Layout)
    self.llNewKillBroadCast = self:getChildWindow("BedWarGameTips-new_kill", GUIType.Layout)

    self.stGameText = self:getChildWindow("BedWarGameTips-game_text", GUIType.StaticText)
    self.stTeamText = self:getChildWindow("BedWarGameTips-content", GUIType.StaticText)
    self.stTeamTitle = self:getChildWindow("BedWarGameTips-title", GUIType.StaticText)
    self.stGuideText = self:getChildWindow("BedWarGameTips-guide_text", GUIType.StaticText)

    self.llTeamTip:SetVisible(false)
    self.llGameTip:SetVisible(false)
    self.llGuideTip:SetVisible(false)

    self:initRegister()
    self:initChatList()
    self:initKillBroadCast()
end

function UIGameTips:initChatList()
    if self.gvChatItems ~= nil then
        self.gvChatItems:destroy()
    end
    self.gvChatItems = IGUIGridView.new("BedWarChatList-Item", self.llChat)
    self.gvChatItems:setArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 })
    self.gvChatItems:setConfig(0, 0, 1)
    self.gvChatItems.root:SetTouchable(false)
    self.ContentSize = self.llChat:GetPixelSize().x
    self.ScrollOffset = self.llChat:GetPixelSize().y
end

function UIGameTips:initKillBroadCast()
    self.llNewKillBroadCastItem = GUINewKillBroadCastItem.new("NewKillBroadCast.json")
    self.llNewKillBroadCastItem.root:SetArea({ -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 })
    self.llNewKillBroadCastItem.root:SetVisible(false)
    self.llNewKillBroadCast:AddChildWindow(self.llNewKillBroadCastItem.root)
end

function UIGameTips:initRegister()
    self:registerDataCallBack("SystemTips", function(data)
        self:onGetSystemTip(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("Chat", function(data)
        self:onGetChat(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("Kill", function(data)
        self:onGetKillBroadCast(data)
    end, JsonBuilderProcessor)
end

function UIGameTips:onGetSystemTip(data)
    --- 系统消息
    local tipType = data:getParam("tipType")
    local text = data:getParam("text") or ""
    local tipStr = Lang:getTipString(tipType, text)
    local config = GameTipSettingConfig:getConfigByTipType(tipType)
    if not config then
        LogUtil.log("Lack SystemTip Config , tipType:" .. tipType, LogUtil.LogLevel.Error)
        return
    end
    for _, box in pairs(config.tipBox) do
        if box == UIGameTips.SystemMapping.GuideTip then
            self:showGuideTip(tipStr)
        elseif box == UIGameTips.SystemMapping.TeamTip then
            self:showTeamTip(tipStr)
        elseif box == UIGameTips.SystemMapping.GameTip then
            self:showGameTip(tipStr)
        elseif box == UIGameTips.SystemMapping.ChatTip then
            self:showChat(0, tipStr)
        end
    end
    self:playTipSound(tipType)
end

function UIGameTips:showGuideTip(tipStr)
    self.stGuideText:SetText(tipStr)
    self.llGuideTip:SetVisible(true)
    if self.GuideTipKey ~= nil then
        LuaTimer:cancel(self.GuideTipKey)
    end
    self.GuideTipKey = LuaTimer:schedule(function()
        self.llGuideTip:SetVisible(false)
    end, 5000)
end

function UIGameTips:showTeamTip(tipStr)
    self.llTeamTip:SetVisible(true)
    self.stTeamText:SetText(tipStr)
    self.stTeamTitle:SetText(Lang:getString("bed_war_danger"))
    if self.TeamTipKey ~= nil then
        LuaTimer:cancel(self.TeamTipKey)
    end
    self.TeamTipKey = LuaTimer:schedule(function()
        self.llTeamTip:SetVisible(false)
    end, 3000)
end

function UIGameTips:showGameTip(tipStr)
    self.stGameText:SetText(tipStr)
    self.llGameTip:SetVisible(true)
    if self.SystemTipKey ~= nil then
        LuaTimer:cancel(self.SystemTipKey)
    end
    self.SystemTipKey = LuaTimer:schedule(function()
        self.stGameText:SetText("")
        self.llGameTip:SetVisible(false)
    end, 5000)
end

function UIGameTips:showChat(sendId, text, context)
    local senderInfo = BaseInfoManager:getPlayerInfo(sendId)
    if senderInfo and senderInfo:getValue("mute_voice") == "1" then
        return
    end
    self:addChatItem(sendId, text or context)
    GUIChatInput:addChatItem(sendId, text or context)
end

function UIGameTips:onGetChat(data)
    local sendId = data:getNumberParam("userId")
    local Chat = data:getParam("Chat")
    self:showChat(sendId, Chat)
end

function UIGameTips:addChatItem(sendId, text)
    local chatItem = GUIChatItem.new("BedWarChatItem.json")
    chatItem:setWidth({ 0, self.ContentSize })
    local name
    local height
    local teamId
    if sendId == 0 or BaseInfoManager:getPlayerInfo(sendId) == nil then
        height = chatItem:onGetSystemTip(text)
    else
        local info = BaseInfoManager:getPlayerInfo(sendId)
        if info then
            name = info:getValue("userName")
            teamId = info:getValue("teamId")
            if tostring(teamId) == "0" then
                height = chatItem:onGetChat("", name, text)
            else
                height = chatItem:onGetChat(TeamConfig:getTeamColor(teamId), name, text)
            end
        end
    end
    self.ScrollOffset = self.ScrollOffset - height
    if self.ScrollOffset < 0 then
        LuaTimer:schedule(function()
            self.gvChatItems.root:SetScrollOffset(self.ScrollOffset)
        end, 1)
    end
    chatItem:setHeight({ 0, height })
    BaseMessage:addChatRecord(name, text)
    self.gvChatItems:addItem(chatItem)
    self.llChat:SetVisible(true)
    LuaTimer:cancel(self.showChatKey)
    self.showChatKey = LuaTimer:schedule(function()
        self.llChat:SetVisible(false)
    end, 10000)
end

function UIGameTips:onGetCommand(sendId, config, context)
    local commandItem = GUICommandTip.new("BedWarMessageTip.json")
    commandItem:setWidth({ 0, 380 })
    commandItem:setHeight({ 0, 66 })

    local commandIndex = commandItem:init(sendId, config.id, config)

    self.llOrder:AddChildWindow(commandItem.root)
    self.command[tostring(commandIndex)] = commandItem
    self.llOrder:SetVisible(true)

    ---新控件偏移量
    commandItem.root:SetYPosition({ 0, self.itemOffSet })
    ---控件滑动增量
    self.itemOffSet = self.itemOffSet + 80

    ---设置控件5秒后自动销毁
    LuaTimer:schedule(function()
        commandItem:onDestroy()
        GUIManager:destroyGUIWindow(commandItem.root)
        self.command[tostring(commandIndex)] = nil

        offsetTimer(80, 4)

        if self:getCurrCommandNum() == 0 then
            self.itemOffSet = 0
        end
    end, config.duration * 1000)

    offsetTimer(160, 4)
end

function UIGameTips:getCurrCommandNum()
    return TableUtil.getTableSize(self.command)
end

function UIGameTips:onGetKillBroadCast(data)
    table.insert(self.BroadCastList, data)
    local killerInfo = GameInfoManager:getPlayerInfo(data.killerId)
    local deadInfo = GameInfoManager:getPlayerInfo(data.deadId)
    if not killerInfo or not deadInfo then
        return
    end
    if tonumber(killerInfo:getValue("teamId")) == 0 or tonumber(deadInfo:getValue("teamId")) == 0 then
        return
    end
    local text = TeamConfig:getTeamColor(killerInfo:getValue("teamId")) .. killerInfo:getValue("userName") .. "\t" ..
            TeamConfig:getTeamColor(deadInfo:getValue("teamId")) .. deadInfo:getValue("userName")
    local tipStr = Lang:getTipString(1008005, text)
    self:addChatItem(0, tipStr)
    if TableUtil.getTableSize(self.BroadCastList) == 1 then
        self:onBroadCastStart()
    end
end

function UIGameTips:onBroadCastStart()
    if GUIBedWarGameTips:getCurrCommandNum() >= 1 or self.broadCastKey ~= nil then
        LuaTimer:schedule(function()
            self:onBroadCastStart()
        end, 1000)
    else
        self.llNewKillBroadCastItem:showKillBroadCast(self.BroadCastList[1])
        self.broadCastKey = LuaTimer:schedule(function()
            table.remove(self.BroadCastList, 1)
            self.broadCastKey = nil
            if TableUtil.getTableSize(self.BroadCastList) > 0 then
                self:onBroadCastStart()
            end
        end, 3500)
    end
end

function UIGameTips:playTipSound(tipType)
    if levelTipType[tipType] == 1 then
        SoundUtil.playSound(SoundConfig.levelUpStore)
    elseif levelTipType[tipType] == 2 then
        SoundUtil.playSound(SoundConfig.selfBedBreak)
    end
end

return UIGameTips

