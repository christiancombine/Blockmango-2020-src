---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50

require "Match"

DBDataListener = {}

DBDataListener.TAG_RANCHER_EXPLORE_INFO = 10

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
