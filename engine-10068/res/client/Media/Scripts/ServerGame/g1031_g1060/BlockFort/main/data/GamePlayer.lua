require "Match"
require "util.DbUtil"
require "data.GameManor"
require "data.GameSchematic"
require "data.GameDecoration"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self:initUserInfo()
end

function GamePlayer:initUserInfo()
    self.score = 0              -- 平台奖励分数
    self.curManorIndex = 0      -- 当前所在的领地id
    self.manorIndex = 0         -- 领地编号
    self.manorLevel = 1         -- 领地等级
    self.exp = 0                -- 玩家经验
    self.playerLevel = 1        -- 玩家等级
    self.manor = nil
    self.schematic = nil
    self.decoration = nil

    self.bagItems = {}
    self.limitItems = {}
    self.tempDecoration = {}
    self.tempSchematicInfo = {}

    self.isWaiting = false  -- 等待玩家购买结束后，再加入新的订单 (用于防止异步造成的重复购买)
    self.gotPreInstall = 0 -- 未拥有

    -- 引导的数据
    self.running_guides = {}
    self.finish_ids = {}
    self.finish_guideId = 0
    self.isInGuide = true
end

function GamePlayer:setHp()
    self:setHealth(20)
end

function GamePlayer:setDefence()
    if self.entityPlayerMP == nil then
        return
    end
    self.entityPlayerMP:changeDefense(20, 20)
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self:setBagItems(data.items or {})
        self:setLimitItems(data.limit or {})
        self:setManorLevel(data.manorLevel or 1)
        self:setPlayerLevel(data.playerLevel or 1)
        self:setExp(data.exp or 0)
        self:setPreInstall(data.gotPreInstall or 0)
        self:setGuideData(data.guide or {})
    end

    self:setManorInfo()
    self:setManorManager()
    self:setPlayerInfo()
    self:setHp()
    self:setDefence()
    self:sendStoreInfo()
    self:setPreInstallItem(self.rakssid)
    GuideManager:sendDataToClient(self, true)
end

function GamePlayer:prepareDataSaveToDB()
    local data = {
        guide = self:getGuideData(),
        items = self:getBagItems(),
        manorLevel = self:getManorLevel(),
        playerLevel = self:getPlayerLevel(),
        exp = self:getExp(),
        limit = self:getLimitItems(),
        gotPreInstall = self:getPreInstall(),
    }
    return data
end

function GamePlayer:setManorLevel(level)
    if not level then
        return
    end
    self.manorLevel = tonumber(level)
end

function GamePlayer:getManorLevel()
    if self.manor then
        return self.manor.level
    end
    return self.manorLevel
end

function GamePlayer:setBagItems(items)
    for id, num in pairs(items) do
        self:addBagItem(id, num)
    end
end

function GamePlayer:getBagItems()
    local items = {}
    for id, num in pairs(self.bagItems) do
        items[tostring(id)] = num
    end
    return items
end

function GamePlayer:addBagItem(id, num)
    id = tonumber(id)
    num = tonumber(num)
    if not self.bagItems[id] then
        self.bagItems[id] = num
    else
        self.bagItems[id] = self.bagItems[id] + num
    end

    local itemInfo = StoreConfig:getItemInfoById(id)
    if itemInfo then
        self:addItem(itemInfo.itemId, num)
    end
end

function GamePlayer:subItem(id, num)
    id = tonumber(id)
    if not self.bagItems[id] then
        return
    end
    self.bagItems[id] = self.bagItems[id] - num
    if self.bagItems[id] <= 0 then
        self.bagItems[id] = nil
    end
    local itemInfo = StoreConfig:getItemInfoById(id)
    if itemInfo then
        self:removeItem(itemInfo.itemId, num)
    end
end

function GamePlayer:checkHasBagItem(id, num)
    id = tonumber(id)
    if not self.bagItems[id] then
        return false
    end
    if self.bagItems[id] < num then
        return false
    end
    return true
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

function GamePlayer:setExp(exp)
    if exp <= 0 then
        return
    end
    self.exp = exp
