require "config.GemStoneConfig"

ChestConfig = {}
ChestConfig.chestStatus = {
    null    = 1,            --箱子未刷新或宝石已经被领取后销毁
    unOpen  = 2,            --箱子已经刷新没有打开过
    hasOpen = 3,            --箱子已经刷新并且打开过，里面的宝石没有被领取
}

function ChestConfig:initConfig(configPos, configCD)
    self.status = self.chestStatus.null
    self.tick = 0
    self.chestCD = tonumber(configCD)
    self.posTable = {}
    GameConfig:initGamePos(self.posTable, configPos, false)
    self.signIdMap = {}
    self.isPlayerReadyFinish = false
    self.isFirstCreateSign = true
end

function ChestConfig:onTick()
    self:checkChest()
end

function ChestConfig:checkChest()
    if self.status == self.chestStatus.null then
        self.tick = self.tick + 1
        if self.isPlayerReadyFinish then
            local time = self.chestCD - self.tick
            if time > 0 then
                self:refreshCD(time)
                if self.isFirstCreateSign then
                    self:createSign()
                    self.isFirstCreateSign = nil
                end
            end
            self.isPlayerReadyFinish = false
        end
    end
    if self.tick > self.chestCD then
        self:createChest()
        self.tick = 0
    end
end

function ChestConfig:createChest()
    local len = #self.posTable
    if len == 0 then
        HostApi.log("ChestConfig:createChest() : #self.posTable == 0")
        return
    end
    self:refreshCD(0)
    self:destroySign()
    local index = 1
    if len > 1 then
        index = math.floor(HostApi.random("createChest", 10000, (len + 1) * 10000 - 1) / 10000)
    end
    local pos = self.posTable[index]
    if pos and pos.x and pos.y and pos.z then
        local entityId = EngineWorld:addSessionNpc(pos, 0, 7, GameConfig.chestActor, function(entity)
            entity:setNameLang("")
        end)
        self.entityId = entityId
        HostApi.log("ChestConfig:createChest() self.entityId is : "..self.entityId.."  x : "..pos.x.." y : "..pos.y.." z : "..pos.z)
        self.status = self.chestStatus.unOpen
    end
end

function ChestConfig:destroyChest()
    EngineWorld:removeEntity(self.entityId)
    self:refreshCD(self.chestCD)
    self:createSign()
    self.status = self.chestStatus.null
end

function ChestConfig:onPlayerClickChest(player)
    local playerPos = player:getPosition()
    if self:inCanClickArea(playerPos) and self.status == self.chestStatus.unOpen then
        local inv = player:getInventory()
        if inv and inv:getFirstEmptyStackInInventory() ~= -1 then
            player:addItem(511, 1, 0)
            LuaTimer:scheduleTimer(function()
                player:checkTreasureInInvetory()
            end, 500, 1)
        else
            local pos = EngineWorld:getWorld():getPositionByEntityId(self.entityId)
            GemStoneConfig:initGemStone(pos, player:getYaw(), 1)
        end
        MsgSender.sendCenterTips(1, Messages:msgGetGemSuccess(tostring(player.name)))
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1049click_treasure_indie", "g1049click_treasure_indie"} )
        else
            GameAnalytics:design(player.userId, 0, { "g1049click_treasure", "g1049click_treasure"} )
        end
        self.status = self.chestStatus.hasOpen
        local pos = EngineWorld:getWorld():getPositionByEntityId(self.entityId)
        EngineWorld:addSimpleEffect(GameConfig.chestOpenEffect, pos, 0, 3000)
        LuaTimer:scheduleTimer(function()
            self:destroyChest()
        end, 3000, 1)
    end
end

function ChestConfig:inCanClickArea(vec3)
    local pos = EngineWorld:getWorld():getPositionByEntityId(self.entityId)
    return math.pow((GameConfig.clickRange), 2) >= math.pow((pos.x - vec3.x), 2) + math.pow((pos.z - vec3.z), 2)
end

function ChestConfig:refreshCD(time)
    for __, pos in pairs(self.posTable) do
        cdPos = VectorUtil.newVector3(pos.x, pos.y + GameConfig.chestCDHeight, pos.z)
        HostApi.deleteWallText(0, cdPos)
        if time > 0 then
            HostApi.addWallText(0, 1, time*1000, "skyblock_chest_cd", cdPos, 3, 180, 0, {1,0,0,1})
        end
    end
end

function ChestConfig:createSign()
    self:destroySign()
    for i, chestPos in pairs(self.posTable) do
        local pos = VectorUtil.newVector3(chestPos.x, chestPos.y, chestPos.z)
        local entityId = EngineWorld:addActorNpc(pos, 0, GameConfig.chestPosActor, function(entity)
            entity:setCanCollided(false)
        end)
        self.signIdMap[i] = entityId
    end
end

function ChestConfig:destroySign()
    for i, entityId in pairs(self.signIdMap) do
        EngineWorld:removeEntity(entityId)
        entityId = 0
    end
end

return ChestConfig