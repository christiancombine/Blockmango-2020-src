local GUI_MIN_SIZE = 0.5
local GUI_MAX_SIZE = 1.0

local Color = {
    Select = { 106 / 255, 60 / 255, 29 / 255, 1 },
    Normal = { 106 / 255, 60 / 255, 29 / 255, 1 },
}

GUISettingBasic = class("GUISettingBasic", IGUIWindow)

function GUISettingBasic:ctor()
    self.super.ctor(self, "BedWarSettingBasic.json")
end

function GUISettingBasic:onLoad()

    self.btnLogout = self:getChildWindowByName("BedWarSettingBasic-Logout", GUIType.Button)

    self.sensitive = self:getChildWindowByName("BedWarSettingBasic-Sensitivity-Slider", GUIType.Slider)
    self.luminance = self:getChildWindowByName("BedWarSettingBasic-Luminance-Slider", GUIType.Slider)
    self.horizon = self:getChildWindowByName("BedWarSettingBasic-Horizon-Slider", GUIType.Slider)
    self.size = self:getChildWindowByName("BedWarSettingBasic-Size-Slider", GUIType.Slider)

    self:getChildWindowByName("BedWarSettingBasic-Title", GUIType.StaticText):SetText(Lang:getString("gui.setting.basic"))
    self:getChildWindowByName("BedWarSettingBasic-Fps-Title", GUIType.StaticText):SetText(Lang:getString("gui.setting.fps"))
    self:getChildWindowByName("BedWarSettingBasic-Fps30-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.fps.30"))
    self:getChildWindowByName("BedWarSettingBasic-Fps45-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.fps.45"))
    self:getChildWindowByName("BedWarSettingBasic-Fps60-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.fps.60"))
    self:getChildWindowByName("BedWarSettingBasic-Camera-Title", GUIType.StaticText):SetText(Lang:getString("gui.setting.camera"))
    self:getChildWindowByName("BedWarSettingBasic-Fpp-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.camera.fpp"))
    self:getChildWindowByName("BedWarSettingBasic-Third-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.camera.third"))
    self:getChildWindowByName("BedWarSettingBasic-Second-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.camera.second"))
    self:getChildWindowByName("BedWarSettingBasic-Luminance", GUIType.StaticText):SetText(Lang:getString("gui.setting.luminance"))
    self:getChildWindowByName("BedWarSettingBasic-Size", GUIType.StaticText):SetText(Lang:getString("gui.setting.gui.size"))
    self:getChildWindowByName("BedWarSettingBasic-Sensitivity", GUIType.StaticText):SetText(Lang:getString("gui.setting.camera.sensitive"))
    self:getChildWindowByName("BedWarSettingBasic-Horizon", GUIType.StaticText):SetText(Lang:getString("gui.setting.horizon"))

    self.fpsGroup = RadioBoxGroup.new(SoundConfig.switchSetting)
    self.cameraGroup = RadioBoxGroup.new(SoundConfig.switchSetting)

    self:register()
end

function GUISettingBasic:register()
    ---Fps register
    self.fpsGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Fps30-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Fps30-Text", GUIType.StaticText)
        if checked then
            CGame.Instance():SetMaxFps(30)
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultFPS = self.fpsGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)
    self.fpsGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Fps45-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Fps45-Text", GUIType.StaticText)
        if checked then
            CGame.Instance():SetMaxFps(45)
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultFPS = self.fpsGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)
    self.fpsGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Fps60-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Fps60-Text", GUIType.StaticText)
        if checked then
            CGame.Instance():SetMaxFps(60)
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultFPS = self.fpsGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)

    ---Camera register
    --SPVT_FIRST = 0, // first person view
    --SPVT_THIRD = 1, // third person view
    --SPVT_SECOND = 2, // second person view
    self.cameraGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Fpp-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Fpp-Text", GUIType.StaticText)
        if checked then
            Blockman.Instance():setPersonView(0)
            GUIBedWarToolBar:cameraChange()
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultCamera = self.cameraGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)
    self.cameraGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Third-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Third-Text", GUIType.StaticText)
        if checked then
            Blockman.Instance():setPersonView(1)
            GUIBedWarToolBar:cameraChange()
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultCamera = self.cameraGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)
    self.cameraGroup:addCheckBox(self:getChildWindowByName("BedWarSettingBasic-Second-Box", GUIType.CheckBox), function(checked)
        local text = self:getChildWindowByName("BedWarSettingBasic-Second-Text", GUIType.StaticText)
        if checked then
            Blockman.Instance():setPersonView(2)
            GUIBedWarToolBar:cameraChange()
            text:SetTextColor(Color.Select)
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultCamera = self.cameraGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            text:SetTextColor(Color.Normal)
        end
    end)

    self.sensitive:registerEvent(GUIEvent.TouchDown, function()
        self:refreshSensitive()
    end)

    self.sensitive:registerEvent(GUIEvent.TouchMove, function()
        self:refreshSensitive()
    end)

    self.luminance:registerEvent(GUIEvent.TouchDown, function()
        self:refreshLuminance()
    end)

    self.luminance:registerEvent(GUIEvent.TouchMove, function()
        self:refreshLuminance()
    end)

    self.horizon:registerEvent(GUIEvent.TouchDown, function()
        self:refreshHorizon()
    end)

    self.horizon:registerEvent(GUIEvent.TouchMove, function()
        self:refreshHorizon()
    end)

    self.size:registerEvent(GUIEvent.TouchDown, function()
        self:refreshSize()
    end)

    self.size:registerEvent(GUIEvent.TouchMove, function()
        self:refreshSize()
    end)
end

function GUISettingBasic:refreshSensitive(progress)
    if progress then
        self.sensitive:SetProgress(progress)
    end
    local minSize = 1.0
    local maxSize = 9.0
    local size = minSize + (maxSize - minSize) * self.sensitive:GetProgress()
    Blockman.Instance().m_gameSettings:setCameraSensitive(size)
    SystemSettingRecord.defaultSensitivity = self.sensitive:GetProgress()
    onSaveSetting()
end

function GUISettingBasic:refreshLuminance(progress)
    if progress then
        self.luminance:SetProgress(progress)
    end
    Blockman.Instance().m_gameSettings.gammaSetting = self.luminance:GetProgress()
    SystemSettingRecord.defaultLuminance = self.luminance:GetProgress()
    onSaveSetting()
end

function GUISettingBasic:refreshHorizon(progress)
    if progress then
        self.horizon:SetProgress(progress)
    end
    Blockman.Instance().m_gameSettings:setFovSetting(self.horizon:GetProgress())
    SystemSettingRecord.defaultHorizon = self.horizon:GetProgress()
    onSaveSetting()
end

function GUISettingBasic:refreshSize(progress)
    if progress then
        self.size:SetProgress(progress)
    end
    local size = GUI_MIN_SIZE + (GUI_MAX_SIZE - GUI_MIN_SIZE) * self.size:GetProgress()
    Blockman.Instance().m_gameSettings.playerActivityGuiSize = size
    SystemSettingRecord.defaultButtonSize = self.size:GetProgress()
    onSaveSetting()
end

function GUISettingBasic:getCurCamera()

end