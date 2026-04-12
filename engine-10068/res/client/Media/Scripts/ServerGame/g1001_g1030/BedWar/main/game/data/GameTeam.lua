---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 15:36
---

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.bedPos = {}
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.isHaveBed = true
GameTeam.storeId = 0
GameTeam.clothBlockId = BlockID.CLOTH

function GameTeam:ctor(config, color)
    self.id = config.id
    self.name = config.name
    self.initPos = config.initPos
    self.bedPos = config.bedPos
    self.resourcePointId = config.resourcePointId
    self.color = color
    self.storeLv = 1
    self.initItems = config.initItems
    self.clothBlockId = config.clothBlockId
    self:initHPArea()

    GameInfoManager:newTeamInfo(tostring(config.id))
    self.info = GameInfoManager:getTeamInfo(config.id)
    GameResourceManager:newTeamResource(config.resourcePointId)
end

function GameTeam:initHPArea()
    local startPos = HotSpringConfig:getStartPos(self.id)
    local endPos = HotSpringConfig:getEndPos(self.id)
    self.hpArea = IArea.new()
    self.hpArea:initByPos(startPos, endPos)
end

function GameTeam:isInHpArea(player)
    local pos = player:getPosition()
    return self.hpArea:inArea(pos)
end

function GameTeam:upgradeIron()
    GameResourceManager:UpgradeResource(self.resourcePointId)
end

function GameTeam:addPlayer(userId)
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:subPlayer(userId)
    if self.curPlayers > 0 then
        self.curPlayers = self.curPlayers - 1
    end
    if GameMatch.hasStartGame then
        if self:getCurPlayerCount() <= 0 then
            self.isHaveBed = false
            local players = PlayerManager:getPlayers()
            for _, _player in pairs(players) do
                MessagesManage:sendSystemTipsToPlayer(_player.rakssid, Messages:msgTeamFail(self:getDisplayName(_player:getLanguage())))
            end
        end
    end
end

function GameTeam:isDeath()
    return self:getCurPlayerCount() <= 0
end

function GameTeam:getCurPlayerCount()
    local count = 0
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.teamId == self.id and player.isLife then
            count = count + 1
        end
    end
    self.info:setValue("liveCount", count)
    return count
end

function GameTeam:destroyBed(destroyer)
    if AIManager:isAI(destroyer.rakssid) == false then
        ---通用上报
        ReportUtil.reportBedDamage(destroyer)

        if AIManager:getAIsByTeamId(self.id) ~= nil then
            ---玩家破坏AI床
            ReportUtil.reportPlayerDamageAIBed(destroyer, destroyer.honor)
        end
    else
        if #AIManager:getAIsByTeamId(self.id) ~= 0 then
            ---AI破坏AI床
            ReportUtil.reportAIDamageBed(true)
        else
            ---AI破坏玩家床
            ReportUtil.reportAIDamageBed(false)
        end
    end

    self.isHaveBed = false
    self.info:setValue("isHaveBed", "0")
    HostApi.sendBedDestroy(self.id)
end

function GameTeam:getDisplayName(lang)
    return self.color .. "[" .. MultiLanTipSetting.getMessageToLua(lang, tonumber(self.name), "") .. "]"
end

function GameTeam:getTeamColor()
    return self.color
end
