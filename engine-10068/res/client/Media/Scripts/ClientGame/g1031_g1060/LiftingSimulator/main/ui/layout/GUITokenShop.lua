local UITokenShop = class("UITokenShop", IGUILayout)

function UITokenShop:onLoad()
    self.tabList = {}--tab列表
    self.selectTab = TokenTabKind.NotKind--选中的tab类型
    self.selectItemId = 0 --选中的商品的Id
    self.itemsGridView = {}--商品列表
    self.tokenNum = 0

    self.ivItemProperty = {}
    self.ivItemPropertyIcon = {}
    self.tvItemPropertyText = {}
    self.tvItemPropertyNum = {}

    self:initUI()
    self:initEvent()
    self:initTabList()
    self:addItemsGridView(true)
end

function UITokenShop:initUI()
    self.tvTitleCenterText = self:getChildWindow("LiftingSimulatorTokenShop-TitleCenter-Text", GUIType.StaticText)
    self.tvTitleCenterText:SetText(Lang:getString("LiftingSimulatorTokenShop.title.text"))
    self.btnTitleRightExit = self:getChildWindow("LiftingSimulatorTokenShop-TitleRight-Exit", GUIType.Button)
    self.lvContentTabList = self:getChildWindow("LiftingSimulatorTokenShop-ContentTab-List", GUIType.List)
    --self.gvContentItemsGridView = self:getChildWindow("LiftingSimulatorTokenShop-ContentItems-GridView", GUIType.GridView)
    self.llContentItemsGridView = self:getChildWindow("LiftingSimulatorTokenShop-ContentItems", GUIType.Layout)
    self.gvContentItemsGridView = IGUIGridView.new("LiftingSimulatorTokenShop-ContentItems-GridView", self.llContentItemsGridView)
    self.gvContentItemsGridView:setArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 })
    self.gvContentItemsGridView:setConfig(10, 10, 4)
    self.tvDetailTitle = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-Title", GUIType.StaticText)
    self.ivDetailItemBg = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-ItemBg", GUIType.StaticImage)
    self.ivDetailItemIcon = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-ItemIcon", GUIType.StaticImage)

    for i = 1, 3 do
        local strItemProperty = string.format("LiftingSimulatorTokenShop-ContentDetail-ItemProperty%d", i)
        self.ivItemProperty[i] = self:getChildWindow(strItemProperty, GUIType.StaticImage)
        local strPropertyIcon = string.format("LiftingSimulatorTokenShop-ContentDetail-ItemProperty%d-Icon", i)
        self.ivItemPropertyIcon[i] = self:getChildWindow(strPropertyIcon, GUIType.StaticImage)
        local strPropertyText = string.format("LiftingSimulatorTokenShop-ContentDetail-ItemProperty%d-Text", i)
        self.tvItemPropertyText[i] = self:getChildWindow(strPropertyText, GUIType.StaticText)
        local strPropertyNum = string.format("LiftingSimulatorTokenShop-ContentDetail-ItemProperty%d-Num", i)
        self.tvItemPropertyNum[i] = self:getChildWindow(strPropertyNum, GUIType.StaticText)
    end

    self.tvDetailDescribe = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-Describe", GUIType.StaticText)
    self.btnDetailButton = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-Button", GUIType.Button)
    self.ivDetailGold = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-Button-Gold", GUIType.StaticImage)

    self.ivContentDetailBg1 = self:getChildWindow("LiftingSimulatorTokenShop-ContentDetail-Bg", GUIType.StaticImage)
    self.tokenNumText = self:getChildWindow("LiftingSimulatorTokenShop-Token-Name", GUIType.StaticText)

end

function UITokenShop:initEvent()
    self.btnTitleRightExit:registerEvent(GUIEvent.ButtonClick, function()
        self:onButtonExit()
    end)

    self.btnDetailButton:registerEvent(GUIEvent.ButtonClick, function()
        self:onDetailButton()
    end)

    self:registerEvent(S2CLSTokenShopFitnessEquipUpdate, function(data)
        self:shopFitnessEquipUpdate(data)
    end)

    self:registerEvent(S2CLSTokenShopGeneUpdate, function(data)
        self:shopGeneUpdate(data)
    end)

    self:registerEvent(S2CLSTokenShopSkillUpdate, function(data)
        self:shopSkillUpdate(data)
    end)

    self:registerEvent(TokenShopItemClickEvent, function(itemId)
        self:onClickItem(itemId)
    end)

    self:registerEvent(S2CLSTokenShopTokenNumUpdate, function(data)
        self:onTokenShopTokenNumUpdate(data)
    end)

    self:registerEvent(S2CBuySuccess, function(data)
        self:onBuySuccess(data)
    end)

    self:registerEvent(S2CLSEquipmentOrUnloadSkin, function(data)
        self:onEquipmentOrUnloadSkin(data)
    end)

    self:registerEvent(PayDataInitEvent,function (payItemList, skinList, skinId)
        self:initItemUpdate(skinList, skinId)
    end)
end

