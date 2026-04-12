---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 10:53
---

TitleConfig = {}
TitleConfig.titles = {}

function TitleConfig:init(titles)
    self:initTitles(titles)
end

function TitleConfig:initTitles(titles)
    for id, title in pairs(titles) do
        local item = {}
        item.index = tonumber(id)
        item.id = title.id
        item.type = tonumber(title.type)
        item.color = title.color
        item.title = title.title
        item.name = title.name
        item.image = title.image
        item.desc = title.desc
        item.price = tonumber(title.price)
        self.titles[#self.titles + 1] = item
    end
    table.sort(self.titles, function(a, b)
        return a.index < b.index
    end)
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
        return self:ifNo1OnTheWorld(player)
    end
    return true
end

function TitleConfig:ifTiro(player)
    return player.games >= 3
end

function TitleConfig:ifGenius(player)
    return player.wingames >= 1
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

function TitleConfig:ifNo1OnTheWorld(player)
    return player.wingames >= 10
end

return TitleConfig