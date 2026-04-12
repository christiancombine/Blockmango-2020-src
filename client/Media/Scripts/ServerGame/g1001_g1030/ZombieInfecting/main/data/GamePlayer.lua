--GamePlayer.lua
require "config.HunterConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.ROLE_PENDING = 0  -- 待定
GamePlayer.ROLE_SOLDIER = 1 -- 士兵
GamePlayer.ROLE_ZOMBIE = 2   -- 僵尸
GamePlayer.ROLE_DEAD = 3 -- 死者

GamePlayer.SKILL_TYPE_NONE = 0
GamePlayer.SKILL_TYPE_DEFENSE = 1
GamePlayer.SKILL_TYPE_SPRINT = 2
GamePlayer.SKILL_TYPE_RELEASE_TOXIC = 3

function GamePlayer:init()

    self:initStaticAttr(0)

    self.isInGame = false
    self.multiKill = 0
    self.lastKillTime = 0
    self.kills = 0
    self.score = 0
    self.isLife = true
    self.multiZombie = 0    -- times of become zombie at the game begin
    self.zombieId = 0
    self.lastPos = nil
    self.standTime = 0
    self.isChangeZombie = false
    self.isSingleReward = false
    self.isHunter = false
    self.skillDurationTime = 0
    self.defenseCoefficient = 1
    self.isDefenseSkill = false
    self.defenseStartTime = os.time()
    self.isSprintSkill = false
    self.sprintStartTime = os.time()
    self.sprintLastBuff = {}
    self.isPoisonAttack = false
    self.poisonDuration = 0
    self.poisonDamage = 0
    self.poisonAttackOnTime = 0
    self.poisonAttackStartTime = os.time()
    self.poisonLastAttackTime = os.time()
    self.poisonZombieUserId = 0
    self.role = GamePlayer.ROLE_PENDING
    self:reset()

end

function GamePlayer:becomeRole(role)
    if self.isInGame then
        self:setRoleAttribute(role)
        MsgSender.sendMsgToTarget(self.rakssid, Messages:becomeRole(role))
        MsgSender.sendMsgToTarget(self.rakssid, Messages:roleSelected(role))
        return true
    end
    return false
end

function GamePlayer:onDieByPlayer()
    self.score = self.score + ScoreConfig.DIE_SCORE
    self.scorePoints[ScoreID.DIE] = self.scorePoints[ScoreID.DIE] + 1
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1

    if self.multiKill == 0 then
        self.multiKill = 1
    else
        if os.time() - self.lastKillTime <= 30 then
            self.multiKill = self.multiKill + 1
            self.scorePoints[ScoreID.SERIAL_KILL] = self.scorePoints[ScoreID.SERIAL_KILL] + 1
        else
            self.multiKill = 1
        end
    end

    self.lastKillTime = os.time()

    if self.multiKill >= 5 then
        self.score = self.score + ScoreConfig.KILL_5_SCORE
        MsgSender.sendMsg(IMessages:msgFiveKill(self:getDisplayName()))
    elseif self.multiKill >= 4 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 3 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 2 then
        self.score = self.score + ScoreConfig.KILL_2_SCORE
        MsgSender.sendMsg(IMessages:msgDoubleKill(self:getDisplayName()))
    end

end

function GamePlayer:setRoleAttribute(role)
    self.role = role
    if self.role == GamePlayer.ROLE_SOLDIER then
        local soldier = GameMatch:getCurGameScene().soldierConfig
        if soldier then
            self:setSoldierAttribute(soldier)
            self.multiZombie = 0
        end
    elseif self.role == GamePlayer.ROLE_ZOMBIE then
        local zombie = GameMatch:getCurGameScene():getBornRandomZombie()
        if zombie then
            self:setZombieAttribute(zombie)
            self.multiZombie = self.multiZombie + 1
        end
    end
end

function GamePlayer:setSoldierAttribute(soldier)
    if soldier then
        --士兵血量
        self:changeMaxHealth(soldier.hp)
        --士兵背包
        for i, v in pairs(soldier.inventory) do
            if v.fm.id ~= 0 then
                self:addEnchantmentItem(v.id, v.num, 0, v.fm.id, v.fm.lv)
            else
                self:addItem(v.id, v.num, 0)
            end
        end
    end
end

function GamePlayer:setZombieAttribute(zombie)
    self.zombieId = zombie.id
    if self.entityPlayerMP ~= nil then
        self:clearInv()
        self:changeMaxHealth(zombie.hp)
        self:changePlayerActorToZombieModel(zombie.model)
        HostApi.sendPlaySound(self.rakssid, 10025)
        HostApi.changeSkillType(self.rakssid, zombie.skillType, zombie.skillDurationTime, zombie.skillCDTime)
        local gameTime = GameMatch:getCurGameScene().gameTime
        for i, v in pairs(zombie.buff) do
            if v.id ~= 0 then
                self.entityPlayerMP:addEffect(v.id, gameTime, v.lv)
            end
        end
    end
end

function GamePlayer:setHunterAttribute()
    if self.entityPlayerMP ~= nil then
        self:clearInv()
        self:changeMaxHealth(HunterConfig.hp)
        self:changePlayerActorToHunterModel(HunterConfig.actor)
        for i, v in pairs(HunterConfig.inventory) do
            self:addItem(v.id, v.num, 0)
        end
    end
end

function GamePlayer:onPoison(seconds)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addPoisonPotionEffect(seconds)
    end
end

function GamePlayer:convertToZombie()
    if GameMatch:isGameStart() and self.role == GamePlayer.ROLE_ZOMBIE then
        local zombie = GameMatch:getCurGameScene():getConvertRandomZombie()
        if zombie then
            self:setZombieAttribute(zombie)
        end
        self.entityPlayerMP:removeEffect(PotionID.POISON)
        self.isChangeZombie = true
    end
