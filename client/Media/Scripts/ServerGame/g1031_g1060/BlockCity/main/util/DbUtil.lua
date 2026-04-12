DbUtil = {}
DbUtil.TAG_PLAYER = "player"
DbUtil.TAG_MANOR = "manor"
DbUtil.TAG_BLOCK = "block"
DbUtil.TAG_DECORATION = "decoration"
DbUtil.TAG_ARCHIVE = "archive"
DbUtil.TAG_ARCHIVE_BLOCK = "abk"
DbUtil.TAG_ARCHIVE_DECORATION = "adn"

DBManager:addMustLoadSubKey(DbUtil.TAG_PLAYER)
DBManager:addMustLoadSubKey(DbUtil.TAG_MANOR)
DBManager:addMustLoadSubKey(DbUtil.TAG_DECORATION)

DbUtil.tempBlockData = {}
DbUtil.isBlockLoadEnd = {}

DbUtil.numOfArchive = {}
DbUtil.isArchiveGetData = {}
DbUtil.tempArchiveBlockData = {}
DbUtil.isArchiveBlockLoadEnd = {}
DbUtil.isArchiveDecorationLoadEnd = {}

function DbUtil:getPlayerData(userId)
    DBManager:getPlayerData(userId, DbUtil.TAG_MANOR)
end