function UITokenShop:onShow()
    GuideSystemSetting.onGuideDone(107)
end

function UITokenShop:shopFitnessEquipUpdate(data)
    local builder = json.decode(data)
    for _, Value in pairs(builder) do
        local temp = TokenFitnessEquipConfig:getFitnessEquipById(Value.Id)
        if temp then
            temp.Status = Value.Status
        end
        --LogUtil.log("UITokenShop:shopFitnessEquipUpdate " .. inspect.inspect(temp))
    end

    if self.selectTab == TokenTabKind.FitnessEquip then
        self:addItemsGridView(false)
        self:onClickFitnessEquipItem(self.selectItemId)
    end
end

function UITokenShop:shopGeneUpdate(data)
    local builder = json.decode(data)
    for _, Value in pairs(builder) do
        local temp = TokenGeneConfig:getGeneById(Value.Id)
        if temp then
            temp.Status = Value.Status
        end
    end

    if self.selectTab == TokenTabKind.Gene then
        self:addItemsGridView(false)
        self:onClickGeneItem(self.selectItemId)
    end
end

function UITokenShop:shopSkillUpdate(data)
    local builder = json.decode(data)
    for _, Value in pairs(builder) do
        local temp = TokenSkillConfig:getSkillById(Value.Id)
        if temp then
            temp.Status = Value.Status
        end
    end

    if self.selectTab == TokenTabKind.Skill then
        self:addItemsGridView(false)
        self:onClickSkillItem(self.selectItemId)
    end
end

function UITokenShop:initTabList()
    local i = 1
    for _, Value in pairs(TokenShopTabConfig:getTabs()) do

        local shopTab = GUITokenShopTab.new("LiftingSimulatorTokenShopTab.json")

        self.tabList[i] = shopTab

        --加图集
        local strBg = "set:TokenShop.json image:tab_nor"

        --local strIcon = "set:LiftingSimulatorShop1.json image:" .. Value.Icon

        local strIcon = Value.Icon

        shopTab:initTab(Value.Id, Value.Type, strBg, strIcon, Value.Name)
        --GUITokenShopTab:initTab(TabId, Kind, Bg, Icon, Text)

        shopTab.ivTabsBg:registerEvent(GUIEvent.Click, function()
            self:onClickTab(Value.Id, Value.Type)
        end)

        shopTab:setArea({ 0, 0 }, { 0, 0 }, { 0, 130 }, { 0, 90 })

        self.lvContentTabList:AddItem(shopTab.root, false)
        --shopTab.root:SetSize({ 0, 130 }, { 0, 90 })
        i = i + 1
    end

    --将第一个Tab设置为选中状态
    if #self.tabList >= 1 then
        --加图集
        local strBg = "set:TokenShop.json image:tab_select"
        self.tabList[1]:changeSelectStatus(strBg, TokenTabSelectStatus.Select)
        self.selectTab = self.tabList[1].kind
    end

end

function UITokenShop:addItemsGridView(flag)
    self.itemsGridView = {}
    self.gvContentItemsGridView:clearItems()
    self.ivDetailItemBg:SetArea({ 0, 0 }, { 0, 55 }, { 0, 120 }, { 0, 110 })
    self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 100 }, { 0, 90 })
    self.tvDetailDescribe:SetArea({ 0, 0 }, { 0, 345 }, { 0, 230 }, { 0, 60 })
    --self.gvContentItemsGridView:SetScrollOffset(0)
   --self.ivCostEffect:SetVisible(false)
    if self.selectTab == TokenTabKind.FitnessEquip then
        --print("addFitnessEquipView()")
        self.tvDetailDescribe:SetArea({ 0, 0 }, { 0, 235 }, { 0, 230 }, { 0, 170 })
        self:addFitnessEquipView(flag)
        --self.ivCostEffect:SetVisible(true)
    end
    if self.selectTab == TokenTabKind.Gene then
        --self.ivCostEffect:SetVisible(true)
        self.tvDetailDescribe:SetArea({ 0, 0 }, { 0, 235 }, { 0, 230 }, { 0, 170 })
        self:addGeneView(flag)
    end
    if self.selectTab == TokenTabKind.Skin then
        self.tvDetailDescribe:SetArea({ 0, 0 }, { 0, 235 }, { 0, 230 }, { 0, 170 })
        self:addSkinView(flag)
    end
    if self.selectTab == TokenTabKind.Skill then
        self.tvDetailDescribe:SetArea({ 0, 0 }, { 0, 260 }, { 0, 230 }, { 0, 170 })
        self:addSkillView(flag)
    end
end

function UITokenShop:addView(kind, Value, ItemIcon, index)
    local shopItem = GUITokenShopItem.new("LiftingSimulatorTokenShopItem.json")

    ItemIcon = Value.Icon

    shopItem:initItem(kind, Value, ItemIcon)

    local contentWidth = self.gvContentItemsGridView:getWidth()
    local contentHeight = self.gvContentItemsGridView:getHeight()
    local itemWidth = (contentWidth - 30) / 4
    local itemHeight = (contentHeight - 20) / 3

    shopItem:setArea({ 0, 0 }, { 0, 0 }, { 0, itemWidth }, { 0, itemHeight })
    self.gvContentItemsGridView:addItemByIndex(shopItem, index)

    return shopItem
