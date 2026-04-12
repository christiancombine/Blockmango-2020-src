ArchiveConfig = {}
ArchiveConfig.archive = {}

function ArchiveConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.nextPrice = tonumber(v.nextPrice)
        self.archive[data.id] = data
    end
end

function ArchiveConfig:getArchiveInfo(id)
    return self.archive[id]
end

function ArchiveConfig:getNextPrice(num)
    if not self.archive[num] then
        return 0
    end

    return self.archive[num].nextPrice
end

function ArchiveConfig:getMaxArchiveNum()
    return #self.archive
end

return ArchiveConfig