ArmorConfig = {}
local armorConfig = {}

function ArmorConfig:init(Armors)
    for _,ArmorData in pairs(Armors) do
        local Armor = {}
        Armor.ArmorId = tonumber(ArmorData.itemId) or 0
        Armor.ArmorValue = tonumber(ArmorData.armorvalue) or 0
        Armor.extraInvCount = tonumber(ArmorData.extraInvCount) or 0
        Armor.multiple = tonumber(ArmorData.multiple) or 10
        armorConfig[Armor.ArmorId] = Armor
    end
end

function ArmorConfig:getArmorValueById(id)
    if armorConfig[tonumber(id)] ~= nil then
        return armorConfig[tonumber(id)]
    end
    return nil
end

function ArmorConfig:getCountByArmorId(itemId)
    local Armor = armorConfig[tonumber(itemId)]
    if Armor ~= nil then
        return Armor.extraInvCount
    end
    return 0
end

return ArmorConfig