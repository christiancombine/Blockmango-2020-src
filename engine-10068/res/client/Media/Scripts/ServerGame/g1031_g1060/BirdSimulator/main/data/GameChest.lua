require "manager.TimerManager"
require "util.Tools"
require "config.Area"

GameChest = class("GameChest")

local ChestType = {
    Disable_Status = 0,
    WaitReceive_Status = 1,
    CountDown_Status = 2
}

function GameChest:ctor(config)
    self.chestId = config.id
    self.actorName = config.actorName
    self.actorBody = config.actorBody
    self.actorBodyId = config.actorBodyId
    self.pos = config.pos
    self.yaw = config.yaw
    self.minPos = config.minPos
    self.maxPos = config.maxPos
    self.type = config.type
    self.itemId = config.itemId
    self.minNum = config.minNum
    self.maxNum = config.maxNum
    self.chestName = config.chestName or ""
    self.icon = config.icon or ""
    self.refreshCD = config.refreshCD
    self.modleName = config.modleName
    self.modleBody = config.modleBody
    self.modleBodyId = config.modleBodyId
    self.bindld = config.bindld
    self.type = config.type
    self.tips = config.tips
    self.birdRation = config.birdRation
    self.itemName = config.itemName
end

function GameChest:isVipChest()
    return self.type == 1
end

function GameChest:OnCreate()
    self:CalcRectInfo()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, 104101, self.actorName, function(entity)
        entity:setNameLang(self.chestName or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
        entity:setSessionContent(tostring(self.itemId) or "")
        entity:setPerson(true)
    end)
    self.npcId = entityId
    AreaInfoManager:push(self.npcId, AreaType.Chest, self.minPos, self.maxPos)
    return self.npcId
end

function GameChest:CalcRectInfo()
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

function GameChest:SyncChestInfo(player)
    local endTime = player:getChestCountDown(self.chestId)
    if type(endTime) ~= "number" then
        player:setChestCountDown(self.chestId, os.time())
        self:RefreshChesetItems(player, 0)
    else
        self:RefreshChesetItems(player, endTime - os.time())
    end
end

function GameChest:CheckTrigger(pos)
    return self.areaRect:inArea(pos)
end

function GameChest:RefreshChesetItems(player, time)
    local name = time > 0 and self.modleName or self.actorName
    local body = time > 0 and self.modleBody or self.actorBody
    local bodyId = time > 0 and self.modleBodyId or self.actorBodyId
    local remainTime = time > 0 and time or 0

    EngineWorld:getWorld():updateSessionNpc(
            self.npcId,
            player.rakssid,
            self.chestName,
            name,
            body,
            bodyId,
            tostring(self.itemId),
            "",
            remainTime * 1000,
            true
    )
    if time > 0 then
        local key = string.format("chestDelay:%s:%d", tostring(player.rakssid), self.chestId)
        TimerManager.AddDelayListener(
                key,
                time,
                function()
                    self:RefreshChesetItems(player, 0)
                end
        )
    end
    -- 通知客户端
end

function GameChest:OnPlayerClick(player)
    local endTime = player:getChestCountDown(self.chestId) or os.time()
    local remainTime = endTime - os.time()

    local price = PaymentConfig:getPaymentByTime(remainTime)
    local key = string.format("%s:%s:%d", "chest", tostring(player.rakssid), self.chestId)
    player:consumeDiamonds(44000 + self.chestId, price, key, false)
    ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.OnReceive, self)
end

function GameChest:onPlayerExit(player)
    local key = string.format("chestDelay:%s:%d", tostring(player.rakssid), self.chestId)
    TimerManager.RemoveListenerById(key)
end

function GameChest:OnPlayerReceiveChest(player)
    local endTime = player:getChestCountDown(self.chestId)
    local remainTime = endTime - os.time()

    if remainTime > 0 then
        return false
    end
    return self:OnReceive(nil, player.rakssid)
end

function GameChest:OnReceive(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local randNum = HostApi.random("chest", self.minNum, self.maxNum)
    local curCarryBirdNum = 0
    if player.userBirdBag ~= nil then
        curCarryBirdNum = player.userBirdBag.curCarryBirdNum
    end
    player:addProp(self.itemId, randNum + self.birdRation * curCarryBirdNum)
    if self.itemId ~= 9000001 then
        local birdGain = BirdGain.new()
        birdGain.itemId = self.itemId
        birdGain.num = randNum
        birdGain.icon = self.icon
        birdGain.name = self.itemName
        HostApi.sendBirdGain(player.rakssid, { birdGain })
    end

    TaskHelper.dispatch(TaskType.HaveChestEvent, player.userId, 1)
    TaskHelper.dispatch(TaskType.GetItemByOpenChestEvent, player.userId, self.itemId, randNum)
    local endTime = os.time() + self.refreshCD
    player:setChestCountDown(self.chestId, endTime)
    self:RefreshChesetItems(player, self.refreshCD)

    return true
end

function GameChest:RefreshNpc(status)
end
