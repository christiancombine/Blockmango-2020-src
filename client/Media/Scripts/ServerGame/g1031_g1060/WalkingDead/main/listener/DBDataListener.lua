DBDataListener = {}

function DBDataListener:init()
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil:savePlayerData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(player, subKey, data)
    DbUtil:onPlayerGetDataFinish(player, data, subKey)
end

return DBDataListener

--endregion
