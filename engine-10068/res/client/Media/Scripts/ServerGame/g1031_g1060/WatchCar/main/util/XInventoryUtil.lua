XInventoryUtil = {}

-- 获取某个玩家背包里所有的物品
function XInventoryUtil:getAllItems(player)
    if player == nil then
        return nil
    end
    local inv = player:getInventory()
    if inv == nil then
        return nil
    end
    local tab = {}
    for index = 0, 40 do
        local info = inv:getItemStackInfo(index - 1)
        if info.id > 0 and info.num > 0 then
            tab[info.id] = info.num
        end
    end
    return tab
end