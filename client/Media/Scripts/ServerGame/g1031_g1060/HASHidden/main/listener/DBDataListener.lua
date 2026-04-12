---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15

require "Match"

DBDataListener = {}

function DBDataListener:init()
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil:savePlayerData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(player, subkey, data)
    player:initDataFromDB(data, subkey)
end

return DBDataListener

--endregion