end

function UITokenShop:addFitnessEquipView(flag)
    local i = 1

    table.sort(TokenFitnessEquipConfig:getFitnessEquips(), function(a, b)
        return a.sortId < b.sortId
    end)

    local skipNum = 0
    for _, Value in pairs(TokenFitnessEquipConfig:getFitnessEquips()) do
        local ItemIcon = "set:LiftingSimulatorShop1.json image:equipOrSkillBg"
        self.itemsGridView[i] = self:addView(TokenTabKind.FitnessEquip, Value, ItemIcon)
        i = i + 1
    end

    if flag and #self.itemsGridView >= 1 then
        self:resetPosFitnessEquip()
    end
end

function UITokenShop:resetPosFitnessEquip()

    table.sort(TokenFitnessEquipConfig:getFitnessEquips(), function(a, b)
        return a.sortId < b.sortId
    end)

    local i = 0
    local flag = false
    local itemId = -1
    for _, Value in pairs(TokenFitnessEquipConfig:getFitnessEquips()) do

        --print("---------"..Value.Id.."-----------")
        i = i + 1
        if Value.Status == TokenItemBuyStatus.Used then
            itemId = Value.Id
            flag = true
            break
        end
    end

    if flag == false then
        i = 1 
        itemId = self.itemsGridView[1].itemId
        flag = true
    end

    if flag then
        local height = 153.66*(math.floor((i-1)/4))+10*(math.floor((i-1)/4))
        self.gvContentItemsGridView.root:ResetPos1(-height)
        self:onClickFitnessEquipItem(itemId)
    end

end

function UITokenShop:addGeneView(flag)

    local i = 1

    table.sort(TokenGeneConfig:getGenes(), function(a, b)
        return a.sortId < b.sortId
    end)

    local skipNum = 0
    for _, Value in pairs(TokenGeneConfig:getGenes()) do       
        local ItemIcon = "set:LiftingSimulatorShop1.json image:geneBg"
        self.itemsGridView[i] = self:addView(TokenTabKind.Gene, Value, ItemIcon)
        i = i + 1
    end

    if flag and #self.itemsGridView >= 1 then
        self:resetPosGene()
    end
end

function UITokenShop:resetPosGene()

    table.sort(TokenGeneConfig:getGenes(), function(a, b)
        return a.sortId < b.sortId
    end)

    local i = 0
    local flag = false
    local itemId = -1
    for _, Value in pairs(TokenGeneConfig:getGenes()) do

        --print("---------"..Value.Id.."-----------")
        i = i + 1
        if Value.Status == TokenItemBuyStatus.Used then
            itemId = Value.Id
            flag = true
            break
        end
    end

    if flag == false then
        i = 1 
        itemId = self.itemsGridView[1].itemId
        flag = true
    end

    if flag then
        local height = 153.66*(math.floor((i-1)/4))+10*(math.floor((i-1)/4))
        self.gvContentItemsGridView.root:ResetPos1(-height)
        self:onClickGeneItem(itemId)
    end

end

function UITokenShop:addSkinView(flag)
    local i = 1
    table.sort(TokenSkinConfig:getSkins(), function(a, b)
        return a.sortId < b.sortId
    end)
    for _, Value in pairs(TokenSkinConfig:getSkins()) do
        --local shopItem = GUITokenShopItem.new("LiftingSimulatorTokenShopItem.json")
        local ItemIcon = "set:LiftingSimulatorShop1.json image:advancedBg"
        self.itemsGridView[i] = self:addView(TokenTabKind.Skin, Value, ItemIcon)
        --加图集
        i = i + 1
    end
    if flag and #self.itemsGridView >= 1 then
        self:onClickSkinItem(self.itemsGridView[1].itemId)
        self.gvContentItemsGridView.root:ResetPos(0)
    end
end

function UITokenShop:addSkillView(flag)

    local i = 1

    table.sort(TokenSkillConfig:getSkills(), function(a, b)
        if a.Status ~= b.Status then
            return a.Status > b.Status
        end
        return a.sortId < b.sortId
    end)

    for _, Value in pairs(TokenSkillConfig:getSkills()) do
        --local shopItem = GUITokenShopItem.new("LiftingSimulatorTokenShopItem.json")
        local ItemIcon = "set:LiftingSimulatorShop1.json image:equipOrSkillBg"
        self.itemsGridView[i] = self:addView(TokenTabKind.Skill, Value, ItemIcon)
        i = i + 1
    end
    if flag and #self.itemsGridView >= 1 then
        self:onClickSkillItem(self.itemsGridView[1].itemId)
        self.gvContentItemsGridView.root:ResetPos(0)
    end
end

