ItemsMappingConfig = {}
ItemsMappingConfig.itemMapping = {}

function ItemsMappingConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.sourceId = tonumber(v.sourceId)
        data.mappingId = tonumber(v.mappingId)
        self.itemMapping[i] = data
    end
end

function ItemsMappingConfig:getSourceIdByMappingId(mappingId)
    for i, v in pairs(self.itemMapping) do
        if v.mappingId == mappingId then
            return v.sourceId
        end
    end

    HostApi.log("===LugLog: ItemsMappingConfig:getSourceIdByMappingId: itemsMapping.csv can not find mappingId["..mappingId.."]")
    return 0
end

function ItemsMappingConfig:getMappingIdBySourceId(sourceId)
    for i, v in pairs(self.itemMapping) do
        if v.sourceId == sourceId then
            return v.mappingId
        end
    end

    HostApi.log("===LugLog: ItemsMappingConfig:getMappingIdBySourceId: itemsMapping.csv can not find sourceId["..sourceId.."]")
    return 0
end

return ItemsMappingConfig