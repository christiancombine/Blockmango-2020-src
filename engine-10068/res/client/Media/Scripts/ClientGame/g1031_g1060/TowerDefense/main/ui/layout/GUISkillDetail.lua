UISkillDetail = class("UISkillDetail", IGUILayout)

local CurrentSkillId = 0
local IsSkip = true
local CurrentPrice = 0

function UISkillDetail:onLoad()
    self.btClose = self:getChildWindow("TowerDefenseSkillDetail-close", GUIType.Button)
    self.btSkip = self:getChildWindow("TowerDefenseSkillDetail-skip_bt", GUIType.Button)
    self.btBuy = self:getChildWindow("TowerDefenseSkillDetail-buy_bt", GUIType.Button)

    self.stBuyTip = self:getChildWindow("TowerDefenseSkillDetail-buy_tip", GUIType.StaticText)
    self.stSkillDesc = self:getChildWindow("TowerDefenseSkillDetail-skill_desc", GUIType.StaticText)
    self.stPrice = self:getChildWindow("TowerDefenseSkillDetail-price_text", GUIType.StaticText)
    self.stOffPrice = self:getChildWindow("TowerDefenseSkillDetail-price_off", GUIType.StaticText)
    self.stDelete = self:getChildWindow("TowerDefenseSkillDetail-delete", GUIType.StaticText)
    self.stFree = self:getChildWindow("TowerDefenseSkillDetail-free", GUIType.StaticText)

    self.siSkip = self:getChildWindow("TowerDefenseSkillDetail-skip_sure", GUIType.StaticImage)
    self.siMoney = self:getChildWindow("TowerDefenseSkillDetail-price_icon", GUIType.StaticImage)

    self:updateLanguage()
    self:initRegister()
end

function UISkillDetail:initRegister()
    self.btClose:registerEvent(GUIEvent.ButtonClick, function()
        self:closeDetail()
    end)

    self.btSkip:registerEvent(GUIEvent.ButtonClick, function()
        self:clickSkip()
    end)

    self.btBuy:registerEvent(GUIEvent.ButtonClick, function()
        self:clickBuy()
    end)

end

function UISkillDetail:updateLanguage()
    self:getChildWindow("TowerDefenseSkillDetail-title_text", GUIType.StaticText):SetText(Lang:getString("gui_skilldetailtitle"))
    self:getChildWindow("TowerDefenseSkillDetail-skip_tip", GUIType.StaticText):SetText(Lang:getString("gui_skillskiptip"))

    self.stFree:SetText(Lang:getString("gui_free"))
end

function UISkillDetail:showDetailPage(skillId, price, offPrice, isFree, isOff)
    IsSkip = true
    CurrentSkillId = skillId
    self.siSkip:SetVisible(true)
    if GUIMain.skillSkipInfo[CurrentSkillId] == false then
        self.siSkip:SetVisible(false)
        IsSkip = false
    end

    local config = SkillStoreConfig:getConfigById(skillId)
    self:updateDetailInfo(config)

    self.stPrice:SetText(price)
    self.stOffPrice:SetText(offPrice)
    self.stDelete:SetVisible(isFree)
    if isFree then
        CurrentPrice = 0
        self.stFree:SetVisible(true)
        self.siMoney:SetVisible(false)
        self.stPrice:SetVisible(false)
        self.stOffPrice:SetVisible(false)
        self.stDelete:SetVisible(false)
    elseif isOff then
        CurrentPrice = offPrice
        self.stFree:SetVisible(false)
        self.siMoney:SetVisible(true)
        self.stPrice:SetVisible(true)
        self.stOffPrice:SetVisible(true)
        self.stDelete:SetVisible(true)
    else
        CurrentPrice = price
        self.stFree:SetVisible(false)
        self.siMoney:SetVisible(true)
        self.stPrice:SetVisible(true)
        self.stOffPrice:SetVisible(false)
        self.stDelete:SetVisible(false)
    end

    self:show()
end

function UISkillDetail:updateDetailInfo(skillConfig)
    self.stBuyTip:SetText(Lang:getString(skillConfig.buyTip))
    self.stSkillDesc:SetText(Lang:getString(skillConfig.effectTip))
end

function UISkillDetail:closeDetail()
    self:hideDetail()
end

function UISkillDetail:clickSkip()
    IsSkip = not self.siSkip:IsVisible()
    self.siSkip:SetVisible(IsSkip)
end

function UISkillDetail:clickBuy()
    PayHelper.checkMoney(MoneyType.Diamonds, CurrentPrice, function(isEnough)
        if isEnough then
            self:onMoneyEnough(CurrentSkillId)
        end
    end)

    GUIMain.skillSkipInfo[CurrentSkillId] = IsSkip
end

function UISkillDetail:onMoneyEnough(skillId)
    GamePacketSender:sendBuySkill(skillId)
    self:closeDetail()
end

function UISkillDetail:hideDetail()
    self:hide()
end

return UISkillDetail
