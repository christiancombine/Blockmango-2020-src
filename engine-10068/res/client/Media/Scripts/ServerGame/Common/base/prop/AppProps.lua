---
--- Created by Jimmy.
--- DateTime: 2018/8/20 0020 11:31
---

AppProps = {}

AppProps.TYPE_ITEM = 1
AppProps.TYPE_BUFF = 2
AppProps.TYPE_CUSTOM = 3

AppProps.props = {}

function AppProps:init(name)
    name = name or "AppProps.csv"
    local path = FileUtil:getMapPath() .. name
    local file = io.open(path, "r")
    if not file then
        return
    end
    io.close(file)
    self:initProps(CsvUtil.loadCsvFile(path, 2))
end

function AppProps:initProps(props)
    for _, prop in pairs(props) do
        local item = {}
        item.id = prop.id
        item.type = tonumber(prop.type)
        item.isRepeat = tonumber(prop.isRepeat or "1") == 1
        item.itemId = tonumber(prop.itemId)
        item.itemMeta = tonumber(prop.itemMeta)
        item.itemNum = tonumber(prop.itemNum)
        local enchants = {}
        if prop.enchants ~= "#" then
            local c_enchants = StringUtil.split(prop.enchants, "#")
            for _, enchant in pairs(c_enchants) do
                local c_enchant = StringUtil.split(enchant, "=")
                local a_item = {}
                a_item[1] = tonumber(c_enchant[1]) or 0
                a_item[2] = tonumber(c_enchant[2]) or 0
                enchants[#enchants + 1] = a_item
            end
        end
        item.enchants = enchants
        local effects = {}
        if prop.effects then
            local c_effects = StringUtil.split(prop.effects, "#")
            for _, effect in pairs(c_effects) do
                local c_effect = StringUtil.split(effect, "=")
                local a_item = {}
                a_item.id = tonumber(c_effect[1]) or 0
                a_item.lv = tonumber(c_effect[2]) or 0
                a_item.time = tonumber(c_effect[3]) or 0
                effects[#effects + 1] = a_item
            end
        end
        item.effects = effects
        self.props[item.id] = item
    end
end

function AppProps:hasProp(player, propId)
    for _, p_prop in pairs(player.staticAttr.props) do
        if p_prop == propId then
            return true
        end
    end
    return false
end

function AppProps:useAppProps(player, isFirst)
    if #player.staticAttr.props == 0 then
        return
    end
    local props = {}
    for propId, prop in pairs(self.props) do
        if self:hasProp(player, propId) then
            props[#props + 1] = prop
        end
    end
    for propId, prop in pairs(props) do
        if isFirst then
            self:useAppProp(player, prop)
        else
            if prop.isRepeat then
                self:useAppProp(player, prop)
            end
        end
    end
end

function AppProps:useAppProp(player, prop)
    if prop.type == AppProps.TYPE_ITEM then
        if #prop.enchants > 0 then
            player:addEnchantmentsItem(prop.itemId, prop.itemNum, prop.itemMeta, prop.enchants)
        else
            player:addItem(prop.itemId, prop.itemNum, prop.itemMeta)
        end
    end
    if prop.type == AppProps.TYPE_BUFF then
        for _, effect in pairs(prop.effects) do
            if effect.id ~= 0 and effect.time ~= 0 then
                player:addEffect(effect.id, effect.time, effect.lv)
            end
        end
    end
    if prop.type == AppProps.TYPE_CUSTOM then
        player:onUseCustomProp(prop.id, prop.itemId)
    end
end

function AppProps:useItemAppProps(player)
    if #player.staticAttr.props == 0 then
        return
    end
    local props = {}
    for propId, prop in pairs(self.props) do
        if prop.type == AppProps.TYPE_ITEM then
            if self:hasProp(player, propId) then
                props[#props + 1] = prop
            end
        end
    end
    for propId, prop in pairs(props) do
        self:useAppProp(player, prop)
    end
end

return AppProps