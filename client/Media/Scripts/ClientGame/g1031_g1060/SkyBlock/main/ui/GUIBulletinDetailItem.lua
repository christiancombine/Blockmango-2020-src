UIBulletinDetailItem = class("UIBulletinDetailItem", IGUIWindow)

function UIBulletinDetailItem:onLoad()
    print("UIBulletinDetailItem:onLoad()")
    self.titleImg = self:getChildWindow(1 , GUIType.StaticImage)
    self.stDetail = self:getChildWindow(2, GUIType.StaticText)
    self.stTitle = self:getChildWindow(3):GetChildByIndex(1)

    self.data = {}
end

function UIBulletinDetailItem:setData(data)
    self.data = data
    self.titleImg:SetImage(self.data.Img)
    self.stTitle:SetText(Lang:getString(self.data.Title))
    self.stDetail:SetText(Lang:getString(self.data.Detail))
end

function UIBulletinDetailItem:getNeedSetHigt()
    local wide_2 = self.stDetail:GetPixelSize().y

    local pos = self.stDetail:GetYPosition()
    local ItemHigt = self.root:GetPixelSize().y

    local cur_pos = pos[1] * ItemHigt + pos[2]

    local needHigt = cur_pos + wide_2 + 5

    if needHigt > ItemHigt then
        return needHigt
    end
    return ItemHigt
end