function DbUtil:initDataFromDB(userId, data, subKey)
    --DBManager:addCache(userId, subKey, data)
    if subKey == DbUtil.TAG_PLAYER then
        local player = PlayerManager:getPlayerByUserId(userId)
        if player ~= nil then
            DBManager:addCache(userId, subKey, data)

            player:initDataFromDB(data)
            if player.archive and player.archive.isGetData == false then
                player.archive:setIsGetData(true)
                DbUtil.numOfArchive[tostring(userId)] = player:getArchiveNum()
                for index, _ in pairs(player.archiveData) do
                    local archiveBlockSubKey = DbUtil.TAG_ARCHIVE ..  index .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. "1"
                    DBManager:getPlayerData(userId, archiveBlockSubKey)

                    local archiveDecorationSubKey = DbUtil.TAG_ARCHIVE .. index .. "_" .. DbUtil.TAG_ARCHIVE_DECORATION
                    DBManager:getPlayerData(userId, archiveDecorationSubKey)
                end
            end
        end
    end

    if subKey == DbUtil.TAG_MANOR then
        local manor = SceneManager:getUserManorInfo(userId)
        if not manor then
            return
        end

        DBManager:addCache(userId, subKey, data)

        if not data.__first then
            manor:initDataFromDB(data)
        end

        manor:addShowName(0)

        local blockSubKey = DbUtil.TAG_BLOCK .. "1"
        DBManager:getPlayerData(userId, blockSubKey)

        local startPos, endPos = ManorConfig:getOpenManorPos(manor.level, manor.manorIndex, true)
        if startPos and endPos then
            SceneManager:fillAreaBaseBlock(startPos, endPos, manor.level)
        end

        ManorLevelConfig:createHouseManager(userId, manor.manorIndex, manor.level)
    end

    if string.find(subKey, DbUtil.TAG_BLOCK) then
        local block = SceneManager:getUserBlockInfo(userId)
        if not block then
            return
        end

        DBManager:addCache(userId, subKey, data)

        if not DbUtil.tempBlockData[tostring(userId)] then
            DbUtil.tempBlockData[tostring(userId)] = {}
        end

        if not data.__first then
            for index, key in pairs(data.blocks or {}) do
                DbUtil.tempBlockData[tostring(userId)][index] = key
            end

            local blockSubKey = string.gsub(subKey, DbUtil.TAG_BLOCK, "")
            blockSubKey = tonumber(blockSubKey) + 1
            blockSubKey = DbUtil.TAG_BLOCK .. blockSubKey
            DBManager:getPlayerData(userId, blockSubKey)
        else
            local blockSubKey = string.gsub(subKey, DbUtil.TAG_BLOCK, "")
            if tonumber(blockSubKey) ~= 1 then
                DBManager:removeCache(userId, {DbUtil.TAG_BLOCK .. blockSubKey})
            end
            DbUtil.isBlockLoadEnd[tostring(userId)] = true
        end

        if DbUtil.isBlockLoadEnd[tostring(userId)] then
            local blockData = { blocks = DbUtil.tempBlockData[tostring(userId)] }
            block:initDataFromDB(blockData)
            DBManager:getPlayerData(userId, DbUtil.TAG_DECORATION)

            DbUtil.tempBlockData[tostring(userId)] = nil
            DbUtil.isBlockLoadEnd[tostring(userId)] = nil
        end

    end

    if subKey == DbUtil.TAG_DECORATION then
        local decoration = SceneManager:getUserDecorationInfo(userId)
        if not decoration then
            return
        end

        DBManager:addCache(userId, subKey, data)

        if not data.__first then
            decoration:initDataFromDB(data)
        end
    end

    if string.find(subKey, DbUtil.TAG_ARCHIVE) then
        local archiveInfo = SceneManager:getUserArchiveInfo(userId)
        if not archiveInfo then
            return
        end

        -- note : the value of key like archive1_block1  or archive1_decoration
        local key = StringUtil.split(subKey, "_")
        if not key[1] or not key[2] then
            return
        end

        DBManager:addCache(userId, subKey, data)

        local archiveId = string.gsub(key[1], DbUtil.TAG_ARCHIVE, "")
        archiveId = tonumber(archiveId)

        if not DbUtil.isArchiveDecorationLoadEnd[tostring(userId)] then
            DbUtil.isArchiveDecorationLoadEnd[tostring(userId)] = {}
        end

        DbUtil.isArchiveDecorationLoadEnd[tostring(userId)][archiveId] = false

        if not DbUtil.isArchiveGetData[tostring(userId)] then
            DbUtil.isArchiveGetData[tostring(userId)] = {}
        end

        DbUtil.isArchiveGetData[tostring(userId)][archiveId] = false

        if not data.__first then
            -- 1. 分割key, 找出是block还是decoration
            -- 2. key[1] 后面的数字确定archiveId
            -- 3. 如果是block, 根据block1后面的数字，动态获取数据库
            -- 4. 全部block获取完后，设置到player.archive:setArchiveBlock(archiveId, data)
            -- 5. 如果是decoration, 直接设置到 player.archive:setArchiveDecoration(archiveId, data)
            -- 6. 拿到最后一个数据后把相关的缓存数据设置为nil
            if string.find(key[2], DbUtil.TAG_ARCHIVE_BLOCK) then
                if not DbUtil.tempArchiveBlockData[tostring(userId)] then
                    DbUtil.tempArchiveBlockData[tostring(userId)] = {}
                end

                if not DbUtil.tempArchiveBlockData[tostring(userId)][archiveId] then
                    DbUtil.tempArchiveBlockData[tostring(userId)][archiveId] = {}
                end

                for i, v in pairs(data.blocks or {}) do
                    DbUtil.tempArchiveBlockData[tostring(userId)][archiveId][i] = v
                end

                local blockIndex = string.gsub(key[2], DbUtil.TAG_ARCHIVE_BLOCK, "")
                blockIndex = tonumber(blockIndex)
                blockIndex = blockIndex + 1
                local blockSubKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. blockIndex
                DBManager:getPlayerData(userId, blockSubKey)
            end

            if string.find(key[2], DbUtil.TAG_ARCHIVE_DECORATION) then
                archiveInfo:setArchiveDecoration(archiveId, data.decoration or {})
                DbUtil.isArchiveDecorationLoadEnd[tostring(userId)][archiveId] = true
            end
        else
            if string.find(key[2], DbUtil.TAG_ARCHIVE_BLOCK) then
                if not DbUtil.isArchiveBlockLoadEnd[tostring(userId)] then
                    DbUtil.isArchiveBlockLoadEnd[tostring(userId)] = {}
                end

                local blockIndex = string.gsub(key[2], DbUtil.TAG_ARCHIVE_BLOCK, "")
                if tonumber(blockIndex) ~= 1 then
                    local blockSubKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. blockIndex
                    DBManager:removeCache(userId, {blockSubKey})
                end
                DbUtil.isArchiveBlockLoadEnd[tostring(userId)][archiveId] = true
            end

            if string.find(key[2], DbUtil.TAG_ARCHIVE_DECORATION) then
                archiveInfo:setArchiveDecoration(archiveId, {})
                DbUtil.isArchiveDecorationLoadEnd[tostring(userId)][archiveId] = true
            end
        end

        if DbUtil.isArchiveBlockLoadEnd[tostring(userId)] and DbUtil.isArchiveBlockLoadEnd[tostring(userId)][archiveId] then
            local archiveBlockData = {}
            if DbUtil.tempArchiveBlockData[tostring(userId)] and DbUtil.tempArchiveBlockData[tostring(userId)][archiveId] then
                archiveBlockData = DbUtil.tempArchiveBlockData[tostring(userId)][archiveId]
            end
            
            archiveInfo:setArchiveBlock(archiveId, archiveBlockData)
            DbUtil.isArchiveGetData[tostring(userId)][archiveId] = true
        end

        local num = DbUtil.numOfArchive[tostring(userId)]
        if not num then
            -- if num is nil, this archiveId is not belong to player, so set the template data as nil.
            DbUtil.isArchiveBlockLoadEnd[tostring(userId)] = nil
            DbUtil.isArchiveDecorationLoadEnd[tostring(userId)] = nil
            DbUtil.tempArchiveBlockData[tostring(userId)] = nil
            DbUtil.isArchiveGetData[tostring(userId)] = nil
            return
        end

        if num < 1 then
            return
        end

        local isReady = true
        for i = 1, num do
            if not DbUtil.isArchiveGetData[tostring(userId)][i] then
                isReady = false
            end
        end

        for _, v in pairs(DbUtil.isArchiveDecorationLoadEnd[tostring(userId)]) do
            if not v then
                isReady = false
            end
        end

        if isReady then
            DbUtil.numOfArchive[tostring(userId)] = nil
            DbUtil.isArchiveBlockLoadEnd[tostring(userId)] = nil
            DbUtil.isArchiveDecorationLoadEnd[tostring(userId)] = nil
            DbUtil.tempArchiveBlockData[tostring(userId)] = nil
            DbUtil.isArchiveGetData[tostring(userId)] = nil
        end
    end
