require "ui.window.GUILevelUpRewardItem"
require "ui.window.GUINumberItem"

local UILevelUpReward = class("UIShop", IGUILayout)

function UILevelUpReward:onLoad()
    self.rewards = {}
    self.beforeLv = 0
    self.nowLv = 0

    self.llReward = self:getChildWindow("LevelUpReward-Reward-Layout", GUIType.Layout)
    self.llNumbers = self:getChildWindow("LevelUpReward-Number-List", GUIType.Layout)

    self.tvLvBefore = self:getChildWindow("LevelUpReward-Lv-Before", GUIType.StaticText)
    self.tvLevelNow = self:getChildWindow("LevelUpReward-Lv-Now", GUIType.StaticText)
    self.tvTitle = self:getChildWindow("LevelUpReward-Title-Text", GUIType.StaticText)
    self.tvTitle:SetText(Lang:getString("level.up.title"))

    self.btnContinue = self:getChildWindow("LevelUpReward-Reward-btn", GUIType.Button)
    self.btnContinue:registerEvent(GUIEvent.ButtonClick, function()
        self:close()
    end)
    self.btnContinue:SetText(Lang:getString("gui_TowerDefenseDress-continue"))

    self:registerDataCallBack("UpgradeInfo", function(data)
        self.rewards = json.decode(data:getParam("rewards"))
        if self.beforeLv == 0 then
            self.beforeLv = data:getNumberParam("beforeLv")
        end
        self.nowLv = data:getNumberParam("nowLv")

    end, DataBuilderProcessor)
end

function UILevelUpReward:showReward()
    if self.beforeLv == 0 and self.nowLv == 0 then
        return
    end

    self.llReward:CleanupChildren()
    local listSize = TableUtil.getTableSize(self.rewards)
    local size = self.llReward:GetPixelSize().x
    local Space = 28
    local Width = (size - Space * (5 - 1)) / 5
    local PosX = ((Width + Space) * (listSize - 1)) / -2

    for type, count in pairs(self.rewards) do
        local shopItem = UILevelUpRewardItem.new()
        shopItem:setArea({ 0, PosX }, { 0, 0 }, { 0, Width }, { 0, 91 })
        shopItem:setReward(tonumber(type), tonumber(count))
        shopItem.root:SetHorizontalAlignment(HorizontalAlignment.Center)
        self.llReward:AddChildWindow(shopItem.root)
        PosX = PosX + (Width + Space)
    end
    self.tvLvBefore:SetText("Lv. " .. self.beforeLv)
    self.tvLevelNow:SetText("Lv. " .. self.nowLv)
    GUINumberItem.setNumberToLayout(self.nowLv, self.llNumbers)
    self:show()

    self.beforeLv = 0
    self.nowLv = 0
end

function UILevelUpReward:close()
    self:hide()
end

return UILevelUpReward