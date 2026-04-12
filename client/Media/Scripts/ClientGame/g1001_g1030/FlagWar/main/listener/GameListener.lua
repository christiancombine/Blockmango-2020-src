GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameReadyEvent, self.onGameReady)
end

function GameListener.onGameReady()
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    print("--------------------------------------")
    TextureAtlasRegister.Instance():addTextureAtlas("throwable.json", "")
end

return GameListener