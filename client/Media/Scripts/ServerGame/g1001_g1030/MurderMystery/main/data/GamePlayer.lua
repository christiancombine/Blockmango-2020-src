--GamePlayer.lua
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.ROLE_PENDING = 0  -- 待定
GamePlayer.ROLE_MURDERER = 1   -- 杀手
GamePlayer.ROLE_SHERIFF = 2 -- 警察
GamePlayer.ROLE_CIVILIAN = 3 -- 平民
GamePlayer.ROLE_DEAD = 4 -- 死者

GamePlayer.BECOME_MURDERER_ODDS_GROWTH = 4

function GamePlayer:init()

    self:initStaticAttr(0)

    self.kills = 0
    self.hp = 0
    self.isInGame = false
    self.onHandItemId = 0
    self.murdererOddsAdd = 0
    self.score = 0
    self.isLife = true
    self.isSingleReward = false

    self:reset()

end

function GamePlayer:becomeRole(role)
    if self.isInGame then
        self.role = role
        self.config = GameMatch:getCurGameScene():getRole(self.role)
        self.hp = self.config.hp
        self.maxHp = self.hp
        self:giveProp()
        self:showGameData()
        GameMatch:getCurGameScene():getStore():prepareStore(self)
        MsgSender.sendMsgToTarget(self.rakssid, Messages:becomeRole(role))
        return true
    end
    return false
end

function GamePlayer:giveProp()
    local prop = self.config.prop
    if prop ~= nil then
        for i, v in pairs(prop) do
            if v.id ~= 0 then
                if v.fm.id == 0 then
                    self:addItem(v.id, v.num, 0)
                else
                    self:addEnchantmentItem(v.id, v.num, 0, v.fm.id, v.fm.lv)
                end
            end
        end
    end
end

function GamePlayer:reset(report)
    self.role = GamePlayer.ROLE_PENDING
    self.isLife = true
    self.hp = 0
    self:teleInitPos()
    self:clearInv()
    self:resetHP()
    self:initScorePoint()
    self:resetItemIndex()
    self:showGameData()
    if report ~= false then
        ReportManager:reportUserEnter(self.userId)
    end
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.LIVE] = 0
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:resetItemIndex()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeCurrentItem(4)
    end
end

function GamePlayer:resetMode()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setWatchMode(false)
    end
end

function GamePlayer:resetHP()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeMaxHealth(20)
    end
end

function GamePlayer:initItems()
    for i, v in pairs(GameConfig.initItems) do
        self:addItem(v.id, v.num, 0)
    end
end

function GamePlayer:teleInitPos()
    HostApi.resetPos(self.rakssid, GameConfig.initPos.x, GameConfig.initPos.y + 0.5, GameConfig.initPos.z)
    self:setShowName(self:buildShowName())
    self.isInGame = false
end

function GamePlayer:teleRandomPos()
    if self.isLife then
        local pos = GameMatch:getCurGameScene():getValidTeleportPos()
        if pos ~= nil then
            HostApi.resetPos(self.rakssid, pos.x, pos.y + 0.5, pos.z)
        end
        self:setShowName(" ")
        self.isInGame = true
    end
end

function GamePlayer:showGameData()
    if self.hp <= 0 then
        self:changeHeart(0, 0)
    else
        self:changeHeart(self.hp, self.maxHp)
    end
end

function GamePlayer:getArmsHurt()
    return GameMatch:getCurGameScene():getArmsHurt(self.onHandItemId)
end

function GamePlayer:death()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setEntityHealth(0)
    end
end

function GamePlayer:onLive(ticks)
    if self.role == GamePlayer.ROLE_CIVILIAN or self.role == GamePlayer.ROLE_SHERIFF then
        if ticks % 10 == 0 and self.isLife then
            self.score = self.score + ScoreConfig.SURVIVAL
        end
        self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
    end
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN
    self.appIntegral = self.appIntegral + 10
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.role = GamePlayer.ROLE_DEAD
        self.hp = 0
        self:clearInv()
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
        self:showGameData()
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
    if self.role ~= GamePlayer.ROLE_DEAD and self.role ~= GamePlayer.ROLE_PENDING then
        self:reward(win, GameMatch:getPlayerRank(self), true)
    end
end

function GamePlayer:reward(isWin, rank, isEnd)

    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin, 3)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end
    ReportManager:reportUserData(self.userId, self.scorePoints[ScoreID.KILL], GameMatch:getPlayerRank(self), 1)
    if isEnd then
        return
    end
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch:isGameOver() == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end


return GamePlayer

--endregion
