---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.ScoreConfig"
require "config.DeformConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.ROLE_PENDING = 0  -- 待定
GamePlayer.ROLE_SEEK = 1   -- 搜寻者
GamePlayer.ROLE_HIDE = 2 -- 躲藏者

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}

    self.hp = 0
    self.maxHp = 0
    self.score = 0
    self.gameScore = 0
    self.multiSeek = 0
    self.isInGame = false
    self.isLife = true
    self.role = GamePlayer.ROLE_PENDING
    self.config = {}
    self.model = {}
    self.lastModel = {}
    self.attackCache = {}
    self.changeModelTimes = 0

    self:reset()

end

function GamePlayer:setWeekRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerWeekRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:setDayRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerDayRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:teleInitPos()
    self.isInGame = false
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleScenePos()
    self.isInGame = true
    self:teleportPos(GameMatch:getCurGameScene().initPos)
end

function GamePlayer:teleRolePos()
    if self.role == GamePlayer.ROLE_SEEK then
        self:teleportPos(GameMatch:getCurGameScene():randomSeekPos())
    end
    if self.role == GamePlayer.ROLE_HIDE then
        self:teleportPos(GameMatch:getCurGameScene():randomHidePos())
    end
end

function GamePlayer:becomeRole(role)
    if self.isInGame then
        self.role = role
        MsgSender.sendMsgToTarget(self.rakssid, Messages:becomeRole(role))
        self.config = GameMatch:getCurGameScene():getConfig(self.role)
        self.model = GameMatch:getCurGameScene():randomModel()
        self.lastModel = self.model
        self.hp = self.config.hp
        self.maxHp = self.config.hp
        if role == GamePlayer.ROLE_SEEK then
            self.multiSeek = self.multiSeek + 1
            self:addEffect(PotionID.MOVE_SPEED, GameConfig.gameTime + GameConfig.changeActorTime, 1)
            HostApi.sendShowHideAndSeekBtnStatus(self.rakssid, false, false, false)
        end

        if role == GamePlayer.ROLE_HIDE then
            self.multiSeek = 0
            self:changeActor()
            HostApi.changePlayerPerspece(self.rakssid, 1)
            HostApi.sendShowHideAndSeekBtnStatus(self.rakssid, true, true, false)
            MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:becomeHideHint())
        end

        self:showGameData()
        return true
    end
    return false
end

function GamePlayer:changeActor()
    self.lastModel = self.model
    self.model = GameMatch:getCurGameScene():randomModel(self.model)
    EngineWorld:changePlayerActor(self, self.model.actor)
end

function GamePlayer:restoreActor()
    EngineWorld:restorePlayerActor(self)
end

function GamePlayer:death()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setEntityHealth(0)
    end
end

function GamePlayer:sendBuildShowName(players)
    local name = self:buildShowName()
    for i, player in pairs(players) do
        if player.role == self.role then
            self:setShowName(name, player.rakssid)
        else
            self:setShowName(" ", player.rakssid)
        end
    end
end

function GamePlayer:reset(report)
    self.role = GamePlayer.ROLE_PENDING
    self.isLife = true
    self.isSingleReward = false
    self.isReport = false
    self.hp = 0
    self.maxHp = 0
    self.changeModelTimes = 0
    self.score = 0
    self.attackCache = {}
    self.model = {}
    self.lastModel = {}
    self:teleInitPos()
    self:clearInv()
    self:resetFood()
    self:resetHP()
    self:initScorePoint()
    self:showGameData()
    self:restoreActor()
    self:setShowName(self:buildShowName())
    self:removeEffect(PotionID.MOVE_SPEED)
    HostApi.changePlayerPerspece(self.rakssid, 0)
    if report ~= false then
        ReportManager:reportUserEnter(self.userId)
    end
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

function GamePlayer:resetHP()
    self:setHealth(20)
end

function GamePlayer:getRoleName()
    if self.config == nil then
        return ""
    end
    if self.role == GamePlayer.ROLE_HIDE then
        return TextFormat.colorGreen .. "[" .. self.config.name .. "]"
    elseif self.role == GamePlayer.ROLE_SEEK then
        return TextFormat.colorRed .. "[" .. self.config.name .. "]"
    else
        return ""
    end
end

function GamePlayer:changeModel()
    local price = DeformConfig:getDeformPriceByTimes(self.changeModelTimes + 1)
    if price == nil then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:changeActorByNoTimes(self.changeModelTimes))
        return
    end
    if self.changeModelTimes == 0 then
        self.changeModelTimes = self.changeModelTimes + 1
        self:changeActor()
    else
        self.clientPeer:buyChangeActor(false, self.changeModelTimes + 1, price.blockymodsPrice)
    end
end

