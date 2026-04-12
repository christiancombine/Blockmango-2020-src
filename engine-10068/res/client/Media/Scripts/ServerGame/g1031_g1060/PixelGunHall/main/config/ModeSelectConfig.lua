ModeSelectConfig = {}
ModeSelectConfig.mode = {}

function ModeSelectConfig:initConfig(modes)
    for i, mode in pairs(modes) do
        local data = {}
        data.id = tonumber(mode.id) or 0
        data.is_open = tonumber(mode.is_open) or 0
        data.img = tostring(mode.img) or ""
        data.name = tostring(mode.name) or ""
        data.info1 = tostring(mode.info1) or ""
        if data.info1 == "#" then
            data.info1 = ""
        end
        data.info2 = tostring(mode.info2) or ""
        if data.info2 == "#" then
            data.info2 = ""
        end
        data.info_num1 = tostring(mode.info_num1) or ""
        if data.info_num1 == "#" then
            data.info_num1 = ""
        end
        data.mode_first = tonumber(mode.mode_first)
        data.info_num2 = tostring(mode.info_num2) or ""
        if data.info_num2 == "#" then
            data.info_num2 = ""
        end
        data.openLv = tonumber(mode.openLv) or 0
        data.gameType = tostring(mode.gameType) or ""
        data.map = tostring(mode.map) or ""
        data.index = tonumber(mode.index) or 1
        data.tip = tostring(mode.tip) or ""
        data.tip_title = tostring(mode.tip_title) or ""
        data.min_money = tonumber(mode.min_money) or 0
        table.insert(self.mode, data)
    end
    table.sort(self.mode, function(a, b)
        return a.index < b.index
    end)
end

function ModeSelectConfig:getModeInfo(player)
    local data = {}
    data.mode = {}
    for _, mode in pairs(ModeSelectConfig.mode) do
        local item = {}
        item.modeimg = mode.img
        item.is_open = mode.is_open
        item.name = mode.name
        item.info1 = mode.info1
        item.info2 = mode.info2
        item.info_num1 = mode.info_num1
        item.info_num2 = mode.info_num2
        item.info_show1 = self:getModeFirst(mode.id) and player:getModeFirst(mode.gameType)
        item.info_show2 = self:canFragmentInfoShow(player, mode.id)
        item.openLv = mode.openLv
        item.gameType = mode.gameType
        item.index = mode.index
        item.tip = mode.tip
        item.tip_title = mode.tip_title
        item.min_money = mode.min_money
        item.unlock = false
        if player.level >= mode.openLv then
            item.unlock = true
        end
        item.map = {}
        local map_info = StringUtil.split(mode.map, "#")
        for i = 1, #map_info do
            local map = ModeSelectMapConfig:getMapInfo(tonumber(map_info[i]))
            if map then
                local map_item = {}
                map_item.map_id = map.id
                map_item.map_img = map.img
                map_item.map_name = map.name
                map_item.map_show_name = map.show_name
                map_item.small_info_img = map.small_info
                map_item.map_size = map.mapSize
                map_item.openMoneyType = map.openMoneyType
                map_item.openMoneyNum = map.openMoneyNum
                map_item.map_type = map.map_type
                map_item.fragment_gun_img = map.fragment_gun_img
                map_item.fragment_gun_num_txt = map.fragment_gun_num_txt
                map_item.fragment_jindu = player:getFragmentJindu(map.name)
                map_item.random_reward_img1 = map.random_reward_img1
                map_item.random_reward_img2 = map.random_reward_img2
                map_item.random_reward_num1 = map.random_reward_num1
                map_item.random_reward_num2 = map.random_reward_num2
                
                map_item.unlock = self:getMapIsUnlock(player, map.map_type, map.openMoneyType, map.openMoneyNum, map.id)

                table.insert(item.map, map_item)
            end
        end
        table.insert(data.mode, item)
    end

    return data
end

function ModeSelectConfig:getMapIsUnlock(player, map_type, openMoneyType, openMoneyNum, map_id)
    if not player then
        return false
    end

    if map_type == 1 then
        return false
    end

    if tonumber(openMoneyType) == ModeSelectMapConfig.OpenTypeLv then
        if player.level >= openMoneyNum then
            return true
        else
            return false
        end
    elseif tonumber(openMoneyType) == ModeSelectMapConfig.OpenTypeDiamond then
        if player:isUnlockMap(map_id) then
            return true
        else
            return false
        end
    elseif tonumber(openMoneyType) == ModeSelectConfig.OpenTypeNotOpen then
        return false
    else
        return true
    end
    return false
end

-- app: String mapId 
function ModeSelectConfig:getFightMapInfo(player, gameType, mapName)
    if not player then
        return nil, nil, nil
    end

    local _gameType = tostring(gameType)
    local _mapId = tostring(mapName)
    local is_random = false

    for _, mode in pairs(ModeSelectConfig.mode) do
        if tostring(gameType) == tostring(mode.gameType) then
            local map = ModeSelectMapConfig:getMapInfoByMapName(mapName)
            if map then
                if map.map_type == 1 then
                    local new_map = StringUtil.split(mode.map, "#") 

                    local random_pool = {}

                    for i = 1, #new_map do
                        local new_map_info = ModeSelectMapConfig:getMapInfo(new_map[i])
                        if new_map_info and self:getMapIsUnlock(player, new_map_info.map_type, new_map_info.openMoneyType, new_map_info.openMoneyNum, new_map_info.id) then
                            table.insert(random_pool, tostring(new_map_info.name))
                        end
                    end

                    if #random_pool > 0 then
                        local random_num = math.random(1, #random_pool)
                        _mapId = tostring(random_pool[random_num])
                        is_random = true
                        return _gameType, _mapId, is_random
                    end
                    
                    return nil, nil, nil
                end
            end
        end
    end
    return _gameType, _mapId, is_random
end

function ModeSelectConfig:getModeFirst(mode_id)
    for _, mode in pairs(ModeSelectConfig.mode) do
        if tonumber(mode.id) == tonumber(mode_id) then
            return tonumber(mode.mode_first) == 1
        end
    end

    return false
end

function ModeSelectConfig:canFragmentInfoShow(player, mode_id)
    local show = false
    for _, mode in pairs(ModeSelectConfig.mode) do
        if tonumber(mode.id) == tonumber(mode_id) then
            local map_info = StringUtil.split(mode.map, "#")
            if map_info then
                local all_has_get = true
                for i = 1, #map_info do
                    local map = ModeSelectMapConfig:getMapInfo(tonumber(map_info[i]))
                    if map and map.map_type == 2 then
                        show = true

                        if player.fragment_jindu and #player.fragment_jindu > 0 then
                            local has_record_this_map = false
                            for _, frag in pairs(player.fragment_jindu) do
                                if tostring(map.name) == tostring(frag.map_id) then
                                    has_record_this_map = true
                                    if frag.has_get == false then
                                        all_has_get = false
                                        break
                                    end
                                end
                            end
                            if has_record_this_map == false then
                                all_has_get = false
                            end
                        else
                            all_has_get = false
                        end
                    end
                end
                if all_has_get == true then
                    show = false
                end
            end
        end
    end
    return show
end

return ModeSelectConfig