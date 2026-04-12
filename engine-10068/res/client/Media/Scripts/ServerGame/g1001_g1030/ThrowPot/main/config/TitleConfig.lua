---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 10:53
---
require "config.SkillConfig"

TitleConfig = {}
TitleConfig.titles = {}

function TitleConfig:init(titles)
    self:initTitles(titles)
end

function TitleConfig:initTitles(titles)
    for id, title in pairs(titles) do
        local item = {}
        item.id = title.id
        item.color = title.color
        item.title = title.title
        item.name = title.name
        item.image = title.image
        item.desc = title.desc
        self.titles[tonumber(id)] = item
    end
end

function TitleConfig:getTitles()
    return self.titles
end

function TitleConfig:getTitle(id)
    return self.titles[tonumber(id)]
end

function TitleConfig:canBuyTitle(id, player)
    if id == "1" then
        return self:ifTiro(player)
    end
    if id == "2" then
        return self:ifGenius(player)
    end
    if id == "3" then
        return self:ifUnstoppable(player)
    end
    if id == "4" then
        return self:ifKingOfBattles(player)
    end
    if id == "5" then
        return self:ifKillThousands(player)
    end
    if id == "6" then
        return self:ifFullTimesPerson(player)
    end
    return false
end

function TitleConfig:ifTiro(player)
    return player.games >= 3
end

function TitleConfig:ifGenius(player)
    return player.hasWin
end

function TitleConfig:ifUnstoppable(player)
    return player.games >= 50
end

function TitleConfig:ifKingOfBattles(player)
    return player.games >= 100
end

function TitleConfig:ifKillThousands(player)
    return player.integral >= 1000
end

function TitleConfig:ifFullTimesPerson(player)
    local skills = SkillConfig:getSkills()
    for id, skill in pairs(skills) do
        if player.skills[skill.id] == nil then
            return false
        end
    end
    return true
end

return TitleConfig