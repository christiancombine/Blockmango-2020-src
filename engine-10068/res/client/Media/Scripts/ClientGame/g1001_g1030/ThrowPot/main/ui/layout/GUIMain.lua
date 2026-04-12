local UIMain = class("UIMain", IGUILayout)

function UIMain:onLoad()
    self.ivDarkMask = self:getChildWindow("ThrowPotMain-Dark-Mask", GUIType.StaticImage)
    self:registerDataCallBack("ShowDarkMask", function(data)
        local duration = data:getNumberParam("duration")
        self.ivDarkMask:SetVisible(true)
        LuaTimer:cancel(self.darkMaskTaskKey)
        self.darkMaskTaskKey = LuaTimer:schedule(function()
            self.ivDarkMask:SetVisible(false)
        end, duration)
    end, DataBuilderProcessor)
end

return UIMain