end

function GamePlayer:addExp(exp)
    if exp < 0 then
        return
    end
    local needExp = PlayerLevelConfig:getExpByLevel(self.playerLevel)
    local maxLevel = PlayerLevelConfig:getMaxLevel()
    if self.playerLevel < maxLevel then
        if self.exp + exp >= needExp then
            self.playerLevel = self.playerLevel + 1
            self.exp = self.exp + exp - needExp
            local nextNeedExp = PlayerLevelConfig:getExpByLevel(self.playerLevel)
            if self.exp >= nextNeedExp then
                self:addExp(0)
            end
        else
            self.exp = self.exp + exp
        end
    else
        self.exp = self.exp + exp
    end
end

function GamePlayer:getExp()
    return self.exp
end

function GamePlayer:setPlayerLevel(level)
    if level <= 1 then
        return
    end
    self.playerLevel = level
end

function GamePlayer:getPlayerLevel()
    return self.playerLevel
end

function GamePlayer:setTempDecorationInfo(info)
    self.tempDecoration = info
end

function GamePlayer:setTempSchematicInfo(info)
    self.tempSchematicInfo = info
end

function GamePlayer:rotateSchematic(leftOrRight)
    -- leftOrRight: 1->left. 2->right
    if not next(self.tempSchematicInfo) then
        return
    end

    if not self.schematic then
        return
    end

    local startPos = self.tempSchematicInfo.startPos
    local endPos = self.tempSchematicInfo.endPos
    local fileName = self.tempSchematicInfo.fileName
    local yaw = self.tempSchematicInfo.yaw
    local pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    SceneManager:createOrDestroyHouse(fileName, false, pos, yaw, 0)
    SceneManager:fillAreaBaseBlock(startPos, endPos, self.manorLevel)
    if leftOrRight == 1 then
        yaw = yaw + 90
    end

    if leftOrRight == 2 then
        yaw = yaw - 90
    end

    if yaw > 360 then
        yaw = yaw - 360
    end

    if yaw < 0 then
        yaw = yaw + 360
    end

    SceneManager:createOrDestroyHouse(fileName, true, pos, yaw, 0)
    self.tempSchematicInfo.yaw = yaw
    local offset = ManorConfig:getOffsetByPos(self.manorIndex, startPos, true)
    local offset2 = ManorConfig:getOffsetByPos(self.manorIndex, endPos, true)
    local minOffset, maxOffset = ManorConfig:sortPos(offset, offset2, true)
    local schematicIndex = self.schematic:getSchematicIndexByOffset(minOffset, maxOffset)
    self.schematic:changeOffsetYawByIndex(schematicIndex, yaw)
end

function GamePlayer:rotateDecoration(leftOrRight)
    if not next(self.tempDecoration) then
        return
    end

    if not self.decoration then
        return
    end

    local entityId = self.tempDecoration.entityId
    local yaw = self.tempDecoration.yaw
    local manorIndex = self.tempDecoration.manorIndex
    local newPos = self.tempDecoration.newPos
    local id = self.tempDecoration.id
    if leftOrRight == 1 then
        yaw = yaw + 90
    end

    if leftOrRight == 2 then
        yaw = yaw - 90
    end

    local decorationInfo = StoreConfig:getItemInfoById(id)
    if decorationInfo then
        local x = 0
        local z = 0

        if yaw % 180 == 0 then
            x = decorationInfo.length / 2
            z = decorationInfo.width / 2
        else
            x = decorationInfo.width / 2
            z = decorationInfo.length / 2
        end

        local startPos = VectorUtil.newVector3(newPos.x - x  , newPos.y, newPos.z - z )
        local endPos = VectorUtil.newVector3(newPos.x + x - 1, newPos.y + decorationInfo.height, newPos.z + z - 1 )

        local isCanPlace = true

        for bx = startPos.x, endPos.x do
            for by = startPos.y, endPos.y - 1 do
                for bz = startPos.z, endPos.z do
                    local blockPos = VectorUtil.newVector3i(bx, by, bz)
                    local blockId = EngineWorld:getBlockId(blockPos)
                    if blockId ~= 0 then
                        isCanPlace = false
                        break
                    end
                end
            end
        end

        if not isCanPlace then
            if leftOrRight == 1 then
                yaw = yaw + 90
            end

            if leftOrRight == 2 then
                yaw = yaw - 90
            end
        end
    end

    if yaw > 360 then
        yaw = yaw - 360
    end

    if yaw < 0 then
        yaw = 360 + yaw
    end

    local offsetYaw = ManorConfig:getOffsetYaw(manorIndex, yaw)
    EngineWorld:getWorld():updateDecorationYaw(entityId, yaw)

    self.decoration:updateDecorationYaw(entityId, offsetYaw)
    self.tempDecoration.yaw = yaw
