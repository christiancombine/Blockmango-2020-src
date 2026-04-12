UIBookDetail = class("UIBookDetail", IGUILayout)

function UIBookDetail:onLoad()
    self.skillLayout = {
        {
            siSkillIcon = self:getChildWindow("TowerDefenseBookDetail-skill1_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseBookDetail-skill1_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseBookDetail-skill1_detail", GUIType.StaticText)
        },
        {
            siSkillIcon = self:getChildWindow("TowerDefenseBookDetail-skill2_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseBookDetail-skill2_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseBookDetail-skill2_detail", GUIType.StaticText)
        }
    }
    self.btClose = self:getChildWindow("TowerDefenseBookDetail-close_bt", GUIType.Button)

    self.stTowerName = self:getChildWindow("TowerDefenseBookDetail-name_text", GUIType.StaticText)
    self.stTowerAttack = self:getChildWindow("TowerDefenseBookDetail-attack_value", GUIType.StaticText)
    self.stTowerAttackType = self:getChildWindow("TowerDefenseBookDetail-attacktype_value", GUIType.StaticText)
    self.stTowerAttackRange = self:getChildWindow("TowerDefenseBookDetail-attackrange_value", GUIType.StaticText)
    self.stTowerAttackSpeed = self:getChildWindow("TowerDefenseBookDetail-attackspeed_value", GUIType.StaticText)
    self.stTowerStars = self:getChildWindow("TowerDefenseBookDetail-stars_num", GUIType.StaticText)
    self.stTowerStatus = self:getChildWindow("TowerDefenseBookDetail-status_text", GUIType.StaticText)

    self.siTowerIcon = self:getChildWindow("TowerDefenseBookDetail-icon", GUIType.StaticImage)
    self.siBackground = self:getChildWindow("TowerDefenseBookDetail-background", GUIType.StaticImage)
    self.siAttackTypeIcon = self:getChildWindow("TowerDefenseBookDetail-attacktype_icon", GUIType.StaticImage)
    self.siTowerFormulaImage1 = self:getChildWindow("TowerDefenseBookDetail-formula_image_1", GUIType.StaticImage)
    self.siTowerFormulaImage2 = self:getChildWindow("TowerDefenseBookDetail-formula_image_2", GUIType.StaticImage)
    self.siTowerFormulaImage3 = self:getChildWindow("TowerDefenseBookDetail-formula_image_3", GUIType.StaticImage)

    self.llTowerFormulaBg1 = self:getChildWindow("TowerDefenseBookDetail-formula_image_1_bg", GUIType.Layout)
    self.llTowerFormulaBg2 = self:getChildWindow("TowerDefenseBookDetail-formula_image_2_bg", GUIType.Layout)
    self.llTowerFormulaBg3 = self:getChildWindow("TowerDefenseBookDetail-formula_image_3_bg", GUIType.Layout)

    self.stAttackTypeName = self:getChildWindow("TowerDefenseBookDetail-attacktype_name", GUIType.StaticText)

    self:initLanguage()
    self:initRegister()
end

function UIBookDetail:initRegister()
    self.btClose:registerEvent(GUIEvent.ButtonClick, function()
        self:closeDetail()
    end)
end

function UIBookDetail:initLanguage()
    self:getChildWindow("TowerDefenseBookDetail-title_text", GUIType.StaticText):SetText(Lang:getString("gui_bookdetailtitle"))
    self:getChildWindow("TowerDefenseBookDetail-attack_name", GUIType.StaticText):SetText(Lang:getString("gui_attack"))
    self:getChildWindow("TowerDefenseBookDetail-attackspeed_name", GUIType.StaticText):SetText(Lang:getString("gui_attackspeed"))
    self:getChildWindow("TowerDefenseBookDetail-attackrange_name", GUIType.StaticText):SetText(Lang:getString("gui_range"))
    self:getChildWindow("TowerDefenseBookDetail-formula_name", GUIType.StaticText):SetText(Lang:getString("gui_formulatip"))
    self.stTowerStatus:SetText(Lang:getString("gui_find"))
