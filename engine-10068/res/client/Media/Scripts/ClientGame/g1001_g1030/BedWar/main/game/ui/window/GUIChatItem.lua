GUIChatItem = class("GUIChatItem", IGUIWindow)

function GUIChatItem:onLoad()
    self.stText = self:getChildWindow(0, GUIType.StaticText)
end

function GUIChatItem:onGetChat(color, name, text)
    self.stText:SetText(string.format("%s<%s>▢FFCCCCCC %s", color, name, tostring(text)))
    return self.stText:GetTextStringHigh() + 5
end

function GUIChatItem:onGetSystemTip(text)
    self.stText:SetText(string.format("▢FFCCCCCC %s", tostring(text)))
    return self.stText:GetTextStringHigh() + 5
end

function GUIChatItem:getHeight()
    return self.stText:GetTextStringHigh() + 5
end
