DbUtil = {}
DbUtil.GAME_DATA = 1
DbUtil.HALL_INFO = 2
DbUtil.GAME_REWARD = 3
DbUtil.TOWER_LIST = 4
DbUtil.RECORD_DATA = 5

DBManager:addMustLoadSubKey(DbUtil.GAME_DATA)
DBManager:addMustLoadSubKey(DbUtil.HALL_INFO)
DBManager:addMustLoadSubKey(DbUtil.GAME_REWARD)
DBManager:addMustLoadSubKey(DbUtil.TOWER_LIST)
DBManager:addMustLoadSubKey(DbUtil.RECORD_DATA)
DBManager:setPlayerDBSaveFunction(function(player, immediate)
    DbUtil:savePlayerData(player, immediate)
end)

function DbUtil:getPlayerData(player)
    DBManager:getPlayerData(player.userId, DbUtil.GAME_DATA, DbUtil.initGameData)
    DBManager:getPlayerData(player.userId, DbUtil.HALL_INFO, DbUtil.initHallInfo)
    DBManager:getPlayerData(player.userId, DbUtil.GAME_REWARD, DbUtil.initGameReward)
    DBManager:getPlayerData(player.userId, DbUtil.TOWER_LIST, DbUtil.initTowerList)
    DBManager:getPlayerData(player.userId, DbUtil.RECORD_DATA, DbUtil.initRecordData)
end

function DbUtil.initGameData(userId, _, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        if not data.__first then
            player.level = data.level or 1
            player.cur_exp = data.cur_exp or 0
            player.checkPointRecord = data.checkPointRecord or {}
            player.Wallet:setMoneyCount(Define.Money.HallGold, data.hall_gold or 0)
            player.skinList = data.skinList or {}
            player.equipSkinId = data.equipSkinId or 0
            player.payItemList = {}
            player.shortCutBar = data.shortCutBar or {}
            player.hasReport = data.hasReport or false
            ---纠正旧用户付费快捷栏数据
            for itemType, value in pairs(data.payItemList or {}) do
                player.payItemList[tostring(itemType)] = value
            end
        end
        player:updateShowName()
        GamePacketSender:sendPayItemList(player.rakssid, player.payItemList)
        GamePacketSender:sendShortCutBarSetting(player.rakssid,  player.shortCutBar)
        player:initSkin()
    end
end

function DbUtil.initHallInfo(userId, _, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        if  not data.__first then
            player.hallInfo = data.hallInfo or {}
        end
    end
end

function DbUtil.initGameReward(userId, _, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        if not data.__first then
            player.gameReward = data.gameReward or {}
        end
    end
end

function DbUtil.initTowerList(userId, _, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        if not data.__first then
            for _, tower in pairs(data.towerList or {}) do
                player.towerList[tower.towerId] = tower
            end
        end
        player:addFreeTowerToList()
        player:initStarTowerList(player.towerList)
        GamePacketSender:sendTowerDefensePlayerTowerList(player.rakssid, player.towerList)
    end
end

function DbUtil.initRecordData(userId, _, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        if not data.__first then
            player.killMonsterTotal = data.killMonsterTotal or 0
            player.killMonsterWeek = data.killMonsterWeek or 0
            player.weekKey = data.weekKey or 0
        end
    end
end

function DbUtil:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    DbUtil:savePlayerGameData(player, immediate)
    DbUtil:savePlayerHallInfo(player, immediate)
    DbUtil:savePlayerGameReward(player, immediate)
    DbUtil:savePlayerTowerList(player, immediate)
    DbUtil:savePlayerRecordData(player, immediate)
end

function DbUtil:savePlayerGameData(player, immediate)
    local data = {}
    data.level = player.level or 1
    data.cur_exp = player.cur_exp or 0
    data.checkPointRecord = player.checkPointRecord or {}
    data.hall_gold = player.Wallet:getMoneyByCoinId(Define.Money.HallGold)
    data.skinList = player.skinList or {}
    data.equipSkinId = player.equipSkinId or 0
    data.payItemList = player.payItemList or {}
    data.hasReport = player.hasReport or false
    data.shortCutBar = player.shortCutBar or {}
    data.rewardExp = player.rewardExp or 0
    DBManager:savePlayerData(player.userId, DbUtil.GAME_DATA, data, immediate)
end

function DbUtil:savePlayerHallInfo(player, immediate)
    local data = {}
    data.hallInfo = {}
    DBManager:savePlayerData(player.userId, DbUtil.HALL_INFO, data, immediate)
end

function DbUtil:savePlayerGameReward(player, immediate)
    local data = {}
    data.gameReward = player.gameReward or {}
    DBManager:savePlayerData(player.userId, DbUtil.GAME_REWARD, data, immediate)
end

function DbUtil:savePlayerTowerList(player, immediate)
    local data = {}
    data.towerList = {}
    for _, tower in pairs(player.towerList) do
        table.insert(data.towerList, tower)
    end
    DBManager:savePlayerData(player.userId, DbUtil.TOWER_LIST, data, immediate)
end

function DbUtil:savePlayerRecordData(player, immediate)
    local data = {}
    data.killMonsterTotal = player.killMonsterTotal or 0
    data.killMonsterWeek = player.killMonsterWeek or 0
    data.weekKey = player.weekKey or 0
    DBManager:savePlayerData(player.userId, DbUtil.RECORD_DATA, data, immediate)
    --- 上报记录
    player:reportRecord()
end
