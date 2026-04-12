GUICommandItem = class("GUICommandItem", IGUIWindow)

function GUICommandItem:onLoad()
    self.stImage = self:getChildWindowByName("BedWarCommandItem", GUIType.StaticImage)
    self.siLock = self:getChildWindowByName("BedWarCommandItem-lock", GUIType.StaticImage)
    self.btButton = self:getChildWindowByName("BedWarCommandItem-bt", GUIType.Button)
    self:registerEvent()
    self.commandId = 0
end

function GUICommandItem:onDataChanged(data)
    self.data = data
    self.commandId = tonumber(data.id)
    self.isLock = not data.canUse
    self.siLock:SetVisible(self.isLock)
    self.stImage:SetImage(data.icon)
end

function GUICommandItem:registerEvent()
    self.btButton:registerEvent(GUIEvent.ButtonClick, function()
        self:onClick()
    end)
end

function GUICommandItem:onClick()
    GameAnalytics:design(1, { "ui_command", tostring(self.commandId) })
    if self.isLock == false then
        GamePacketSender:sendShortCommand(self.commandId)
        SoundUtil.playSound(SoundConfig.sendMessage)
        GUIMain:onCommandCD()
        GUICommandList:hide()
    else
        --购买弹窗
        GUICommandBuy:onOpen(self.commandId)
    end
end

return GUICommandItem
