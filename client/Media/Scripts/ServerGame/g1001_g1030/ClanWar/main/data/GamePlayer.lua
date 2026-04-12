--GamePlayer.lua
require "config.TeamConfig"
require "data.GameTeam"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.team = {}
    self.isLogout = false

    self:initTeam()
    self:reset()

end

function GamePlayer:buildShowName()
    local nameList = {}
    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:reset()
    self.kills = 0
    self.dies = 0
    self.score = 0
    self.flags = 0
    self:teleInitPos()
    self:resetInv()
    self:resetFood()
    self:initScorePoint()
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.DIE] = 0
end

function GamePlayer:resetFood()
    local foodStats = self.entityPlayerMP:getFoodStats()
    foodStats:setFoodLevel(20)
end

function GamePlayer:initItems()
    for i, v in pairs(GameConfig.initItems) do
        self:addItem(v.id, v.num, 0)
    end
end

function GamePlayer:initTeam()
    local team = GameMatch.Teams[self.dynamicAttr.team]
    if team ~= nil then
        team:addPlayer()
        self.team = team
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.NoThisTeam)
    end
end

function GamePlayer:resetInv()
    self:clearInv()
    self:initItems()
end

function GamePlayer:clearInv()
    if self.entityPlayerMP ~= nil then
        local inv = self.entityPlayerMP:getInventory()
        inv:clear()
    end
end

function GamePlayer:getDisplayName()
    return self.team:getDisplayName() .. TextFormat.colorWrite .. self.name
end

function GamePlayer:teleInitPos()
    HostApi.resetPos(self.rakssid, self.team.initPos.x, self.team.initPos.y + 0.5, self.team.initPos.z)
end

function GamePlayer:onLogout()
    if self.isLogout == false then
        self.isLogout = true
        self.team:subPlayer()
    end
end

function GamePlayer:onDie()
    RespawnHelper.sendRespawnCountdown(self, 3)
    self.dies = self.dies + 1
    if self.score + GameMatch.SCORE_DIE < 0 then
        self.score = 0
    else
        self.score = self.score + GameMatch.SCORE_DIE
    end
    self.team:onPlayerDeath()
    self.scorePoints[ScoreID.DIE] = self.scorePoints[ScoreID.DIE] + 1
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + GameMatch.SCORE_KILL
    self.team:onPlayerKill()
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
end

function GamePlayer:onPlaceFlag()
    self.flags = self.flags + 1
    self.score = self.score + GameMatch.SCORE_FLAG
end

return GamePlayer

--endregion