end

function GamePlayer:convertToHunter()
    if GameMatch:isGameStart() and self.role == GamePlayer.ROLE_SOLDIER then
        self.isHunter = true
        self:setHunterAttribute()
    end
end

function GamePlayer:skillDefense(skillStatus)
    if skillStatus then
        local zombie = GameMatch:getCurGameScene():getZombieByZombieId(self.zombieId)
        if zombie then
            self.defenseCoefficient = zombie.defenseCoefficient
            self.isDefenseSkill = true
            self.defenseStartTime = os.time()
        end
    else
        self.defenseCoefficient = 1
        self.isDefenseSkill = false
        HostApi.setPlayerKnockBackCoefficient(self.rakssid, 1)
    end
end

function GamePlayer:skillSprint(skillStatus, speed, duration, buff)
    if skillStatus then
        self.entityPlayerMP:addEffect(PotionID.MOVE_SPEED, duration, speed)
        self.isSprintSkill = true
        self.sprintStartTime = os.time()
        self.sprintLastBuff = buff
    else
        if buff ~= nil then
            for i, v in pairs(buff) do
                if v.id == PotionID.MOVE_SPEED then
                    local gameTime = GameMatch:getCurGameScene().gameTime
                    self.entityPlayerMP:addEffect(v.id, gameTime, v.lv)
                    self.isSprintSkill = false
                end
            end
        end
        HostApi.setPlayerKnockBackCoefficient(self.rakssid, 1)
    end
end

function GamePlayer:skillReleaseToxic(range, duration, damage, poisonOnTime)
    local pos = self:getPosition()
    GameMatch:poisonRangeAttack(self.userId, pos, range, duration, damage, poisonOnTime)
end

function GamePlayer:teleportScene()
    self.isInGame = true
    HostApi.resetPos(self.rakssid, self:getScene().scenePos.x, self:getScene().scenePos.y + 0.5, self:getScene().scenePos.z)
end

function GamePlayer:getHunterName()
    return TextFormat.colorBlue .. "[" .. HunterConfig.name .. "]"
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- hunter name
    if self.isHunter == true then
        table.insert(nameList, self:getHunterName())
    end
    -- pureName line
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName, TextFormat.colorBlue)
    return table.concat(nameList, "\n")
end

function GamePlayer:changePlayerActorToZombieModel(model)
    self:setShowName(" ")
    EngineWorld:changePlayerActor(self, model)
end

function GamePlayer:changePlayerActorToHunterModel(model)
    self:setShowName(self:buildShowName())
    for i, chothes in pairs(model) do
        self:changeClothes(chothes.first, chothes.second)
    end
end

function GamePlayer:reset(report)
    if self.isHunter == true then
        self:resetClothes()
    end
    self:restoreActor()
    self:removeAllEffect()
    self.role = GamePlayer.ROLE_PENDING
    self.isLife = true
    self.score = 0
    self.kills = 0
    self.multiKill = 0
    self.lastKillTime = 0
    self.isInGame = false
    self.isHunter = false
    self:teleInitPos()
    self:clearInv()
    self:resetFood()
    self:resetHP()
    self:initScorePoint()
    self:setShowName(self:buildShowName())
    self.defenseCoefficient = 1
    HostApi.setPlayerKnockBackCoefficient(self.rakssid, 1)
    if report ~= false then
        ReportManager:reportUserEnter(self.userId)
    end
end

function GamePlayer:removeAllEffect()
    if self.role == GamePlayer.ROLE_ZOMBIE and self.entityPlayerMP ~= nil then
        local zombie = GameMatch:getCurGameScene():getZombieByZombieId(self.zombieId)
        if zombie then
            for i, v in pairs(zombie.buff) do
                if v.id ~= 0 then
                    self.entityPlayerMP:removeEffect(v.id)
                end
            end
        end
    end
end

function GamePlayer:restoreActor()
    EngineWorld:restorePlayerActor(self)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.DIE] = 0
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
    self.scorePoints[ScoreID.LIVE] = 0
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

function GamePlayer:resetHP()
    self:changeMaxHealth(20)
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

function GamePlayer:getScene()
    return GameMatch:getCurGameScene()
end

function GamePlayer:teleRolePos()
    if self.isLife then
        if self.role == GamePlayer.ROLE_ZOMBIE then
            local pos = self:getScene():getZombiePos()
            if pos ~= nil then
                HostApi.resetPos(self.rakssid, pos.x, pos.y + 0.5, pos.z)
            end
        else
            if self.role == GamePlayer.ROLE_PENDING then
                self:becomeRole(GamePlayer.ROLE_SOLDIER)
            end
        end
        if self.role == GamePlayer.ROLE_SOLDIER then
            local pos = self:getScene():getSoldierPos()
            if pos ~= nil then
                HostApi.resetPos(self.rakssid, pos.x, pos.y + 0.5, pos.z)
            end
        end
        self.isInGame = true
    end
end

function GamePlayer:onLifeTick(ticks)
    if self.isLife and ticks ~= 0 and self.isInGame then
        self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
        self.score = self.score + ScoreConfig.LIFE_SCORE
    end
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN_SCORE
    self.appIntegral = self.appIntegral + 10
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.isInGame = false
        self.role = GamePlayer.ROLE_DEAD
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
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

    UserExpManager:addUserExp(self.userId, isWin, 2)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end
    ReportManager:reportUserData(self.userId, self.kills, GameMatch:getPlayerRank(self), 1)
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
