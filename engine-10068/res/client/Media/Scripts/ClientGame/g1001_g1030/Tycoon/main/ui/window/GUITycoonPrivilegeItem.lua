GUITycoonPrivilegeItem = class("GUITycoonPrivilegeItem", IGUIWindow)

local heroIcon = {
    ["8"] = "set:tycoon_privilege.json image:wolverine",
    ["9"] = "set:tycoon_privilege.json image:thor",
    ["10"] = "set:tycoon_privilege.json image:loki",
    ["11"] = "set:tycoon_privilege.json image:thanos",
    ["13"] = "set:tycoon_privilege.json image:captainmarvel",
    ["15"] = "set:tycoon_privilege.json image:hawkeye",
}

function GUITycoonPrivilegeItem:ctor()
    self.super.ctor(self, "TycoonPrivilegeItem.json")
end

function GUITycoonPrivilegeItem:onLoad()
    self.siIcon = self:getChildWindowByName("TycoonPrivilegeItem-Icon", GUIType.StaticImage)
    self.stTime = self:getChildWindowByName("TycoonPrivilegeItem-Time", GUIType.StaticText)

end

function GUITycoonPrivilegeItem:setData(occIds, time)
    self.siIcon:SetImage(heroIcon[tostring(occIds)])
    self.Tick = time
end

function GUITycoonPrivilegeItem:onTick()
    self.Tick = self.Tick - 1
    if self.Tick >= 0 then
        self:setTime(math.floor(self.Tick / 60))
    end
end

function GUITycoonPrivilegeItem:setTime(time)
    local minute = math.fmod(time, 60)
    if minute < 10 then
        minute = "0" .. minute
    end
    if minute > 1000 then
        minute = "999"
    end
    local hours = math.floor(time / 60)
    if hours < 10 then
        hours = "0" .. hours
    end

    local showTime = hours .. ":" .. minute
    self.stTime:SetText(showTime)
end