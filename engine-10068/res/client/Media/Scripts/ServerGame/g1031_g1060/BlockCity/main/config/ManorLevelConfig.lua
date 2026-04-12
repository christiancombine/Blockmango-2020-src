ManorLevelConfig = {}
ManorLevelConfig.manorLevel = {}
ManorLevelConfig.managerNpc = {}

function ManorLevelConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.offsetStartX = tonumber(v.offsetStartX)
        data.offsetStartY = tonumber(v.offsetStartY)
        data.offsetStartZ = tonumber(v.offsetStartZ)
        data.offsetEndX = tonumber(v.offsetEndX)
        data.offsetEndY = tonumber(v.offsetEndY)
        data.offsetEndZ = tonumber(v.offsetEndZ)
        data.score = tonumber(v.score)
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.moneyTypeToDelete = tonumber(v.moneyTypeToDelete)
        data.priceToDelete = tonumber(v.priceToDelete)
        data.repairBlockId = tonumber(v.repairBlockId)
        data.repairBlockMeta = tonumber(v.repairBlockMeta)

        local Npc1 = {}
        Npc1.houseManagerX = (v.singx1 ~= "@@@" and { tonumber(v.singx1) } or { "" })[1]
        Npc1.houseManagerY = (v.singy1 ~= "@@@" and { tonumber(v.singy1) } or { "" })[1]
        Npc1.houseManagerZ = (v.singz1 ~= "@@@" and { tonumber(v.singz1) } or { "" })[1]
        Npc1.houseManagerYaw = tonumber(v.yaw1)

        local Npc2 = {}
        Npc2.houseManagerX = (v.singx2 ~= "@@@" and { tonumber(v.singx2) } or { "" })[1]
        Npc2.houseManagerY = (v.singy2 ~= "@@@" and { tonumber(v.singy2) } or { "" })[1]
        Npc2.houseManagerZ = (v.singz2 ~= "@@@" and { tonumber(v.singz2) } or { "" })[1]
        Npc2.houseManagerYaw = tonumber(v.yaw2)

        data.tipNor = (v.tipNor ~= "@@@" and { v.tipNor } or { "" })[1]
        data.tipPre = (v.tipPre ~= "@@@" and { v.tipPre } or { "" })[1]
        data.imageNor = (v.imageNor ~= "@@@" and { v.imageNor } or { "" })[1]
        data.imagePre = (v.imagePre ~= "@@@" and { v.imagePre } or { "" })[1]

        data.body = v.body
        data.bodyId = tonumber(v.bodyId)
        data.name = v.name
        data.actor = v.actor
        self.manorLevel[data.level] = data
        self.manorLevel[data.level].allManagerNpc = {
            Npc1,
            Npc2
        }
    end
end

function ManorLevelConfig:getOffsetPosByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getOffsetPosByLevel: manorLevel.csv can not find level =", level)
        return nil, nil
    end

    local manorLevel = self.manorLevel[tonumber(level)]

    local minX = math.min(manorLevel.offsetStartX, manorLevel.offsetEndX)
    local maxX = math.max(manorLevel.offsetStartX, manorLevel.offsetEndX)
    local minY = math.min(manorLevel.offsetStartY, manorLevel.offsetEndY)
    local maxY = math.max(manorLevel.offsetStartY, manorLevel.offsetEndY)
    local minZ = math.min(manorLevel.offsetStartZ, manorLevel.offsetEndZ)
    local maxZ = math.max(manorLevel.offsetStartZ, manorLevel.offsetEndZ)

    return VectorUtil.newVector3(minX, minY, minZ), VectorUtil.newVector3(maxX, maxY, maxZ)
end

function ManorLevelConfig:getRepairBlockIdByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockId
    else
        return nil
    end
end

function ManorLevelConfig:getRepairBlockMetaByLevel(level)
    if self.manorLevel[tonumber(level)] ~= nil then
        return self.manorLevel[tonumber(level)].repairBlockMeta
    else
        return nil
    end
end

function ManorLevelConfig:getScoreByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getScoreByLevel: manorLevel.csv can not find level =", level)
        return 0
    end

    return self.manorLevel[tonumber(level)].score
end

function ManorLevelConfig:getMoneyTypeByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getMoneyTypeByLevel: manorLevel.csv can not find level =", level)
        return 3
    end

    return self.manorLevel[tonumber(level)].moneyType
end

function ManorLevelConfig:getNextPriceByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getNextPriceByLevel: manorLevel.csv can not find level =", level)
        return 0
    end

    return self.manorLevel[tonumber(level)].price
end

function ManorLevelConfig:getMaxLevel()
    return #self.manorLevel
end

function ManorLevelConfig:getMoneyTypeToDelByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getMoneyTypeToDelByLevel:manorLevel.csv can not find level =", level)
        level = 3
    end
    return self.manorLevel[tonumber(level)].moneyTypeToDelete
end

function ManorLevelConfig:getPriceToDeleteByLevel(level)
    if self.manorLevel[tonumber(level)] == nil then
        print("ManorLevelConfig:getPriceToDeleteByLevel:manorLevel.csv can not find level =", level)
        level = 3
    end
    return self.manorLevel[tonumber(level)].priceToDelete
end

function ManorLevelConfig:releaseHouseManagerByUserId(userId)
    if self.managerNpc[userId] then
        for _, v in pairs(self.managerNpc[userId]) do
            EngineWorld:removeEntity(v)
        end
        self.managerNpc[userId] = nil
    end
end

function ManorLevelConfig:createHouseManager(userId, manorIndex, level)
    local manorInfo = self.manorLevel[tonumber(level)]
    if not manorInfo then
        return
    end
    ManorLevelConfig:releaseHouseManagerByUserId(userId)

    if level < self:getMaxLevel() then
        local npcs = {}
        for _, v in pairs(manorInfo.allManagerNpc) do
            local offsetX = v.houseManagerX
            local offsetY = v.houseManagerY
            local offsetZ = v.houseManagerZ
            local offYaw = v.houseManagerYaw
            local manager_OffsetPos = VectorUtil.newVector3(offsetX, offsetY, offsetZ)
            local manager_OffsetYaw = offYaw
            local manager_Pos = ManorConfig:getPosByOffset(manorIndex, manager_OffsetPos, false)
            local manager_Yaw = ManorConfig:getYawByOffsetYaw(manorIndex, manager_OffsetYaw)
            local manager_MaxTouchPos = VectorUtil.newVector3(manager_Pos.x + 1, manager_Pos.y + 1, manager_Pos.z + 1)
            local entityId = EngineWorld:addSessionNpc(manager_Pos, manager_Yaw, 104706, manorInfo.actor, function(entity)
                entity:setNameLang(manorInfo.name or "")
                entity:setActorBody(manorInfo.body or "body")
                entity:setActorBodyId(tostring(manorInfo.bodyId) or "")
                entity:setSessionContent(tostring(userId) or "")
            end)
            local npcId = entityId
            AreaInfoManager:push(
                    npcId,
                    AreaType.Manager,
                    manorInfo.tipNor,
                    manorInfo.tipPre,
                    manorInfo.imageNor,
                    manorInfo.imagePre,
                    false,
                    manager_Pos,
                    manager_MaxTouchPos
            )
            table.insert(npcs, npcId)
        end
        self.managerNpc[userId] = npcs
    end
end

return ManorLevelConfig