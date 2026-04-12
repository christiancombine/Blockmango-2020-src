---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "util.RewardUtil"
require "Match"
require "data.GameSeason"
require "data.GameJob"
require "data.GameJobPond"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self.money = 0

    self.standByTime = 0
    self.moveHash = ""
    self:teleInitPos()

    self.cur_jobId = 0    --当前职业id
    self.lastJobId = 0  --上一次职业id
    self.choiceJob = 0

    self.jobs_data = {}
    self.Jobs_Pond = {}     --玩家的奖池数据
    self.pondRefreshTimes = 1 --记录玩家刷新奖池的次数

    self.advert_record = nil
    self.init_prop = {}
    self.rewards = {}

    self.GameJob = GameJob.new(self)
    self.GameJobPond = GameJobPond.new(self)
end

function GamePlayer:initDataFromDB(data, subKey)
    if subKey == DbUtil.GAME_DATA then
        self:initGameDataFromDB(data)
    end
    if subKey == DbUtil.ADVERT_DATA then
        self:initAdvertDataFromDB(data)
    end
end

function GamePlayer:initGameDataFromDB(data)
    if not data.__first then
        self.jobs_data = data.jobs_data or {}
        self.lastJobId = data.lastJobId or 0
        self.init_prop = data.init_prop or {}
    end
    self:setShowName(self:buildShowName())
    self:teleInitPos()
    self:setCurrency(self.money)
    self:syncHallInfo()
    GameSeason:getUserSeasonInfo(self.userId, false, 3)
end

function GamePlayer:initAdvertDataFromDB(data)
    if not data.__first then
        self.advert_record = data.advert_record
    end
    AdvertConfig:initPlayerAds(self)
end

function GamePlayer:setCurJobId(id)
    self.cur_jobId = id
    self.lastJobId = id
end

function GamePlayer:updateHealth()
    self:changeMaxHealth(self.maxHealth + self.addHealth)
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:getHallInfo()

end

function GamePlayer:syncHallInfo()

end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:teleInitPos()
    self:teleportPosWithYaw(GameConfig.initPos, 90)
    HostApi.changePlayerPerspece(self.rakssid, 1)
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:onTick()
    self.standByTime = self.standByTime + 1
    if self.standByTime >= GameConfig.standByTime then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:doPlayerQuit()
    BaseListener.onPlayerLogout(self.userId)
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:nextGameUseProp()
    if self.init_prop ~= nil then
        for i, item in pairs(self.init_prop) do
            GamePacketSender:sendUsePropData(self.rakssid, item.id, item.num)
        end
    end
end

return GamePlayer

--endregion


