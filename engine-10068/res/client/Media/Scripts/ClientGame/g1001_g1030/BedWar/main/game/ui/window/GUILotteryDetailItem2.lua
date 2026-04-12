UIDetailItem = class("BedWarLotteryDetailItem", IGUIWindow)

function UIDetailItem:onLoad()

    self.ivSelect = self:getChildWindow(1, GUIType.StaticImage)
    self.ivEnchantBackGround = self:getChildWindow(2, GUIType.StaticImage)
    self.ivImage = self:getChildWindow(3, GUIType.StaticImage)

end

function UIDetailItem:init(i)
    self.index = i
    self.itemId = nil
end

function UIDetailItem:setRewardImage(rewardId)
    local reward = LotteryConfig.findRewardById(rewardId)
    if #reward then
        self.itemId = rewardId
        self.ivImage:SetImage(tostring(reward.Image))
        if reward.IsEnchant == 1 then
            self.ivEnchantBackGround:SetVisible(true)
        end
    end 
end

function UIDetailItem:SelectReward(rewardId)
    if rewardId == self.itemId then
        self.ivSelect:SetVisible(true)
    end
end

function UIDetailItem:onNotSelected()
    self.ivSelect:SetVisible(false)
end

return UIDetailItem