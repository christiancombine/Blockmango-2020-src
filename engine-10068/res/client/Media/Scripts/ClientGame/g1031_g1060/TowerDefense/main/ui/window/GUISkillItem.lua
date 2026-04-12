UISkillItem = class("UISkillItem", IGUIWindow)

UISkillItem.skillId = 0
UISkillItem.skillPrice = 0
UISkillItem.offPrice = 0
UISkillItem.isFree = false
UISkillItem.isOff = false

function UISkillItem:onLoad()
    self.siSkill = self:getChildWindow(0, GUIType.StaticImage)
    self.llPrice = self:getChildWindow(0, GUIType.Layout, self.siSkill)

    self.siMoneyIcon = self:getChildWindow(0, GUIType.StaticImage, self.llPrice)
    self.stPrice = self:getChildWindow(1, GUIType.StaticText, self.llPrice)
    self.stOff = self:getChildWindow(2, GUIType.StaticText, self.llPrice)
    self.stDelete = self:getChildWindow(3, GUIType.StaticText, self.llPrice)
    self.stFree = self:getChildWindow(4, GUIType.StaticText, self.llPrice)

    self.siRedPoint = self:getChildWindow(1, GUIType.StaticImage)
    self.btSelect = self:getChildWindow(2, GUIType.Button)
    self.siEffect = self:getChildWindow(3, GUIType.StaticImage)
    self.siMask = self:getChildWindow(4, GUIType.StaticImage)

    self.btSelect:registerEvent(GUIEvent.ButtonClick, function()
        GUIMain:onClickSkillItem(self.skillId, self.skillPrice, self.offPrice, self.isFree, self.isOff)
    end)

    self.stFree:SetText(Lang:getString("gui_free"))
end

function UISkillItem:init(skillId)
    self.skillId = skillId
    local config = SkillStoreConfig:getConfigById(skillId)
    self.skillPrice = config.price
    self.siSkill:SetImage(config.icon)

    if config.off and #config.off > 0 then
        if config.off[1] == 0 then
            self:onFree()
            self.isFree = true
        elseif config.off[1] > 0 then
            self.skillPrice = config.price
            self.offPrice = config.off[1]

            self.stPrice:SetText(config.price)
            self.stOff:SetText(config.off[1])

            self:onOff()
            self.isOff = true
        end
    else
        self.stPrice:SetText(config.price)
        self.skillPrice = config.price
        self:onNotOff()
    end
end

function UISkillItem:onBuySuccess(data)
    local propConfig = PropsConfig:getConfigById(data.propId)
    self:updateStatus(data)
    self.llPrice:SetVisible(false)
    self.btSelect:SetTouchable(false)
    self.siEffect:SetEffectName(propConfig.boxEffect)
    self.siEffect:SetArea({ 0, -23 }, { 0, -88 }, { 0, 108 }, { 0, 118 })
    self.siEffect:SetVisible(true)

    self.siMask:SetVisible(true)
    self.siMask:setCooldownAnim(true, propConfig.cd)

    LuaTimer:scheduleTimer(function()
        self.llPrice:SetVisible(true)
        self.btSelect:SetTouchable(true)
        self.siEffect:SetVisible(false)
        self.siMask:SetVisible(false)
    end, propConfig.cd, 1)
end

function UISkillItem:updateStatus(data)
    if data.isFree then
        self.isFree = true
        self.isOff = false
        self:onFree()
    elseif not data.isFree and data.isOff then
        self.stPrice:SetText(data.price)
        self.stOff:SetText(data.off)

        self.isFree = false
        self.isOff = true
        self.skillPrice = data.price
        self.offPrice = data.off

        self:onOff()
    else
        self.stPrice:SetText(data.price)

        self.isFree = false
        self.isOff = false
        self.skillPrice = data.price

        self:onNotOff()
    end
end

function UISkillItem:onFree()
    self.siMoneyIcon:SetVisible(false)
    self.stPrice:SetVisible(false)
    self.stOff:SetVisible(false)
    self.stDelete:SetVisible(false)
    self.siRedPoint:SetVisible(true)
    self.stFree:SetVisible(true)
end

function UISkillItem:onOff()
    self.siMoneyIcon:SetVisible(true)
    self.stPrice:SetVisible(true)
    self.stOff:SetVisible(true)
    self.stDelete:SetVisible(true)
    self.siRedPoint:SetVisible(true)
    self.stFree:SetVisible(false)
end

function UISkillItem:onNotOff()
    self.siMoneyIcon:SetVisible(true)
    self.stPrice:SetVisible(true)
    self.stOff:SetVisible(false)
    self.stDelete:SetVisible(false)
    self.siRedPoint:SetVisible(false)
    self.stFree:SetVisible(false)
end

return UISkillItem
