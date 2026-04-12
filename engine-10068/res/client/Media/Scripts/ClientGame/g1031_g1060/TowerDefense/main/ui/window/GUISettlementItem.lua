UISettlementItem = class("UISettlementItem", IGUIWindow)

function UISettlementItem:onLoad()
    self.siBg =  self:getChildWindow(0, GUIType.StaticImage)
    self.stRank = self:getChildWindow(1, GUIType.StaticText)
    self.stName = self:getChildWindow(2, GUIType.StaticText)
    self.stGold = self:getChildWindow(3, GUIType.StaticText)
    self.stKill = self:getChildWindow(4, GUIType.StaticText)
    self.stMiss = self:getChildWindow(5, GUIType.StaticText)
    self.stMaxRound = self:getChildWindow(6, GUIType.StaticText)
end

function UISettlementItem:initInfo(i)
    self.index = i
    self.stRank:SetText("")
    self.stName:SetText("")
    self.stGold:SetText("")
    self.stKill:SetText("")
    self.stMiss:SetText("")
    self.stMaxRound:SetText("")
end

function UISettlementItem:updateInfo(data)
    self.stRank:SetText(data.rank)
    self.stName:SetText(data.name)
    self.stGold:SetText(data.totalMoney)
    self.stKill:SetText(data.kill)
    self.stMiss:SetText(data.miss)
    self.stMaxRound:SetText(data.maxRound)
end

return UISettlementItem