end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
end

function GamePlayer:onQuit()
    if self.manor ~= nil then
        self.manor:setManorName(self.name)
        self.manor:masterLogout()
    end
    self:saveMyLevelData()
end

function GamePlayer:setManorManager()
    if self.manor then
        local maxLevel = ManorLevelConfig:getMaxLevel()
        local curLevel = self.manor:getManorLevel()
        local nextLevel = self.manor:getNextManorLevel()
        local upgradeMoney = ManorLevelConfig:getNextPriceByLevel(curLevel)
        local upgradeMoneyType = ManorLevelConfig:getMoneyTypeByLevel(curLevel)

        local length = 0
        local width = 0
        local height = 0
        local curManorArea = ""
        local nextManorArea = ""

        local startPos, endPos = ManorLevelConfig:getOffsetPosByLevel(curLevel)
        if startPos ~= nil and endPos ~= nil then
            length = endPos.x - startPos.x + 1
            width = endPos.z - startPos.z + 1
            height = endPos.y - startPos.y
            curManorArea = length .. "x" .. width .. "x" .. height
        end

        local curManor = BlockFortManor.new()
        curManor.level = curLevel
        curManor.area = curManorArea

        startPos, endPos = ManorLevelConfig:getOffsetPosByLevel(nextLevel)
        if startPos ~= nil and endPos ~= nil then
            length = endPos.x - startPos.x + 1
            width = endPos.z - startPos.z + 1
            height = endPos.y - startPos.y
            nextManorArea = length .. "x" .. width .. "x" .. height
        end

        local nextManor = BlockFortManor.new()
        nextManor.level = nextLevel
        nextManor.area = nextManorArea

        local unlockIds = {}
        local unlockItems = StoreConfig:getIdsByLevel(nextLevel)
        for _, id in pairs(unlockItems) do
            table.insert(unlockIds, id)
        end

        local manor = BlockFortManorManager.new()
        manor.upgradeMoney = upgradeMoney
        manor.upgradeMoneyType = upgradeMoneyType
        manor.maxLevel = maxLevel
        manor.curManor = curManor
        manor.nextManor = nextManor
        manor.unlockIds = unlockIds

        self.entityPlayerMP:getBlockFort():setManorManager(manor)
    end
end

function GamePlayer:setManorInfo()
    local manorInfo = BlockFortManorInfo.new()

    local startPos = VectorUtil.newVector3(0, 0, 0)
    local endPos = VectorUtil.newVector3(0, 0, 0)

    local canPlaceStartPos = VectorUtil.newVector3(0, 0, 0)
    local canPlaceEndPos = VectorUtil.newVector3(0, 0, 0)

    if self.manor then
        local manorStartPos, manorEndPos = ManorConfig:getManorPosByIndex(self.manor.manorIndex)
        if manorStartPos and manorEndPos then
            startPos = manorStartPos
            endPos = manorEndPos
        end

        local manorCanPlaceStartPos, manorCanPlaceEndPos = ManorConfig:getOpenManorPos(self.manor.level, self.manor.manorIndex, false)
        if manorCanPlaceStartPos and manorCanPlaceEndPos then
            canPlaceStartPos = manorCanPlaceStartPos
            canPlaceEndPos = manorCanPlaceEndPos
        end
    end

    local sortStartPos, sortEndPos = ManorConfig:sortPos(canPlaceStartPos, canPlaceEndPos, false)

    manorInfo.startPos = startPos
    manorInfo.endPos = endPos
    manorInfo.canPlaceStartPos = sortStartPos
    manorInfo.canPlaceEndPos = sortEndPos
    self.entityPlayerMP:getBlockFort():setManorInfo(manorInfo)
