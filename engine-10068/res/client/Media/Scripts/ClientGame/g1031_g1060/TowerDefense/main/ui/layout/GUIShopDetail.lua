require "ui.layout.GUIBuyHallGoldContent"

UIShopDetail = class("UIShopDetail", IGUILayout)
UIShopDetail.itemId = nil

function UIShopDetail:onLoad()
    self.skillLayout = {
        {
            siSkillIcon = self:getChildWindow("TowerDefenseHallShopDetail-skill1_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseHallShopDetail-skill1_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseHallShopDetail-skill1_detail", GUIType.StaticText)
        },
        {
            siSkillIcon = self:getChildWindow("TowerDefenseHallShopDetail-skill2_icon", GUIType.StaticImage),
            siSkillNull = self:getChildWindow("TowerDefenseHallShopDetail-skill2_null", GUIType.StaticImage),
            stTowerSkillDesc = self:getChildWindow("TowerDefenseHallShopDetail-skill2_detail", GUIType.StaticText)
        }
    }

    self.btBuy = self:getChildWindow("TowerDefenseHallShopDetail-buy", GUIType.Button)
    self.btClose = self:getChildWindow("TowerDefenseHallShopDetail-close_bt", GUIType.Button)

    self.stTowerTitle = self:getChildWindow("TowerDefenseHallShopDetail-title_text", GUIType.StaticText)
    self.stTowerName = self:getChildWindow("TowerDefenseHallShopDetail-name_text", GUIType.StaticText)
    self.stTowerPrice = self:getChildWindow("TowerDefenseHallShopDetail-price_num", GUIType.StaticText)
    self.stTowerAttack = self:getChildWindow("TowerDefenseHallShopDetail-attack_value", GUIType.StaticText)
    self.stTowerAttackType = self:getChildWindow("TowerDefenseHallShopDetail-attacktype_value", GUIType.StaticText)
    self.stTowerAttackRange = self:getChildWindow("TowerDefenseHallShopDetail-attackrange_value", GUIType.StaticText)
    self.stTowerAttackSpeed = self:getChildWindow("TowerDefenseHallShopDetail-attackspeed_value", GUIType.StaticText)
    self.stTowerStars = self:getChildWindow("TowerDefenseHallShopDetail-stars_num", GUIType.StaticText)
    self.stTowerStatus = self:getChildWindow("TowerDefenseHallShopDetail-status_text", GUIType.StaticText)
    self.stTowerOwnedTip = self:getChildWindow("TowerDefenseHallShopDetail-owntip", GUIType.StaticText)

    self.siBackground = self:getChildWindow("TowerDefenseHallShopDetail-background", GUIType.StaticImage)
    self.siTowerIcon = self:getChildWindow("TowerDefenseHallShopDetail-icon", GUIType.StaticImage)
    self.siMoneyIcon = self:getChildWindow("TowerDefenseHallShopDetail-money_icon", GUIType.StaticImage)
    self.siStatusIcon = self:getChildWindow("TowerDefenseHallShopDetail-status", GUIType.StaticImage)
    self.siAttackTypeIcon = self:getChildWindow("TowerDefenseHallShopDetail-attacktype_icon", GUIType.StaticImage)
    self.siTowerFormulaImage1 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_1", GUIType.StaticImage)
    self.siTowerFormulaImage2 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_2", GUIType.StaticImage)
    self.siTowerFormulaImage3 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_3", GUIType.StaticImage)
    self.siNameBg = self:getChildWindow("TowerDefenseHallShopDetail-name_bg", GUIType.StaticImage)

    self.llTowerFormula = self:getChildWindow("TowerDefenseHallShopDetail-formula", GUIType.Layout)
    self.llTowerFormulaBg1 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_1_bg", GUIType.Layout)
    self.llTowerFormulaBg2 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_2_bg", GUIType.Layout)
    self.llTowerFormulaBg3 = self:getChildWindow("TowerDefenseHallShopDetail-formula_image_3_bg", GUIType.Layout)

    self.stAttackTypeName = self:getChildWindow("TowerDefenseHallShopDetail-attacktype_name", GUIType.StaticText)

    self:initLanguage()
    self:initRegister()
end

function UIShopDetail:initRegister()
    self.btBuy:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickBuyButton()
    end)

    self.btClose:registerEvent(GUIEvent.ButtonClick, function()
        self:closeDetail()
    end)

    self:registerDataCallBack("BuySuccess", function(data)
        self:onGetBuySuccess(data)
    end, DataBuilderProcessor)

    self:registerEvent(GoRechargeEvent, function()
        GameAnalytics:design(1, { "G_buy_gcube" })
    end)
end

