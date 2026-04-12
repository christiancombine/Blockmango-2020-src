DbUtil = {}
DbUtil.TAG_PLAYER = 1
DbUtil.TAG_BIRD = 2
DbUtil.TAG_BAG = 3
DbUtil.TAG_TASK = 4
DbUtil.TAG_CHEST = 5
DbUtil.TAG_MONSTER = 6

DBManager:addMustLoadSubKey(DbUtil.TAG_PLAYER)
DBManager:addMustLoadSubKey(DbUtil.TAG_BIRD)
DBManager:addMustLoadSubKey(DbUtil.TAG_BAG)
DBManager:addMustLoadSubKey(DbUtil.TAG_TASK)
DBManager:addMustLoadSubKey(DbUtil.TAG_CHEST)
DBManager:addMustLoadSubKey(DbUtil.TAG_MONSTER)

function DbUtil:getPlayerData(player)
    DBManager:getPlayerData(player.userId, DbUtil.TAG_PLAYER)
end

function DbUtil:onPlayerGetDataFinish(player, data, subKey)
    player:initDataFromDB(data, subKey)
end

function DbUtil:savePlayerAllData(player, immediate)
    if player == nil then
        return
    end

    DbUtil:savePlayerData(player, immediate)
    DbUtil:saveBirdData(player, immediate)
    DbUtil:saveBirdBagData(player, immediate)
    DbUtil:saveMonsterData(player, immediate)
    DbUtil:saveTaskData(player)
end

function DbUtil:savePlayerData(player, immediate)
    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_PLAYER) then
        local playerData = player:prepareDataSaveToDB()
        DBManager:savePlayerData(player.userId, DbUtil.TAG_PLAYER, playerData, immediate)
    end
end

function DbUtil:saveBirdData(player, immediate)
    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_BIRD) then
        if player.userBird ~= nil then
            local birdData = player.userBird:prepareDataSaveToDB()
            DBManager:savePlayerData(player.userId, DbUtil.TAG_BIRD, birdData, immediate)
        end
    end
end

function DbUtil:saveBirdBagData(player, immediate)
    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_BAG) then
        if player.userBirdBag ~= nil then
            local bagData = player.userBirdBag:prepareDataSaveToDB()
            DBManager:savePlayerData(player.userId, DbUtil.TAG_BAG, bagData, immediate)
        end
    end
end

function DbUtil:saveMonsterData(player, immediate)
    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_MONSTER) then
        if player.userMonster ~= nil then
            local monsterData = player.userMonster:prepareDataSaveToDB()
            DBManager:savePlayerData(player.userId, DbUtil.TAG_MONSTER, monsterData, immediate)
        end
    end
end

function DbUtil:saveTaskData(player)
    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_TASK) then
        if player.taskControl ~= nil then
            player.taskControl:SaveData()
        end
    end
end

function DbUtil:CanSavePlayerData(player, subKey)
    return DBManager:isLoadDataFinished(player.userId, subKey)
end

return DbUtil