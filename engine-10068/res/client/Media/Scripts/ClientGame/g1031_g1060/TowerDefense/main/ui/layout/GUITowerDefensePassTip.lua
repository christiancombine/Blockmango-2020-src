local UITowerDefensePassTip = class("UITowerDefensePassTip", IGUILayout)

function UITowerDefensePassTip:onLoad()
    self.TowerDefensePassTip = self:getChildWindow("TowerDefensePassTip", GUIType.Layout)
    self.stOldRound = self:getChildWindow("TowerDefensePassTip-oldround_num", GUIType.StaticText)
    self.stNewRound = self:getChildWindow("TowerDefensePassTip-newround_num", GUIType.StaticText)
    self.stGoldReward = self:getChildWindow("TowerDefensePassTip-reward_text", GUIType.StaticText)

    self.TowerDefensePassTip:SetYPosition({ -0.1, 0 })

    self:updateLanguage()

    self:registerDataCallBack("RoundPassTip", function(data)
        self:showTip(data)
    end, DataBuilderProcessor)
end

function UITowerDefensePassTip:updateLanguage()
    self:getChildWindow("TowerDefensePassTip-oldround_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefensePassTip-OldRound"))
    self:getChildWindow("TowerDefensePassTip-newround_text", GUIType.StaticText):SetText(Lang:getString("gui_TowerDefensePassTip-NewRound"))
end

function UITowerDefensePassTip:showTip(data)
    local oldRound = data:getNumberParam("oldRound")
    local newRound = data:getNumberParam("newRound")
    local reward = data:getNumberParam("reward")
    local isWarning = data:getBoolParam("isWarning")
    self.stOldRound:SetText(oldRound)
    self.stNewRound:SetText(newRound)
    self.stGoldReward:SetText(reward)

    GUIPassTip.TowerDefensePassTip:SetYPosition({ -0.1, 0 })

    self:show()

    LuaTimer:scheduleTimer(function()
        local yPos = GUIPassTip.TowerDefensePassTip:GetYPosition()
        yPos[1] = yPos[1] + 0.01
        GUIPassTip.TowerDefensePassTip:SetYPosition(yPos)
    end, 25, 20)

    LuaTimer:scheduleTimer(function(showWarning)
        --GUIPassTip:hide()
        LuaTimer:scheduleTimer(function()
            local yPos = GUIPassTip.TowerDefensePassTip:GetYPosition()
            yPos[1] = yPos[1] - 0.025
            GUIPassTip.TowerDefensePassTip:SetYPosition(yPos)
        end, 10, 20)
        if showWarning then
            LuaTimer:schedule(function()
                GUIWarning:showWarning()
            end, 300)
        end
    end, 5000, 1, isWarning)

end

return UITowerDefensePassTip

