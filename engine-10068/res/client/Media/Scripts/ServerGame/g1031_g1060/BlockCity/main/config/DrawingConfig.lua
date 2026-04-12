DrawingConfig = {}
DrawingConfig.drawing = {}
DrawingConfig.drawingData = {}

function DrawingConfig:initDrawing(config)
    for _, v in pairs(config) do
        local data = {}
        data.order = tonumber(v.order)
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.level = tonumber(v.level)
        data.fileName =  v.fileName
        data.length = tonumber(v.length)
        data.width = tonumber(v.width)
        data.height = tonumber(v.height)
        data.name = v.name
        if data.name == "@@@" then
            data.name = ""
        end
        data.icon = v.icon
        data.type = tonumber(v.type)
        data.price = tonumber(v.price)
        data.cube = tonumber(v.cube)
        data.isFake = tonumber(v.isFake)
        data.isNew = tonumber(v.isNew)
        data.isHot = tonumber(v.isHot)
        data.moneyType = tonumber(v.moneyType)
        self.drawing[data.order] = data
    end
end

function DrawingConfig:getDrawingById(id)
    id = tonumber(id)
    for _, v in pairs(self.drawing) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function DrawingConfig:getDrawingByItemId(itemId)
    itemId = tonumber(itemId)
    for _, v in pairs(self.drawing) do
        if v.itemId == itemId then
            return v
        end
    end

    return nil
end

function DrawingConfig:initDrawingData(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.drawingId = tonumber(v.drawingId)
        data.blockId = tonumber(v.blockId)
        data.meta = tonumber(v.meta)
        data.num = tonumber(v.num)
        self.drawingData[data.id] = data
    end
end

function DrawingConfig:getBlockDataByDrawingId(drawingId)
    drawingId = tonumber(drawingId)
    local data = {}
    for _, v in pairs(self.drawingData) do
        if v.drawingId == drawingId then
            table.insert(data, v)
        end
    end

    return data
end


return DrawingConfig