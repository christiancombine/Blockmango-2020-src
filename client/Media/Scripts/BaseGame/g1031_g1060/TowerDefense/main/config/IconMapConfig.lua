IconMapConfig = {}

--星级背景图片映射表
local StarsBackground = {
    [1] = "set:tower_defense_hall_shop.json image:white_bg",
    [2] = "set:tower_defense_hall_shop.json image:green_bg",
    [3] = "set:tower_defense_hall_shop.json image:blue_bg",
    [4] = "set:tower_defense_hall_shop.json image:purple_bg",
    [5] = "set:tower_defense_hall_shop.json image:gold_bg",
}

--攻击类型图标图片映射表
local AttackTypeIcon = {
    [1] = "set:tower_defense_attacktype.json image:monomer_icon",
    [2] = "set:tower_defense_attacktype.json image:Multiple_icon",
    [3] = "set:tower_defense_attacktype.json image:catapult_icon",
}

--玩家排名背景图片映射表
local RankBackground = {
    [1] = "set:tower_defense_main.json image:settlement_no1",
    [2] = "set:tower_defense_main.json image:settlement_no2",
    [3] = "set:tower_defense_main.json image:settlement_no3",
    [4] = "set:tower_defense_main.json image:settlement_no4",
}


function IconMapConfig:getStarsBackground(stars)
    return StarsBackground[stars] or ""
end

function IconMapConfig:getAttackTypeIcon(type)
    return AttackTypeIcon[type] or ""
end

function IconMapConfig:getRankBackground(rank)
    return RankBackground[rank] or ""
end
