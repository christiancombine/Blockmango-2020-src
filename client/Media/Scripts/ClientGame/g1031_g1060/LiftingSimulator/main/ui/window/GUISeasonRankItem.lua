GUISeasonRankItem = class("GUISeasonRankItem", IGUIWindow)

function GUISeasonRankItem:onLoad()
    self.siMyDataBg = self:getChildWindow(0, GUIType.StaticImage) 
    self.stRank = self:getChildWindow(1)
    self.stName = self:getChildWindow(2)
    self.stKill = self:getChildWindow(3)
    self.stScore = self:getChildWindow(4)
    self.stLv = self:getChildWindow(5)
end

function GUISeasonRankItem:setItemData(data)
    self.data = data
    self:setDataUpdate()
end

function GUISeasonRankItem:setDataUpdate()
    self.stRank:SetText(self.data.rank)
    self.stName:SetText(self.data.name)
    self.stKill:SetText(self.data.kill)
    self.stScore:SetText(self.data.score)
    self.stLv:SetText(self.data.lv)

    local duan = self:getDuanConfigByLv(self.data.lv)
    if duan > 0 then
        self.siMyDataBg:SetImage("set:LiftingSeasonRank.json image:duan_rankdatabg" .. duan)
    end
end

function GUISeasonRankItem:getDuanConfigByLv(lv)
    local lvConfig = GameConfig.championshipsLvConfig

    for i = 1, #lvConfig do
        if tonumber(lv) >= tonumber(lvConfig[i].lvDown) and tonumber(lv) <= tonumber(lvConfig[i].lvUp) then
            return i
        end
    end
    return 0
end
