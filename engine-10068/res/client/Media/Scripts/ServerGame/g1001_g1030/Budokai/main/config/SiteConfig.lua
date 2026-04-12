---
--- Created by Jimmy.
--- DateTime: 2018/7/17 0017 14:21
---
require "data.GameSite"

SiteConfig = {}
SiteConfig.sites = {}

function SiteConfig:init(sites)
    self:initSites(sites)
end

function SiteConfig:initSites(sites)
    for _, site in pairs(sites) do
        self.sites[site.id] = GameSite.new(site)
    end
end

function SiteConfig:getSitesByType(type)
    local sites = {}
    for _, site in pairs(self.sites) do
        if site.type == type then
            sites[#sites + 1] = site
        end
    end
    return sites
end

function SiteConfig:onTick(tick)
    for _, site in pairs(self.sites) do
        site:onTick(tick)
    end
end

function SiteConfig:onGameStart()
    for _, site in pairs(self.sites) do
        site:onGameStart()
    end
end

function SiteConfig:onGameEnd()
    for _, site in pairs(self.sites) do
        site:onGameEnd()
    end
end

function SiteConfig:onGameReset()
    for _, site in pairs(self.sites) do
        site:onGameReset()
    end
end

function SiteConfig:onPlayerDie(player)
    for _, site in pairs(self.sites) do
        site:onPlayerDie(player)
    end
end

function SiteConfig:onPlayerQuit(player)
    for _, site in pairs(self.sites) do
        site:onPlayerQuit(player)
    end
end

function SiteConfig:ifRoundEnd()
    local isEnd = true
    for _, site in pairs(self.sites) do
        if site:ifGameEnd() == false then
            isEnd = false
        end
    end
    return isEnd
end

return SiteConfig