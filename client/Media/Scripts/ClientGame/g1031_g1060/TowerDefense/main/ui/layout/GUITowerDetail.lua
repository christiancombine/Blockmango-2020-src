UITowerDetail = class("UITowerDetail", IGUILayout)

local CurrentTower = nil

function UITowerDetail:onLoad()
    self.skillLayout = {
        {
            siSkillIcon = self:getChildWindow("TowerDefenseDetail-skill1_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseDetail-skill1_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseDetail-skill1_text", GUIType.StaticText)
        },
        {
            siSkillIcon = self:getChildWindow("TowerDefenseDetail-skill2_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseDetail-skill2_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseDetail-skill2_text", GUIType.StaticText)
        }
    }
    self.stTowerName = self:getChildWindow("TowerDefenseDetail-title_text", GUIType.StaticText)
    self.stTowerValue = self:getChildWindow("TowerDefenseDetail-sell_value", GUIType.StaticText)
    self.stTowerAttack = self:getChildWindow("TowerDefenseDetail-attack_value", GUIType.StaticText)
    self.stTowerAttackSpeed = self:getChildWindow("TowerDefenseDetail-attackspeed_value", GUIType.StaticText)
    self.stTowerAttackRange = self:getChildWindow("TowerDefenseDetail-attackrange_value", GUIType.StaticText)

    self.stTowerAttackTypeInfo = self:getChildWindow("TowerDefenseDetail-attacktype_value", GUIType.StaticText)
    self.stTowerAttackTypeName = self:getChildWindow("TowerDefenseDetail-attacktype_text", GUIType.StaticText)

    self.stTowerStarNum = self:getChildWindow("TowerDefenseDetail-star_num", GUIType.StaticText)

    self.siTowerIcon = self:getChildWindow("TowerDefenseDetail-tower_icon", GUIType.StaticImage)
    self.siLock = self:getChildWindow("TowerDefenseDetail-lock_icon", GUIType.StaticImage)
    self.siUnlock = self:getChildWindow("TowerDefenseDetail-unlock_icon", GUIType.StaticImage)
    self.siTowerAttackTypeIcon = self:getChildWindow("TowerDefenseDetail-attacktype_icon", GUIType.StaticImage)

    self.llBackground = self:getChildWindow("TowerDefenseDetail-tower", GUIType.Layout)

    self.btLock = self:getChildWindow("TowerDefenseDetail-lock", GUIType.Button)
    self.btSell = self:getChildWindow("TowerDefenseDetail-sell", GUIType.Button)
    self.btClose = self:getChildWindow("TowerDefenseDetail-close", GUIType.Button)

    self:updateLanguage()

    self:registerEvent(ClickSelfTowerEvent, function(tower)
        self:showDetail(tower)
    end)
    self.btLock:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickLockButton()
    end)
    self.btSell:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickSellButton()
    end)
    self.btClose:registerEvent(GUIEvent.ButtonClick, function()
        self:closeDetail()
    end)

end

function UITowerDetail:updateLanguage()
    self:getChildWindow("TowerDefenseDetail-sell_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-sell"))
    self:getChildWindow("TowerDefenseDetail-attack_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-attack"))
    self:getChildWindow("TowerDefenseDetail-attackspeed_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-attackspeed"))
    self:getChildWindow("TowerDefenseDetail-attackrange_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-attackrange"))
    self:getChildWindow("TowerDefenseDetail-lock_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-lock"))
    self:getChildWindow("TowerDefenseDetail-unlock_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefenseDetail-unlock"))
end