function UIShopDetail:initLanguage()
    self:getChildWindow("TowerDefenseHallShopDetail-title_text", GUIType.StaticText):SetText(Lang:getString("gui_shopdetailtitle"))
    self:getChildWindow("TowerDefenseHallShopDetail-attack_name", GUIType.StaticText):SetText(Lang:getString("gui_attack"))
    self:getChildWindow("TowerDefenseHallShopDetail-attackspeed_name", GUIType.StaticText):SetText(Lang:getString("gui_attackspeed"))
    self:getChildWindow("TowerDefenseHallShopDetail-attackrange_name", GUIType.StaticText):SetText(Lang:getString("gui_range"))
    self:getChildWindow("TowerDefenseHallShopDetail-formula_name", GUIType.StaticText):SetText(Lang:getString("gui_formulatip"))
    self.stTowerOwnedTip:SetText(Lang:getString("gui_own"))
    self.stTowerStatus:SetText(Lang:getString("gui_notown"))
end

function UIShopDetail:showDetailPage(itemId, fromGuide)
    self.itemId = itemId
    self.isGuide = fromGuide
    self:updateDetailInfo(itemId, fromGuide)
    self:show()

    if fromGuide then
        GameAnalytics:design(1, { "G_referer " })
    else
        GameAnalytics:design(1, { "G_shop_ID", self.itemId })
    end
end

function UIShopDetail:updateDetailInfo(itemId, fromGuide)
    local itemConfig = TowerStoreConfig:getConfigById(itemId)
    local towerConfig = TowerConfig:getConfigById(itemConfig.towerId)

    if fromGuide then
        self.stTowerTitle:SetText(Lang:getString("gui_shopguidetitle"))
        self.llTowerFormula:SetVisible(false)
        self.siStatusIcon:SetVisible(false)
        self.siTowerIcon:SetArea({ 0, 0 }, { 0, 50 }, { 0, 130 }, { 0, 130 })
        self.siNameBg:SetYPosition({ 0, -50 })
    else
        self.stTowerTitle:SetText(Lang:getString("gui_shopdetailtitle"))
        self.llTowerFormula:SetVisible(true)
        self.siStatusIcon:SetVisible(true)
        self.siTowerIcon:SetArea({ 0, 0 }, { 0, 30 }, { 0, 110 }, { 0, 110 })
        self.siNameBg:SetYPosition({ 0, -102 })
    end

    self.stTowerPrice:SetText(itemConfig.price)
    self.siMoneyIcon:SetImage(PayHelper.getMoneyIcon(itemConfig.moneyType))

    self.stTowerName:SetText(Lang:getString(towerConfig.Name))
    self.stTowerAttack:SetText(towerConfig.Attack)
    self.stAttackTypeName:SetText(Lang:getString(towerConfig.AttackTypeName))
    self.stTowerAttackType:SetText(Lang:getString(towerConfig.AttackTypeInfo))

    self.stTowerAttackRange:SetText(towerConfig.AttackRangeLang)
    self.stTowerAttackSpeed:SetText(self:getFalseTowerAttackSpeed(towerConfig.AttackSpeedA, towerConfig.AttackSpeedB, towerConfig.AttackSpeed))
    self.stTowerStars:SetText(towerConfig.StarLevel)
    self:resetSkill()
    for index, skillId in pairs(towerConfig.skillIds) do
        local skillConfig = SkillConfig:getConfigById(tonumber(skillId))
        if skillConfig ~= nil then
            self.skillLayout[index].siSkillNull:SetVisible(false)
            self.skillLayout[index].siSkillIcon:SetImage(skillConfig.Icon)
            self.skillLayout[index].stTowerSkillDesc:SetText(Lang:getString(skillConfig.Desc))
            self.skillLayout[index].stTowerSkillDesc:SetTextLineExtraSpace(-1)
        end
    end

    self.siTowerIcon:SetImage(towerConfig.IconName)
    self.siBackground:SetImage(IconMapConfig:getStarsBackground(tonumber(towerConfig.StarLevel)))
    self.siAttackTypeIcon:SetImage(IconMapConfig:getAttackTypeIcon(towerConfig.AttackType))

    if self.llTowerFormula:IsVisible() then
        if towerConfig.StarLevel >= 2 and towerConfig.FormulaImage1 ~= "#" then
            self.llTowerFormulaBg1:SetBackImage(IconMapConfig:getStarsBackground(towerConfig.StarLevel - 1))
            self.llTowerFormulaBg2:SetBackImage(IconMapConfig:getStarsBackground(towerConfig.StarLevel - 1))
            self.llTowerFormulaBg3:SetBackImage(IconMapConfig:getStarsBackground(towerConfig.StarLevel - 1))
        else
            self.llTowerFormulaBg1:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
            self.llTowerFormulaBg2:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
            self.llTowerFormulaBg3:SetBackImage("set:tower_defense_hall_detail.json image:null_bg")
        end

        self.siTowerFormulaImage1:SetImage(towerConfig.FormulaImage1)
        self.siTowerFormulaImage2:SetImage(towerConfig.FormulaImage2)
        self.siTowerFormulaImage3:SetImage(towerConfig.FormulaImage3)
    end

    local towerList = GamePlayer:Instance():getOwnedTowerList()
    if towerList[itemConfig.towerId] ~= nil and towerList[itemConfig.towerId].isHave then
        self.stTowerStatus:SetText(Lang:getString("gui_own"))
        self.btBuy:SetEnabled(false)
        self.btBuy:SetTouchable(false)
        self.stTowerPrice:SetTouchable(false)
        self.siMoneyIcon:SetTouchable(false)
        self.stTowerPrice:SetVisible(false)
        self.siMoneyIcon:SetVisible(false)
        self.stTowerOwnedTip:SetVisible(true)
    else
        self.stTowerStatus:SetText(Lang:getString("gui_notown"))
        self.btBuy:SetEnabled(true)
        self.btBuy:SetTouchable(true)
        self.stTowerPrice:SetTouchable(true)
        self.siMoneyIcon:SetTouchable(true)
        self.stTowerPrice:SetVisible(true)
        self.siMoneyIcon:SetVisible(true)
        self.stTowerOwnedTip:SetVisible(false)
    end
