ExpConfig = {}
ExpConfig.exp = {}

function ExpConfig:initConfig(exps)
    for i, exp in pairs(exps) do
        local data = {}
        data.exp_lv = tonumber(exp.exp_lv) or 0
        data.exp_max = exp.exp_max or ""
        table.insert(self.exp, data)
    end
    table.sort(self.exp, function(a, b)
        return a.exp_lv < b.exp_lv
    end)
end

function ExpConfig:getMaxExp(exp_lv)
    for _, exp in pairs(self.exp) do
        if tonumber(exp.exp_lv) == tonumber(exp_lv) then
            if tostring(exp.exp_max) == "###" then
                return 1, true
            else
                return tonumber(exp.exp_max), false
            end
        end
    end
    return 1, false
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



return ExpConfig