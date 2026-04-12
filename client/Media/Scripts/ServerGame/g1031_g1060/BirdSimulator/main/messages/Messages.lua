
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1041001)
end

function Messages:backpackFull()
    return 1041002
end

function Messages:notEnoughFruit()
    return 1041003
end

function Messages:notCarryBird()
    return 1041004
end

function Messages:treeAppearInField(qualityId, fieldId)
    return 1041012, TextFormat:getTipType(qualityId) .. TextFormat.argsSplit .. TextFormat:getTipType(fieldId)
end

function Messages:getFightPoint(point)
    return 1041030, tostring(point)
end

return Messages