--点击Tab
function UITokenShop:onClickTab(Id, Type)

    for _, shopTab in pairs(self.tabList) do
        if shopTab.tabId ~= Id then

            --加图集
            local strBg = "set:TokenShop.json image:tab_nor"
            shopTab:changeSelectStatus(strBg, TokenTabSelectStatus.NotSelect)

        else
            --加图集
            local strBg = "set:TokenShop.json image:tab_select"
            shopTab:changeSelectStatus(strBg, TokenTabSelectStatus.Select)
            --self.selectTab = shopTab.kind
        end
    end

    if self.selectTab ~= Type then
        self.selectTab = Type
        self:addItemsGridView(true)
    end

end

function UITokenShop:selectItem(itemId)
    for _, shopItem in pairs(self.itemsGridView) do
        if shopItem.itemId ~= itemId then

            --加图集
            local strBg = "set:LiftingSimulatorShop1.json image:ItemBg2"
            shopItem:changeSelectStatus(strBg, TokenTabSelectStatus.NotSelect)

        else
            --加图集
            local strBg = "set:LiftingSimulatorShop1.json image:itemBg1"
            shopItem:changeSelectStatus(strBg, TokenTabSelectStatus.Select)
        end

    end
end

function UITokenShop:lockItemDetail(Kind)
    self.btnDetailButton:SetVisible(false)
end

function UITokenShop:showItemDetail(Kind)
    for i = 1, 3 do
        self.ivItemProperty[i]:SetVisible(true)
    end
    self.btnDetailButton:SetVisible(true)
    self.ivDetailGold:SetVisible(true)
    self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 100 }, { 0, 90 })
end

function UITokenShop:skinSetDetail()
    for i = 1, 3 do
        self.ivItemProperty[i]:SetVisible(false)
    end
    self.btnDetailButton:SetVisible(true)
    self.ivDetailGold:SetVisible(true)
    self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 100 }, { 0, 80 })
end

function UITokenShop:getMoneyIconByMoneyType(Kind)
    local strMoneyIcon = ""
    if Kind == Define.MoneyType.SLGolds then
        --strMoneyIcon = PayHelper.getMoneyIcon(MoneyType.Golds)
        strMoneyIcon = "set:LiftingSimulatorShop1.json image:gold"
    end
    if Kind == Define.MoneyType.DiamondGolds2 then
        strMoneyIcon = PayHelper.getMoneyIcon(MoneyType.Diamonds)
        --strMoneyIcon = "set:LiftingSimulatorShop1.json image:DiamondBlues"
    end
    if Kind == Define.MoneyType.DiamondGolds1 then
        strMoneyIcon = PayHelper.getMoneyIcon(MoneyType.Diamonds)
        --strMoneyIcon = "set:LiftingSimulatorShop1.json image:DiamondGolds"
    end

    if Kind == Define.MoneyType.Token then
        strMoneyIcon = PayHelper.getMoneyIcon(Define.MoneyType.Token)
        --strMoneyIcon = "set:LiftingSimulatorShop1.json image:DiamondGolds"
    end

    return strMoneyIcon
end

function UITokenShop:onClickItem(itemId)
    if self.selectTab == TokenTabKind.FitnessEquip then
        self:onClickFitnessEquipItem(itemId)
    end
    if self.selectTab == TokenTabKind.Gene then
        self:onClickGeneItem(itemId)
    end
    if self.selectTab == TokenTabKind.Skin then
        self:onClickSkinItem(itemId)
    end
    if self.selectTab == TokenTabKind.Skill then
        self:onClickSkillItem(itemId)
    end
end

