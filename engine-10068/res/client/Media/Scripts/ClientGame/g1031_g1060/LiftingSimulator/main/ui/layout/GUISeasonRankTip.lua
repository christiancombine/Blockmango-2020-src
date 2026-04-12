local UISeasonRankTip = class("UISeasonRankTip", IGUILayout)

function UISeasonRankTip:onLoad()
    self.frameTime = 0
    self:initUI()
    self:initEvent()
end

function UISeasonRankTip:initUI()
    self.closeBtn = self:getChildWindow("LiftingSimulatorSeasonRankTip-CloseBtn", GUIType.Button)
    self.btnNext  = self:getChildWindow("LiftingSimulatorSeasonRankTip-BtnNext" ,GUIType.Button)
    self.btnNext:setGray(true)
    self.stDes = self:getChildWindow("LiftingSimulatorSeasonRankTip-Content-Desc", GUIType.StaticText)
    self.siDesBg = self:getChildWindow("LiftingSimulatorSeasonRankTip-Detail-Img", GUIType.StaticImage)
    self.stTitle = self:getChildWindow("LiftingSimulatorSeasonRankTip-Title-Name", GUIType.StaticText)
end

function UISeasonRankTip:initEvent()
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickCloseButton()
    end)

    self:registerEvent(SeasonRankTipOpenEvent, function ()
        self:onOpenSeasonRankTip()
    end)

    self.btnNext:registerEvent(GUIEvent.ButtonClick,function ()
        self:onClickNextButton()
    end)
end

function UISeasonRankTip:onOpenSeasonRankTip()
    self:show()
end

function UISeasonRankTip:onClickCloseButton()
    self:hide()
end

function UISeasonRankTip:onShow()
    self.index = 1
    self:updateUI()
    self:setNextBtnStatus()
end

function UISeasonRankTip:onClickNextButton()
    if self.time > 0 then
        return
    end
    self.index = self.index + 1
    self:updateUI()
    self:setNextBtnStatus()
end

function UISeasonRankTip:onTick(frameTime)
    self.frameTime = self.frameTime + frameTime 
    if self.frameTime >= 1000 then
        self.frameTime = 0
        self:setNextBtnStatus()
    end
end

function UISeasonRankTip:updateUI()
    if self.index >= 3 then
        self.btnNext:SetVisible(false)
    else
        self.btnNext:SetVisible(true)
    end
    self.time = GameConfig.championshipsNextBtnCd
    self.stDes:SetText(Lang:getString("gui_season_rank_info" .. self.index))
    self.siDesBg:SetImage("set:LiftingSeasonRank.json image:tip" .. self.index)
    self.stTitle:SetText(Lang:getString("gui_season_rank_info_title"))
end

function UISeasonRankTip:setNextBtnStatus()
    local tiemStr = Lang:getString("gui_season_rank_next")
    if self.time > 0 then
        tiemStr = tiemStr .. " ("..tostring(self.time)..")"
        self.btnNext:setGray(true)
        self.time = self.time - 1
    else
        self.btnNext:setGray(false)
    end
    self.btnNext:SetText(tiemStr)
end

return UISeasonRankTip