end

function DbUtil:savePlayerData(player, immediate)
    if player == nil then
        return
    end

    DbUtil:savePlayerGameData(player, immediate)
    DbUtil:savePlayerManorData(player, immediate)
    DbUtil:savePlayerBlockData(player, immediate)
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

function DbUtil:savePlayerBlockData(player, immediate)
    if player.block ~= nil and player.block:getReady() then
        local blockData = player.block:prepareDataSaveToDB()
        local blockIndex = 1
        local tempData = {}
        local subKeyIndex = 1

        for key, value in pairs(blockData.blocks) do
            tempData[key] = value
            blockIndex = blockIndex + 1
            -- 如果存储的方块数 大于 10000个,需要分subKey保存
            if blockIndex > 10000 then
                local data = { blocks = tempData }
                local subKey = DbUtil.TAG_BLOCK .. subKeyIndex
                if not DBManager:isLoadDataFinished(player.userId, subKey) then
                    DBManager:addCache(player.userId, subKey, {})
                end
                DBManager:savePlayerData(player.userId, subKey, data, immediate)
                tempData = {}
                blockIndex = 1
                subKeyIndex = subKeyIndex + 1
            end
        end

        if next(tempData) then
            local data = { blocks = tempData }
            local subKey = DbUtil.TAG_BLOCK .. subKeyIndex
            if not DBManager:isLoadDataFinished(player.userId, subKey) then
                DBManager:addCache(player.userId, subKey, {})
            end
            DBManager:savePlayerData(player.userId, subKey, data, immediate)
        else
            if subKeyIndex == 1 then
                local subKey = DbUtil.TAG_BLOCK .. subKeyIndex
                DBManager:savePlayerData(player.userId, subKey, {}, immediate)
            end
        end

        DbUtil:clearPlayerBlockData(player, subKeyIndex + 1)
    else
        if not player.block then
            --print("DbUtil:savePlayerBlockData: player[", tostring(player.userId),"] block is nil")
        else
            if not player.block:getReady() then
                print("DbUtil:savePlayerBlockData: player[", tostring(player.userId),"] block getReady is false")
            end
        end
    end
