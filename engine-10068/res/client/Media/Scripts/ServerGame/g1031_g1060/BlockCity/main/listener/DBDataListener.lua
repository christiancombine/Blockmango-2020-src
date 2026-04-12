require "Match"
require "manager.SceneManager"

DBDataListener = {}

function DBDataListener:init()
    GetDataFromDBEvent:registerCallBack(self.onGetDataFromDB)
    GetDBDataErrorEvent:registerCallBack(self.onGetDBDataError)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil:savePlayerData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(userId, tag, data)
    DbUtil:initDataFromDB(userId, data, tag)
end

function DBDataListener.onGetDBDataError(userId, tag)
    -- save error target player id
    SceneManager:initErrorTarget(userId)
    SceneManager:releaseManor(userId)
end

return DBDataListener

--endregion