function UITokenShop:onClickFitnessEquipItem(itemId)

    local oneFitnessEquip = TokenFitnessEquipConfig:getFitnessEquipById(itemId)
    if oneFitnessEquip == nil then
        return
    end
    self:selectItem(itemId)

    --加属性

    if oneFitnessEquip.Status == TokenItemBuyStatus.Lock then
        self:lockItemDetail(TokenTabKind.FitnessEquip)
    else
        self:showItemDetail(TokenTabKind.FitnessEquip)
    end

    self.tvDetailTitle:SetText(Lang:getString(oneFitnessEquip.Name))
    --加图集
    local strDetailItemIcon = oneFitnessEquip.Icon
    self.ivDetailItemIcon:SetImage(strDetailItemIcon)

    --加图集
    local strItemPropertyIcon1 = "set:LiftingSimulatorShop1.json image:equipFeature"
    local strItemPropertyText1 = "Efficiency"
    local strItemPropertyNum1 = ""
    local payFitnessEquip = PayFitnessEquipConfig:getPayFitnessEquipConfig(oneFitnessEquip.payConfigId)
    local strDetailDescribe = Lang:getString(oneFitnessEquip.Desc)

    if (oneFitnessEquip.MoneyType == Define.MoneyType.DiamondGolds1 or oneFitnessEquip.MoneyType == Define.MoneyType.DiamondGolds2)
            and payFitnessEquip then
        local onePrivilege = PayItemConfig:getPayItemByType(Define.PayItemType.InfinityMuscle)
        local player = PlayerManager:getClientPlayer()
        if onePrivilege and (onePrivilege.Status == PayItemBuyStatus.Used or onePrivilege.Status == PayItemBuyStatus.Using) then
            strDetailDescribe = Lang:getString(payFitnessEquip.efficiencyFixHugeDes)
            local specialNum = string.format("%.0f", payFitnessEquip.efficiencyFixHuge)
            strItemPropertyNum1 =  Utility.toKMBString(tostring(payFitnessEquip.efficiencyFixHuge), 2)
            strDetailDescribe = string.format(strDetailDescribe,specialNum)
        elseif player and tonumber(player.property.advancedLevel) <= payFitnessEquip.unlockAdvancedLevel  then
            strDetailDescribe = Lang:getString(oneFitnessEquip.Desc)
            local specialStr = "+"..payFitnessEquip.efficiencyPercentage.."%"
            strItemPropertyNum1 = payFitnessEquip.efficiencyPercentage.."%"
            strDetailDescribe = string.format(strDetailDescribe,specialStr)
        elseif player and tonumber(player.property.advancedLevel) > payFitnessEquip.unlockAdvancedLevel then
            strDetailDescribe = Lang:getString(payFitnessEquip.efficiencyFixDes)
            if player.property.advancedLevel >= payFitnessEquip.invailedAdvancedLevel then
                local specialNum = string.format("%.0f", payFitnessEquip.efficiencyFix)
                strItemPropertyNum1 = Utility.toKMBString(tostring(payFitnessEquip.efficiencyFix), 2)
                strDetailDescribe = string.format(strDetailDescribe,specialNum)
            else
                local specialStr = "+"..payFitnessEquip.efficiencyPercentage.."%"
                strItemPropertyNum1 = payFitnessEquip.efficiencyPercentage.."%"
                strDetailDescribe = string.format(strDetailDescribe,specialStr)
            end

        end
    else
        strItemPropertyNum1 = Utility.toKMBString(tostring(oneFitnessEquip.Efficiency), 2)
        strDetailDescribe = string.format(strDetailDescribe,strItemPropertyNum1)
    end

    self.ivItemPropertyIcon[1]:SetImage(strItemPropertyIcon1)
    self.tvItemPropertyText[1]:SetText(Lang:getString(strItemPropertyText1))
    self.tvItemPropertyNum[1]:SetText(strItemPropertyNum1)
    self.ivItemProperty[2]:SetVisible(false)
    self.ivItemProperty[3]:SetVisible(false)

    self.tvDetailDescribe:SetText(strDetailDescribe)

    local strMoneyIcon = self:getMoneyIconByMoneyType(oneFitnessEquip.MoneyType)
    self.ivDetailGold:SetImage(strMoneyIcon)
    --self.tvDetailNum:SetText(tostring(oneFitnessEquip.Price))
    if oneFitnessEquip.Status == TokenItemBuyStatus.Unlock then
        local strButtonTitle = Utility.toKMBString(tostring(oneFitnessEquip.Price), 2)
        if self:itemNotShowBigNumMoney(oneFitnessEquip.MoneyType) then
            strButtonTitle = tostring(oneFitnessEquip.Price)
        end
        self.btnDetailButton:SetRenderOffset({ x = 20, y = 0 })
        self.btnDetailButton:SetText(strButtonTitle)
        if itemId == 111 then
            self.btnDetailButton:SetVisible(false)
        end
    end
    if oneFitnessEquip.Status == TokenItemBuyStatus.Buy then
        --多语言
        local strButtonTitle = "GUIShop_Used" --使用
        self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
        self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
        self.ivDetailGold:SetVisible(false)
        --self.tvDetailNum:SetVisible(false)

    end
    if oneFitnessEquip.Status == TokenItemBuyStatus.Used then
        --多语言
        local strButtonTitle = "GUIShop_Using" --正使用
        self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
        self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
        self.ivDetailGold:SetVisible(false)
        --self.tvDetailNum:SetVisible(false)
    end

    self.selectItemId = itemId

end

