local UIToolBar = class("UIToolBar", IGUILayout)

function UIToolBar:onLoad()
    self.target_exp = 0
    self.stGoldNum = self:getChildWindow("TowerDefenseHallToolBar-hallgold_text", GUIType.StaticText)
    self.stLevel = self:getChildWindow("TowerDefenseHallToolBar-level", GUIType.StaticText)
    self.stExp = self:getChildWindow("TowerDefenseHallToolBar-exp_text", GUIType.StaticText)
    self.proExp = self:getChildWindow("TowerDefenseHallToolBar-progress", GUIType.ProgressBar)
    self.stGoldNum:SetText(0)

    self:registerEvent(UpdateMoneyEvent, function()
        self.stGoldNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
    end)

    self:registerDataCallBack("LevelInfo", function(data)
        self.clientPlayer = PlayerManager:getClientPlayer()
        self.clientPlayer.cur_exp = data:getNumberParam("exp")
        self.clientPlayer.level = data:getNumberParam("level")
        self:setExpUI(self.clientPlayer.cur_exp, self.clientPlayer.level)
        AreaManager:checkPlayerLevel(self.clientPlayer)
    end, DataBuilderProcessor)

    self:registerDataCallBack("AddExp", function(data)
        self.target_exp = self.target_exp + data:getNumberParam("exp")
        self:runExpProgressBar(2500 / self.target_exp)
    end, DataBuilderProcessor)
end

function UIToolBar:runExpProgressBar(time)
    if not self.clientPlayer or self.target_exp <= 0 then
        return
    end
    local max_exp, is_max = ExpConfig:getMaxExp(self.clientPlayer.level)
    if is_max then
        self.target_exp = 0
        GUILevelUpReward:showReward()
        return
    end

    if self.expTimer then
        LuaTimer:cancel(self.expTimer)
        self.expTimer = nil
    end

    self.addNum = self.target_exp
    if self.target_exp >= max_exp then
        self.addNum = max_exp - self.clientPlayer.cur_exp
    end

    self.expTimer = LuaTimer:scheduleTimer(function()
        if not self.clientPlayer or self.target_exp <= 0 then
            return
        end
        self:addExp()
        self.addNum = self.addNum - 1
        if self.addNum <= 0 and self.expTimer then
            self:runExpProgressBar(time)
        end
    end, time)
end

function UIToolBar:addExp()
    local max_exp, is_max = ExpConfig:getMaxExp(self.clientPlayer.level)
    self.clientPlayer.cur_exp = self.clientPlayer.cur_exp + 1
    self.target_exp = self.target_exp - 1

    if self.clientPlayer.cur_exp == max_exp then
        --levelUp
        self.clientPlayer.level = self.clientPlayer.level + 1
        self.clientPlayer.cur_exp = 0
        AreaManager:checkPlayerLevel(self.clientPlayer)
    end

    if self.target_exp <= 0 then
        GUILevelUpReward:showReward()
    end

    self:setExpUI(self.clientPlayer.cur_exp, self.clientPlayer.level)
end

function UIToolBar:setExpUI(cur_exp, cur_level)
    local max_exp, is_max = ExpConfig:getMaxExp(cur_level)
    local proExpText = cur_exp / max_exp
    local stExpText = cur_exp .. "/" .. max_exp
    if is_max then
        max_exp = ExpConfig:getMaxExp(cur_level - 1)
        proExpText = 1
        stExpText = "Max / Max"
    end
    self.stLevel:SetText(cur_level)
    self.proExp:SetProgress(proExpText)
    self.stExp:SetText(stExpText)
end

return UIToolBar