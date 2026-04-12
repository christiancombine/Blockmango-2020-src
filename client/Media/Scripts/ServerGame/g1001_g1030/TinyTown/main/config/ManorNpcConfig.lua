require "config.Area"

ManorNpcConfig = {}
ManorNpcConfig.manorNpc = {}
ManorNpcConfig.sessionNpc = {}
ManorNpcConfig.rankNpc = {}
ManorNpcConfig.manor = {}
ManorNpcConfig.SessionType_GET_MANOR = 1
ManorNpcConfig.SessionType_SELL_MANOR = 2
ManorNpcConfig.SessionType_MANOR_FURNITURE = 3
ManorNpcConfig.SessionType_TIP_NPC = 4

function ManorNpcConfig:init(config)
    self:initManor(config.manor)
    self:initManorNpc(config.manorNpc)
    self:initSessionNpc(config.sessionNpc)
    self:initRankNpc(config.rankNpc)
end

function ManorNpcConfig:initSessionNpc(config)
    for i, v in pairs(config) do
        local sessionNpc = {}
        sessionNpc.pos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        sessionNpc.yaw = tonumber(v.initPos[4])
        sessionNpc.message = tostring(v.message)
        sessionNpc.name = tostring(v.name)
        sessionNpc.actor = tostring(v.actor)
        self.sessionNpc[i] = sessionNpc
    end
end

function ManorNpcConfig:initRankNpc(config)
    for i, v in pairs(config) do
        local rankNpc = {}
        rankNpc.pos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        rankNpc.yaw = tonumber(v.initPos[4])
        rankNpc.name = tostring(v.name)
        rankNpc.actor = tostring(v.actor)
        self.rankNpc[i] = rankNpc
    end
end

function ManorNpcConfig:initManorNpc(config)
    for i, v in pairs(config) do
        local npc = {}
        npc.pos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        npc.yaw = tonumber(v.initPos[4])
        npc.name = v.name
        npc.actor = v.actor
        npc.content = v.content
        npc.type = tonumber(v.type)
        self.manorNpc[i] = npc
    end
end

function ManorNpcConfig:initManor(config)
    for i, v in pairs(config) do
        local manor = {}
        manor.id = tonumber(v.id)
        manor.orientation = tonumber(v.orientation)
        manor.landPosition = Area.new(v.landPosition)
        manor.telePosition = VectorUtil.newVector3(v.telePosition[1], v.telePosition[2], v.telePosition[3])
        manor.signPosition = VectorUtil.newVector3i(v.signPosition[1], v.signPosition[2], v.signPosition[3])
        self.manor[tonumber(v.id)] = manor
    end
end

function ManorNpcConfig:prepareManorNpc()
    for i, v in pairs(self.manorNpc) do
        if v.type == 1 then
            local entityId = EngineWorld:addSessionNpc(v.pos, v.yaw, ManorNpcConfig.SessionType_GET_MANOR, v.actor, function(entity)
                entity:setNameLang(v.name or "")
                entity:setSessionContent(v.content or "")
            end)
            v.entityId = entityId
        else
            local entityId = EngineWorld:addSessionNpc(v.pos, v.yaw, ManorNpcConfig.SessionType_SELL_MANOR, v.actor, function(entity)
                entity:setNameLang(v.name or "")
                entity:setSessionContent(v.content or "")
            end)
            v.entityId = entityId
        end
    end
end

function ManorNpcConfig:prepareSessionNpc()
    for i, v in pairs(self.sessionNpc) do
        local entityId = EngineWorld:addSessionNpc(v.pos, v.yaw, ManorNpcConfig.SessionType_TIP_NPC, v.actor, function(entity)
            entity:setNameLang(v.name or "")
            entity:setSessionContent(v.message or "")
        end)
        v.entityId = entityId
    end
end

function ManorNpcConfig:prepareRankNpc()
    for i, v in pairs(self.rankNpc) do
        v.entityId = EngineWorld:addRankNpc(v.pos, v.yaw, v.name)
    end
end

function ManorNpcConfig:getManorById(id)
    for i, v in pairs(self.manor) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function ManorNpcConfig:teleportEvent(player)
    local manor = self:getManorById(player.manorIndex)
    if manor ~= nil then
        player:teleportPos(manor.telePosition)
    end
