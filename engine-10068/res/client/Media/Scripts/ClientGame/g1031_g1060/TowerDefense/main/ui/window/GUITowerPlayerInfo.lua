UITowerPlayerInfo = class("UITowerPlayerInfo", IGUIWindow)

function UITowerPlayerInfo:onLoad()
    self.siBg = self:getChildWindow(0, GUIType.StaticImage)
    self.stName = self:getChildWindow(1, GUIType.StaticText)
    self.stMoney = self:getChildWindow(2, GUIType.StaticText)
    self.stKill = self:getChildWindow(3, GUIType.StaticText)
    self.stKillIcon = self:getChildWindow(4, GUIType.StaticImage)
    self.stMoneyIcon = self:getChildWindow(5, GUIType.StaticImage)
    self.stRank = self:getChildWindow(6, GUIType.StaticText)
    self.stRank:SetVisible(false)
end

function UITowerPlayerInfo:initInfo(i)
    self.index = i
    self.siBg:SetImage(IconMapConfig:getRankBackground(self.index))
    self.stRank:SetText("")
    self.stName:SetText("")
    self.stMoney:SetText("")
    self.stKill:SetText("")
    self.stMiss:SetText("")
end

function UITowerPlayerInfo:updateInfo(data, isShowRank)
    if isShowRank then
        self.stRank:SetVisible(true)
        self.siBg:SetImage("set:tower_defense_main.json image:own_rank")
        self.stKillIcon:SetImage("set:tower_defense_main.json image:own_kill_icon")
        self.stKillIcon:SetArea({ 0, -78 }, { 0, 0 }, { 0, 18 }, { 0, 13 })
        self.stMoneyIcon:SetImage("set:tower_defense_main.json image:own_gold")
        self.stMoneyIcon:SetArea({ 0, -198 }, { 0, 0 }, { 0, 18 }, { 0, 18 })
        self.stMoney:SetFont(GUIFontManager:Instance():GetFont("HT14"))
        self.stKill:SetFont(GUIFontManager:Instance():GetFont("HT14"))
        self.stName:SetFont(GUIFontManager:Instance():GetFont("HT14"))
        self.stName:SetTextColor({255 / 255, 211 / 255, 5 / 255})
        self.stRank:SetText(data.rank)
    else
        self.stRank:SetVisible(false)
        self.siBg:SetImage("set:tower_defense_main.json image:other_rank")
        self.stKillIcon:SetImage("set:tower_defense_main.json image:other_kill_icon")
        self.stKillIcon:SetArea({ 0, -80 }, { 0, 0 }, { 0, 16 }, { 0, 11 })
        self.stMoneyIcon:SetImage("set:tower_defense_main.json image:other_gold")
        self.stMoneyIcon:SetArea({ 0, -200 }, { 0, 0 }, { 0, 14 }, { 0, 14 })
        self.stMoney:SetFont(GUIFontManager:Instance():GetFont("HT12"))
        self.stKill:SetFont(GUIFontManager:Instance():GetFont("HT12"))
        self.stName:SetFont(GUIFontManager:Instance():GetFont("HT12"))
        self.stName:SetTextColor({192 / 255, 192 / 255, 192 / 255})
    end
    self.stName:SetText(data.name)
    self.stMoney:SetText(data.totalMoney)
    self.stKill:SetText(data.kill)
end

return UITowerPlayerInfo