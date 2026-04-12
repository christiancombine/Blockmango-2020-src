---
--- Created by Jimmy.
--- DateTime: 2018/9/13 0013 16:10
---

GameModeNpc = class()

function GameModeNpc:ctor(config)
    self.entityId = 0
    self.id = config.id
    self.name = config.name or ""
    self.actor = config.actor
    self.initPos = VectorUtil.newVector3(config.x, config.y, config.z)
    self.yaw = tonumber(config.yaw)
    self.gameType = config.gameType
    self.hint = config.hint
    self.campName1 = config.campName1
    self.campName2 = config.campName2
    self.actorName1 = config.actorName1
    self.actorName2 = config.actorName2
    self.players = {}
    self:onCreate()
end

function GameModeNpc:onCreate()
    local entityId = EngineWorld:addActorNpc(self.initPos, self.yaw, self.actor, function(entity)
        entity:setHeadName(self.name)
        entity:setSkillName("idle")
        entity:setHaloEffectName("flag_halo_2.effect")
    end)
    self.entityId = entityId
    GameManager:onAddModeNpc(self)
end

function GameModeNpc:onTick(tick)
    self:checkPlayers()
    self:tryPlayerMatching()
end

function GameModeNpc:checkPlayers()
    for userId, player in pairs(self.players) do
        if PlayerManager:getPlayerByRakssid(player.rakssid) == nil then
            self.players[userId] = nil
        end
    end
end

function GameModeNpc:tryPlayerMatching()
    for userId, player in pairs(self.players) do
        if os.time() - player.time > 10 then
            player.time = os.time()
            local c_player = PlayerManager:getPlayerByRakssid(player.rakssid)
            if c_player ~= nil and c_player.gameType ~= nil then
                EngineUtil.sendEnterOtherGame(c_player.rakssid, c_player.gameType, c_player.userId)
            end
        end
    end
end

function GameModeNpc:onPlayerHit(player)
    HostApi.sendCustomTip(player.rakssid, self.hint, "GameMode=" .. tostring(self.entityId))
end

function GameModeNpc:onPlayerSelected(player)
    local data = {}
    data.type = 1
    data.forceOpen = false
    local actorName = self:getPlayerActor(player, "2")
    data.police = {
        name = self.campName1,
        title = self.campName1,
        model = actorName
    }
    actorName = self:getPlayerActor(player, "1")
    data.criminal = {
        name = self.campName2,
        title = self.campName2,
        model = actorName
    }
    player.gameType = self.gameType
    HostApi.sendSelectRoleData(player.rakssid, json.encode(data))
end

function GameModeNpc:onPlayerSelectRole(player)
    EngineUtil.sendEnterOtherGame(player.rakssid, player.gameType, player.userId)
    self.players[tostring(player.userId)] = { rakssid = player.rakssid, time = os.time() }
    MsgSender.sendBottomTipsToTarget(player.rakssid, 3600, Messages:msgWaitMatching())
end

function GameModeNpc:getPlayerActor(player, type)
    for clothingId, v in pairs(player.specialClothing) do
        if v == type then
            local actorName = SpecialClothingConfig:getActorByClothingId(clothingId)
            return actorName
        end
    end
    if type == "1" then
        return self.actorName1
    elseif type == "2" then
        return self.actorName2
    end
    return self.actorName1
end

return GameModeNpc