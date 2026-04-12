---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}

    self.money = 0
    self.unlockedCommodities = {}

    self:setCurrency(self.money)
    self.scorePoints = {}

    self.score_all = 0
    self.winner_times = 0

    self:reset()

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:teleInitPos()
    self:teleportPosByRecord(GameConfig.initPos, 0)
end

function GamePlayer:reward()

end

function GamePlayer:insertGradeList(room_id, grade)
    for k, v in pairs(self.grade_list) do
        if room_id == v.room_id then
            v.grade = grade
            return
        end
    end
    local item = {}
    item.room_id = room_id
    item.grade = grade
    table.insert(self.grade_list, item)
end

function GamePlayer:getHighGrade()
    local list = {}

    table.sort(self.grade_list, function(a, b)
        if a.grade ~= b.grade then
            return a.grade > b.grade
        end
        return a.room_id > b.room_id
    end)

    for i = 1, GameConfig.gradeFavouriteNum do
        if i <= #self.grade_list then
            table.insert(list, self.grade_list[i])
        end
    end

    return list
end

function GamePlayer:setAllowFlyingOn(sign)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setAllowFlying(sign)
    end
end

function GamePlayer:teleBuildPos()
    local target_pos, yaw = GameMatch:getTargetPos(self.room_id)

    if target_pos and yaw then
        self:teleportPosByRecord(target_pos, yaw)
    end
end

function GamePlayer:teleportPosByRecord(target_pos, yaw)
    HostApi.sendPlaySound(self.rakssid, 141)
    self:teleportPosWithYaw(target_pos, yaw)
    local distance = 300
    local d = 0
    for k, v in pairs(GameConfig.targetPos) do
        d = (v.pos[1] - target_pos.x) * (v.pos[1] - target_pos.x) + (v.pos[2] - target_pos.y) * (v.pos[2] - target_pos.y) + (v.pos[3] - target_pos.z) * (v.pos[3] - target_pos.z)
        if d < distance then
            self.at_room_id = v.id
            return
        end
    end

    d = (GameConfig.initPos.x - target_pos.x) * (GameConfig.initPos.x - target_pos.x) 
        + (GameConfig.initPos.y - target_pos.y) * (GameConfig.initPos.y - target_pos.y) 
        + (GameConfig.initPos.z - target_pos.z) * (GameConfig.initPos.z - target_pos.z)

    if d < distance then
        self.at_room_id = GameConfig.waitingRoomId
    end
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money or 0
        self.unlockedCommodities = data.unlockedCommodities or {}
    end
    self:initPlayer()
end

function GamePlayer:initPlayer()
    self:setCurrency(self.money)
    self:setUnlockedCommodities(self.unlockedCommodities)
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

function GamePlayer:addMoney(money)
    money = NumberUtil.getIntPart(money)
    self:addCurrency(money)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:addUnlockedCommodity(unlockedCommodity, meta)
    table.insert(self.unlockedCommodities, {unlockedCommodity, meta})
end

function GamePlayer:overGame()
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:reset()
    self:setAllowFlyingOn(true)
    self:initScorePoint()
    self:resetFood()
    self.room_id = GameMatch:getRoomFormPool(self.userId)
    self.guess_room_id = 0
    self.grade_list = {}
    self.at_room_id = 0
    self.cur_grade = 0
    self:teleInitPos()
end

function GamePlayer:getCurGrade()
    if self.cur_grade > 0 then
        return Messages:msgShowYourGrade() + self.cur_grade - 1
    else
        return 0
    end
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

return GamePlayer

--endregion


