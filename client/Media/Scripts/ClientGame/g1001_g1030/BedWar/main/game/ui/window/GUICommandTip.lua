GUICommandTip = class("GUICommandTip", IGUIWindow)

GUICommandTip.commandIndex = 0

function GUICommandTip:onLoad()
    self.stText = self:getChildWindowByName("BedWarMessageTip-text", GUIType.StaticText)
    self.stName = self:getChildWindowByName("BedWarMessageTip-name", GUIType.StaticText)
    self.llSenderBox = self:getChildWindowByName("BedWarMessageTip-sender", GUIType.Layout)
end

function GUICommandTip:init(sendId, commandId, config)
    local sender = GameInfoManager:getPlayerInfo(sendId)
    local teamId = tostring(sender:getValue("teamId"))
    self.stText:SetText(Lang:getString(config.text))
    self:playSound(commandId)

    local headIcon = GUIHeadIcon.new("BedWarHeadIcon.json")
    headIcon:initHeadIcon(sendId, teamId, "square")
    self.llSenderBox:AddChildWindow(headIcon.root)

    local info = GameInfoManager:getPlayerInfo(sendId)
    self.stName:SetText(info:getValue("userName"))

    local otherPlayer = PlayerManager:getPlayerByUserId(tonumber(sendId))
    if otherPlayer and otherPlayer.userId ~= PlayerManager:getClientPlayer().userId and otherPlayer.Player then
        otherPlayer.Player:enableXRay({ 0.09411, 0.68235, 0.95686, 1 })
    end

    LuaTimer:schedule(function()
        if otherPlayer.Player then
            otherPlayer.Player:disableXRay()
        end
    end, config.perspectiveTime * 1000)

    self.commandIndex = BaseMessage:addOrderList(teamId, commandId)
    return self.commandIndex
end

function GUICommandTip:playSound(commandId)
    if commandId == "1" or commandId == "2" or commandId == "3" then
        SoundUtil.playSound(SoundConfig.attackCommand)
    elseif commandId == "4" or commandId == "5" or commandId == "6" then
        SoundUtil.playSound(SoundConfig.defenseCommand)
    elseif commandId == "7" or commandId == "8" or commandId == "9" then
        SoundUtil.playSound(SoundConfig.togetherCommand)
    elseif commandId == "10" or commandId == "11" or commandId == "12" then
        SoundUtil.playSound(SoundConfig.callHelpCommand)
    end
end

return GUICommandTip
