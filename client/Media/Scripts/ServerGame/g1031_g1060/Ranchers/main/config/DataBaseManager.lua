
DataBaseManager = {}
DataBaseManager.TAG_PLAYER = 1
DataBaseManager.TAG_MANOR = 2
DataBaseManager.TAG_AREA = 3
DataBaseManager.TAG_HOUSE = 4
DataBaseManager.TAG_FIELD = 5
DataBaseManager.TAG_WAREHOUSE = 6
DataBaseManager.TAG_BUILDING = 7
DataBaseManager.TAG_ACHIEVEMENT = 8
DataBaseManager.TAG_TASK = 9
DataBaseManager.TAG_RANCHER_EXPLORE_INFO = 10 -- for rancher explore
DataBaseManager.TAG_ACCELERATE_ITEMS = 11
DataBaseManager.TAG_ROADS = 12

DBManager:addMustLoadSubKey(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)

function DataBaseManager:getDataFromDB(userId, tag)
    DBManager:getPlayerData(userId, tag)
end

function DataBaseManager:saveHoseData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_HOUSE, data, immediate or false)
end

function DataBaseManager:saveAreaData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_AREA, data, immediate or false)
end

function DataBaseManager:saveFieldData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_FIELD, data, immediate or false)
end

function DataBaseManager:saveManorData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_MANOR, data, immediate or false)
end

function DataBaseManager:savePlayerData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_PLAYER, data, immediate or false)
end

function DataBaseManager:saveWareHouseData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_WAREHOUSE, data, immediate or false)
end

function DataBaseManager:saveBuildingData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_BUILDING, data, immediate or false)
end

function DataBaseManager:saveAchievementData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_ACHIEVEMENT, data, immediate or false)
end

function DataBaseManager:saveTaskData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_TASK, data, immediate or false)
end

function DataBaseManager:saveRanchExploreData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_RANCHER_EXPLORE_INFO, data, immediate or false)
end

function DataBaseManager:saveAccelerateItemsData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_ACCELERATE_ITEMS, data, immediate or false)
end

function DataBaseManager:saveRoadsData(userId, data, immediate)
    DBManager:savePlayerData(userId, self.TAG_ROADS, data, immediate or false)
end

function DataBaseManager:isGetAllDataFinished(player)
    for key = DataBaseManager.TAG_PLAYER, DataBaseManager.TAG_ROADS do
        if key ~= DataBaseManager.TAG_AREA then
            if not DBManager:isLoadDataFinished(player.userId, key) then
                return false
            end
        end
    end
    return true
end

return DataBaseManager