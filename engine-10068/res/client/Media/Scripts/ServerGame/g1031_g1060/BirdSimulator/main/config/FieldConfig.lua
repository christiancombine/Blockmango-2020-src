require "util.Tools"
require "config.Area"
require "manager.SceneManager"

FieldConfig = {}
FieldConfig.fields = {}
FieldConfig.door = {}
FieldConfig.vipArea = {}
FieldConfig.shopArea = {}
FieldConfig.doorTip = {}
FieldConfig.treeWeight = {}

function FieldConfig:initField(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.minPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.maxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.monsterIds = StringUtil.split(v.monsterIds, "#")
        data.monsterPos = StringUtil.split(v.monsterPos, "#")
        data.countDownPos = StringUtil.split(v.countDownPos, "#")
        data.fruitId = StringUtil.split(v.fruitId, "#")
        data.chance = StringUtil.split(v.chance, "#")
        data.type = tonumber(v.type)
        data.level = tonumber(v.level)
        data.treePos = Tools.CastToVector3(v.treex, v.treey, v.treez)
        data.treeYaw = tonumber(v.treeYaw)
        data.treeRatio = tonumber(v.treeRatio)
        data.extraReward = tonumber(v.extraReward)
        data.tipId = tonumber(v.tipId)
        self.fields[data.id] = data
    end
end

function FieldConfig:initDoor(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.entityId = tonumber(0)
        data.type = tonumber(v.type)
        data.requirement = tonumber(v.requirement)
        data.minPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.maxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.actorName = v.actorName
        data.bodyName = v.bodyName
        data.bodyId1 = v.bodyId1
        data.bodyId2 = v.bodyId2
        self.door[data.id] = data
    end
end

function FieldConfig:initDoorTip(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.minPos = Tools.CastToVector3i(v.x, v.y, v.z)
        data.maxPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.time = tonumber(v.time)
        data.level = tonumber(v.level)
        data.dialog = v.dialog
        self.doorTip[data.id] = data
    end
end

function FieldConfig:initVipArea(vipArea)
    self.vipArea = Area.new(vipArea)
end

function FieldConfig:initGuideShopArea(shopArea)
    self.shopArea = Area.new(shopArea)
end

function FieldConfig:initTreeWeight(config)
    for i, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.treeWeight = tonumber(v.treeWeight)
        self.treeWeight[data.level] = data
    end
end

function FieldConfig:enterGuideShopArea(player)
    if self.shopArea:inArea(player:getPosition()) then
        player.isShowShopArrow = false
        HostApi.changeGuideArrowStatus(player.rakssid, GameConfig.arrowPointToShopPos, false)
    end
end

function FieldConfig:enterFieldArea(player)
    if player.enterFieldIndex ~= 0 then
        if self.fields[player.enterFieldIndex] ~= nil then
            local fieldPos = self:getAreaPos(self.fields[player.enterFieldIndex])
            if fieldPos:inArea(player:getPosition()) then
                return
            end
        end

        player.enterFieldIndex = 0
        return
    end

    if player.enterFieldIndex == 0 then
        for _, v in pairs(self.fields) do
            local fieldPos = self:getAreaPos(v)
            if fieldPos:inArea(player:getPosition()) then
                player.enterFieldIndex = v.id
                return
            end
        end
    end
end

function FieldConfig:enterDoor(player)
    if player.enterDoorIndex ~= 0 then
        if self.door[player.enterDoorIndex] ~= nil then
            local door = self.door[player.enterDoorIndex]
            local doorPos = self:getAreaPos(door)
            if doorPos:inArea(player:getPosition()) then
                if door.type == 0 and player.userBirdBag.maxCarryBirdNum < door.requirement then
                    player:setHealth(0)
                end

                if door.type == 1 and player.isVipArea == false then
                    player:setHealth(0)
                end
                return
            end
        end

        player.enterDoorIndex = 0
        return
    end

    if player.enterDoorIndex == 0 then
        for _, v in pairs(self.door) do
            local doorPos = self:getAreaPos(v)
            if doorPos:inArea(player:getPosition()) then
                player.enterDoorIndex = v.id
                return
            end
        end
    end

    for _, v in pairs(self.doorTip) do
        local doorTipPos = self:getAreaPos(v)
        local refresh = player:getDoorTipTime(v.id)
        if doorTipPos:inArea(player:getPosition()) and player.userBirdBag.maxCarryBirdNum < v.level then
            if refresh == nil or refresh.isContinue == false or os.time() - refresh.time >= v.time + 2 then
                MsgSender.sendCenterTipsToTarget(player.rakssid, 2, tonumber(v.dialog))
                player:setDoorTipTime(v.id, os.time(), true)
                return
            end
        elseif refresh ~= nil then
            if refresh.isContinue == true then
                player:setDoorTipTime(v.id, refresh.time, false)
                return
            end
        end
    end
end

function FieldConfig:enterVipArea(player)
    if self.vipArea:inArea(player:getPosition()) then
        if player.isInVipArea == false and player.isVipArea == false then
            player.isInVipArea = true
            HostApi.sendCommonTipByType(player.rakssid, 14, "vipArea")
        end
    else
        if player.isInVipArea == true then
            player.isInVipArea = false
        end
    end
end

function FieldConfig:getAreaPos(data)
    local pos = {}
    pos[1] = {}
    pos[1][1] = data.minPos.x
    pos[1][2] = data.minPos.y
    pos[1][3] = data.minPos.z
    pos[2] = {}
    pos[2][1] = data.maxPos.x
    pos[2][2] = data.maxPos.y
    pos[2][3] = data.maxPos.z
    return Area.new(pos)
end

function FieldConfig:getFieldInfo(fieldId)
    if self.fields[fieldId] ~= nil then
        return self.fields[fieldId]
    end

    return nil
end

function FieldConfig:getFieldIdByPos(pos)
    for i, v in pairs(self.fields) do
        local fieldPos = self:getAreaPos(v)
        if fieldPos:inArea(pos) then
            return v.id
        end
    end

    return 0
end

function FieldConfig:getFieldInfoByPos(pos)
    for i, v in pairs(self.fields) do
        local fieldPos = self:getAreaPos(v)
        if fieldPos:inArea(pos) then
            return v
        end
    end

    return nil
end

function FieldConfig:generateCoin(id, vec3)
    if self.fields[id] == nil then
        print("FieldConfig:generateCoin: can not find fields[" .. id .. "] 's info ")
        return
    end

    if #self.fields[id].fruitId ~= #self.fields[id].chance then
        print("FieldConfig:generateCoin: the count of 'fruitId' is not equal to 'chance'")
        return
    end

    for i = 1, #self.fields[id].fruitId do
        if self.fields[id].fruitId[i] ~= "@@@" and self.fields[id].chance[i] ~= "@@@" then
            local randomNum = HostApi.random("fieldBuff", 1, 10000)
            if randomNum <= tonumber(self.fields[id].chance[i]) then
                local num = 1
                local meta = 0
                local lifeTime = 10
                local x = HostApi.random("pos", vec3.x - 2, vec3.x + 2)
                local y = vec3.y + 1
                local z = HostApi.random("pos", vec3.z - 2, vec3.z + 2)
                local pos = VectorUtil.newVector3(x, y, z)
                EngineWorld:addEntityItem(self.fields[id].fruitId[i], num, meta, lifeTime, pos)
            end
        end
    end
end

function FieldConfig:prepareDoorActor()
    for i, v in pairs(self.door) do
        if v.actorName ~= "@@@" and v.bodyName ~= "@@@" and v.bodyId1 ~= "@@@" then
            local entityId = EngineWorld:addSessionNpc(v.pos, v.yaw, 3, v.actorName, function(entity)
                entity:setActorBody(v.bodyName or "body")
                entity:setActorBodyId(v.bodyId1 or "")
                entity:setPerson(true)
            end)
            self.door[i].entityId = entityId
        end
    end
end

function FieldConfig:getTreeWeightByLevel(level)
    for _, v in pairs(self.treeWeight) do
        if tonumber(v.level) == tonumber(level) then
            return v.treeWeight
        end
    end

    return 0
end

function FieldConfig:getGenerateTreeLevel()
    local level1 = 0
    local level2 = 0
    local level3 = 0
    local level4 = 0

    -- check player's nest num
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.userBirdBag ~= nil then
            if v.userBirdBag.maxCarryBirdNum > 9 then
                level4 = level4 + 1
                level3 = level3 + 1
                level2 = level2 + 1
                level1 = level1 + 1
            elseif v.userBirdBag.maxCarryBirdNum > 7 then
                level3 = level3 + 1
                level2 = level2 + 1
                level1 = level1 + 1
            elseif v.userBirdBag.maxCarryBirdNum > 3 then
                level2 = level2 + 1
                level1 = level1 + 1
            else
                level1 = level1 + 1
            end
        end
    end

    if level1 == 0 then
        -- no player in this game
        return 0
    end

    local weight1 = FieldConfig:getTreeWeightByLevel(1)
    local weight2 = FieldConfig:getTreeWeightByLevel(2)
    local weight3 = FieldConfig:getTreeWeightByLevel(3)
    local weight4 = FieldConfig:getTreeWeightByLevel(4)

    if level1 > 0 and level1 < 2 then
        weight1 = weight1 / 2
    end

    if level2 == 0 then
        weight2 = 0
    elseif level2 > 0 and level2 < 2 then
        weight2 = weight2 / 2
    end

    if level3 == 0 then
        weight3 = 0
    elseif level3 > 0 and level3 < 2 then
        weight3 = weight3 / 2
    end

    if level4 == 0 then
        weight4 = 0
    elseif level4 > 0 and level4 < 2 then
        weight4 = weight4 / 2
    end

    local totalWeight = weight1 + weight2 + weight3 + weight4
    local num = HostApi.random("generateTree", 1, totalWeight)

    if num <= weight1 then
        return 1
    end

    if num <= weight1 + weight2 then
        return 2
    end

    if num <= weight1 + weight2 + weight3 then
        return 3
    end

    return 4
end

function FieldConfig:getGenerateFieldId(level)
    if level == 0 then
        return 0
    end

    local field = {}

    for _, v in pairs(self.fields) do
        if tonumber(v.level) == tonumber(level) then
            if SceneManager:isTreeInField(v.id) == false then
                field[#field + 1] = tonumber(v.id)
            end
        end
    end

    if next(field) then
        local id = HostApi.random("generateField", 1, #field)
        if field[id] ~= nil then
            return field[id]
        end

        return 0
    else
        return FieldConfig:getGenerateFieldId(level - 1)
    end

end

return FieldConfig