function GamePlayer:showChangeModel()
    if self.role ~= GamePlayer.ROLE_HIDE then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:changeActorByNoHide())
        return
    end
    local price = DeformConfig:getDeformPriceByTimes(self.changeModelTimes + 1)
    if price == nil then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:changeActorByNoTimes(self.changeModelTimes))
        return
    end
    if self.changeModelTimes == 0 then
        self:changeActor()
        self.changeModelTimes = self.changeModelTimes + 1
    else
        self.entityPlayerMP:setChangePlayerActor(false, self.changeModelTimes + 1, price.blockymodsPrice, self.model.actor)
    end
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    local roleName = self:getRoleName()
    if #roleName ~= 0 then
        table.insert(nameList, roleName)
    end

    -- pureName line
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.LIVE] = 0
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:onLive(ticks)
    if self.role == GamePlayer.ROLE_HIDE then
        if ticks % 30 == 0 and self.isLife then
            self.score = self.score + ScoreConfig.LIVE
            self.gameScore = self.gameScore + ScoreConfig.LIVE
            self:addAppIntegral(ScoreConfig.LIVE)
        end
        if self.isLife then
            self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
        end
    end
end

function GamePlayer:onHurted()
    if self.hp > 0 then
        self.hp = self.hp - 1
        self:showGameData()
    end
end

function GamePlayer:onAttack()
    if self.hp < self.maxHp then
        self.hp = self.hp + 1
        self:showGameData()
    end
end

function GamePlayer:showGameData()
    if self.hp == 0 then
        self:changeHeart(0, 0)
    else
        self:changeHeart(self.hp, self.maxHp)
    end
end

function GamePlayer:getModelName()
    return self.model.name
end

function GamePlayer:onUseHeart()
    if self.hp < self.maxHp then
        self.hp = self.hp + 1
        self:showGameData()
    end
end

function GamePlayer:onUseShoes()
    self:addEffect(PotionID.MOVE_SPEED, 10, 3)
end

function GamePlayer:onUseArrow()
    local player = self:getNearestHidePlayer()
    if player then
        local builder = DataBuilder.new()
        builder:addVector3Param("startPos", self:getPosition())
        builder:addVector3Param("endPos", player:getPosition())
        builder:addParam("duration", 5)
        PacketSender:sendLuaCommonData(self.rakssid, "UseArrowTip", builder:getData())
    end
end

function GamePlayer:onUseQuestion()
    local player = GameMatch:randomLifeHidePlayer()
    if player then
        MsgSender.sendMsgToTarget(self.rakssid, Messages:msgLastModelName(player:getModelName()))
    end
end

function GamePlayer:getNearestHidePlayer()
    local nearsetP
    local distance = 999999999
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player ~= self and player.role == GamePlayer.ROLE_HIDE and player.isLife then
            local pos1 = player:getPosition()
            local pos2 = self:getPosition()
            local d = (pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z)
            if d < distance then
                nearsetP = player
                distance = d
            end
        end
    end
    return nearsetP
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN
    self.gameScore = self.gameScore + ScoreConfig.WIN
    self:addAppIntegral(ScoreConfig.WIN)
end

function GamePlayer:onKill()
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.score = self.score + ScoreConfig.KILL
    self.gameScore = self.gameScore + ScoreConfig.KILL
    self:addAppIntegral(ScoreConfig.KILL)
end

function GamePlayer:addAppIntegral(integral)
    if self.role == GamePlayer.ROLE_HIDE then
        self.appIntegral = self.appIntegral + integral
    elseif self.role == GamePlayer.ROLE_SEEK then
        self.appIntegral = self.appIntegral + integral * 5
    end
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.hp = 0
        self:clearInv()
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
    settlement.name = self.name
    settlement.isWin = 0
    settlement.points = self.scorePoints
    settlement.gold = self.gold
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), false)
end

function GamePlayer:onGameEnd(win)
    if self.role ~= GamePlayer.ROLE_PENDING then
        self:reward(win, GameMatch:getPlayerRank(self), true)
    end
end

function GamePlayer:setCameraLocked(isLocked)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setCameraLocked(isLocked)
    end
end

function GamePlayer:changeInvisible(rakssid, invisible)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeInvisible(rakssid, invisible)
    end
end

function GamePlayer:reward(isWin, rank, isEnd)

    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin, 2)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    RankNpcConfig:savePlayerRankScore(self)
    ReportManager:reportUserData(self.userId, self.scorePoints[ScoreID.KILL], GameMatch:getPlayerRank(self), 1)
    if isEnd then
        return
    end
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch:isGameOver() == false then
            self:sendPlayerSettlement()
        end
    end)
    self.isReport = true
    self.isSingleReward = true
end

return GamePlayer

--endregion