end

function ManorNpcConfig:sendRankData(rakssid, data)
    for i, v in pairs(self.rankNpc) do
        HostApi.sendRankData(rakssid, v.entityId, data)
    end
end

function ManorNpcConfig:rankNpcData(key, response, func)
    local data = json.decode(response)
    local userIds = {}
    for i, v in pairs(data) do
        userIds[i] = v.userId
    end
    if #userIds == 0 then
        return
    end
    UserInfoCache:GetCacheByUserIds(userIds, function(func, data)
        for i, v in pairs(data) do
            local info = UserInfoCache:GetCache(v.userId)
            v.vip = 0
            v.name = "Steve" .. v.rank
            if info then
                v.vip = info.vip
                v.name = info.name
            end
        end
        func(data)
    end, key, func, data)
end

function ManorNpcConfig:newManorInfo(player, level)
    local info = ManorInfo.new()
    local nextInfo = ManorInfo.new()
    local yardInfo = YardConfig:getYardByLevel(level, player.manorIndex)

    if yardInfo ~= nil then
        info.manorId = tostring(player.manorId)
        info.isMaster = player.isMaster
        info.rose = yardInfo.praiseNum
        info.level = yardInfo.level
        info.price = yardInfo.blockymodsPrice
        info.value = player.manorValue
        info.houseId = player.currentHouseId
        info.charm = player.manorCharm
        info.maxLevel = YardConfig.maxLevel
        info.seed = tostring(yardInfo.currSeedNum) .. "/" .. tostring(yardInfo.totalSeedNum)
        info.ground = tostring(yardInfo.xLength) .. "x" .. tostring(yardInfo.zLength)

        local house = HouseConfig:getHouseById(player.currentHouseId)
        if house ~= nil then
            info.icon = tostring(house.icon)
        else
            info.icon = YardConfig.defaultIcon
        end
        info.currencyType = yardInfo.coinId

        nextInfo.manorId = tostring(player.manorId)
        nextInfo.isMaster = player.isMaster
        nextInfo.rose = yardInfo.addThumbUpNum
        nextInfo.level = yardInfo.level + 1
        nextInfo.price = yardInfo.nextBlockymodsPrice
        nextInfo.value = 0
        nextInfo.houseId = 0
        nextInfo.charm = yardInfo.addCharmValue
        nextInfo.maxLevel = YardConfig.maxLevel
        nextInfo.seed = tostring(yardInfo.addSeedNum)
        nextInfo.ground = tostring(yardInfo.addUpgradeArea)
        nextInfo.icon = ""
        nextInfo.currencyType = yardInfo.nextCoinId
    end

    return info, nextInfo
end

function ManorNpcConfig:updateManorInfo(player)
    local curInfo, nextInfo = ManorNpcConfig:newManorInfo(player, player.yardLevel)
    local buildHouse = HouseConfig:newManorHouse(player, player.manorIndex)
    local furnitures = FurnitureConfig:newFurniture(player)
    HostApi.updateManorInfo(player.rakssid, curInfo, nextInfo, buildHouse, furnitures)
end

function ManorNpcConfig:newManorOwners()

    local owners = {}
    local index = 1
    for i, v in pairs(GameMatch.userManorIndex) do
        local originX = self.manor[tonumber(v)].landPosition.minX
        local originY = self.manor[tonumber(v)].landPosition.minY
        local originZ = self.manor[tonumber(v)].landPosition.minZ

        local terminiX = self.manor[tonumber(v)].landPosition.maxX
        local terminiY = self.manor[tonumber(v)].landPosition.maxY
        local terminiZ = self.manor[tonumber(v)].landPosition.maxZ

        local origin = VectorUtil.newVector3i(originX, originY, originZ)
        local termini = VectorUtil.newVector3i(terminiX, terminiY, terminiZ)

        local owner = ManorOwner.new()
        owner.index = tonumber(v)
        owner.ownerId = tonumber(i)
        owner.origin = origin
        owner.termini = termini
        owners[index] = owner
        index = index + 1
    end

    return owners
end

return ManorNpcConfig