end

function UIBookDetail:showDetailPage(towerId)
    if GamePlayer:Instance().towerList[towerId] and GamePlayer:Instance().towerList[towerId].isHaveBook then
        GamePlayer:Instance().towerList[towerId].isNew = false
        GUIBook:updateItemList()
    end
    GamePacketSender:sendOpenBookDetail(towerId)
    GUIBook:hideItemNewIcon(towerId)
    self:updateDetailInfo(towerId)
    self:show()
end

function UIBookDetail:updateDetailInfo(towerId)
    local config = TowerConfig:getConfigById(towerId)

    self.stTowerName:SetText(Lang:getString(config.Name))
    self.stTowerAttack:SetText(config.Attack)
    self.stAttackTypeName:SetText(Lang:getString(config.AttackTypeName))
    self.stTowerAttackType:SetText(Lang:getString(config.AttackTypeInfo))

    self.stTowerAttackRange:SetText(config.AttackRangeLang)
    self.stTowerAttackSpeed:SetText(self:getFalseTowerAttackSpeed(config.AttackSpeedA, config.AttackSpeedB, config.AttackSpeed))
    self.stTowerStars:SetText(config.StarLevel)
    self:resetSkill()
    for index, skillId in pairs(config.skillIds) do
        local skillConfig = SkillConfig:getConfigById(tonumber(skillId))
        if skillConfig ~= nil then
            self.skillLayout[index].siSkillNull:SetVisible(false)
            self.skillLayout[index].siSkillIcon:SetImage(skillConfig.Icon)
            self.skillLayout[index].stTowerSkillDesc:SetText(Lang:getString(skillConfig.Desc))
            self.skillLayout[index].stTowerSkillDesc:SetTextLineExtraSpace(-1)
        end
    end

    if GamePlayer:Instance():getOwnedTowerList()[towerId] and GamePlayer:Instance():getOwnedTowerList()[towerId].isHaveBook then
        self.siTowerIcon:SetImage(config.IconName)
    else
        self.siTowerIcon:SetImage(config.UnKnowIconName)
    end
    self.siAttackTypeIcon:SetImage(IconMapConfig:getAttackTypeIcon(config.AttackType))
    self.siBackground:SetImage(IconMapConfig:getStarsBackground(tonumber(config.StarLevel)))
    if config.StarLevel >= 2 and config.FormulaImage1 ~= "#" then
        self.llTowerFormulaBg1:SetBackImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))
        self.llTowerFormulaBg2:SetBackImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))
        self.llTowerFormulaBg3:SetBackImage(IconMapConfig:getStarsBackground(config.StarLevel - 1))
    else
        self.llTowerFormulaBg1:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
        self.llTowerFormulaBg2:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
        self.llTowerFormulaBg3:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
    end

    self.siTowerFormulaImage1:SetImage(config.FormulaImage1)
    self.siTowerFormulaImage2:SetImage(config.FormulaImage2)
    self.siTowerFormulaImage3:SetImage(config.FormulaImage3)

    local towerList = GamePlayer:Instance():getOwnedTowerList()
    if towerList[towerId] ~= nil and towerList[towerId].isHave then
        if towerList[towerId].isHaveBook then
            self.stTowerStatus:SetText(Lang:getString("gui_find"))
        else
            self.stTowerStatus:SetText(Lang:getString("gui_notfind"))
        end
    else
        self.stTowerStatus:SetText(Lang:getString("gui_notown"))
    end
end

function UIBookDetail:closeDetail()
    self:hideDetail()
end

function UIBookDetail:hideDetail()
    self:hide()
end

function UIBookDetail:getFalseTowerAttackSpeed(a, b, attackSpeed)
    return math.ceil(((a - attackSpeed / 1000) / a) * b)
end

function UIBookDetail:resetSkill()
    for _, skill in pairs(self.skillLayout) do
        skill.siSkillNull:SetVisible(true)
    end
end

return UIBookDetail
