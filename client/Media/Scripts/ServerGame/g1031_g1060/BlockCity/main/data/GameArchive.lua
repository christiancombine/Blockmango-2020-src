GameArchive = class()

function GameArchive:ctor(manorIndex, userId)
    self.manorIndex = manorIndex
    self.userId = userId
    self.isGetData = false
    self.archiveBlock = {}
    self.archiveDecoration = {}
end

function GameArchive:isArchiveEmpty(archiveId)
    if self.archiveBlock[archiveId] and next(self.archiveBlock[archiveId]) then
        return false
    end

    if self.archiveDecoration[archiveId] and next(self.archiveDecoration[archiveId]) then
        return false
    end

    return true
end

function GameArchive:setArchiveBlock(archiveId, data)
    self.archiveBlock[archiveId] = data
end

function GameArchive:setArchiveDecoration(archiveId, data)
    self.archiveDecoration[archiveId] = data
end

function GameArchive:setIsGetData(status)
    self.isGetData = status
end

function GameArchive:prepareArchiveBlockDataToDB(archiveId)
    return self.archiveBlock[archiveId] or {}
end

function GameArchive:prepareArchiveDecorationDataToDB(archiveId)
    return self.archiveDecoration[archiveId] or {}
end

function GameArchive:getArchiveBlockData(archiveId)
    return self.archiveBlock[archiveId] or {}
end

function GameArchive:getArchiveDecorationData(archiveId)
    return self.archiveDecoration[archiveId] or {}
end


