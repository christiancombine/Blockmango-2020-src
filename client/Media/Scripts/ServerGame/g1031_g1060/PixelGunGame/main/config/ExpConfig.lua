ExpConfig = {}
ExpConfig.exp = {}

function ExpConfig:initConfig(exps)
    for i, exp in pairs(exps) do
        local data = {}
        data.exp_lv = tonumber(exp.exp_lv) or 0
        data.exp_max = exp.exp_max or ""
        data.money = tonumber(exp.money) or 0
        data.yaoshi = tonumber(exp.yaoshi) or 0
        data.gun_id = tonumber(exp.gun_id) or 0
        data.frag_num = tonumber(exp.frag_num) or 0
        data.frag_img = tostring(exp.frag_img) or ""
        if data.frag_img == "#" then
            data.frag_img = ""
        end
        data.max_hp = tonumber(exp.max_hp) or 1
        
        table.insert(self.exp, data)
    end
    table.sort(self.exp, function(a, b)
        return a.exp_lv < b.exp_lv
    end)
end

function ExpConfig:getPlayerInitLv(level)
    local max_lv = 1
    for _, exp in pairs(self.exp) do
        if max_lv < exp.exp_lv then
            max_lv = exp.exp_lv
        end
    end
    if level > max_lv then
        return max_lv
    end
    return math.max(level, 1)
end

function ExpConfig:getMaxExp(exp_lv)
    for _, exp in pairs(self.exp) do
        if tonumber(exp.exp_lv) == tonumber(exp_lv) then
            if tostring(exp.exp_max) == "###" then
                return 10000, true
            else
                return tonumber(exp.exp_max), false
            end
        end
    end
    return 10000, true
end

function ExpConfig:getLevelReward(level)
    for _, exp in pairs(self.exp) do
        if tonumber(exp.exp_lv) == tonumber(level) then
            return exp.money, exp.yaoshi, exp.gun_id, exp.frag_num
        end
    end

    return 0, 0, 0, 0
end

function ExpConfig:getLevelFragImg(level)
    for _, exp in pairs(self.exp) do
        if tonumber(exp.exp_lv) == tonumber(level) then
            return tostring(exp.frag_img)
        end
    end
    return ""
end

function ExpConfig:getMaxHp(level)
    for _, exp in pairs(self.exp) do
        if tonumber(exp.exp_lv) == tonumber(level) then
            return tonumber(exp.max_hp) or 1
        end
    end
    return 1
end

function ExpConfig:getDiffhp(level)
    local preLevel = level - 1
    local curHp = 0
    local preHp = 0
    for _, exp in pairs(self.exp) do
        if tonumber(level) == tonumber(exp.exp_lv) then
            curHp = exp.max_hp
        end 
        if tonumber(preLevel) == tonumber(exp.exp_lv) then
            preHp = exp.max_hp
        end 
    end

    HostApi.log("getDiffhp")

    if curHp > 0 and preHp > 0 then
        return curHp - preHp
    end
    return 0
end

return ExpConfig