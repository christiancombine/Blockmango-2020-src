---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15

DBDataListener = {}

function DBDataListener:init()
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        DbUtil:savePlayerData(player, immediate)
    end)
end

return DBDataListener

--endregion
