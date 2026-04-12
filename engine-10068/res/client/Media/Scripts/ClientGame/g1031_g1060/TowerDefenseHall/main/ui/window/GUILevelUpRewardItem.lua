UILevelUpRewardItem = class("UILevelUpRewardItem", IGUIWindow)

function UILevelUpRewardItem:ctor()
    self.super.ctor(self, "LevelUpRewardItem.json")
end

function UILevelUpRewardItem:onLoad()
    self.llContent = DynamicCast.dynamicCastWindow(GUIType.Layout, self.root)

    self.ivIcon = self:getChildWindow(0, GUIType.StaticImage, self.llContent)
    self.tvNum = self:getChildWindow(0, GUIType.StaticText, self.ivIcon)
end

function UILevelUpRewardItem:setReward(type, count)
    local image = self:getImageByType(type)
    if image then
        self.ivIcon:SetImage(image)
        self.tvNum:SetText(count)
    end
end

function UILevelUpRewardItem:getImageByType(type)
    if tonumber(type) == Define.RewardType.Money then
        return "set:tower_defense_hall_main.json image:hall_money"
    end
end