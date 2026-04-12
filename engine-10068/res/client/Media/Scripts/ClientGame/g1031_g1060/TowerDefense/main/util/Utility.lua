Utility = {}

function Utility.refreshPoleControlState(state)
    local gui_menu = DynamicCastHelper.dynamicCastGuiMenu(RootGuiLayout.Instance():getLayout(7))
    if not gui_menu then
        return
    end

    local gui_gameSetting = DynamicCastHelper.dynamicCastGuiGameSetting(gui_menu:getRadioPanel(3))
    if not gui_gameSetting then
        return
    end

    gui_gameSetting:refreshPoleControlState(state)

    LogUtil.log(string.format("Utility.refreshPoleControlState %d", state))
end

return Utility