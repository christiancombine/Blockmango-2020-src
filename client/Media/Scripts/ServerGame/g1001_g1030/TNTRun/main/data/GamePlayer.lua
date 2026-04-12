--GamePlayer.lua
require "config.GameConfig"
require "config.ScoreConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.ROLE_PENDING = 0
GamePlayer.ROLE_RUNNING_MAN = 1
GamePlayer.ROLE_DESTROYER = 2

function GamePlayer:init()

    self:initStaticAttr(0)

    self.positionX = GameConfig.initPos.x
    self.positionY = GameConfig.initPos.y
    self.positionZ = GameConfig.initPos.z

    self.kills = 0
    self.score = 0
    self.isLife = true
    self.isLogout = false

    self.lastMoveTime = 0
    self.removeBlock = {}
    self.tntBlock = nil
    self.lastAddTNTTime = 0
    self.role = GamePlayer.ROLE_PENDING
    self.loginTime = tonumber(HostApi.curTimeString())
    self.isSingleReward = false

    self:initInv()
    self:teleInitPos()
    self:resetHealth()
    self:initScorePoint()

    ReportManager:reportUserEnter(self.userId)
end


function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.LIVE] = 0
end


function GamePlayer:initInv()

end

function GamePlayer:addMoveSpeedPotionEffect(seconds)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addMoveSpeedPotionEffect(seconds)
    end
end

function GamePlayer:teleInitPos()
    HostApi.resetPos(self.rakssid, GameConfig.initPos.x, GameConfig.initPos.y + 0.5, GameConfig.initPos.z)
end

function GamePlayer:addRemoveBlock(x, y, z)
    if self.isLife == false then
        return
    end
    local vec3 = VectorUtil.toBlockVector3(x, y - 1, z)
    local hash = VectorUtil.hashVector3(vec3)
    if self.tntBlock ~= nil then
        local tntHash = VectorUtil.hashVector3(self.tntBlock.pos)
        if hash == tntHash then
            return
        end
    end
    if self:addTNTBlock(x, y, z) == false then
        self.removeBlock[hash] = {}
        self.removeBlock[hash].pos = vec3
        self.removeBlock[hash].time = tonumber(HostApi.curTimeString())
    end
end

function GamePlayer:addTNTBlock(x, y, z)
    if self.role == GamePlayer.ROLE_DESTROYER and self.isLife then
        if tonumber(HostApi.curTimeString()) - self.lastAddTNTTime > 10 * 1000 then
            self.lastAddTNTTime = tonumber(HostApi.curTimeString())
            self.tntBlock = {}
            self.tntBlock.pos = VectorUtil.toBlockVector3(x, y - 1, z)
            self.tntBlock.time = tonumber(HostApi.curTimeString())
            EngineWorld:setBlock(self.tntBlock.pos, 46)
            local floor = VectorUtil.newVector3i(self.tntBlock.pos.x, self.tntBlock.pos.y - 1, self.tntBlock.pos.z)
            EngineWorld:setBlock(floor, 24)
            EngineWorld:getWorld():fireTNT(self.tntBlock.pos)
            return true
        end
    end
    return false
end

function GamePlayer:fireTNT()
    if self.tntBlock ~= nil and tonumber(HostApi.curTimeString()) - self.tntBlock.time > 500 then
        EngineWorld:getWorld():fireTNT(self.tntBlock.pos)
        self.tntBlock = nil
    end
end

function GamePlayer:removeFootBlock()
    if self.isLife == false then
        return
    end
    local time = tonumber(HostApi.curTimeString())
    for i, v in pairs(self.removeBlock) do
        if time - v.time > 15 then
            EngineWorld:setBlockToAir(v.pos)
            self.removeBlock[VectorUtil.hashVector3(v.pos)] = nil
        end
    end
    self:fireTNT()
end

function GamePlayer:removeFootBlocks()
    if self.isLife == false then
        return
    end
    local pos = VectorUtil.toBlockVector3(self.positionX, self.positionY, self.positionZ)
    for x = -1, 1 do
        for z = -1, 1 do
            local vec = VectorUtil.newVector3i(pos.x + x, pos.y - 1, pos.z + z)
            EngineWorld:setBlockToAir(vec)
        end
    end
end

function GamePlayer:resetHealth()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeMaxHealth(20)
    end
end

function GamePlayer:clearInv()
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        inv:clear()
    end
end

function GamePlayer:becomeRole(role)
    self.role = role
    MsgSender.sendMsgToTarget(self.rakssid, Messages:becomeRole(role))
    self:clearInv()
    self.lastAddTNTTime = tonumber(HostApi.curTimeString())
end

function GamePlayer:onLogout()
    if self.isLogout == false then
        isLogout = true
    end
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self:reward(false, self.defaultRank, GameMatch:getLifePlayer() == 0)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
    settlement.name = self.name
    settlement.isWin = 0
    settlement.gold = self.gold
    settlement.points = self.scorePoints
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
end

function GamePlayer:onLifeTick(ticks)
    if self.isLife and ticks ~= 0 then
        self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
        self.appIntegral = self.appIntegral + 1
        if ticks % 10 == 0 then
            self.score = self.score + ScoreConfig.LIFE_SCORE
        end
    end
end

function GamePlayer:onWin()
    if self.isLife then
        ReportManager:reportUserWin(self.userId)
        self.score = self.score + ScoreConfig.WIN_SCORE
    end
end

function GamePlayer:onGameEnd(win)
    self:reward(win, GameMatch:getPlayerRank(self), true)
end

function GamePlayer:overGame(death, win)
    self:onLogout()
    if death then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverDeath(), GameOverCode.GameOver)
    else
        if win then
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
        else
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
        end
    end
end

function GamePlayer:reward(isWin, rank, isEnd)

    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    self.kills = os.time() - RewardManager:getStartGameTime()
    if isEnd then
        return
    end
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch.hasEndGame == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, 0, GameMatch:getPlayerRank(self), isCount)
end

return GamePlayer

--endregion
