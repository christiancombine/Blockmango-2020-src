---
--- Created by Jimmy.
--- DateTime: 2018/4/28 0028 10:10

require "Match"

DBDataListener = {}

function DBDataListener:init()
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        GameMatch:savePlayerData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(player, role, data)
    player:initDataFromDB(data)
end

return DBDataListener

--endregion