end

function GamePlayer:setPlayerInfo()
    local playerInfo = BlockFortPlayerInfo.new()
    playerInfo.exp = self:getExp()
    playerInfo.totalExp = PlayerLevelConfig:getExpByLevel(self:getPlayerLevel())
    playerInfo.level = self:getPlayerLevel()
    self.entityPlayerMP:getBlockFort():setPlayerInfo(playerInfo)
end

function GamePlayer:sendStoreInfo()
    local fortItems = {}
    local wallItems = {}
    local turretItems = {}

    local fortGroups = {}
    local wallGroups = {}
    local turretGroups = {}

    local fortTab = BlockFortStoreTab.new()
    local wallTab = BlockFortStoreTab.new()
    local turretTab = BlockFortStoreTab.new()

    local Stores = {}

    ---------
    for _, v in pairs(StoreConfig.allItems) do
        if v.inStore == InStore.Yes then
            local itemNum = 0
            if v.limit > 0 then
                itemNum = (self.limitItems[v.id] and {self.limitItems[v.id]} or {0})[1]
            else
                itemNum = (self.bagItems[v.id] and {self.bagItems[v.id]} or {0})[1]
            end

            local item = BlockFortStoreItem.new()
            item.id = v.id
            item.itemMate = 0
            item.limit = v.limit
            item.weight = v.weight
            item.itemId = v.itemId
            item.groupId = v.groupId
            item.moneyType = v.moneyType
            item.itemNum = itemNum

            if v.type == BuildingType.Fort then -- type 1: 堡垒  ;
                table.insert(fortItems, item)

            elseif v.type == BuildingType.Wall then -- type 2: 城墙  ;
                table.insert(wallItems, item);

            elseif v.type == BuildingType.Turret then -- type 3: 防御塔  ;
                table.insert(turretItems, item)

            end
        end
    end

    ---------
    for _, v in pairs(TagConfig.tag) do
        local group = BlockFortStoreGroup.new()
        group.groupId = v.tag
        group.groupName = v.name
        group.groupIcon = v.icon

        if v.type == BuildingType.Fort then -- type 1: 堡垒  ;
            table.insert(fortGroups, group)

        elseif v.type == BuildingType.Wall then -- type 2: 城墙  ;
            table.insert(wallGroups, group)

        elseif v.type == BuildingType.Turret then -- type 3: 防御塔  ;
            table.insert(turretGroups, group)

        end

    end

    ---------
    fortTab.tabId = BuildingType.Fort
    fortTab.items = fortItems
    fortTab.groups = fortGroups

    wallTab.tabId = BuildingType.Wall
    wallTab.items = wallItems
    wallTab.groups = wallGroups

    turretTab.tabId = BuildingType.Turret
    turretTab.items = turretItems
    turretTab.groups = turretGroups

    table.insert(Stores, fortTab)
    table.insert(Stores, turretTab)
    table.insert(Stores, wallTab)

    ---------
    self.entityPlayerMP:getBlockFort():setStores(Stores)
end

function GamePlayer:syncStoreInfo(id)
    id = tonumber(id)
    local itemInfo = StoreConfig:getItemInfoById(id)
    if not itemInfo then
        return
    end

    local itemNum = 0
    if itemInfo.limit > 0 then
        itemNum = (self.limitItems[id] and {self.limitItems[id]} or {0})[1]
    else
        itemNum = (self.bagItems[id] and {self.bagItems[id]} or {0})[1]
    end

    local item = BlockFortStoreItem.new()
    item.id = itemInfo.id
    item.limit = itemInfo.limit
    item.weight = itemInfo.weight
    item.itemId = itemInfo.itemId
    item.groupId = itemInfo.groupId
    item.Mate = itemInfo.meta
    item.itemNum = itemNum

    local tabId = itemInfo.type

    PacketSender:getSender():updateBlockFortItem(self.rakssid, tabId, item)
