UILotteryItem = class("BedWarLotteryItem", IGUIWindow)



function UILotteryItem:onLoad()

    -- self.mWindow = DynamicCast.dynamicCastWindow(GUIType.Layout, self.root)
    self.ivEnchantBackGround = self:getChildWindow(0, GUIType.StaticImage,self:getChildWindow(0, GUIType.StaticImage))
    self.ivItem = self:getChildWindow(1, GUIType.StaticImage,self:getChildWindow(0, GUIType.StaticImage))
    self.tvItemName = self:getChildWindow(1, GUIType.StaticText)
    self.tvItemDesc = self:getChildWindow(2, GUIType.StaticText)

    self.root:SetEnabled(true)
    self.ivItem:SetVisible(true)
end

function UILotteryItem:init(i)
    self.index = i
    self.ItemInfo = {}
    self.ItemInfo.itemId = 0
end

function UILotteryItem:setGet(rewardId)
    if self.ItemInfo.itemId == 0 or self.ItemInfo.itemId  ~= rewardId then
        return
    else
        self.root:SetEnabled(false)
    end
end

function UILotteryItem:updateItem(id)
    self.ItemInfo.itemId = id
    self.root:SetEnabled(true)
    local reward = LotteryConfig.findRewardById(id)
    if #reward then

        self.ivItem:SetImage(tostring(reward.Image))
        self.ivEnchantBackGround:SetVisible(false)
        if reward.IsEnchant == 1 then
            self.ivEnchantBackGround:SetVisible(true)
        end
        self.tvItemName:SetText(Lang:getString(tostring(reward.Name)))
        self.tvItemDesc:SetText(Lang:getString(tostring(reward.Desc1)))

    end
end


return UILotteryItem