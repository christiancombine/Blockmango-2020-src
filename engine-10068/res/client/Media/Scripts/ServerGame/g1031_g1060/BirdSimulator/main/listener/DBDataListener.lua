require "Match"

DBDataListener = {}

function DBDataListener:init()
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    GetDBDataErrorEvent:registerCallBack(self.onGetDBDataError)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil:savePlayerAllData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(player, tag, data)
    DbUtil:onPlayerGetDataFinish(player, data, tag)
end

function DBDataListener.onGetDBDataError(userId, tag)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player ~= nil then
        HostApi.sendReleaseManor(player.userId)
    end
end

return DBDataListener

--endregion
