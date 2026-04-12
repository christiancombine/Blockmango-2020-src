GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameReadyEvent, self.onGameReady)
end

function GameListener.onGameReady()
    require "ui.GUIFactory"
end