require "manager.TimerManager"
require "util.Tools"
require "config.Area"
require "config.BonusConfig"
require "data.DrawLuckyPool"

GameEggChest = class("GameEggChest")

function GameEggChest:ctor(config)
    self.eggChestId = config.id
    self.actorName = config.actorName
    self.actorBody = config.actorBody
    self.actorBodyId = config.actorBodyId
    self.modleName = config.modleName
    self.modleBody = config.modleBody
    self.modleBodyId = config.modleBodyId
    self.eggChestName = config.eggChestName or ""
    self.pos = config.pos
    self.yaw = config.yaw
    self.minPos = config.minPos
    self.maxPos = config.maxPos
    self.refreshCD = config.refreshCD
    self.tips = config.tips
    self.pools = config.pools
    self.poolList = {}
end

function GameEggChest:OnCreate()
    self:CalcRectInfo()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, 104106, self.actorName, function(entity)
        entity:setNameLang(self.eggChestName or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
        entity:setPerson(true)
    end)
    self.npcId = entityId
    AreaInfoManager:push(self.npcId, AreaType.EggChest, self.minPos, self.maxPos)
    self.NewBonusPool(self)
    return self.npcId
end

function GameEggChest:CalcRectInfo()
    local pos = {}
    pos[1] = {}
    pos[1][1] = self.minPos.x
    pos[1][2] = self.minPos.y
    pos[1][3] = self.minPos.z
    pos[2] = {}
    pos[2][1] = self.maxPos.x
    pos[2][2] = self.maxPos.y
    pos[2][3] = self.maxPos.z
    self.areaRect = Area.new(pos)
end

function GameEggChest:SyncEggChestInfo(player)
    local endTime = player:getEggChestCountDown(self.eggChestId)
    if type(endTime) ~= "number" then
        player:setEggChestCountDown(self.eggChestId, os.time())
        self:RefreshEggChesetItems(player, 0)
    else
        self:RefreshEggChesetItems(player, endTime - os.time())
    end
end

function GameEggChest:CheckTrigger(pos)
    return self.areaRect:inArea(pos)
end

function GameEggChest:RefreshEggChesetItems(player, time)
    local name = time > 0 and self.modleName or self.actorName
    local body = time > 0 and self.modleBody or self.actorBody
    local bodyId = time > 0 and self.modleBodyId or self.actorBodyId
    local remainTime = time > 0 and time or 0

    EngineWorld:getWorld():updateSessionNpc(
            self.npcId,
            player.rakssid,
            self.eggChestName,
            name,
            body,
            bodyId,
            "",
            "",
            remainTime * 1000,
            true
    )
    if time > 0 then
        local key = string.format("eggChestDelay:%s:%d", tostring(player.rakssid), self.eggChestId)
        TimerManager.AddDelayListener(
                key,
                time,
                function()
                    self:RefreshEggChesetItems(player, 0)
                end
        )
    end
    -- 通知客户端
end

function GameEggChest:NewBonusPool()
    local configs = BonusConfig.bonusPools
    local index = 1
    for _, poolid in ipairs(self.pools) do
        local config = configs[poolid]
        if type(config) == "table" then
            local pool = DrawLuckyPool.new(index, config)
            table.insert(self.poolList, pool)
        end
        index = index + 1
    end
end

function GameEggChest:OnPlayerClick(player)
    self:OnReceive(nil, player.rakssid)
end

function GameEggChest:onPlayerExit(player)
    local key = string.format("eggChestDelay:%s:%d", tostring(player.rakssid), self.eggChestId)
    TimerManager.RemoveListenerById(key)
end

function GameEggChest:OnReceive(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local allweight = 0
    local maxCount = self.poolList[1]:GetMaxCount()
    local offset = player:GetDrawLuckyIndexByPoolId(self.eggChestId, maxCount)
    for index, pool in ipairs(self.poolList) do
        local weight = pool:RefreshRange(player, allweight, offset)
        if weight > allweight then
            allweight = weight
        end
    end

    local randNumber = HostApi.random("poolList", 1, allweight)
    for _, pool in ipairs(self.poolList) do
        if pool:CheckRangeVal(randNumber) then
            pool:OnRandomPool(player)

            if pool.isKey == true then
                player:OnRandKey(self.id)
            end

            local endTime = os.time() + self.refreshCD
            player:setEggChestCountDown(self.eggChestId, endTime)
            self:RefreshEggChesetItems(player, self.refreshCD)

            return true
        end
    end
    return true
end
