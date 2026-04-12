BlackBoard = {}

local BlackBoardData = {}

function BlackBoard:setValue(key, value)
    if key == nil then
        return
    end
    BlackBoardData[key] = value
end

function BlackBoard:getValue(key)
    if key == nil or BlackBoardData[key] == nil then
        return nil
    end
    return BlackBoardData[key]
end

function BlackBoard:deleteValue(key)
    if key == nil then
        return false
    end
    BlackBoardData[key] = nil
    return true
end

return BlackBoard