function UITokenShop:onClickGeneItem(itemId)

    local gene = TokenGeneConfig:getGeneById(itemId)

    if gene == nil then
        return
    end

    self:selectItem(itemId)

    if gene.Status == TokenItemBuyStatus.Lock then
        self:lockItemDetail(TokenTabKind.Gene)
    else
        self:showItemDetail(TokenTabKind.Gene)
    end
        self.tvDetailTitle:SetText(Lang:getString(gene.Name))
        --加图集
        local strDetailItemIcon = gene.Icon
        self.ivDetailItemIcon:SetImage(strDetailItemIcon)
        self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 85 }, { 0, 85 })
        --加图集
        local strItemPropertyIcon1 = "set:LiftingSimulatorShop1.json image:geneFeature"
        local strItemPropertyText1 = "MaxMuscle"

        local strItemPropertyNum1 = Utility.toKMBString(string.format("%f", gene.MaxMuscle), 2)

        self.ivItemPropertyIcon[1]:SetImage(strItemPropertyIcon1)
        self.tvItemPropertyText[1]:SetText(Lang:getString(strItemPropertyText1))
        self.tvItemPropertyNum[1]:SetText(strItemPropertyNum1)
        self.ivItemProperty[2]:SetVisible(false)
        self.ivItemProperty[3]:SetVisible(false)

        local strDesc = string.format(Lang:getString(gene.Desc), strItemPropertyNum1)
        self.tvDetailDescribe:SetText(strDesc)

        local strMoneyIcon = self:getMoneyIconByMoneyType(gene.MoneyType)
        self.ivDetailGold:SetImage(strMoneyIcon)
        --self.tvDetailNum:SetText(tostring(gene.Price))
        if gene.Status == TokenItemBuyStatus.Unlock then
            
            local strButtonTitle = Utility.toKMBString(tostring(gene.Price), 2)
            if self:itemNotShowBigNumMoney(gene.MoneyType) then
                strButtonTitle = tostring(gene.Price)
            end

            self.btnDetailButton:SetRenderOffset({ x = 20, y = 0 })
            self.btnDetailButton:SetText(strButtonTitle)
            if itemId == 208 then
                self.btnDetailButton:SetVisible(false)
            end
        end
        if gene.Status == TokenItemBuyStatus.Buy then
            --多语言
            local strButtonTitle = "GUIShop_Used" --使用
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)
            --self.btnDetailButton:SetVisible(false)

        end
        if gene.Status == TokenItemBuyStatus.Used then
            --多语言
            local strButtonTitle = "GUIShop_Using" --正使用
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)
        end

    self.selectItemId = itemId

end

function UITokenShop:onClickSkinItem(itemId)

    local skin = TokenSkinConfig:getSkinById(itemId)
    if skin == nil then
        return
    end
    self:selectItem(itemId)

    -- if skin.Status == TokenItemBuyStatus.Lock then
    --     self:lockItemDetail(TokenTabKind.Skin)
    -- else
        self:skinSetDetail(TokenTabKind.Skin)
    -- end
        self.tvDetailTitle:SetText(Lang:getString(skin.Name))

        --self.ivDetailItemBg:SetArea({ 0, 0 }, { 0, 55 }, { 0, 170 }, { 0, 105 })
        --加图集
        local strDetailItemIcon = skin.Icon
        self.ivDetailItemIcon:SetImage(strDetailItemIcon)

        self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 100 }, { 0, 80 })

        self.tvDetailDescribe:SetText(Lang:getString(skin.Desc))

        local strMoneyIcon = self:getMoneyIconByMoneyType(skin.MoneyType)
        self.ivDetailGold:SetImage(strMoneyIcon)
        --self.tvDetailNum:SetText(tostring(skin.Price))

        if skin.Status == TokenItemBuyStatus.Unlock then
            local strButtonTitle = Utility.toKMBString(string.format("%f", skin.Price), 2)
            self.btnDetailButton:SetRenderOffset({ x = 20, y = 0 })
            self.btnDetailButton:SetText(strButtonTitle)
        end

        if skin.Status == TokenItemBuyStatus.Buy then
            --多语言
            local strButtonTitle = "GUIShop_Used"
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)

        end
        if skin.Status == TokenItemBuyStatus.Used then
            --多语言
            local strButtonTitle = "GUIShop_Unload"
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)
        end

    self.selectItemId = itemId

end

function UITokenShop:onClickSkillItem(itemId)

    local skill = TokenSkillConfig:getSkillById(itemId)

    if skill == nil then
        return
    end

    self:selectItem(itemId)

    if skill.Status == TokenItemBuyStatus.Lock then
        self:lockItemDetail(TokenTabKind.Skill)
    else
        self:showItemDetail(TokenTabKind.Skill)
    end
        self.tvDetailTitle:SetText(Lang:getString(skill.Name))
        --加图集
        local strDetailItemIcon = skill.Icon
        self.ivDetailItemIcon:SetImage(strDetailItemIcon)
        self.ivDetailItemIcon:SetArea({ 0, 0 }, { 0, 0 }, { 0, 100 }, { 0, 100 })
        --加图集
        local strItemPropertyIcon1 = "set:LiftingSimulatorShop1.json image:hurtFeature"
        local strItemPropertyText1 = "Hurt"
        local strItemPropertyNum1 = tostring(skill.Hurt)
        self.ivItemPropertyIcon[1]:SetImage(strItemPropertyIcon1)
        self.tvItemPropertyText[1]:SetText(Lang:getString(strItemPropertyText1))
        self.tvItemPropertyNum[1]:SetText(strItemPropertyNum1)

        local strItemPropertyIcon2 = "set:LiftingSimulatorShop1.json image:cdFeature"
        local strItemPropertyText2 = "Cd"
        local strItemPropertyNum2 = tostring(skill.Cd)
        self.ivItemPropertyIcon[2]:SetImage(strItemPropertyIcon2)
        self.tvItemPropertyText[2]:SetText(Lang:getString(strItemPropertyText2))
        self.tvItemPropertyNum[2]:SetText(strItemPropertyNum2)

        self.ivItemProperty[3]:SetVisible(false)

        self.tvDetailDescribe:SetText(Lang:getString(skill.Desc))

        local strMoneyIcon = self:getMoneyIconByMoneyType(skill.MoneyType)
        self.ivDetailGold:SetImage(strMoneyIcon)
        --self.tvDetailNum:SetText(tostring(skill.Price))
        if skill.Status == TokenItemBuyStatus.Unlock then
            local strButtonTitle = Utility.toKMBString(tostring(skill.Price), 2)
            self.btnDetailButton:SetRenderOffset({ x = 20, y = 0 })
            self.btnDetailButton:SetText(strButtonTitle)
        end
        if skill.Status == TokenItemBuyStatus.Buy then
            --多语言
            local strButtonTitle = "GUIShop_Used" --使用
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)

        end
        if skill.Status == TokenItemBuyStatus.Used then
            --多语言
            local strButtonTitle = "GUIShop_Unload" --卸下
            self.btnDetailButton:SetRenderOffset({ x = 0, y = 0 })
            self.btnDetailButton:SetText(Lang:getString(strButtonTitle))
            self.ivDetailGold:SetVisible(false)
            --self.tvDetailNum:SetVisible(false)
        end

    self.selectItemId = itemId

