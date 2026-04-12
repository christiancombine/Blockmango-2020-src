---
--- Created by Jimmy.
--- DateTime: 2018/8/7 0007 10:11
---
TalentConfig = {}
TalentConfig.ATTACK = "1"
TalentConfig.DEFENSE = "2"
TalentConfig.HEALTH = "3"

TalentConfig.talents = {}

function TalentConfig:init(talents)
    self:initTalents(talents)
end

function TalentConfig:initTalents(talents)
    for _, talent in pairs(talents) do
        local item = {}
        item.id = talent.id
        item.type = talent.type
        item.level = tonumber(talent.level)
        item.name = talent.name
        item.desc = talent.desc
        item.image = talent.image
        item.value = tonumber(talent.value)
        item.nextValue = tonumber(talent.nextValue)
        item.price = tonumber(talent.price)
        self.talents[#self.talents + 1] = item
    end
end

function TalentConfig:getTalent(id)
    for _, talent in pairs(self.talents) do
        if talent.id == id then
            return talent
        end
    end
    return nil
end

function TalentConfig:getTalentByParams(type, level)
    for _, talent in pairs(self.talents) do
        if talent.type == type and talent.level == level then
            return talent
        end
    end
    return nil
end

return TalentConfig