end

function DbUtil:clearPlayerBlockData(player, index)
    index = index or 1
    local subKey = DbUtil.TAG_BLOCK .. index
    while DBManager:isLoadDataFinished(player.userId, subKey) do
        DBManager:savePlayerData(player.userId, subKey, {}, false)
        index = index + 1
        subKey = DbUtil.TAG_BLOCK .. index
    end
end

function DbUtil:savePlayerDecorationData(player, immediate)
    if player.decoration ~= nil then
        local decorationData = player.decoration:prepareDataSaveToDB()
        DBManager:savePlayerData(player.userId, DbUtil.TAG_DECORATION, decorationData, immediate)
    end
end

function DbUtil:savePlayerArchiveData(player, archiveId, blockData, decorationData)
    -- 1. 把 blockData 和 decorationData 数据设置到对应archiveId的存档
    -- 2. archiveDecorationData 直接存到数据库里, 需要判断有没有cache
    -- 3. archiveBlockData 需要每10000个方块分一个key保存，需要判断有没有cache

    if not player.archive then
        return
    end

    player.archive:setArchiveBlock(archiveId, blockData)
    player.archive:setArchiveDecoration(archiveId, decorationData)

    local archiveBlockData = player.archive:prepareArchiveBlockDataToDB(archiveId)
    local archiveDecorationData = player.archive:prepareArchiveDecorationDataToDB(archiveId)

    local decorationKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_DECORATION
    if not DBManager:isLoadDataFinished(player.userId, decorationKey) then
        DBManager:addCache(player.userId, decorationKey, {})
    end

    local decorationSaveData = {}
    if next(archiveDecorationData) then
        decorationSaveData = {decoration = archiveDecorationData}
    end
    DBManager:savePlayerData(player.userId, decorationKey, decorationSaveData, true)


    local blockIndex = 1
    local tempData = {}
    local subKeyIndex = 1
    for key, value in pairs(archiveBlockData) do
        tempData[key] = value
        blockIndex = blockIndex + 1
        if blockIndex > 10000 then
            local data = { blocks = tempData }
            local subKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. subKeyIndex
            if not DBManager:isLoadDataFinished(player.userId, subKey) then
                DBManager:addCache(player.userId, subKey, {})
            end

            DBManager:savePlayerData(player.userId, subKey, data, true)
            tempData = {}
            blockIndex = 1
            subKeyIndex = subKeyIndex + 1
        end
    end

    if next(tempData) then
        local data = { blocks = tempData }
        local subKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. subKeyIndex
        if not DBManager:isLoadDataFinished(player.userId, subKey) then
            DBManager:addCache(player.userId, subKey, {})
        end
        DBManager:savePlayerData(player.userId, subKey, data, true)
    else
        if subKeyIndex == 1 then
            local subKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. subKeyIndex
            if not DBManager:isLoadDataFinished(player.userId, subKey) then
                DBManager:addCache(player.userId, subKey, {})
            end
            DBManager:savePlayerData(player.userId, subKey, {}, true)
        end
    end

    DbUtil:clearPlayerArchiveBlockData(player, archiveId, subKeyIndex + 1)
end

function DbUtil:clearPlayerArchiveBlockData(player, archiveId, blockIndex)
    blockIndex = blockIndex or 1
    local subKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. blockIndex
    while DBManager:isLoadDataFinished(player.userId, subKey) do
        DBManager:savePlayerData(player.userId, subKey, {}, true)
        blockIndex = blockIndex + 1
        subKey = DbUtil.TAG_ARCHIVE .. archiveId .. "_" .. DbUtil.TAG_ARCHIVE_BLOCK .. blockIndex
    end
end


return DbUtil