end

function UITokenShop:onButtonExit()
    self:hide()
end

function UITokenShop:showShop()
    RootGuiLayout.Instance():showMainControl()
    self:show()

    self:onClickTab(TokenTabKind.FitnessEquip, TokenTabKind.FitnessEquip)

    if self.selectTab == TokenTabKind.FitnessEquip then
        self:resetPosFitnessEquip()
    end
end

function UITokenShop:onToolBarGoldClick()
    self:show()
end

function UITokenShop:onFitnessEquipButton()

    local oneFitnessEquip = TokenFitnessEquipConfig:getFitnessEquipById(self.selectItemId)

    if oneFitnessEquip == nil then
        return
    end

    if oneFitnessEquip.Status == TokenItemBuyStatus.Unlock and
            (oneFitnessEquip.MoneyType == Define.MoneyType.DiamondGolds1 or oneFitnessEquip.MoneyType == Define.MoneyType.DiamondGolds2) then
        PayHelper.checkMoney(MoneyType.Diamonds, oneFitnessEquip.Price, function(isEnough)
            if isEnough then
                --print("onFitnessEquipButton()")
                self:senderDetailButtonClick()
            end
        end)
    elseif oneFitnessEquip.Status == TokenItemBuyStatus.Unlock and oneFitnessEquip.MoneyType == Define.MoneyType.Token
            and self.tokenNum < oneFitnessEquip.Price then

        local listener = function()
            LuaTimer:schedule(function()
                -- GUIMainControl:onClickWorkout()
            end, 1)
        end

        CustomDialog.builder()
                    .setContentText(Lang:getString("token_not_enough"))
                    .setRightClickListener(listener)
                    .setLeftText()
                    .show()

    else
        self:senderDetailButtonClick()
    end

end

function UITokenShop:onGeneButton()

    local gene = TokenGeneConfig:getGeneById(self.selectItemId)

    if gene == nil then
        return
    end

    if gene.Status == TokenItemBuyStatus.Unlock and
            (gene.MoneyType == Define.MoneyType.DiamondGolds1 or gene.MoneyType == Define.MoneyType.DiamondGolds2) then
        PayHelper.checkMoney(MoneyType.Diamonds, gene.Price, function(isEnough)
            if isEnough then
                --print("onGeneButton()")
                self:senderDetailButtonClick()
            end
        end)
    elseif gene.Status == TokenItemBuyStatus.Unlock and gene.MoneyType == Define.MoneyType.Token
            and self.tokenNum < gene.Price then

        local listener = function()
            LuaTimer:schedule(function()
                -- GUIMainControl:onClickWorkout()
            end, 1)
        end

        CustomDialog.builder()
                    .setContentText(Lang:getString("token_not_enough"))
                    .setRightClickListener(listener)
                    .setLeftText()
                    .show()

    else
        self:senderDetailButtonClick()
    end

end

function UITokenShop:onSkinButton()
    local oneSkin = TokenSkinConfig:getSkinById(self.selectItemId)

    if oneSkin == nil then
        return
    end

    if oneSkin.Status == TokenItemBuyStatus.Unlock
            and (oneSkin.MoneyType == Define.MoneyType.DiamondGolds1 or oneSkin.MoneyType == Define.MoneyType.DiamondGolds2) then
        PayHelper.checkMoney(MoneyType.Diamonds, oneSkin.Price, function(isEnough)
            if isEnough then
                --print("onGeneButton()")
                self:senderDetailButtonClick()
            end
        end)
    elseif oneSkin.Status == TokenItemBuyStatus.Unlock and oneSkin.MoneyType == Define.MoneyType.Token
            and self.tokenNum < oneSkin.Price then

        local listener = function()
            LuaTimer:schedule(function()
                -- GUIMainControl:onClickWorkout()
            end, 1)
        end

        CustomDialog.builder()
                    .setContentText(Lang:getString("token_not_enough"))
                    .setRightClickListener(listener)
                    .setLeftText()
                    .show()

    else
        self:senderDetailButtonClick()
    end