function UITowerDetail:showDetail(tower)
    CurrentTower = tower
    if not CurrentTower or CurrentTower.config == nil then
        return
    end

    CurrentTower:showAttackRangeEffect()

    self.stTowerName:SetText(Lang:getString(CurrentTower.config.Name))
    self.stTowerStarNum:SetText(CurrentTower.config.StarLevel)
    self.stTowerValue:SetText(CurrentTower.config.SellPrice)
    self.stTowerAttack:SetText(CurrentTower.config.Attack)

    self.stTowerAttackRange:SetText(Lang:getString(CurrentTower.config.AttackRangeLang))
    self.stTowerAttackTypeName:SetText(Lang:getString(CurrentTower.config.AttackTypeName))
    self.stTowerAttackTypeInfo:SetText(Lang:getString(CurrentTower.config.AttackTypeInfo))

    if CurrentTower:getAttackValue() > CurrentTower.config.Attack then
        local attackAddValue = tostring(math.ceil(CurrentTower:getAttackValue() - CurrentTower.config.Attack))
        self.stTowerAttack:SetText(CurrentTower.config.Attack .. "▢FF00FF00" .. " + " .. attackAddValue)
    end
    local oldAttackSpeed = self:getFalseTowerAttackSpeed(CurrentTower.config.AttackSpeedA, CurrentTower.config.AttackSpeedB, CurrentTower.config.AttackSpeed)
    local newAttackSpeed = self:getFalseTowerAttackSpeed(CurrentTower.config.AttackSpeedA, CurrentTower.config.AttackSpeedB, CurrentTower:getAttackSpeedValue())
    if oldAttackSpeed < newAttackSpeed then
        self.stTowerAttackSpeed:SetText(oldAttackSpeed .. "▢FF00FF00" .. " + " .. (newAttackSpeed - oldAttackSpeed))
    else
        self.stTowerAttackSpeed:SetText(oldAttackSpeed)
    end

    self.siTowerIcon:SetImage(CurrentTower.config.IconName)
    self.siTowerAttackTypeIcon:SetImage(IconMapConfig:getAttackTypeIcon(CurrentTower.config.AttackType))

    self:resetSkill()
    for index, skillId in pairs(CurrentTower.config.skillIds) do
        local skillConfig = SkillConfig:getConfigById(tonumber(skillId))
        if skillConfig ~= nil then
            self.skillLayout[index].siSkillNull:SetVisible(false)
            self.skillLayout[index].siSkillIcon:SetImage(skillConfig.Icon)
            self.skillLayout[index].stTowerSkillDesc:SetText(Lang:getString(skillConfig.Desc))
            self.skillLayout[index].stTowerSkillDesc:SetTextLineExtraSpace(-2)
        end
    end

    if CurrentTower.isLock or tostring(CurrentTower.userId) ~= tostring(GamePlayer:Instance().userId) then
        self.siLock:SetVisible(true)
        self.siUnlock:SetVisible(false)
    else
        self.siLock:SetVisible(false)
        self.siUnlock:SetVisible(true)
    end

    if tostring(CurrentTower.userId) ~= tostring(GamePlayer:Instance().userId) then
        self.btSell:SetEnabled(false)
    else
        self.btSell:SetEnabled(true)
    end

    self.llBackground:SetBackImage(IconMapConfig:getStarsBackground(CurrentTower.config.StarLevel))

    self:show()
end

function UITowerDetail:onClickLockButton()
    if tostring(CurrentTower.userId) ~= tostring(GamePlayer:Instance().userId) then
        return
    end
    if CurrentTower == nil or CurrentTower.entityId == 0 then
        self:hideDetail()
        return
    end
    CurrentTower:onClickLock()
    if CurrentTower.isLock then
        self.siLock:SetVisible(true)
        self.siUnlock:SetVisible(false)
    else
        self.siLock:SetVisible(false)
        self.siUnlock:SetVisible(true)
    end
end

function UITowerDetail:onClickSellButton()
    if CurrentTower == nil or CurrentTower.entityId == 0 then
        self:hideDetail()
        return
    end
    GamePacketSender:sendSellTower(CurrentTower.serverId)
    self:hideDetail()
end

function UITowerDetail:closeDetail()
    self:hideDetail()
end

function UITowerDetail:hideDetail()
    CurrentTower:hideAttackRangeEffect()
    CurrentTower = nil
    self:hide()
end

function UITowerDetail:onTowerDestroy(tower)
    if CurrentTower == tower then
        self:hideDetail()
    end
end

function UITowerDetail:getFalseTowerAttackSpeed(a, b, attackSpeed)
    return math.ceil((1 / (attackSpeed / 1000)) * b)
end

function UITowerDetail:updateDetail(tower)
    if CurrentTower ~= tower then
        return
    end
    self:showDetail(tower)
end

function UITowerDetail:resetSkill()
    for _, skill in pairs(self.skillLayout) do
        skill.siSkillNull:SetVisible(true)
    end
end

return UITowerDetail

