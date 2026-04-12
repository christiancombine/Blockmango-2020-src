---
--- Created by Jimmy.
--- DateTime: 2018/9/27 0027 14:53
---
ItemSettingConfig = {}
ItemSettingConfig.settings = {}

function ItemSettingConfig:init(settings)
    self:initSettings(settings)
end

function ItemSettingConfig:initSettings(settings)
    for _, setting in pairs(settings) do
        local item = {}
        item.id = setting.id
        item.itemId = tonumber(setting.itemId)
        item.type = setting.type
        item.maxNum = tonumber(setting.maxNum)
        self.settings[setting.itemId] = item
    end
end

function ItemSettingConfig:getItemSetting(itemId)
    return self.settings[tostring(itemId)]
end

function ItemSettingConfig:getItemSettingsByType(type)
    local settings = {}
    for _, setting in pairs(self.settings) do
        if setting.type == type then
            table.insert(settings, setting)
        end
    end
    return settings
end

function ItemSettingConfig:canAddItem(player, itemId, itemNum)
    local setting = self:getItemSetting(itemId)
    if setting == nil then
        return true, itemNum
    end
    local settings = self:getItemSettingsByType(setting.type)
    local p_setting, num
    for _, setting in pairs(settings) do
        if player.isLife then
            num = player:getItemNumById(setting.itemId)
        else
            num = self:getItemNumById(player, setting.itemId)
        end
        if num ~= 0 then
            p_setting = setting
            break
        end
    end
    if p_setting == nil then
        return true, math.min(setting.maxNum, itemNum)
    else
        if p_setting.itemId == itemId then
            if setting.maxNum - num <= 0 then
                return false, 0
            else
                return true, math.min(setting.maxNum - num, itemNum)
            end
        else
            return false, 0
        end
    end
end

function ItemSettingConfig:getItemNumById(player, itemId)
    local num = 0
    local inventory = player.inventory
    if inventory == nil then
        return num
    end
    if inventory.item ~= nil then
        for _, c_item in pairs(inventory.item) do
            if c_item.id == itemId then
                num = num + c_item.num
            end
        end
    end
    if inventory.gun ~= nil then
        for _, c_item in pairs(inventory.gun) do
            if c_item.id == itemId then
                num = num + c_item.num
            end
        end
    end
    return num
end

return ItemSettingConfig