GUISettingOperation = class("GUISettingOperation", IGUIWindow)

function GUISettingOperation:ctor()
    self.super.ctor(self, "BedWarSettingOperation.json")
end

function GUISettingOperation:onLoad()

    self.poleMove = GUIManager:getWindowByName("Main-PoleControl-Move")
    self.poleControl = GUIManager:getWindowByName("Main-PoleControl")
    self.poleBG = GUIManager:getWindowByName("Main-PoleControl-BG", GUIType.StaticImage)
    self.poleWidth = self.poleControl:GetPixelSize().x
    self.poleHeight = self.poleControl:GetPixelSize().y

    self:getChildWindowByName("BedWarSettingOperation-Detail", GUIType.StaticText):SetText(Lang:getString("gui.setting.pole.toggle"))
    self:getChildWindowByName("BedWarSettingOperation-Button-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.pole.button"))
    self:getChildWindowByName("BedWarSettingOperation-Flexed-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.pole.flexed"))
    self:getChildWindowByName("BedWarSettingOperation-Flexible-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.pole.flexible"))

    self.opGroup = RadioBoxGroup.new(SoundConfig.switchSetting)
    self:register()

    LuaTimer:schedule(function ()
        local ivButton = self:getChildWindowByName("BedWarSettingOperation-Button-Img")
        local width = math.min(ivButton:GetPixelSize().x, 156)
        ivButton:SetWidth({ 0, width })
        ivButton:SetHeight({ 0, width })
        local ivFlexible = self:getChildWindowByName("BedWarSettingOperation-Flexible-Img")
        width = math.min(ivFlexible:GetPixelSize().x, 166)
        ivFlexible:SetWidth({ 0, width })
        ivFlexible:SetHeight({ 0, width })
        local ivFlexed = self:getChildWindowByName("BedWarSettingOperation-Flexed-Img")
        width = math.min(ivFlexed:GetPixelSize().x, 168)
        ivFlexed:SetWidth({ 0, width })
        ivFlexed:SetHeight({ 0, width })
    end, 100)
end

function GUISettingOperation:register()
    self.opGroup:addCheckBox(self:getChildWindowByName("BedWarSettingOperation-Button-Box", GUIType.CheckBox), function(checked)
        local image = self:getChildWindowByName("BedWarSettingOperation-Button-Bg", GUIType.StaticImage)
        local check = self:getChildWindowByName("BedWarSettingOperation-Button-Check", GUIType.StaticImage)
        if checked then
            Blockman.Instance().m_gameSettings.usePole = 0
            image:SetImage("set:bedwar_setting.json image:operation_select")
            check:SetImage("set:bedwar_setting.json image:checkbox_select")
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultOperation = self.opGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            image:SetImage("set:bedwar_setting.json image:operation_normal")
            check:SetImage("set:bedwar_setting.json image:checkbox_bg")
        end
    end)
    self.opGroup:addCheckBox(self:getChildWindowByName("BedWarSettingOperation-Flexed-Box", GUIType.CheckBox), function(checked)
        local image = self:getChildWindowByName("BedWarSettingOperation-Flexed-Bg", GUIType.StaticImage)
        local check = self:getChildWindowByName("BedWarSettingOperation-Flexed-Check", GUIType.StaticImage)
        if checked then
            self.poleMove:SetArea({ 0, 50 }, { 0, -70 }, { 0, self.poleWidth }, { 0, self.poleHeight })
            self.poleBG:SetImage("set:pole.json image:pole_bg")
            Blockman.Instance().m_gameSettings.usePole = 1
            image:SetImage("set:bedwar_setting.json image:operation_select")
            check:SetImage("set:bedwar_setting.json image:checkbox_select")
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultOperation = self.opGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            image:SetImage("set:bedwar_setting.json image:operation_normal")
            check:SetImage("set:bedwar_setting.json image:checkbox_bg")
        end
    end)

    self.opGroup:addCheckBox(self:getChildWindowByName("BedWarSettingOperation-Flexible-Box", GUIType.CheckBox), function(checked)
        local image = self:getChildWindowByName("BedWarSettingOperation-Flexible-Bg", GUIType.StaticImage)
        local check = self:getChildWindowByName("BedWarSettingOperation-Flexible-Check", GUIType.StaticImage)
        if checked then
            self.poleMove:SetArea({ 0, 0 }, { 0, 0 }, { 0, self.poleWidth * 1.8 }, { 0, self.poleHeight * 2.0 })
            self.poleBG:SetImage("set:pole.json image:pole_bg_2")
            Blockman.Instance().m_gameSettings.usePole = 2
            image:SetImage("set:bedwar_setting.json image:operation_select")
            check:SetImage("set:bedwar_setting.json image:checkbox_select")
            LuaTimer:schedule(function()
                SystemSettingRecord.defaultOperation = self.opGroup:getCurIndex()
                onSaveSetting()
            end, 0)
        else
            image:SetImage("set:bedwar_setting.json image:operation_normal")
            check:SetImage("set:bedwar_setting.json image:checkbox_bg")
        end
    end)
end