end

function UITokenShop:onSkillButton()

    local skill = TokenSkillConfig:getSkillById(self.selectItemId)

    if skill == nil then
        return
    end

    if skill.Status == TokenItemBuyStatus.Unlock and
            (skill.MoneyType == Define.MoneyType.DiamondGolds1 or skill.MoneyType == Define.MoneyType.DiamondGolds2) then
        PayHelper.checkMoney(MoneyType.Diamonds, skill.Price, function(isEnough)
            if isEnough then
                --print("onSkillButton()")
                self:senderDetailButtonClick()
            end
        end)
    elseif skill.Status == TokenItemBuyStatus.Unlock and skill.MoneyType == Define.MoneyType.Token
            and self.tokenNum < skill.Price then

        local listener = function()
            LuaTimer:schedule(function()
                -- GUIMainControl:onClickWorkout()
            end, 1)
        end

        CustomDialog.builder()
                    .setContentText(Lang:getString("token_not_enough"))
                    .setRightClickListener(listener)
                    .setLeftText()
                    .show()

    else
        self:senderDetailButtonClick()
    end
end

function UITokenShop:senderDetailButtonClick()
    local data = {
        Kind = self.selectTab,
        ItemId = self.selectItemId
    }
    local builder = DataBuilder.new():fromTable(data):getData()
    PacketSender:sendLuaCommonData("TokenShopButtonClick", builder)
end

--购买、使用逻辑
function UITokenShop:onDetailButton()
    if self.selectTab == TokenTabKind.FitnessEquip then
        self:onFitnessEquipButton()
    end
    if self.selectTab == TokenTabKind.Gene then
        self:onGeneButton()
    end
    if self.selectTab == TokenTabKind.Skin then
        self:onSkinButton()
    end
    if self.selectTab == TokenTabKind.Skill then
        self:onSkillButton()
    end
end

function UITokenShop:itemNotShowBigNumMoney(monryType)
    if (monryType == Define.MoneyType.DiamondGolds1 or monryType == Define.MoneyType.DiamondGolds2) then
        return true
    end
    return false
end

function UITokenShop:onTokenShopTokenNumUpdate(data)
    if not self:isLoad() then
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local curToken = builder:getNumberParam("token")
    self.tokenNum = curToken
    local curTokenNum = Utility.toKMBString(self.tokenNum, 2)
    -- local str = string.format(Lang:getString("token_shop_token_num_text"), curTokenNum)
    self.tokenNumText:SetText(curTokenNum)
end

function UITokenShop:onBuySuccess(data)

    local builder = DataBuilder.new():fromData(data)
    local type = builder:getNumberParam("Type")
    local skinList = {}

    if type == Define.PayStoreItem.Skin then
        local param = builder:getNumberParam("Param")
        skinList[tostring(param)] = 1
        --local str = string.format("GMHandler.onBuySuccess %d %d", type, param)
        --print(str)
    end

    self:initItemUpdate(skinList)
end

function UITokenShop:initItemUpdate(skinList, skinId)
    for key, value in pairs(skinList) do
        local oneSkin = TokenSkinConfig:getSkinById(tonumber(key))
        if oneSkin then
            oneSkin.Status = TokenItemBuyStatus.Buy
        end
    end

    local oneSkin = TokenSkinConfig:getSkinById(tonumber(skinId))
    if oneSkin then
        oneSkin.Status = TokenItemBuyStatus.Used
    end

    self:addItemsGridView(false)
    if self.selectTab == TokenTabKind.Skin then
        --self:addItemsGridView(false)
        self:onClickSkinItem(self.selectItemId)
    end
end

function UITokenShop:onEquipmentOrUnloadSkin(data)
    local builder = DataBuilder.new():fromData(data)
    local id = builder:getNumberParam("id")
    local status = builder:getBoolParam("status")
    local oneSkin = SkinConfig:getSkinById(id)
    local tokenOneSkin = TokenSkinConfig:getSkinById(id)

    if oneSkin == nil and tokenOneSkin == nil then
        return
    end


    for i, value in pairs(TokenSkinConfig:getSkins()) do
        if value.Status == TokenItemBuyStatus.Used then
            value.Status = TokenItemBuyStatus.Buy
        end
    end

    if tokenOneSkin then
        if status  == false then
            tokenOneSkin.Status = TokenItemBuyStatus.Buy
        else
            tokenOneSkin.Status = TokenItemBuyStatus.Used
        end
    end

    self:addItemsGridView(false)

    if self.selectTab == TokenTabKind.Skin then
        --self:addItemsGridView(false)
        self:onClickSkinItem(self.selectItemId)
    end
end

return UITokenShop