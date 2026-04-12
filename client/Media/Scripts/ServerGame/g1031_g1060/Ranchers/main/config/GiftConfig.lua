GiftConfig = {}
GiftConfig.gift = {}

function GiftConfig:init(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.type = tonumber(v.type)
        item.rewards = StringUtil.split(v.rewards, "#")
        self.gift[item.id] = item
    end
end

function GiftConfig:getGiftIdByType(type)
    local items = {}
    local index = 1
    for i, v in pairs(self.gift) do
        if v.type == type then
            items[index] = v
            index = index + 1
        end
    end

    if index == 1 then
        HostApi.log("==== LuaLog: GiftConfig:getGiftIdByType can not find type[".. type .."] in gift.csv")
        return nil
    end

    local randNum = math.random(1, index - 1)
    return items[randNum].id
end

function GiftConfig:getRewardById(id)
    for i, v in pairs(self.gift) do
        if v.id == id then
            local rewards = {}
            for j, reward in pairs(v.rewards) do
                local item = StringUtil.split(reward, ":")
                if item[2] ~= 0 then
                    rewards[j] = {}
                    rewards[j].itemId = tonumber(item[1])
                    rewards[j].num = tonumber(item[2])
                end
            end

            return rewards
        end
    end

    return nil
end


return GiftConfig