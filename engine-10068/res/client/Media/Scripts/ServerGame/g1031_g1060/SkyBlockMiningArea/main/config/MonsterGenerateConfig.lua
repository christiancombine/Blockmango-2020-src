MonsterGenerateConfig = {}
MonsterGenerateConfig.monsters = {}

function MonsterGenerateConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.monsterPos = StringUtil.split(v.pos, ",")
        data.mosterId = tonumber(v.mosterId)
        data.refreshCD = tonumber(v.refreshCD)
        self.monsters[data.id] = data
    end
end

function MonsterGenerateConfig:getMonsterInfoById(id)
    if self.monsters[tonumber(id)] ~= nil then
        return self.monsters[tonumber(id)]
    end
    return nil
end

return MonsterGenerateConfig