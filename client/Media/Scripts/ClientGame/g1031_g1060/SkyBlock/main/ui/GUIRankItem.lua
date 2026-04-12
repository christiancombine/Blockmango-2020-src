
UIRankItem = class("UIRankItem", IGUIWindow)

function UIRankItem:onLoad()
    print("UIRankItem:onLoad()")
    self.icon = self:getChildWindow(0 , GUIType.UrlImage)
    self.icon:setDefaultImage("set:userItem.json image:icon")
    self.name = self:getChildWindow(1)
    self.visitBtn = self:getChildWindow(2)
    self.visitBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onVisitButtonEvent()
    end)
    self.rank = self:getChildWindow(3):GetChildByIndex(0)
    self.score = self:getChildWindow(4):GetChildByIndex(0)
end

function UIRankItem:onVisitButtonEvent()
    PacketSender:sendCallOnManor(self.data.userId, 3, true, 0)
end

function UIRankItem:setItemData(data)
    self.data = data
    self:setDataUpdate()
end

function UIRankItem:setDataUpdate()
    if self.data.picUrl then
        self.icon:setUrl(self.data.picUrl)
    end
    self.name:SetText(self.data.name)
    self.rank:SetText(self.data.rank)
    self.score:SetText(self.data.score)
end