end

function UIShopDetail:onClickBuyButton()
    local towerList = GamePlayer:Instance():getOwnedTowerList()
    local itemConfig = TowerStoreConfig:getConfigById(self.itemId)
    if towerList[itemConfig.towerId] ~= nil and towerList[itemConfig.towerId].isHave then
        return
    end

    if itemConfig.moneyType == Define.Money.DiamondsGold then
        PayHelper.checkMoney(MoneyType.Diamonds, itemConfig.price, function(isEnough)
            if isEnough then
                self:onMoneyEnough(self.itemId, self.isGuide)
            end
        end)
    elseif itemConfig.moneyType == Define.Money.HallGold then
        if GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold) >= itemConfig.price then
            self:onMoneyEnough(self.itemId, self.isGuide)
        else
            self:onMoneyNotEnough(self.itemId, itemConfig.price - GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold), Params)
        end
    end
end

function UIShopDetail:onMoneyEnough(itemId, isGuide)
    GamePacketSender:sendBuyShopItem(itemId)
    if isGuide then
        GameAnalytics:design(1, { "G_referer_ID_buy", itemId })
    else
        GameAnalytics:design(1, { "G_shop_ID_buy", itemId })
    end

end

function UIShopDetail:onMoneyNotEnough(itemId, needMoneyNum, isGuide)
    self.isNotCloseTip = false
    if isGuide then
        GameAnalytics:design(0, { "G_referer_ID_buy", itemId })
    else
        GameAnalytics:design(0, { "G_shop_ID_buy", itemId })
    end

    local DiamondsNum = math.ceil(needMoneyNum / GamePlayer:Instance().ExchangeRate)

    BuyHallGoldContent:setCurrencyInfo(needMoneyNum, DiamondsNum, DiamondsNum * GamePlayer:Instance().ExchangeRate)
    local listener = function()
        PayHelper.checkMoney(MoneyType.Diamonds, DiamondsNum, function(isEnough)
            if isEnough then
                GamePacketSender:sendExchangeHallGold(itemId, DiamondsNum)
                self.isNotCloseTip = false
            else
                self.isNotCloseTip = true
            end
        end)
        return self.isNotCloseTip
    end

    CustomDialog.builder()
                .setTitleText(Lang:getString("lack_of_money"))
                .setContentView(BuyHallGoldContent)
                .setRightText(Lang:getString("gui_TowerDefenseDress-sure"))
                .setRightClickListener(listener)
                .setHideLeftButton()
                .show()
end

function UIShopDetail:closeDetail()
    self:hideDetail()
end

function UIShopDetail:hideDetail()
    self:hide()
end

function UIShopDetail:onGetBuySuccess(data)
    if self.itemId == data:getNumberParam("itemId") then
        self:updateDetailInfo(self.itemId)
    end

    LuaTimer:scheduleTimer(function()
        self:hideDetail()
    end, 250, 1)
end

function UIShopDetail:getFalseTowerAttackSpeed(a, b, attackSpeed)
    return math.ceil(((a - attackSpeed / 1000) / a) * b)
end

function UIShopDetail:resetSkill()
    for _, skill in pairs(self.skillLayout) do
        skill.siSkillNull:SetVisible(true)
    end
end

return UIShopDetail

