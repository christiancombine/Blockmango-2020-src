local UIToolBar = class("UIToolBar", IGUILayout)

function UIToolBar:onLoad()
    self.stTimeNum = self:getChildWindow("TowerDefenseToolBar-time_text", GUIType.StaticText)
    self.stRoundNum = self:getChildWindow("TowerDefenseToolBar-round_text", GUIType.StaticText)
    self.stGoldNum = self:getChildWindow("TowerDefenseToolBar-hallgold_text", GUIType.StaticText)
    self.stGoldNum:SetText(0)

    self:initRegister()
end

function UIToolBar:initRegister()
    self:registerDataCallBack("RoundInfo", function(data)
        self:onUpdateRound(data)
    end, DataBuilderProcessor)

    self:registerEvent(UpdateMoneyEvent, function()
        self.stGoldNum:SetText(GamePlayer:Instance().Wallet:getMoneyByCoinId(Define.Money.HallGold))
    end)
end

function UIToolBar:isNeedLoad()
    return not Game:isEnableIndie()
end

function UIToolBar:onUpdateRound(data)
    local round = data:getNumberParam("round")
    local allRound = data:getNumberParam("allRound")
    local prepareTime = data:getNumberParam("prepareTime")
    needSyn = data:getBoolParam("needSyn")
    self.stRoundNum:SetText(tostring(round) .. "/" .. tostring(allRound))
    MapManager:Instance():clearMonster()
    LuaTimer:scheduleTimer(function()
        local stTime = GUIManager:getWindowByName("TowerDefenseToolBar-time_text", GUIType.StaticText)
        if prepareTime > 1 then
            prepareTime = prepareTime - 1
            stTime:SetText(prepareTime)
        elseif prepareTime == 1 then
            stTime:SetText("--")
        end
    end, 1000, prepareTime / 1)
end

return UIToolBar