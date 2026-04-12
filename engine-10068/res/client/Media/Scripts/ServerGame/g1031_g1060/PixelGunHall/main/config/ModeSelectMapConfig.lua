ModeSelectMapConfig = {}
ModeSelectMapConfig.map = {}
ModeSelectMapConfig.OpenTypeInvalid = 0
ModeSelectMapConfig.OpenTypeLv = 1
ModeSelectMapConfig.OpenTypeDiamond = 2
ModeSelectMapConfig.OpenTypeNotOpen = 3

function ModeSelectMapConfig:initConfig(maps)
    for i, map in pairs(maps) do
        local data = {}
        data.id = tonumber(map.id)
        data.img = tostring(map.img) or ""
        data.name = tostring(map.name) or ""
        data.show_name = tostring(map.show_name) or ""
        data.small_info = tostring(map.small_info) or ""
        if data.small_info == "#" then
            data.small_info = ""
        end
        data.mapSize = tostring(map.mapSize) or ""
        data.openMoneyType = tonumber(map.openMoneyType) or 0
        data.openMoneyNum = tonumber(map.openMoneyNum) or 0
        data.map_type = tonumber(map.map_type) or 1
        data.fragment_gun_img = tostring(map.fragment_gun_img) or ""
        if data.fragment_gun_img == "#" then
            data.fragment_gun_img = ""
        end 
        data.fragment_gun_num_txt = tostring(map.fragment_gun_num_txt) or ""
        if data.fragment_gun_num_txt == "#" then
            data.fragment_gun_num_txt = ""
        end
        data.fragment_gun_id = tonumber(map.fragment_gun_id)
        data.fragment_gun_num = tonumber(map.fragment_gun_num)

        data.random_reward_img1 = tostring(map.random_reward_img1) or ""
        if data.random_reward_img1 == "#" then
            data.random_reward_img1 = ""
        end

        data.random_reward_img2 = tostring(map.random_reward_img2) or ""
        if data.random_reward_img2 == "#" then
            data.random_reward_img2 = ""
        end

        data.random_reward_num1 = tonumber(map.random_reward_num1) or 0
        data.random_reward_num2 = tonumber(map.random_reward_num2) or 0

        table.insert(self.map, data)
    end
    table.sort(self.map, function(a, b)
        return a.id < b.id
    end)
end

function ModeSelectMapConfig:getMapInfo(map_id)
    for _, map in pairs(self.map) do
        if tonumber(map.id) == tonumber(map_id) then
            return map
        end
    end
    return nil
end

function ModeSelectMapConfig:getMapInfoByMapName(mapName)
    for _, map in pairs(self.map) do
        if map.name == mapName then
            return map
        end
    end
    return nil
end

function ModeSelectMapConfig:getFragment(map_name)
    for _, map in pairs(self.map) do
        if tostring(map.name) == tostring(map_name) then
            return map.fragment_gun_id, map.fragment_gun_num
        end
    end
    return 0, 0
end



return ModeSelectMapConfig