end

function GamePlayer:updateSelfManor()
    self.manor:upgradeLevel()
    self.manorLevel = self.manor:getManorLevel()
    self:setManorManager()
    self:sendStoreInfo()
    self:setManorInfo()
end

function GamePlayer:recallItems(index, type)
    if type == BuildingType.Fort or type == BuildingType.Wall then
        self:recallSchematic(index)
    elseif type == BuildingType.Turret then
        self:recallDecoration(index)
    end
end

function GamePlayer:recallSchematic(index)
    if not self.schematic then
        return
    end

    local schematic = self.schematic:getSchematicByIndex(index)
    if not schematic then
        return
    end

    --self:addSchematic(schematic.id, 1)
    self:addBagItem(schematic.id, 1)
    self.schematic:removeSchematicFromWorld(index, self.manorLevel)
    self.schematic:removeSchematicData(index)
end

function GamePlayer:recallDecoration(entityId)
    if not self.decoration then
        return
    end

    local decoration = self.decoration:getDecorationByEntityId(entityId)
    if not decoration then
        return
    end
    self:addBagItem(decoration.id, 1)
    self.decoration:removeDecorationFromWorld(entityId)
    self.decoration:removeDecorationData(entityId)
end

function GamePlayer:saveMyLevelData()
    local playerLevel = self:getPlayerLevel()
    WebService:UpdateRank(self.userId, GameMatch.gameType, playerLevel, "playerLevel")
end

function GamePlayer:setLimitItems(items)
    for id, num in pairs(items) do
        self:addLimitItem(id, num)
    end
end

function GamePlayer:addLimitItem(id, num)
    id = tonumber(id)
    num = tonumber(num)
    if not self.limitItems[id] then
        self.limitItems[id] = num
    else
        self.limitItems[id] = self.limitItems[id] + num
    end
end

function GamePlayer:getLimitItems()
    local items = {}
    for id, num in pairs(self.limitItems) do
        items[tostring(id)] = num
    end
    return items
end

function GamePlayer:isCanBuying(id)
    id = tonumber(id)
    local storeInfo = StoreConfig:getItemInfoById(id)
    if not storeInfo then
        return false
    end

    if storeInfo.limit == 0 then
        return true
    end

    local gotNum = (self.limitItems[id] and {self.limitItems[id]} or {0})[1]
    if gotNum >= storeInfo.limit then
        return false
    end

    return true
end

function GamePlayer:setIsWaitForPay(status)
    self.isWaiting = status
end

function GamePlayer:getIsWaitForPay()
    return self.isWaiting
end

function GamePlayer:setPreInstall(status)
    self.gotPreInstall = status
end

function GamePlayer:getPreInstall()
    return self.gotPreInstall
end

function GamePlayer:setPreInstallItem(rakssid)
    if self:getPreInstall() == 0 then
        self:setPreInstall(1)
        PreInstallConfig:preparePreInstall(rakssid)
    end
end

function GamePlayer:getGuideData()
    local data = {}
    data.running_guides = self.running_guides
    data.finish_ids = self.finish_ids
    data.finish_guideId = self.finish_guideId
    data.isInGuide = self.isInGuide
    return data
end

function GamePlayer:setGuideData(data)
    if data ~= nil then
        GuideManager:recoverGuide(self, data)
        self.finish_guideId = data.finish_guideId
        self.isInGuide = data.isInGuide
    end
end

function GamePlayer:setIsInGuide(status)
    self.isInGuide = status
end

function GamePlayer:getIsInGuide()
    return self.isInGuide
end

function GamePlayer:guideAnalytics(actionName)
    GameAnalytics:design(self.userId, 0, {actionName})
end

return GamePlayer


