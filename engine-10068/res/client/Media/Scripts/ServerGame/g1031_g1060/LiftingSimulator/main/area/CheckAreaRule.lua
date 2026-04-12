local NotAllowAttack = {
    [Define.Area.SafeArea] = true,
    [Define.Area.ShopArea] = true,
    [Define.Area.SellArea] = true,
    [Define.Area.AutoArea] = true,
    [Define.Area.AdvancedArea] = false,
    [Define.Area.BackHallArea] = false,
    [Define.Area.StartAdvancedArea] = false,
}

local NotAllowWorkout = {
    [Define.Area.SafeArea] = true,
    [Define.Area.ShopArea] = true,
    [Define.Area.AdvancedArea] = true,
    [Define.Area.BackHallArea] = true,
    [Define.Area.StartAdvancedArea] = true,
}

local NotAllowHurt = {
    [Define.Area.SafeArea] = true,
    [Define.Area.ShopArea] = true,
    [Define.Area.AutoArea] = true,
    [Define.Area.AdvancedArea] = true,
    [Define.Area.BackHallArea] = true,
    [Define.Area.StartAdvancedArea] = true,
}

local NotAllowTeleport = {}

CheckAreaRule = {}

function CheckAreaRule:CheckAreaRule(playerArea, needCheckRule)
    if needCheckRule == Define.AreaRule.NotAllowAttack then
        return self:isNotAllowAttack(playerArea)
    elseif needCheckRule == Define.AreaRule.NotAllowWorkout then
        return self:isNotAllowWorkout(playerArea)
    elseif needCheckRule == Define.AreaRule.NotAllowHurt then
        return self:isNotAllowHurt(playerArea)
    elseif needCheckRule == Define.AreaRule.NotAllowTeleport then
        return self:isNotAllowTeleport(playerArea)
    end
    return false
end

function CheckAreaRule:isNotAllowAttack(playerArea)
    return NotAllowAttack[playerArea] or false
end

function CheckAreaRule:isNotAllowWorkout(playerArea)
    return NotAllowWorkout[playerArea] or false
end

function CheckAreaRule:isNotAllowHurt(playerArea)
    return NotAllowHurt[playerArea] or false
end

function CheckAreaRule:isNotAllowTeleport(playerArea)
    --预留  在升阶区域时不允许传送
    return NotAllowTeleport[playerArea] or false
end