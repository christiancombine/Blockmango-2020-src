--region *.lua

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10001)
end

function Messages:breakEgg(team, destroy)
    return team..TextFormat.argsSplit..destroy
end

function Messages:getStoreName(type)
    local name = ""
    if BaseMain:isChina() then
        if type == 1 then
            name = name .. "防具"
        elseif type == 2 then
            name = name .. "武器"
        elseif type == 3 then
            name = name .. "方块"
        elseif type == 4 then
            name = name .. "食物"
        else
            name = name .. "其他"
        end
    else
        if type == 1 then
            name = name .. "Armor"
        elseif type == 2 then
            name = name .. "Arms"
        elseif type == 3 then
            name = name .. "Block"
        elseif type == 4 then
            name = name .. "Food"
        else
            name = name .. "Other"
        end
    end
    return name
end

function Messages:highestLevel(level, itemName, num, time)
    return 10002, level .. TextFormat.argsSplit .. self:getResourceName(itemName) .. TextFormat.argsSplit .. num .. TextFormat.argsSplit .. time
end

function Messages:updateInfo(level, itemName, num, time)
    return 10003, level .. TextFormat.argsSplit .. self:getResourceName(itemName) .. TextFormat.argsSplit .. num .. TextFormat.argsSplit .. time
end

function Messages:upgradeSuccess()
    return 10007
end

function Messages:upgradeFailed()
    return 10008
end

function Messages:getResourceName(name)
    return TextFormat:getTipType(name)
end

function Messages:EnchantMentOpen()
    return 1018001
end

function Messages:EnchantMentLeftTime(time)
    return 1018002, tonumber(time)
end

function Messages:EnchantMentConsumeLack()
    return 1018003
end

return Messages


--endregion
