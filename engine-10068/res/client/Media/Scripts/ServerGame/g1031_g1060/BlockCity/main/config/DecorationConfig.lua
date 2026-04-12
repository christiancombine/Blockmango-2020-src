DecorationConfig = {}
DecorationConfig.decoration = {}

function DecorationConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.score = tonumber(v.score)
        data.type = tonumber(v.type)
        data.level = tonumber(v.level)
        data.interact = tonumber(v.interact)
        data.price = tonumber(v.price)
        data.cube = tonumber(v.cube)
        data.length = tonumber(v.length)
        data.width = tonumber(v.width)
        data.height = tonumber(v.height)
        data.isInteraction = tonumber(v.isInteraction)
        data.name = v.name
        data.icon = v.icon
        data.weight = tonumber(v.weight)
        data.tipNor = v.tipNor
        data.tipPre = v.tipPre
        data.limit = tonumber(v.limit)
        if data.tipNor == "@@@" then
            data.tipNor = ""
        end
        if data.tipPre == "@@@" then
            data.tipPre = ""
        end
        data.imageNor = v.imageNor
        data.imagePre = v.imagePre
        if data.imageNor == "@@@" then
            data.imageNor = ""
        end
        if data.imagePre == "@@@" then
            data.imagePre = ""
        end
        data.isNew = tonumber(v.isNew)
        data.moneyType = tonumber(v.moneyType)
        self.decoration[data.id] = data
    end
end

function DecorationConfig:getScore(id)
    if not self.decoration[tonumber(id)] then
        return 0
    end

    return self.decoration[tonumber(id)].score
end

function DecorationConfig:getDecorationInfo(id)
    if not self.decoration[tonumber(id)] then
        return nil
    end
    return self.decoration[tonumber(id)]
end

return DecorationConfig