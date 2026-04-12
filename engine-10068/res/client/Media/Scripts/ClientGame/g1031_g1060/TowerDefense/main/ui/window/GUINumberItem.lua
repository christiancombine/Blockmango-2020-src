GUINumberItem = class("GUINumberItem", IGUIWindow)

--映射数字图集名
local numberPng = {
    [0] = "set:tower_defense_number.json image:0",
    [1] = "set:tower_defense_number.json image:1",
    [2] = "set:tower_defense_number.json image:2",
    [3] = "set:tower_defense_number.json image:3",
    [4] = "set:tower_defense_number.json image:4",
    [5] = "set:tower_defense_number.json image:5",
    [6] = "set:tower_defense_number.json image:6",
    [7] = "set:tower_defense_number.json image:7",
    [8] = "set:tower_defense_number.json image:8",
    [9] = "set:tower_defense_number.json image:9",
}

local numberPng_Blue = {
    [0] = "set:float_number.json image:number_3_0",
    [1] = "set:float_number.json image:number_3_1",
    [2] = "set:float_number.json image:number_3_2",
    [3] = "set:float_number.json image:number_3_3",
    [4] = "set:float_number.json image:number_3_4",
    [5] = "set:float_number.json image:number_3_5",
    [6] = "set:float_number.json image:number_3_6",
    [7] = "set:float_number.json image:number_3_7",
    [8] = "set:float_number.json image:number_3_8",
    [9] = "set:float_number.json image:number_3_9",
}

function GUINumberItem:ctor(number, useBlue)
    self.number = number
    self.useBlue = useBlue
    self.super.ctor(self, "NumberItem.json")
end

function GUINumberItem:onLoad()
    self.ivNumber = DynamicCast.dynamicCastWindow(GUIType.StaticImage, self.root)
    if self.useBlue then
        self.ivNumber:SetImage(numberPng_Blue[tonumber(self.number)])
    else
        self.ivNumber:SetImage(numberPng[tonumber(self.number)])
    end

end

function GUINumberItem.setNumberToLayout(number, parent, useBlue)
    number = math.floor(number)
    parent:CleanupChildren()
    local numberHeight = parent:GetPixelSize().y
    local numberWidth = numberHeight / 139 * 105
    local numberList = {}
    while number ~= 0 do
        table.insert(numberList, number % 10)
        number = math.floor(number / 10)
    end
    local PosX = (numberWidth * (#numberList - 1)) / -2
    for i = #numberList, 1, -1 do
        -- print(numberList[i])
        local item = GUINumberItem.new(numberList[i],useBlue)
        item:setArea({ 0, PosX }, { 0, 0 }, { 0, numberWidth }, { 0, numberHeight })
        item.root:SetHorizontalAlignment(HorizontalAlignment.Center)
        parent:AddChildWindow(item.root)
        PosX = PosX + numberWidth
    end
end