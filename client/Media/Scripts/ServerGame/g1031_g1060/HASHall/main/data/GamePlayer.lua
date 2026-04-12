---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.NOT_HAVE_ACTOR_STATE = 0
GamePlayer.HAVE_ACTOR_STATE = 2
GamePlayer.USE_ACTOR_STATE = 3

function GamePlayer:init()

    self.isDBReady = false
    self:initStaticAttr(0)
    self:teleInitPos()
    self.actors = {}

    self.randomSeed = 0
    self.standByTime = 0
    self.money = 0
    self:setCurrency(self.money)

    self.lotteryTimes = 0
    self.LastLoginDay = ""
    self.moveHash = ""
    self.subKey_num = 0
    self.lookAdTime = 0

end

function GamePlayer:initDataFromDB(data, subKey)
    if not data.__first then
        if subKey == DbUtil.ACTORS_TAG then
            self.lotteryTimes = data.lotteryTimes or 0
            self.actors = data.actors or {}
            self.isDBReady = true
            self.LastLoginDay = data.LastLoginDay or ""
            self:initUseActor()
            self:useAppProps()

        elseif subKey == DbUtil.MONEY_TAG then
            self.money = data.money or 0
            self.lookAdTime = data.lookAdTime or 0
            self:setCurrency(self.money)
            VideoAdHelper:initPlayerData(self)
        end
    end
    if data.__first then
        if subKey == DbUtil.ACTORS_TAG then
            self.isDBReady = true
            self:useAppProps()
        elseif subKey == DbUtil.MONEY_TAG then
            self.lookAdTime = 0
            VideoAdHelper:initPlayerData(self)
        end
    end
    self.subKey_num = self.subKey_num + 1
    if self.subKey_num == 2 then
        self:sendActorsData()
        self:IsTodayFirstLogin()
    end
end

function GamePlayer:IsTodayFirstLogin()
    local data = DateUtil.getDate()
    if data ~= self.LastLoginDay then
        self:addMoney(GameConfig.todayFirstLogin)
        self.LastLoginDay = data
    end
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
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

function GamePlayer:onTick()
    self.standByTime = self.standByTime + 1

    if self.standByTime == GameConfig.standByTime - 10 then
        MsgSender.sendBottomTipsToTarget(self.rakssid, GameConfig.gameTipShowTime, Messages:msgNotOperateTip())
    end
    if self.standByTime >= GameConfig.standByTime then
        self:doPlayerQuit()
    end
end

function GamePlayer:doPlayerQuit()
    BaseListener.onPlayerLogout(self.userId)
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:addMoney(money)
    if money ~= 0 then
        self.money = self.money + money
        self:addCurrency(money)
    end
end

function GamePlayer:subMoney(money)
    if money ~= 0 then
        self.money = self.money - money
        self:subCurrency(money)
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:sendActorsData()
    if self.entityPlayerMP == nil then
        return
    end
    local useActorId = nil
    local config = ActorsConfig:getActors()
    local data = {}
    data.props = {}
    data.title = "my_goods"
    for id1, item in pairs(config) do
        local actor = {}
        actor.id = item.id
        actor.name = item.name
        actor.image = item.image
        actor.price = item.price or 0
        actor.desc = item.desc
        actor.rare = item.rare
        actor.status = GamePlayer.NOT_HAVE_ACTOR_STATE
        for id2, status in pairs(self.actors) do
            if actor.id == id2 then
                if status == GamePlayer.USE_ACTOR_STATE then
                    actor.status = GamePlayer.HAVE_ACTOR_STATE
                    useActorId = id2
                else
                    actor.status = status
                end
            end
        end
        table.insert(data.props, actor)
    end

    table.sort(data.props, function(a, b)
        if a.status == b.status then
            if a.rare == b.rare then
                return tonumber(a.id) < tonumber(b.id)
            else
                return a.rare > b.rare
            end
        else
            return a.status > b.status
        end
    end)

    if useActorId then
        for k, v in pairs(data.props) do
            if useActorId == v.id then
                data.props[k].status = GamePlayer.USE_ACTOR_STATE
                break
            end
        end
    end
    self.entityPlayerMP:changeSwitchableProps(json.encode(data))
end

function GamePlayer:hasActor(id)
    local actor = self.actors[id]
    if actor then
        return true
    end
    return false
end

function GamePlayer:initUseActor()
    for k, v in pairs(self.actors) do
        if v == GamePlayer.USE_ACTOR_STATE then
            self.actors[k] = GamePlayer.HAVE_ACTOR_STATE
            return
        end
    end
end

function GamePlayer:useActor(id)
    if self.actors[id] == nil then
        return
    end

    for k, status in pairs(self.actors) do
        if status == GamePlayer.USE_ACTOR_STATE then
            self.actors[k] = GamePlayer.HAVE_ACTOR_STATE
        end
    end
    self.actors[id] = GamePlayer.USE_ACTOR_STATE
    local actor = ActorsConfig:getActorById(id)
    if actor == nil then
        return
    end

    EngineWorld:changePlayerActor(self, actor.actor_name)
end

function GamePlayer:onUseCustomProp(id, itemId)
    if self.isDBReady == false then
        return
    end
    VideoAdHelper:onLotteryVideoAdReward(self, tostring(itemId))
    PacketSender:sendDataReport(self.rakssid, "hide_getitem_ad", BaseMain:getGameType())
end

return GamePlayer

--endregion


