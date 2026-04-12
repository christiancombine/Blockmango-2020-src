RechargeUtil = {}

function RechargeUtil:doSumRechargeRewards(player, type, id, count, meta)
    if player == nil then
        return
    end

    id = tonumber(id)
    if type == "Money" then
        --游戏币
        player:addCurrency(count)
    elseif type == "Item" then
        --物品
        player:addItem(id, count, meta)
    end
end