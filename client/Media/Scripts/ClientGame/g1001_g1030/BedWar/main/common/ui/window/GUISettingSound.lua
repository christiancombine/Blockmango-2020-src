GUISettingSound = class("GUISettingSound", IGUIWindow)

function GUISettingSound:ctor()
    self.super.ctor(self, "BedWarSettingSound.json")
end

function GUISettingSound:onLoad()
    self.bgSlider = self:getChildWindowByName("BedWarSettingSound-BackGround-Slider", GUIType.Slider)
    self.effectSlider = self:getChildWindowByName("BedWarSettingSound-Effect-Slider", GUIType.Slider)
    self.bgBox = self:getChildWindowByName("BedWarSettingSound-BackGround-Open", GUIType.CheckBox)
    self.effectBox = self:getChildWindowByName("BedWarSettingSound-Effect-Open", GUIType.CheckBox)

    self:getChildWindowByName("BedWarSettingSound-Title", GUIType.StaticText):SetText(Lang:getString("gui.setting.sound.title"))
    self:getChildWindowByName("BedWarSettingSound-BackGround-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.music.title"))
    self:getChildWindowByName("BedWarSettingSound-BackGround", GUIType.StaticText):SetText(Lang:getString("gui.setting.music.volume"))
    self:getChildWindowByName("BedWarSettingSound-Effect-Text", GUIType.StaticText):SetText(Lang:getString("gui.setting.effect.title"))
    self:getChildWindowByName("BedWarSettingSound-Effect", GUIType.StaticText):SetText(Lang:getString("gui.setting.effect.volume"))

    self.bgGroup = RadioBoxGroup.new(SoundConfig.switchSetting)
    self.soundGroup = RadioBoxGroup.new(SoundConfig.switchSetting)

    self:register()
end

function GUISettingSound:register()
    self.bgGroup:addCheckBox(self:getChildWindowByName("BedWarSettingSound-BackGround-Open", GUIType.CheckBox), function(checked)
        self:openBackGroundMusic(checked)
        SystemSettingRecord.defaultBackGroundSwitch = self.bgGroup:getCurIndex()
        onSaveSetting()
    end)
    self.bgGroup:addCheckBox(self:getChildWindowByName("BedWarSettingSound-BackGround-Close", GUIType.CheckBox), function(checked)
        self:openBackGroundMusic(not checked)
        SystemSettingRecord.defaultBackGroundSwitch = self.bgGroup:getCurIndex()
        onSaveSetting()
    end)

    self.soundGroup:addCheckBox(self:getChildWindowByName("BedWarSettingSound-Effect-Open", GUIType.CheckBox), function(checked)
        self:openEffect(checked)
        SystemSettingRecord.defaultEffectSwitch = self.soundGroup:getCurIndex()
        onSaveSetting()
    end)
    self.soundGroup:addCheckBox(self:getChildWindowByName("BedWarSettingSound-Effect-Close", GUIType.CheckBox), function(checked)
        self:openEffect(not checked)
        SystemSettingRecord.defaultEffectSwitch = self.soundGroup:getCurIndex()
        onSaveSetting()
    end)

    self.bgSlider:registerEvent(GUIEvent.TouchMove, function()
        self:refreshBackGround()

    end)

    self.bgSlider:registerEvent(GUIEvent.TouchDown, function()
        self:refreshBackGround()
    end)

    self.effectSlider:registerEvent(GUIEvent.TouchMove, function()
        self:refreshEffect()
    end)

    self.effectSlider:registerEvent(GUIEvent.TouchDown, function()
        self:refreshEffect()
    end)
end

function GUISettingSound:openBackGroundMusic(open)
    SoundSystem.Instance():setBackgroundMusicSwitch(open)
end

function GUISettingSound:openEffect(open)
    ClientHelper.putBoolPrefs("SoundEffectSwitch", open)
end

function GUISettingSound:refreshEffect(progress)
    if progress then
        self.effectSlider:SetProgress(progress)
    end
    SoundSystem.Instance():setEffectsVolume(self.effectSlider:GetProgress())
    SystemSettingRecord.defaultEffectVolume = self.effectSlider:GetProgress()
    onSaveSetting()
end

function GUISettingSound:refreshBackGround(progress)
    if progress then
        self.bgSlider:SetProgress(progress)
    end
    SoundSystem.Instance():setBackgroundMusicVolume(self.bgSlider:GetProgress())
    SystemSettingRecord.defaultBackGroundVolume = self.bgSlider:GetProgress()
    onSaveSetting()
end