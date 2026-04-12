---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:44
---
require "config.GunMerchantConfig"
require "config.GameConfig"

GameManager = {}
GameManager.gunMerchants = {}

function GameManager:onTick(ticks)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick(ticks)
    end
end

function GameManager:getNpcByEntityId(entityId)
    return nil
end

function GameManager:onAddGunMerchant(merchant)
    self.gunMerchants[tostring(merchant.entityId)] = merchant
end

function GameManager:prepareGame()
    self:preparePlayers()
end

function GameManager:preparePlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onPrepare()
    end
end

function GameManager:onPlayerInGame(player)
    for _, merchant in pairs(self.gunMerchants) do
        merchant:onPlayerInGame(player)
    end
end

function GameManager:updateTeamKills()
    local data = {}
    for _, team in pairs(GameMatch.Teams) do
        local index = #data + 1
        data[index] = {
            teamId = team.id,
            value = team.kills,
            maxValue = GameConfig.winKill
        }
    end
    HostApi.updateTeamResources(json.encode(data))
end

return GameManager