GUINumberItem = class("GUINumberItem", IGUIWindow)

--映射数字图集名
local numberPng = {
    [0] = "set:bed_war_number.json image:0",
    [1] = "set:bed_war_number.json image:1",
    [2] = "set:bed_war_number.json image:2",
    [3] = "set:bed_war_number.json image:3",
    [4] = "set:bed_war_number.json image:4",
    [5] = "set:bed_war_number.json image:5",
    [6] = "set:bed_war_number.json image:6",
    [7] = "set:bed_war_number.json image:7",
    [8] = "set:bed_war_number.json image:8",
    [9] = "set:bed_war_number.json image:9",
}

function GUINumberItem:ctor(number)
    self.number = number
    self.super.ctor(self, "NumberItem.json")
end

function GUINumberItem:onLoad()
    self.ivNumber = DynamicCast.dynamicCastWindow(GUIType.StaticImage, self.root)
    self.ivNumber:setMaterial(20001)
    self.ivNumber:SetImage(numberPng[tonumber(self.number)])
end

function GUINumberItem.setNumberToLayout(number, parent)
    number = math.floor(number)
    local Spacing = 10

    parent:CleanupChildren()
    local numberHeight = parent:GetPixelSize().y
    local numberWidth = numberHeight / 167 * 110
    local numberList = {}
    while number ~= 0 do
        table.insert(numberList, number % 10)
        number = math.floor(number / 10)
    end
    local PosX = ((numberWidth + Spacing) * (#numberList - 1)) / -2
    for i = #numberList, 1, -1 do
        local item = GUINumberItem.new(numberList[i])
        item:setArea({ 0, PosX }, { 0, 0 }, { 0, numberWidth }, { 0, numberHeight })
        -- item.root:SetBackgroundColor({ 1, 217, 217, 217 })
        item.root:SetHorizontalAlignment(HorizontalAlignment.Center)
        parent:AddChildWindow(item.root)
        PosX = PosX + numberWidth + Spacing
    end
end