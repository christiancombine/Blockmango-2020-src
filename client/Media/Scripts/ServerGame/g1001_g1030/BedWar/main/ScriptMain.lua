package.path = package.path .. ';..\\?.lua';

BaseListener.registerCallBack(GameInitEvent, function(config)
    if GameType == "g1046" then
        require "hall.BedWarServer"
    else
        require "game.BedWarServer"
    end
    local keys = DbUtil:getAllKey()
    for _, key in pairs(keys) do
        DBManager:addMustLoadSubKey(key)
    end
    BedWarServer.onGameInit(config)
end)