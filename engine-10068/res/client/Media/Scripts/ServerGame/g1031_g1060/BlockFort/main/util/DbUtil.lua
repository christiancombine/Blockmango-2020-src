DbUtil = {}
DbUtil.TAG_PLAYER = "player"
DbUtil.TAG_MANOR = "manor"
DbUtil.TAG_SCHEMATIC = "schematic"
DbUtil.TAG_DECORATION = "decoration"

DBManager:addMustLoadSubKey(DbUtil.TAG_PLAYER)
DBManager:addMustLoadSubKey(DbUtil.TAG_MANOR)
DBManager:addMustLoadSubKey(DbUtil.TAG_SCHEMATIC)
DBManager:addMustLoadSubKey(DbUtil.TAG_DECORATION)

function DbUtil:getPlayerData(userId)
    DBManager:getPlayerData(userId, DbUtil.TAG_MANOR)
end

function DbUtil:initDataFromDB(userId, data, subKey)
    DBManager:addCache(userId, subKey, data)
    if subKey == DbUtil.TAG_PLAYER then
        local player = PlayerManager:getPlayerByUserId(userId)
        if player ~= nil then
            player:initDataFromDB(data)
        end
    end

    if subKey == DbUtil.TAG_MANOR then
        local manor = SceneManager:getUserManorInfo(userId)
        if not manor then
            return
        end
        if not data.__first then
            manor:initDataFromDB(data)
        end
        manor:addShowName(0)
        local startPos, endPos = ManorConfig:getOpenManorPos(manor.level, manor.manorIndex, true)
        if startPos and endPos then
            SceneManager:fillAreaBaseBlock(startPos, endPos, manor.level)
        end
    end

    if subKey == DbUtil.TAG_SCHEMATIC then
        local schematic = SceneManager:getUserSchematicInfo(userId)
        if not schematic then
            return
        end
        if not data.__first then
            schematic:initDataFromDB(data)
        end
    end

    if subKey == DbUtil.TAG_DECORATION then
        local decoration = SceneManager:getUserDecorationInfo(userId)
        if not decoration then
            return
        end
        if not data.__first then
            decoration:initDataFromDB(data)
        end
    end
end

function DbUtil:savePlayerData(player, immediate)
    if player == nil then
        return
    end

    DbUtil:savePlayerGameData(player, immediate)
    DbUtil:savePlayerManorData(player, immediate)
    DbUtil:savePlayerSchematicData(player, immediate)
    DbUtil:savePlayerDecorationData(player, immediate)
end

function DbUtil:savePlayerGameData(player, immediate)
    local playerData = player:prepareDataSaveToDB()
    DBManager:savePlayerData(player.userId, DbUtil.TAG_PLAYER, playerData, immediate)
end

function DbUtil:savePlayerManorData(player, immediate)
    if player.manor ~= nil then
        local manorData = player.manor:prepareDataSaveToDB()
        DBManager:savePlayerData(player.userId, DbUtil.TAG_MANOR, manorData, immediate)
    end
end

function DbUtil:savePlayerSchematicData(player, immediate)
    if player.schematic ~= nil then
        local schematicData = player.schematic:prepareDataSaveToDB()
        DBManager:savePlayerData(player.userId, DbUtil.TAG_SCHEMATIC, schematicData, immediate)
    end
end

function DbUtil:savePlayerDecorationData(player, immediate)
    if player.decoration ~= nil then
        local decorationData = player.decoration:prepareDataSaveToDB()
        DBManager:savePlayerData(player.userId, DbUtil.TAG_DECORATION, decorationData, immediate)
    end
end