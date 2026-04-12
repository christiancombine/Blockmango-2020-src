---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(134)
end

function Messages:breakDown(second)
    return 135, second
end

function Messages:openChest(playerName, chestName)
    return 136, playerName .. TextFormat.argsSplit .. self:getChestName(chestName)
end

function Messages:getChestName(name)
    return TextFormat.colorAqua .. "[" .. TextFormat:getTipType(name) .. "]"
end

function Messages:showDeep(deep)
    return 137, deep
end

function Messages:showDeepAndHp(deep, hp)
    return 138, deep .. TextFormat.argsSplit .. hp
end

return Messages