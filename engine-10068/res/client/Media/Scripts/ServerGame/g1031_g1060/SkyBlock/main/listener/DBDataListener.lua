---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15

DBDataListener = {}

function DBDataListener:init()
    GetDataFromDBEvent:registerCallBack(self.onGetDataFromDB)
    GetDBDataErrorEvent:registerCallBack(self.onGetDBDataError)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil.savePlayerData(player, immediate)
    end)
end

function DBDataListener.onGetDataFromDB(userId, subKey, data)
	DbUtil.initDataFromDB(userId, subKey, data)
end

function DBDataListener.onGetDBDataError(userId, tag)
    GameMatch:initErrorTarget(userId)
    GameMatch:releaseManor(userId)
end

return DBDataListener

--endregion
