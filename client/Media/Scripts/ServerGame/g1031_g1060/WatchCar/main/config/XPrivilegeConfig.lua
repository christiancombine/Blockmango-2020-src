XPrivilegeConfig = {}
XPrivilegeConfig.PrivilegeConfigs = {}
XPrivilegeConfig.PrivilegeItems = {}

function XPrivilegeConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local tab = {}
        tab.Id = tonumber(config.Id)
        tab.RespawnTime = tonumber(config.RespawnTime)
        tab.ExtraHp = tonumber(config.ExtraHp) or 0
        tab.ShowCount = tonumber(config.PropCount)
        tab.IsGiftBag = config.IsGiftBag == '1' and true or false
        tab.Prop = nil
        if tab.IsGiftBag == false and config.PropId ~= '#' then
            tab.Prop =            {
                PropId = tonumber(config.PropId),
                PropMate = tonumber(config.PropMate),
                PropCount = tonumber(config.PropCount),
                FumoId = tonumber(config.FumoId),
                FumoLv = tonumber(config.FumoLv),
            }
        else
            local items = StringUtil.split(config.PropId, '#')
            self.PrivilegeItems[tab.Id] = {}
            for _, privilegeId in pairs(items) do
                table.insert(self.PrivilegeItems[tab.Id], tonumber(privilegeId))
            end
        end

        if self.PrivilegeConfigs[tab.Id] == nil then
            self.PrivilegeConfigs[tab.Id] = {}
        end
        self.PrivilegeConfigs[tab.Id] = tab
    end
end

function XPrivilegeConfig:getReawrdItemCount(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return nil end
    end
    local privilege = self.PrivilegeConfigs[id]
    if type(privilege) == 'table' then
        return privilege.ShowCount
    end
    return nil
end

function XPrivilegeConfig:HasGiftBagById(id)
    local res = self.PrivilegeConfigs[id]
    return res and res.IsGiftBag or false
end

function XPrivilegeConfig:GetRewardPropInfoById(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return nil end
    end
    local privilege = self.PrivilegeConfigs[id]
    if type(privilege) == 'table' and type(privilege.Prop) == 'table' then
        return privilege.Prop
    end
    return nil
end

function XPrivilegeConfig:getPrivilegeInfobById(id)
    local tab = {}
    tab.Items = {}
    tab.AddHp = 0
    tab.RespawnTime = nil

    local callback = function(itemid)
        local temp = self.PrivilegeConfigs[itemid]
        if type(temp) == 'table' then
            if temp.Prop ~= nil then
                table.insert(tab.Items, temp.Prop)
            end
            tab.AddHp = tab.AddHp + temp.ExtraHp
            if tab.RespawnTime == nil and type(temp.RespawnTime) == 'number' then
                tab.RespawnTime = temp.RespawnTime
            elseif type(temp.RespawnTime) == 'number' then
                tab.RespawnTime = math.min(tab.RespawnTime, temp.RespawnTime)
            end
        end
    end

    if self:HasGiftBagById(id) then
        local privilegeList = self.PrivilegeItems[id]
        if type(privilegeList) == 'table' then
            for _, itemid in pairs(privilegeList) do
                callback(itemid)
            end
        end
    else
        callback(id)
    end
    return tab
end

function XPrivilegeConfig:GetRewardPropGiftBagInfoById(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return nil end
    end
    local privilege = self.PrivilegeConfigs[id]


    return nil
end

function XPrivilegeConfig:GetRewardFumoInfo(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return nil end
    end

    local privilege = self.PrivilegeItems[id]
    if type(privilege) == 'table' and type(privilege.Prop) == 'table' then
        local prop = privilege.Prop
        return prop.FumoId, prop.FumoLv
    end
    return nil
end

function XPrivilegeConfig:GetRewardRespawnTime(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return nil end
    end
    local privilege = self.PrivilegeConfigs[id]
    if type(privilege) == 'table' then
        return privilege.RespawnTime or nil
    end
    return nil
end

function XPrivilegeConfig:GetRewardHp(id)
    if type(id) ~= "number" then
        id = tonumber(id)
        if id == nil then return 0 end
    end
    local privilege = self.PrivilegeConfigs[id]
    if type(privilege) == 'table' then
        return privilege.ExtraHp or nil
    end
    return nil
end