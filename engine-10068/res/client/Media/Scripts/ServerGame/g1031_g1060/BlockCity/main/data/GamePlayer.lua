require "Match"
require "util.DbUtil"
require "util.VideoAdHelper"
require "data.GameManor"
require "data.GameBlock"
require "data.GameDecoration"
require "data.GameArchive"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)
    self:initUserInfo()
end

function GamePlayer:initUserInfo()
    self.score = 0          -- 平台奖励分数
    self.lastScore = 0      -- 领地等级分+方块分+装饰分
    self.curManorIndex = 0  -- 当前所在的领地id
    self.manorIndex = 0
    self.level = 1          -- 领地等级

    self.manor = nil
    self.block = nil
    self.decoration = nil
    self.archive = nil

    self.flyingEndTime = 0      --飞行到期时间
    self.isEditMode = false     --编辑模式
    self.isEditModeChange = false
    self.enterDoorIndex = 0
    self.enterDoorLastIndex = 0
    self.refreshDoorTipTime = {}

    --签到
    self.checkIn = {
        week = 0,
        day = 0,
        totalDays = 0,
        fulfil = 0
    }

    --地图商店
    self.itemStatusInfos = {}
    --翅膀
    self.wingId = nil
    self.custom_wing = 0
    --枪
    self.gunId = nil
    --花
    self.flowerId = nil

    --老虎机剩余时间
    self.tigerLotteryCountdowns = {}

    -- 背包的方块
    self.bagBlocks = {}
    -- 背包的装饰
    self.bagDecoration = {}
    -- 背包的玫瑰
    self.bagRoses = {}

    --放置模板时，需要缓存的数据
    self.tempSchematicInfo = {}
    -- 放置装饰时，需要缓存的数据
    self.tempDecoration = {}

    -- 购买时缺少的材料
    self.lackItems = {}

    -- 缺少的材料时支付后保存的数据(用于防止异步造成的重复购买)
    self.isWaitForPayItems = false

    -- 缓存商店&仓库标签页的数据
    self.isFirstGetStoreTags = true
    self.tempDrawStoreTags = {}
    self.tempBlockStoreTags = {}
    self.tempDecorationStoreTags = {}

    -- 缓存商店&仓库的更新数据
    self.tempStoreChangeData = {}

    self.partner = {}

    -- 工具栏的临时保存
    self.hotBar = {
        edit = {},
        normal = {},
    }

    -- 引导的数据
    self.running_guides = {}
    self.finish_ids = {}
    self.finish_guideId = 0
    self.chooseTemplateId = 0
    self.isInGuide = true
    self.isCameraLocked = false
    --self.templateCorrectOrder = -1

    -- 存档
    self.archiveData = {}
    self:initDefaultArchiveData()

    -- 是否已领取免费飞行
    self.isGotFreeFly = false

    --读取存档时转换过的方块
    self.loadArchiveBlock = {}
    --读取存档时的装饰
    self.loadArchiveDecoration = {}

    -- 保存上次点赞的日期
    self.praiseDay = 0
    -- 今天点过赞的玩家
    self.praiseUserIds = {}

    -- 周榜结算日期
    self.rankWeek = 0

    -- 上周排名 (如果领取过奖励，则置为0)
    self.lastWeekRank = 0

    --保存拆除时门的位置
    self.tempDoorBlockNeedRemove = {}

    --用来保存翅膀的剩余时间
    self.wingsRemainTime = {}

    -- 在线时长奖励游戏币
    self.CDRewardStartTime = 0
    self.CDRewardAvailableTime = 0
    self.CDRewardData = {
        level   = 0,
        cDay    = 0,
    }

    self.money = 0
    self:setCurrency(self.money)

    -- 玩家第一次进入游戏赠送绿钞
    self.isGotFreeR_Currency = false

    -- 广告观看保存时间
    self.tiger1AdWatchTime = 0
    self.tiger2AdWatchTime = 0
    self.flyingAdWatchTime = 0

    --赛车数据
    self.race = {
        isRacing = false,
        competitorUId = nil,
        competitorName = nil,
        isRaceWin = false,
        vEntityId = 0, --vehicle EntityId
        isTakeOffVehicle = false,
        inviteTime = 0,
        inviterUId = nil,
    }
end

function GamePlayer:initDefaultArchiveData()
    if GameConfig.freeArchiveNum < 1 then
        -- 默认最少有一个存档
        self.archiveData[1] = {
            name = GameConfig.archiveName .. 1,
            updateAt = "0"
        }

        if self.archive then
            self.archive:setArchiveBlock(1, {})
            self.archive:setArchiveDecoration(1, {})
        end
        return
    end

    for i = 1, GameConfig.freeArchiveNum do
        self.archiveData[i] = {
            name = GameConfig.archiveName .. i,
            updateAt = "0"
        }
        if self.archive then
            self.archive:setArchiveBlock(i, {})
            self.archive:setArchiveDecoration(i, {})
        end
    end
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self:setCheckInData(data.checkIn or nil)
        self:setBagRoses(data.roses or {})
        self:addStoreItemsHasGot(data.itemWing or {})
        self:setLastScore(data.score or 0)
        self:setPartner(data.partner or {})
        self:setAllTigerLotteryCountdowns(data.tigerLotteryCountdowns or {})
        self:setBagBlocks(data.blocks or {})
        self:setBagDecoration(data.decorations or {})
        self:setFlyingEndTime(data.flyingTime or 0)
        self:setManorLevel(data.level or 1)
        self:setGuideData(data.guide or nil)
        self:setArchiveData(data.archive or  nil)
        self:setIsGotFreeFly(data.isGotFreeFly or false)
        self:setPraiseUserIds(data.praiseUserIds or {})
        self:setPraiseDay(data.praiseDay or 0)
        self:setRankWeek(data.rankWeek or 0)
        self:setWingsRemainTime(data.wingsTime or {})
        self:setMoneyNum(data.money or 0)
        self:setIsGotFreeR_Currency(data.isGotFreeR_Currency or false)
        self:setAdTime(data.adTime or {})
    end

    self:setCDRewardData(data.CDRewardData)
    self:setLastWeekRank()
    self:setPlayerInfo()
    self:setManorInfo()
    self:setArchiveInfo()
    self:sendManorManager()
    self:sendCheckInData()
    TigerLotteryManager:SyncTigerLotteryInfo(self)
    self:sendStoreInfo()
    ShopHouseManager:syncShopInfo(self.rakssid)
    GuideManager:sendDataToClient(self, true)
    self:syncCDRewardData()
    self:presentFreeR_Currency()
    VideoAdHelper:checkShowAd(self)
end

function GamePlayer:prepareDataSaveToDB()
    local data = {
        guide = self:getGuideData(),
        itemWing = self:getStoreItemsHasGot(),
        checkIn = self:getCheckInData(),
        score = self:getThisScore(),
        partner = self:getPartner(),
        tigerLotteryCountdowns = self:getAllTigerLotteryCountdowns(),
        blocks = self:getBagBlocks(),
        decorations = self:getBagDecoration(),
        flyingTime = self:getFlyingEndTime(),
        level = self:getManorLevel(),
        archive = self:getArchiveData(),
        isGotFreeFly = self:getIsGotFreeFly(),
        praiseUserIds = self:getPraiseUserIds(),
        praiseDay = self:getPraiseDay(),
        rankWeek = self:getRankWeek(),
        roses = self:getBagRoses(),
        wingsTime = self:getWingsRemainTime(),
        money = self:getCurrency(),
        CDRewardData = self:getCDRewardData(),
        isGotFreeR_Currency = self:getIsGotFreeR_Currency(),
        adTime = self:getAdTime(),
    }
    return data
end


function GamePlayer:addStoreItemsHasGot(data)
    for k, v in pairs(data) do
        self.itemStatusInfos[tonumber(k)] = v
    end
    for itemId, status in pairs(self.itemStatusInfos) do
        local curItem = ShopHouseManager:getItemInfosByItemId(itemId)
        if curItem ~= nil then
            if status == ShopItemStatus.OnUser then
                if curItem.itemType == ShopItemType.Car then

                elseif curItem.itemType == ShopItemType.Wings then
                    self:setWing(itemId)
                else
                    self:setShopOtherItem(curItem.itemType, itemId, false)
                end
            end
        end
    end
end

function GamePlayer:setPartner(partner)
    for _, userId in pairs(partner) do
        self:addPartner(userId)
    end
end

function GamePlayer:getPartner()
    local partner = {}
    for _, v in pairs(self.partner) do
        table.insert(partner, v)
    end
    return partner
end

function GamePlayer:setBagBlocks(blocks)
    for key, value in pairs(blocks) do
        local block = StringUtil.split(key,":" )
        local id = block[1]
        local meta = block[2]
        local num = value
        self:addBlock(id, meta, num, true)
    end
end

function GamePlayer:getBagBlocks()
    local blocks = {}
    for _, v in pairs(self.bagBlocks) do
        local block = v.id .. ":" .. v.meta
        blocks[block] = v.num
    end
    return blocks
end

function GamePlayer:setBagDecoration(decoration)
    for id, num in pairs(decoration) do
        self:addDecoration(id, num, true)
    end
end

function GamePlayer:getBagDecoration()
    local decorations = {}
    for id, num in pairs(self.bagDecoration) do
        decorations[tostring(id)] = num
    end
    return decorations
end

function GamePlayer:getFlyingEndTime()
    return self.flyingEndTime
end

function GamePlayer:setFlyingEndTime(time)
    self.flyingEndTime = time
end

function GamePlayer:getIsCanFlying()
    return self:getFlyingRemainTime() > 0
end

function GamePlayer:getFlyingRemainTime()
    local remainTime = self.flyingEndTime - os.time()
    if remainTime < 0 then
        remainTime = 0
    end

    return remainTime
end

function GamePlayer:getIsGotFreeFly()
    return self.isGotFreeFly
end

function GamePlayer:setIsGotFreeFly(status)
    self.isGotFreeFly = status
end

function GamePlayer:getBagRoses()
    local roses = {}
    for itemId, num in pairs(self.bagRoses) do
        roses[tostring(itemId)] = num
    end

    return roses
end

function GamePlayer:setBagRoses(roses)
    for itemId, num in pairs(roses) do
        self:addRoseNum(itemId, num, true)
    end
end

function GamePlayer:getWingsRemainTimeById(wingId)
    wingId = tonumber(wingId)
    return self.wingsRemainTime[wingId]
end

function GamePlayer:setWingsRemainTimeById(wingId, time)
    wingId = tonumber(wingId)
    self.wingsRemainTime[wingId] = time
end

function GamePlayer:getWingsRemainTime()
    local item = {}
    for wingId, startTime in pairs(self.wingsRemainTime) do
        item[tostring(wingId)] = startTime
    end
    return item
end

function GamePlayer:setWingsRemainTime(wingsRemainTime)
    for wingId, startTime in pairs(wingsRemainTime) do
        self:setWingsRemainTimeById(wingId, startTime)
    end
end

function GamePlayer:getAdTime()
    local data = {
        tiger1Time = self:getTiger1AdWatchTime(),
        tiger2Time = self:getTiger2AdWatchTime(),
        flyingTime = self:getFlyingAdWatchTime()
    }
    return data
end

function GamePlayer:setAdTime(data)
    self:setTiger1AdWatchTime(data.tiger1Time or 0)
    self:setTiger2AdWatchTime(data.tiger2Time or 0)
    self:setFlyingAdWatchTime(data.flyingTime or 0)
end

function GamePlayer:getTiger1AdWatchTime()
    return self.tiger1AdWatchTime
end

function GamePlayer:setTiger1AdWatchTime(time)
    self.tiger1AdWatchTime = time
end

function GamePlayer:getTiger2AdWatchTime()
    return self.tiger2AdWatchTime
end

function GamePlayer:setTiger2AdWatchTime(time)
    self.tiger2AdWatchTime = time
end

function GamePlayer:getFlyingAdWatchTime()
    return self.flyingAdWatchTime
end

function GamePlayer:setFlyingAdWatchTime(time)
    self.flyingAdWatchTime = time
end

function GamePlayer:addRoseNum(itemId, num, notSyncShop)
    itemId = tonumber(itemId)
    num = tonumber(num)
    if num <= 0 then
        return
    end

    if RoseConfig:isRoseItem(itemId) then
        if not self.bagRoses[itemId] then
            self.bagRoses[itemId] = num
        else
            self.bagRoses[itemId] = self.bagRoses[itemId] + num
        end

        notSyncShop = (notSyncShop and {notSyncShop} or {false})[1]
        if not notSyncShop then
            ShopHouseManager:syncSingleShop(self.rakssid, ShopItemType.Flower, itemId)
        end
    end
end

function GamePlayer:subRoseNum(itemId, num)
    local removeHotBar = false
    itemId = tonumber(itemId)
    num = tonumber(num)

    if num <= 0 then
        return
    end

    if not self.bagRoses[itemId] then
        return
    end

    self.bagRoses[itemId] = self.bagRoses[itemId] - num
    if self.bagRoses[itemId] <= 0 then
        self.bagRoses[itemId] = nil
        removeHotBar = true
    end

    ShopHouseManager:syncSingleShop(self.rakssid, ShopItemType.Flower, itemId, removeHotBar)
end

function GamePlayer:addPraiseUserId(userId)
    table.insert(self.praiseUserIds, tostring(userId))
end

function GamePlayer:setPraiseUserIds(userIds)
    for _, userId in pairs(userIds) do
        self:addPraiseUserId(userId)
    end
end

function GamePlayer:getPraiseUserIds()
    return self.praiseUserIds
end

function GamePlayer:getPraiseDay()
    return self.praiseDay
end

function GamePlayer:setPraiseDay(day)
    if day ~= DateUtil.getYearDay() then
        self.praiseUserIds = {}
    end

    self.praiseDay = DateUtil.getYearDay()
end

function GamePlayer:setRankWeek(week)
    self.rankWeek = week
end

function GamePlayer:getRankWeek()
    return self.rankWeek
end

function GamePlayer:setManorLevel(level)
    if not level then
        return
    end

    self.level = tonumber(level)
end

function GamePlayer:getManorLevel()
    if self.manor then
        return self.manor.level
    end

    return self.level
end

function GamePlayer:getArchiveData()
    local archiveData = {}
    for _, v in pairs(self.archiveData) do
        local data = {}
        data.name = v.name or GameConfig.archiveName .. 1
        data.updateAt = v.updateAt or "0"
        table.insert(archiveData, data)
    end
    return archiveData
end

function GamePlayer:setLastScore(score)
    self.lastScore = tonumber(score)
end

function GamePlayer:setLastWeekRank()
    local rankWeek = tostring(self:getRankWeek())
    local thisWeek = tostring(DateUtil.getYearWeek())
    if rankWeek == "0" then
        self.lastWeekRank = 0
        self:setRankWeek(thisWeek)
        return
    end

    local rankYear = string.sub(rankWeek, 1, 4)
    local thisYear = string.sub(thisWeek, 1, 4)
    if tonumber(rankYear) == tonumber(thisYear) then
        if tonumber(thisWeek) - tonumber(rankWeek) ~= 1 then
            self:setRankWeek(thisWeek)
            self.lastWeekRank = 0
            return
        end
    end

    if next(GameMatch.lastWeekRankUserIds) then
        for rank, userId in pairs(GameMatch.lastWeekRankUserIds) do
            if userId == tostring(self.userId) then
                self.lastWeekRank = rank
                break
            end
        end
    end
end

function GamePlayer:setPlayerInfo()
    local playerInfo = BlockCityPlayerInfo.new()
    playerInfo.score = self:getThisScore()
    playerInfo.isCanFlying = self:getIsCanFlying()
    playerInfo.flyingRemainTime = self:getFlyingRemainTime() * 1000
    playerInfo.isEditMode = self.isEditMode
    playerInfo.isGotFreeFly = self:getIsGotFreeFly()
    playerInfo.lastWeekRank = self.lastWeekRank
    self.entityPlayerMP:getBlockCity():setPlayerInfo(playerInfo)
end

function GamePlayer:setManorInfo()
    local manorInfo = BlockCityManorInfo.new()

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

    local minX = math.min(canPlaceStartPos.x, canPlaceEndPos.x)
    local maxX = math.max(canPlaceStartPos.x, canPlaceEndPos.x)
    local minY = math.min(canPlaceStartPos.y, canPlaceEndPos.y)
    local maxY = math.max(canPlaceStartPos.y, canPlaceEndPos.y)
    local minZ = math.min(canPlaceStartPos.z, canPlaceEndPos.z)
    local maxZ = math.max(canPlaceStartPos.z, canPlaceEndPos.z)

    canPlaceStartPos = VectorUtil.newVector3(minX, minY, minZ)
    canPlaceEndPos = VectorUtil.newVector3(maxX, maxY, maxZ)

    manorInfo.startPos = startPos
    manorInfo.endPos = endPos
    manorInfo.canPlaceStartPos = canPlaceStartPos
    manorInfo.canPlaceEndPos = canPlaceEndPos
    manorInfo.flyingInfo = FlyingConfig:getFlyingInfo()
    self.entityPlayerMP:getBlockCity():setManorInfo(manorInfo)
end

function GamePlayer:setArchiveInfo()
    local archiveInfo = BlockCityArchive.new()
    local archiveItem = {}
    local archiveNum = 0
    for _, v in pairs(self.archiveData) do
        local item = BlockCityArchiveItem.new()
        item.name = v.name
        item.updateAt = v.updateAt
        table.insert(archiveItem, item)
        archiveNum = archiveNum + 1
    end
    archiveInfo.items = archiveItem
    local nextPrice = 0
    local maxNum = ArchiveConfig:getMaxArchiveNum()
    if archiveNum > 1 then
        nextPrice = ArchiveConfig:getNextPrice(archiveNum)
    end
    archiveInfo.nextPrice = nextPrice
    archiveInfo.maxNum = maxNum
    self.entityPlayerMP:getBlockCity():setArchive(archiveInfo)
end

function GamePlayer:getStoreItemsHasGot()
    local data = {}
    for k, v in pairs(self.itemStatusInfos) do
        data[tostring(k)] = v
    end
    return data
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
end

function GamePlayer:onQuit()
    if self.manor ~= nil then
        self.manor:setManorName(self.name)
        self.manor:masterLogout()
        self:saveMyRankData()
    end

    self:clearHotBar()
    --self.isReady = false
    --SceneManager:deleteCallOnTarget(self.userId)
end

function GamePlayer:toggleEditMode()
    if self:getEditMode() then
        self:setEditMode(false)
    else
        if self.curManorIndex ~= self.manor.manorIndex then
            return
        end
        self:setEditMode(true)
    end
end

function GamePlayer:getEditMode()
    return self.isEditMode
end

function GamePlayer:showHotBarItem(hotBarTab)
    self:clearInv()
    table.sort(hotBarTab, function(a, b)
        return a.index < b.index
    end)

    for _, v in pairs(hotBarTab) do
        self:replaceItem(v.itemId, 1, v.meta, tonumber(v.index))
    end
end

function GamePlayer:setHotBarItem(hotBarTab, itemId, meta, index)
    local num = self:getItemNumById(itemId, meta)
    if num == 0 then
        local tIndex = 0
        if index ~= nil then
            tIndex = index
            self:removeTempHotBarByIndex(index)
        else
            local i = 0
            for _, v in pairs(hotBarTab) do
                if v.index ~= i then
                    break
                end
                i = i + 1
            end
            tIndex = i
        end
        local item = {
            itemId = itemId,
            meta = meta,
            index = tIndex
        }
        table.insert(hotBarTab, item)
        table.sort(hotBarTab, function(a, b)
            return a.index < b.index
        end)

        if (self:getEditMode() and hotBarTab == self.hotBar.normal) or
                (not self:getEditMode() and hotBarTab == self.hotBar.edit) then
            return
        end

        self:replaceItem(itemId, 1, meta, tonumber(tIndex))
    end
end

function GamePlayer:setEditMode(editMode)
    self.isEditMode = editMode

    if editMode then
        if self:getIsCanFlying() then
            self:setAllowFlying(true)
        end
        self:showHotBarItem(self.hotBar.edit)
    else
        if not self.isCameraLocked then
            self:setAllowFlying(false)
        end
        self:sendManorManager()
        self:showHotBarItem(self.hotBar.normal)
    end

    self:setPlayerInfo()
end

function GamePlayer:isHeldItem()
    if self.curManorIndex ~= self.manorIndex then
        return false
    end
    if not self:getEditMode() then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgEditMode())
        return false
    end

    --local heldItemId = self:getHeldItemId()
    --if heldItemId ~= GameConfig.toolId then
    --    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHeldItem())
    --    return false
    --end

    return true
end

function GamePlayer:setWing(id)
    local isCanFly = false
    if id == self.wingId and self.itemStatusInfos[self.wingId] == ShopItemStatus.OnUser then
        self.itemStatusInfos[self.wingId] = ShopItemStatus.NotUse
        isCanFly = false
        self.wingId = nil
    else
        if self.wingId then
            self.itemStatusInfos[self.wingId] = ShopItemStatus.NotUse
        end
        isCanFly = true
        self.wingId = id
        self.itemStatusInfos[self.wingId] = ShopItemStatus.OnUser
    end

    self:setWingFly(id, isCanFly)
end

function GamePlayer:setWingFly(windId, isCanFly)
    local wingInfo = WingsConfig:getWingInfo(windId)
    local speed = 0
    local reduction = 0
    if wingInfo then
        speed = wingInfo.speed
        reduction = wingInfo.reduction
    end

    self:setFlyWing(isCanFly, speed, reduction)
end

function GamePlayer:setShopOtherItem(itemType, itemId, isNeedRemove, index)
    local currentId = nil
    if itemType == ShopItemType.Gun and self.gunId then
        currentId = self.gunId
        self.gunId = nil

    elseif itemType == ShopItemType.Flower and self.flowerId then
        currentId = self.flowerId
        self.flowerId = nil
    end

    if isNeedRemove and itemId == currentId and self:getItemStatusInfos(itemId) == ShopItemStatus.OnUser then
        self:setItemStatusInfos(itemId, ShopItemStatus.NotUse)
        self:removeItem(itemId, 1, 0)
        self:removeTempHotBar(itemId, 0)

    else
        if currentId then
            self:setItemStatusInfos(currentId, ShopItemStatus.NotUse)
        end
        if itemType == ShopItemType.Flower then
            self.flowerId = itemId
        elseif itemType == ShopItemType.Gun then
            self.gunId = itemId
        end
        self:setItemStatusInfos(itemId, ShopItemStatus.OnUser)
        self:addTempHotBar(itemId, 0, self.hotBar.normal, index)
    end
end


function GamePlayer:getItemStatusInfos(itemId)
    local status = self.itemStatusInfos[itemId]
    return status == nil and ShopItemStatus.NotBuy or status
end

function GamePlayer:setItemStatusInfos(itemId, status)
    status = tonumber(status)
    itemId = tonumber(itemId)
    if status == ShopItemStatus.NotBuy then
        self.itemStatusInfos[itemId] = nil
        return
    end
    self.itemStatusInfos[itemId] = status
end

function GamePlayer:getCheckInData()
    local data = {
        cWeek = self.checkIn.week,
        cDay = self.checkIn.day,
        cTDays = self.checkIn.totalDays,
        fulfil = self.checkIn.fulfil
    }
    return data
end

function GamePlayer:getGuideData()
    local data = {}
    data.running_guides = self.running_guides
    data.finish_ids = self.finish_ids
    data.finish_guideId = self.finish_guideId
    data.isInGuide = self.isInGuide
    data.chooseTemplateId = self.chooseTemplateId
    return data
end

local function _getNewYearDay(day)
    day = tostring(day)
    if not day or day == "0" or day == "" then
        return "0"
    end

    if #day < 7 then
        local _day = string.sub(day, 5, #day)
        local _year = string.sub(day, 1, 4)
        day = _year .. (#day == 5 and {"00"} or {"0"})[1] .. _day
    end
    return day
end
function GamePlayer:setCheckInData(data)
    if data ~= nil then
        self.checkIn.week = tonumber(data.cWeek) or 0
        self.checkIn.day = tonumber(_getNewYearDay(data.cDay))
        self.checkIn.totalDays = tonumber(data.cTDays) or 0
        self.checkIn.fulfil = tonumber(data.fulfil) or 0
    end
end

function GamePlayer:setGuideData(data)
    if data ~= nil then
        GuideManager:recoverGuide(self, data)
        self.finish_guideId = data.finish_guideId
        self.isInGuide = data.isInGuide
        self.chooseTemplateId = data.chooseTemplateId
    end
end

function GamePlayer:setArchiveData(data)
    if not data then
        return
    end

    for i, v in pairs(data) do
        self.archiveData[i] = {
            name = v.name or GameConfig.archiveName .. 1,
            updateAt = v.updateAt or "0"
        }
    end
end

function GamePlayer:syncCheckInData()
    local week = DateUtil.getYearWeek()
    local day = _getNewYearDay(DateUtil.getYearDay())

    local needToGetCheckInFulfil = true
    if self.checkIn.totalDays == 7 then
        if self.checkIn.day < tonumber(day) then
            self.checkIn.fulfil = self.checkIn.fulfil + 1
            if self.checkIn.fulfil >= 5 then
                self.checkIn.fulfil = 1
            end
            self.checkIn.totalDays = 0
            needToGetCheckInFulfil = false
        end
    end

    local rewards = {}
    if self.checkIn.fulfil == 0 then
        rewards = CheckInConfig:getRewardDataByFulfil(self.checkIn.fulfil)
        if self.checkIn.totalDays > 0 then
            rewards = self:setCheckInState(rewards, tonumber(day))
        else
            rewards = self:setCheckInStateAsNewWeek(rewards)
        end
    elseif self.checkIn.week == tonumber(week) then
        rewards = CheckInConfig:getRewardDataByFulfil(self.checkIn.fulfil)
        rewards = self:setCheckInState(rewards, tonumber(day))
    else
        if needToGetCheckInFulfil then
            self.checkIn.fulfil = self.checkIn.fulfil + 1
            if self.checkIn.fulfil >= 5 then
                self.checkIn.fulfil = 1
            end
        end
        rewards = CheckInConfig:getRewardDataByFulfil(self.checkIn.fulfil)
        rewards = self:setCheckInStateAsNewWeek(rewards)
    end
    self.checkIn.week = tonumber(week)
    return rewards
end

function GamePlayer:setCheckInState(rewards, day)
    for _, v in pairs(rewards) do
        if v.id <= self.checkIn.totalDays then
            v.state = 3     -- 3 -> already checkIn
        end

        if v.id == self.checkIn.totalDays + 1 then
            if self.checkIn.day < day then
                v.state = 2     -- 2 -> can checkIn now
            end
        end
    end

    return rewards
end

function GamePlayer:setCheckInStateAsNewWeek(rewards)
    self.checkIn.totalDays = 0
    for _, v in pairs(rewards) do
        if v.id == 1 then
            v.state = 2
            break
        end
    end
    return rewards
end

function GamePlayer:doCheckIn()
    local week = DateUtil.getYearWeek()
    local day = _getNewYearDay(DateUtil.getYearDay())

    if self.checkIn.day == tonumber(day) or self.checkIn.totalDays >= 7 then
        return
    end

    local rewards = CheckInConfig:getRewardDataByFulfil(self.checkIn.fulfil)
    local reward = rewards[self.checkIn.totalDays + 1]
    if reward == nil then
        return
    end

    -- todo: add praiseNum (disable for now ...)
    -- self:addRoseNum(GameConfig.roseId, GameConfig.roseNum)

    local itemIdentifier = reward.itemId -- = rewardId --物品的唯一ID
    if reward.type == 1 then --方块
        if itemIdentifier == 0 then
            itemIdentifier = self:getRandomItem(BlockConfig.block, reward.level)
        end
        if itemIdentifier ~= 0 then
            local blockInfo = BlockConfig:getBlockById(itemIdentifier)
            if blockInfo then
                self:addBlock(blockInfo.blockId, blockInfo.meta, reward.num)
                HostApi.notifyGetItem(self.rakssid, blockInfo.blockId, blockInfo.meta, reward.num)
            end
        end
    elseif reward.type == 2 then --装饰
        if itemIdentifier == 0 then
            itemIdentifier = self:getRandomItem(DecorationConfig.decoration, reward.level)
        end
        if itemIdentifier ~= 0 then
            self:addDecoration(itemIdentifier, reward.num)
            local decoration = DecorationConfig.decoration[itemIdentifier]
            HostApi.notifyGetItem(self.rakssid, decoration.itemId, 0, reward.num)
        end
    elseif reward.type == 3 then --图纸
        if itemIdentifier == 0 then
            itemIdentifier = self:getRandomItem(DrawingConfig.drawing, reward.level)
        end
        if itemIdentifier ~= 0 then
            local data = DrawingConfig:getBlockDataByDrawingId(itemIdentifier)
            for _, v in pairs(data) do
                self:addBlock(v.blockId, v.meta, v.num)
            end
            local draw = DrawingConfig:getDrawingById(itemIdentifier)
            if draw.isFake == 1 then
                HostApi.notifyGetItem(self.rakssid, draw.itemId, 0, reward.num)
            elseif draw.isFake == 0 then
                HostApi.notifyGetGoods(self.rakssid, reward.icon, reward.num)
            end
        end
    elseif reward.type == 4 then --翅膀
        ShopHouseManager:syncSingleShop(self.rakssid, ShopItemType.Wings, reward.itemId)

    elseif reward.type == 5 then --给钱
        self:addMoney(reward.num)

    end

    self.checkIn.week = tonumber(week)
    self.checkIn.day = tonumber(day)
    self.checkIn.totalDays = self.checkIn.totalDays + 1
    if self.checkIn.totalDays >= 7 then
        self.checkIn.totalDays = 7
    end
end

function GamePlayer:getRandomItem(rewardTable, level)
    local itemTable = {}
    local i = 1
    for _, v in pairs(rewardTable) do
        if v.level == tonumber(level) then
            itemTable[i] = v
            i = i + 1
        end
    end
    if not next(itemTable) then -- not next(itemTable) <=> next(itemTable) == nil
        return 0
    end
    -- ...随机返回1到#itemTable = i-1 的一个值
    local index = HostApi.random("", 1, i-1)
    return itemTable[index].id
end

function GamePlayer:sendCheckInData()
    local curTime = os.time()
    local dayTime = 24 * 60 * 60
    local tomorrowTime = curTime + dayTime
    local tomorrowDate = os.date("*t", tomorrowTime)
    local timeLeft = os.time({year = tomorrowDate.year, month = tomorrowDate.month, day = tomorrowDate.day, hour = 0}) - curTime

    local data = {
        data = self:syncCheckInData(),
        timeLeft = timeLeft * 1000,
        needGuide = self:getIsInGuide(),
    }
    HostApi.sendCheckinData(self.rakssid, json.encode(data))
end

function GamePlayer:setTigerLotteryCountdowns(tigerLotteryId, time)
    self.tigerLotteryCountdowns[tigerLotteryId] = time
end

function GamePlayer:setAllTigerLotteryCountdowns(datas)
    for id, time in pairs(datas) do
        self:setTigerLotteryCountdowns(id, time)
    end
end

function GamePlayer:getTigerLotteryCountdowns(tigerLotteryId)
    return self.tigerLotteryCountdowns[tigerLotteryId]
end

function GamePlayer:getAllTigerLotteryCountdowns()
    local data = {}
    for id, time in pairs(self.tigerLotteryCountdowns) do
        data[id] = time
    end
    return data
end

function GamePlayer:addPartner(userId)
    self.partner[tostring(userId)] = userId
end

function GamePlayer:removePartner(userId)
    self.partner[tostring(userId)] = nil
end

function GamePlayer:checkPartner(userId)
    if self.partner[tostring(userId)] then
        return true
    end

    return false
end

function GamePlayer:lackingBlock(id, meta, totalNum)
    local key = id .. ":" .. meta
    local uniqueId = 0
    local haveNum = 0
    local price = 0
    if self.bagBlocks[key] then
        haveNum = self.bagBlocks[key].num
    end

    local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(id, meta)
    local blockInfo = BlockConfig:getBlockInfo(id, masterMeta)
    if blockInfo then
        uniqueId = blockInfo.id
        price = blockInfo.price * (totalNum - haveNum)
    end

    local lackBlock = {
        id = uniqueId,
        itemId = id,
        itemMeta = masterMeta,
        haveNum = haveNum,
        needNum = totalNum,
        price = price,
    }

    return lackBlock
end

function GamePlayer:checkHasBlocks(blocks)
    local price = 0
    local items = {}
    for _, v in pairs(blocks) do
        if not self:checkHasBlock(v.id, v.meta, v.num) then
            local lackBlock = {}
            lackBlock = self:lackingBlock(v.id, v.meta, v.num)
            price = price + lackBlock.price
            table.insert(items, lackBlock)
        end
    end

    local buyLackItems = {}
    buyLackItems.items = items
    buyLackItems.price = price
    buyLackItems.type = 1       --  1是图纸
    buyLackItems.priceCube = math.ceil(price / 100)
    return buyLackItems
end

function GamePlayer:checkHasBlock(id, meta, num)
    if not id or id <= 0 or not num or num <= 0 then
        return true
    end

    local key = id .. ":" .. meta
    if not self.bagBlocks[key] then
        return false
    end

    if self.bagBlocks[key].num < num then
        return false
    end

    return true
end

function GamePlayer:addBlock(id, meta, num, isFirst)
    id = tonumber(id)
    meta = tonumber(meta)
    num = tonumber(num)
    isFirst = isFirst or false

    if num <= 0 then
        return
    end

    local key = id .. ":" .. meta
    if not self.bagBlocks[key] then
        self.bagBlocks[key] = {
            id = id,
            meta = meta,
            num = num
        }
    else
        self.bagBlocks[key].num = self.bagBlocks[key].num + num
    end

    if not isFirst then
        local item = BlockCityStoreItem.new()
        item.itemId = id
        item.itemMate = meta
        item.num = self.bagBlocks[key].num
        table.insert(self.tempStoreChangeData, item)
        SceneManager:addStoreInfoList(self.userId)
    end
end

function GamePlayer:subBlocks(blocks)
    for _, v in pairs(blocks) do
        self:subBlock(v.id, v.meta, v.num)
    end
end

function GamePlayer:subBlock(id, meta, num)
    local key = id .. ":" .. meta
    if not self.bagBlocks[key] then
        return
    end
    if num <= 0 then
        return
    end
    self.bagBlocks[key].num = self.bagBlocks[key].num - num
    if self.bagBlocks[key].num <= 0 then
        self.bagBlocks[key] = nil
    end
    local item = BlockCityStoreItem.new()
    item.itemId = id
    item.itemMate = meta
    if not self.bagBlocks[key] then
        item.num = 0
    else
        item.num = self.bagBlocks[key].num
    end
    table.insert(self.tempStoreChangeData, item)
    SceneManager:addStoreInfoList(self.userId)
end

function GamePlayer:checkHasDecoration(id, num)
    local items = {}
    local price = 0

    if not self.bagDecoration[tonumber(id)] or self.bagDecoration[tonumber(id)] < num then
        local itemId = 0
        local needNum = 0
        local haveNum = 0

        if self.bagDecoration[tonumber(id)] then
            haveNum = self.bagDecoration[tonumber(id)]
            needNum = num - self.bagDecoration[tonumber(id)]
        else
            needNum = num
        end

        local decorationInfo = DecorationConfig:getDecorationInfo(id)
        if decorationInfo then
            itemId = decorationInfo.itemId
            price = decorationInfo.price * (needNum - haveNum)
        end

        local lackDecoration = {
            id = id,
            itemId = itemId,
            needNum = needNum,
            haveNum = haveNum,
            itemMeta = -1,
            price = price
        }

        table.insert(items, lackDecoration)
    end

    local buyLackItems = {}
    buyLackItems.type = 3       -- 3是装饰
    buyLackItems.items = items
    buyLackItems.price = price
    buyLackItems.priceCube = math.ceil(price / 100)
    return buyLackItems
end

function GamePlayer:addDecoration(id, num, isFirst)
    id = tonumber(id)
    if not self.bagDecoration[id] then
        self.bagDecoration[id] = num
    else
        self.bagDecoration[id] = self.bagDecoration[id] + num
    end
    isFirst = isFirst or false
    if not isFirst then
        local decoration = DecorationConfig:getDecorationInfo(id)
        if decoration then
            local item = BlockCityStoreItem.new()
            item.id = id
            item.num = self.bagDecoration[id]
            item.itemId = decoration.itemId
            item.itemMate = -2
            table.insert(self.tempStoreChangeData, item)
            SceneManager:addStoreInfoList(self.userId)
        end
    end
end

function GamePlayer:subDecoration(id, num)
    id = tonumber(id)
    if not self.bagDecoration[id] then
        return
    end

    self.bagDecoration[id] = self.bagDecoration[id] - num
    if self.bagDecoration[id] <= 0 then
        self.bagDecoration[id] = nil
    end
    local decoration = DecorationConfig:getDecorationInfo(id)
    if decoration then
        local item = BlockCityStoreItem.new()
        item.id = id
        item.num = self.bagDecoration[id] or 0
        item.itemId = decoration.itemId
        item.itemMate = -2
        table.insert(self.tempStoreChangeData, item)
        SceneManager:addStoreInfoList(self.userId)
    end
end

function GamePlayer:buyDrawing(id)
    local drawingBlocks = DrawingConfig:getBlockDataByDrawingId(id)
    if drawingBlocks ~= nil then
        for _, v in pairs(drawingBlocks) do
            self:addBlock(v.blockId, v.meta, v.num)
        end
    end
end

function GamePlayer:getThisScore()
    if not self.manor then
        return self.lastScore
    end

    local blockScore = 0
    local manorScore = 0
    local decorationScore = 0

    if self.block then
        blockScore = self.block:getBlockScore()
    end

    if self.manor then
        manorScore = self.manor:getManorScore()
    end

    if self.decoration then
        decorationScore = self.decoration:getDecorationScore()
    end

    return blockScore + manorScore + decorationScore
end

-- 通过相减获得增量，上报给App层
--function GamePlayer:getScoreChangeValue()
--    return self:getThisScore() - self.lastScore
--end

function GamePlayer:choiceTigerLotteryItem(selectId)
    local itemInfo = TigerLotteryItems:GetOneLineTigerLotteryItemInfoById(selectId)
    if itemInfo ~= nil then
        local itemType = tonumber(itemInfo.type)
        local id = tonumber(itemInfo.itemId)
        local itemMeta = tonumber(itemInfo.itemIdMate)
        local itemNum = tonumber(itemInfo.num)
        if itemType == 1 then --方块
            self:addBlock(id, itemMeta, itemNum)
            HostApi.notifyGetItem(self.rakssid, id, itemMeta, itemNum)
        elseif itemType == 2 then --装饰
            -- 对于装饰，id与Decoration.csv里唯一id相一致！！
            self:addDecoration(id, itemNum)
            local decoration = DecorationConfig.decoration[id]
            HostApi.notifyGetItem(self.rakssid, decoration.itemId, 0, itemNum)
        elseif itemType == 3 then --图纸
            -- 对于图纸，id与Drawing.csv里唯一id相一致！！
            for i=1, itemNum, 1 do
                local data = DrawingConfig:getBlockDataByDrawingId(id)
                for _, v in pairs(data) do
                    self:addBlock(v.blockId, v.meta, v.num)
                end
            end
            local draw = DrawingConfig:getDrawingById(id)
            HostApi.notifyGetItem(self.rakssid, draw.itemId, 0, itemNum)
        end
    end
end

function GamePlayer:sendManorManager()
    if self.manor then
        local maxLevel = ManorLevelConfig:getMaxLevel()
        local curLevel = self.manor:getManorLevel()
        local nextLevel = self.manor:getNextManorLevel()
        local manorScore = ManorLevelConfig:getScoreByLevel(curLevel)
        local nextManorScore = ManorLevelConfig:getScoreByLevel(nextLevel)
        local blockScore = self.block:getBlockScore()
        local decorateScore = self.decoration:getDecorationScore()
        local upgradeMoney = ManorLevelConfig:getNextPriceByLevel(curLevel)
        local upgradeMoneyType = ManorLevelConfig:getMoneyTypeByLevel(curLevel)
        local deleteMoneyType = ManorLevelConfig:getMoneyTypeToDelByLevel(curLevel)
        local deleteMoney = ManorLevelConfig:getPriceToDeleteByLevel(curLevel)

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

        local curManor = BlockCityManor.new()
        curManor.level = curLevel
        curManor.area = curManorArea
        curManor.score = manorScore

        startPos, endPos = ManorLevelConfig:getOffsetPosByLevel(nextLevel)
        if startPos ~= nil and endPos ~= nil then
            length = endPos.x - startPos.x + 1
            width = endPos.z - startPos.z + 1
            height = endPos.y - startPos.y
            nextManorArea = length .. "x" .. width .. "x" .. height
        end

        local nextManor = BlockCityManor.new()
        nextManor.level = nextLevel
        nextManor.area = nextManorArea
        nextManor.score = nextManorScore

        local manor = BlockCityManorManager.new()
        manor.blockScore = blockScore
        manor.decorateScore = decorateScore
        manor.allScore = manorScore + blockScore + decorateScore
        manor.openStatus = 1
        manor.upgradeMoney = upgradeMoney
        manor.upgradeMoneyType = upgradeMoneyType
        manor.deleteMoneyType = deleteMoneyType
        manor.deleteMoney = deleteMoney
        manor.maxLevel = maxLevel
        manor.curManor = curManor
        manor.nextManor = nextManor

        self.entityPlayerMP:getBlockCity():setManorManager(manor)
    end
end

function GamePlayer:deleteEverythingInManor()
    if self.block then
        local delBlocks = self.block.blocks
        if next(delBlocks) then
            for _, v in pairs(delBlocks) do
                -- 判断门的情况
                if v.id ~= 64 then
                    if v.id == 75 then
                        v.id = 76
                    end

                    if v.id == 124 then
                        v.id = 123
                    end

                    local num = 1
                    if BlockConfig:isDoubleSlabBlock(v.id) then
                        v.id = BlockConfig:getSingleSlabIdByDoubleSlabId(v.id)
                        num = 2
                    end
                    local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(v.id, v.meta)
                    self:addBlock(v.id, masterMeta, num)
                    self.block:removeBlock(v.offset, v.id, v.meta)
                else
                    local vec3 = ManorConfig:getPosByOffset(self.manorIndex, v.offset, true)
                    if vec3 then
                        local sign = VectorUtil.hashVector3(vec3)
                        self.tempDoorBlockNeedRemove[sign] = false
                        SceneManager:removeDoor(self, self.block, vec3, v.offset, v.id, v.meta)
                    end
                end
            end
            self.block.blockScore = 0
        end

        self.block:removeFromWorld()
    end

    if self.decoration then
        local delDecorations = self.decoration.decoration
        if next(delDecorations) then
            self.decoration:removeFromWorld()
            for i, v in pairs(delDecorations) do
                self:addDecoration(v.id, 1)
                self.decoration:removeDecoration(v.entityId)
            end
            self.decoration.decorationScore = 0
            GameMatch.isDecorationChange = true
        end
    end

    self:sendManorManager()

    self.tempDecoration = {}
    self.tempSchematicInfo = {}

    local manorInfo = ManorConfig:getManorInfoByIndex(self.manorIndex)
    if manorInfo then
        EngineWorld:addSimpleEffect(manorInfo.effectName, VectorUtil.newVector3(manorInfo.effectX, manorInfo.effectY, manorInfo.effectZ),
                manorInfo.effectYaw, manorInfo.effectTime, 0, manorInfo.effectScale)
    end

    -- 一键清空时保存一下数据，减少下次数据同步
    DbUtil:clearPlayerBlockData(self)
    DBManager:savePlayerData(self.userId, DbUtil.TAG_DECORATION, {}, false)
    DbUtil:savePlayerGameData(self, false)
    DbUtil:savePlayerManorData(self, false)
end

function GamePlayer:updateSelfManor()
    self.manor:upgradeLevel()
    self.level = self.manor:getManorLevel()
    self:sendManorManager()
    self:sendStoreInfo()
    self:setManorInfo()
end

function GamePlayer:setTempSchematicInfo(info)
    self.tempSchematicInfo = info
end

function GamePlayer:setTempDecorationInfo(info)
    self.tempDecoration = info
end

function GamePlayer:rotateSchematic(leftOrRight, master)
    -- leftOrRight: 1->left. 2->right
    if not next(self.tempSchematicInfo) then
        return
    end

    if not master.block then
        return
    end

    local startPos = self.tempSchematicInfo.startPos
    local endPos = self.tempSchematicInfo.endPos
    local fileName = self.tempSchematicInfo.fileName
    local manorIndex = self.tempSchematicInfo.manorIndex
    local rotation = self.tempSchematicInfo.rotation
    local pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    local drawingId = self.tempSchematicInfo.drawingId

    -- 只用fillAreaByBlockIdAndMate清空范围方块, 会导致花朵等特殊的方块同步有一帧的延迟
    -- 所以需要用createOrDestroyHouse清空方块

    SceneManager:createOrDestroyHouse(fileName, false, pos, 0, rotation)
    if leftOrRight == 1 then
        rotation = rotation + 1
    end

    if leftOrRight == 2 then
        rotation = rotation - 1
    end

    if rotation < 1 then
        rotation = 4
    end

    if rotation > 4 then
        rotation = 1
    end


    local startOffset = ManorConfig:getOffsetByPos(manorIndex, startPos, true)
    local endOffset = ManorConfig:getOffsetByPos(manorIndex, endPos, true)

    local minX = math.min(startOffset.x, endOffset.x)
    local maxX = math.max(startOffset.x, endOffset.x)
    local minY = math.min(startOffset.y, endOffset.y)
    local maxY = math.max(startOffset.y, endOffset.y)
    local minZ = math.min(startOffset.z, endOffset.z)
    local maxZ = math.max(startOffset.z, endOffset.z)

    -- 回收范围内的方块
    for x = minX, maxX do
        for y = minY, maxY do
            for z = minZ, maxZ do
                local id, meta = master.block:checkHasBlock(VectorUtil.newVector3i(x, y, z))
                if id ~= 0 then
                    master.block:removeBlock(VectorUtil.newVector3i(x, y, z), id, meta)
                end
            end
        end
    end

    SceneManager:removeBlockFromWorld(startPos, endPos)
    SceneManager:createOrDestroyHouse(fileName, true, pos, 0, rotation)
    self.tempSchematicInfo.rotation = rotation
    self:rotateAnalytics(drawingId, leftOrRight)
end

function GamePlayer:rotateDecoration(leftOrRight, master)
    if not next(self.tempDecoration) then
        return
    end

    if not master.decoration then
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

    local decorationInfo = DecorationConfig:getDecorationInfo(id)
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

    master.decoration:updateDecorationYaw(entityId, offsetYaw)
    self.tempDecoration.yaw = yaw

    if decorationInfo and decorationInfo.isInteraction == 1 then
        GameMatch.isDecorationChange = true
    end
    self:rotateAnalytics(id, leftOrRight)
end

function GamePlayer:checkShutDownEditMode()
    if not self.manor then
        return
    end

    if not self.isEditMode then
        return
    end

    if self.curManorIndex ~= self.manor.manorIndex then
        self:setEditMode(false)
    end
end

function GamePlayer:getDoorTipTime(sign)
    if self.refreshDoorTipTime[sign] ~= nil then
        return self.refreshDoorTipTime[sign]
    end

    return nil
end

function GamePlayer:setDoorTipTime(sign, time, isContinue)
    if self.refreshDoorTipTime[sign] == nil then
        self.refreshDoorTipTime[sign] = {}
    end

    self.refreshDoorTipTime[sign].time = time
    self.refreshDoorTipTime[sign].isContinue = isContinue
end

function GamePlayer:sendStoreInfo(isResetOrder)
    local items = {}
    isResetOrder = isResetOrder or false
    local unLockIndex = 1
    for _, draw in pairs(DrawingConfig.drawing) do
        if draw.isFake == 1 then
            local limit = 0
            local score = 0
            local itemMeta = -1
            local groupPrice = 0
            local groupPriceCube = 0
            local id = draw.id
            local weight = draw.order
            local isNew = draw.isNew
            local isHot = draw.isHot
            if self:getIsInGuide() and self.chooseTemplateId == id and isResetOrder then
                weight = -1
            end
            local moneyType = draw.moneyType
            local groupId = draw.type
            local itemId = draw.itemId
            local onePrice = draw.price
            local onePriceCube = math.ceil(draw.price / 100)
            local volume = draw.length .. "x" .. draw.width .. "x" .. draw.height
            local minimumDrawNum = 0
            if self.level < draw.level then
                limit = draw.level
                minimumDrawNum = 0
            else
                limit = 0
            end
            local materials = {}
            local drawData = DrawingConfig:getBlockDataByDrawingId(id)
            for _, v in pairs(drawData) do
                local haveNum = 0
                local meta = v.meta
                local needNum = v.num
                local drawDataId = v.id
                local blockId = v.blockId
                local block = BlockConfig:getBlockInfo(blockId, meta)
                score = score + BlockConfig:getScoreByOnlyId(block.id) * needNum
                local key = blockId .. ":" .. meta
                if self.bagBlocks[key] then
                    haveNum = self.bagBlocks[key].num
                end

                local formula = BlockCityMaterial.new()
                formula.id = drawDataId
                formula.itemMate = meta
                formula.itemId = blockId
                formula.needNum = needNum
                formula.haveNum = haveNum
                table.insert(materials, formula)
            end
            local item = BlockCityStoreItem.new()
            item.id = id
            item.score = score
            item.limit = limit
            item.isNew = isNew
            item.isHot = isHot
            item.itemId = itemId
            item.volume = volume
            item.weight = weight
            item.groupId = groupId
            item.itemMate = itemMeta
            item.onePrice = onePrice
            item.num = minimumDrawNum
            item.materials = materials
            item.moneyType = moneyType
            item.groupPrice = groupPrice
            item.onePriceCube = onePriceCube
            item.groupPriceCube = groupPriceCube
            if item.limit ~= 0 then
                table.insert(items, item)
            else
                table.insert(items, unLockIndex, item)
                unLockIndex = unLockIndex + 1
            end
        end
    end
    local drawStore = BlockCityStoreTab.new()
    drawStore.tabId = 1  --图纸
    drawStore.items = items

    ----------------------------------------------

    items = {}
    for _, block in pairs(BlockConfig.block) do
        local itemBlock = BlockCityStoreItem.new()
        itemBlock.limit = 0
        itemBlock.id = block.id
        itemBlock.isNew = block.isNew
        itemBlock.materials = {}
        itemBlock.score = block.score
        itemBlock.groupId = block.type
        itemBlock.itemMate = block.meta
        itemBlock.weight = block.weight
        itemBlock.itemId = block.blockId
        itemBlock.onePrice = block.price
        itemBlock.volume = block.feature
        itemBlock.moneyType = block.moneyType
        itemBlock.groupPrice = 64 * block.price
        itemBlock.onePriceCube = math.ceil(block.price / 100)
        itemBlock.groupPriceCube = math.ceil((64 * block.price) / 100)
        itemBlock.num = 0
        local key = block.blockId .. ":" .. block.meta
        if self.bagBlocks[key] then
            itemBlock.num = self.bagBlocks[key].num
        end
        table.insert(items, itemBlock)
    end
    local blockStore = BlockCityStoreTab.new()
    blockStore.tabId = 2   --方塊
    blockStore.items = items

    ----------------------------------------------

    items = {}
    for _, decoration in pairs(DecorationConfig.decoration) do
        local itemDecoration = BlockCityStoreItem.new()
        local limit = 0
        itemDecoration.num = 0
        itemDecoration.limit = 0
        if self.level < decoration.limit then
            limit = decoration.limit
        end
        itemDecoration.limit = limit
        itemDecoration.itemMate = -2
        itemDecoration.groupPrice = 0
        itemDecoration.groupPriceCube = 0
        itemDecoration.materials = {}
        itemDecoration.id = decoration.id
        itemDecoration.isNew = decoration.isNew
        itemDecoration.score = decoration.score
        itemDecoration.groupId = decoration.type
        itemDecoration.weight = decoration.weight
        itemDecoration.itemId = decoration.itemId
        itemDecoration.onePrice = decoration.price
        itemDecoration.moneyType = decoration.moneyType
        itemDecoration.onePriceCube = math.ceil(decoration.price / 100)
        itemDecoration.volume = decoration.length .. "x" .. decoration.width .. "x" .. decoration.height
        if self.bagDecoration[tonumber(decoration.id)] then
            itemDecoration.num = self.bagDecoration[tonumber(decoration.id)]
        end
        table.insert(items, itemDecoration)
    end
    local DecorationStore = BlockCityStoreTab.new()
    DecorationStore.tabId = 3 --裝飾
    DecorationStore.items = items

    ----------------------------------------------

    if self.isFirstGetStoreTags then
        self.isFirstGetStoreTags = false
        for _, v in pairs(TagConfig.tag) do
            local group = BlockCityStoreGroup.new()
            group.groupId = v.tag
            group.groupName = v.name
            group.groupIcon = v.icon
            group.isNew = tonumber(v.isNew)
            if v.type == 1 then
                table.insert(self.tempDrawStoreTags, group)
            elseif v.type == 2 then
                table.insert(self.tempBlockStoreTags, group)
            elseif v.type == 3 then
                table.insert(self.tempDecorationStoreTags, group)
            end
        end
    end

    drawStore.groups = self.tempDrawStoreTags
    blockStore.groups = self.tempBlockStoreTags
    DecorationStore.groups = self.tempDecorationStoreTags

    local Stores = {}
    table.insert(Stores, drawStore)
    table.insert(Stores, blockStore)
    table.insert(Stores, DecorationStore)

    self.entityPlayerMP:getBlockCity():setStores(Stores)
    HostApi.syncBlockCityData(self.rakssid)
end

function GamePlayer:syncStoreInfo()
    if not next(self.tempStoreChangeData) then
        return
    end
    HostApi.updateBlockCityItems(self.rakssid, self.tempStoreChangeData)
    self.tempStoreChangeData = {}
end

function GamePlayer:setBuyLackItems(lackBlocks)
    local items = {}
    for _, v in pairs(lackBlocks.items) do
        local lackItem = BlockCityLackItem.new()
        lackItem.id = v.id
        lackItem.itemId = v.itemId
        lackItem.itemMeta = v.itemMeta
        lackItem.haveNum = v.haveNum
        lackItem.needNum = v.needNum
        table.insert(items, lackItem)
    end

    local buyLackItems = BlockCityBuyLackItems.new()
    buyLackItems.items = items
    buyLackItems.price = lackBlocks.price
    buyLackItems.priceCube = lackBlocks.priceCube
    buyLackItems.type = lackBlocks.type

    self.entityPlayerMP:getBlockCity():setBuyLackItems(buyLackItems)
end

function GamePlayer:setLackItems(lackItems)
    self.lackItems = {}
    self.lackItems = lackItems
end

function GamePlayer:setIsWaitForPayItems(status)
    self.isWaitForPayItems = status
end

function GamePlayer:getIsWaitForPayItems()
    return self.isWaitForPayItems
end

function GamePlayer:saveMyRankData()
    local score = self:getThisScore()
    WebService:UpdateRank(self.userId, GameMatch.gameType, score, "score")
end

function GamePlayer:addTempHotBar(itemId, meta, hotBarTab, index)
    --if self:getEditMode() then
    --    self:setHotBarItem(self.hotBar.edit, itemId, meta, index)
    --else
    --    self:setHotBarItem(self.hotBar.normal, itemId, meta, index)
    --end
    self:setHotBarItem(hotBarTab, itemId, meta, index)
end

function GamePlayer:removeTempHotBar(itemId, meta)
    local hotBar = self.hotBar.edit
    if not self:getEditMode() then
        hotBar = self.hotBar.normal
    end
    for index, v in pairs(hotBar) do
        if itemId == v.itemId and meta == v.meta then
            table.remove(hotBar, index)
            break
        end
    end
end

function GamePlayer:removeTempHotBarByIndex(index)
    index = tonumber(index)
    local hotBar = self.hotBar.edit
    if not self:getEditMode() then
        hotBar = self.hotBar.normal
    end

    for hotBarIndex, v in pairs(hotBar) do
        if index == v.index then
            table.remove(hotBar, hotBarIndex)
            break
        end
    end
end

function GamePlayer:clearHotBar()
    self.hotBar.edit = {}
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

function GamePlayer:buyDrawAnalytics(id, moneyType)
    GameAnalytics:design(self.userId, 0, {"g1047buy5", moneyType, id})
end

function GamePlayer:buyBlocksAnalytics(id, meta, num, moneyType)
    GameAnalytics:design(self.userId, num, {"g1047buy6", moneyType, id, meta})
end

function GamePlayer:buyDecorationAnalytics(id, moneyType)
    GameAnalytics:design(self.userId, 0, {"g1047buy7", moneyType, id})
end

function GamePlayer:buyFlyingAnalytics(day)
    if not self.block then
        return
    end

    GameAnalytics:design(self.userId, self.totalScore, {"g1047buy2", self.level, day, "totalScore"})
    GameAnalytics:design(self.userId, self.block:getTotalBlockNum(), {"g1047buy2", self.level, day, "totalBlockNum"})
end

function GamePlayer:refreshTigerLotteryAnalytics(id)
    GameAnalytics:design(self.userId, 0, {"g1047buy3", id})
end

function GamePlayer:buyLackItemsAnalytics(id, meta, moneyType)
    GameAnalytics:design(self.userId, 0, {"g1047buy4", moneyType, id, meta})
end

function GamePlayer:rotateAnalytics(id, leftOrRight)
    local rotate = "left"
    if leftOrRight == 1 then
        rotate = "left"
    end

    if leftOrRight == 2 then
        rotate = "right"
    end
    GameAnalytics:design(self.userId, id, {"g1047rotate1", rotate})
end

function GamePlayer:invitationAnalytics(isFriend, from)
    local friend = 0
    if isFriend then
        friend = 1
    end
    GameAnalytics:design(self.userId, 0, {"g1047player1", friend, from})
end

function GamePlayer:acceptInvitationAnalytics(isFriend)
    local friend = 0
    if isFriend then
        friend = 1
    end
    GameAnalytics:design(self.userId, 0, {"g1047player2", tostring(friend)})
end

function GamePlayer:visitAnalytics(from, score)
    GameAnalytics:design(self.userId, tonumber(tostring(score)), {"g1047player3", tostring(from)})
end

function GamePlayer:driveCarAnalytics(id)
    GameAnalytics:design(self.userId, 0, {"g1047interactive1", tostring(id)})
end

function GamePlayer:useCannonAnalytics(id)
    GameAnalytics:design(self.userId, 0, {"g1047interactive2", tostring(id)})
end

function GamePlayer:userDecorationAnalytics(id)
    GameAnalytics:design(self.userId, 0, {"g1047interactive3", tostring(id)})
end

function GamePlayer:loadArchiveAnalytics()
    GameAnalytics:design(self.userId, 0, {"g1047loadArchive"})
end

function GamePlayer:setFlyingTime(duration)
    local flyingEndTime = duration + os.time()
    self:setFlyingEndTime(flyingEndTime)
    self:setPlayerInfo()
    if self:getEditMode() and self:getIsCanFlying() then
        self:setAllowFlying(true)
    end
end

function GamePlayer:getArchiveNum()
    return #self.archiveData
end

function GamePlayer:isCanUnlockArchive()
    if self:getArchiveNum() >= GameConfig.maxArchiveNum then
        return false
    end

    return true
end

function GamePlayer:unlockNewArchive()
    if not self:isCanUnlockArchive() then
        return
    end

    local index = self:getArchiveNum() + 1
    local data = {
        name = GameConfig.archiveName .. index,
        updateAt = "0"
    }
    table.insert(self.archiveData, data)
    if self.archive then
        self.archive:setArchiveBlock(index, {})
        self.archive:setArchiveDecoration(index, {})
    end
    self:setArchiveInfo()
end

function GamePlayer:changeArchiveName(archiveId, name)
    if self.archiveData[archiveId] then
        self.archiveData[archiveId].name = name
    end
    self:setArchiveInfo()
end

function GamePlayer:saveArchiveById(archiveId)
    if archiveId > self:getArchiveNum() then
        return
    end

    if not self.block or not self.decoration then
        return
    end

    local blockData = self.block:prepareBlocksToDB()
    local decorationData = self.decoration:prepareDecorationDataSaveToDB()

    DbUtil:savePlayerArchiveData(self, archiveId, blockData, decorationData)

    if self.archiveData[archiveId] then
        if self.archiveData[archiveId].updateAt == "0" then
            self.archiveData[archiveId].name = GameConfig.archiveName .. archiveId
        end
        self.archiveData[archiveId].updateAt = os.date("%Y-%m-%d %H:%M:%S", os.time())
    end

    HostApi.showBlockCityCommonTip(self.rakssid, {}, 0, "gui_blockcity_save_file_success")

    self:setArchiveInfo()
end

function GamePlayer:deleteArchive(archiveId)
    if archiveId > self:getArchiveNum() then
        return
    end

    local blockData = {}
    local decorationData = {}
    DbUtil:savePlayerArchiveData(self, archiveId, blockData, decorationData)

    if self.archiveData[archiveId] then
        self.archiveData[archiveId].name = GameConfig.archiveName .. archiveId
        self.archiveData[archiveId].updateAt = "0"
    end
    self:setArchiveInfo()
end

function GamePlayer:loadArchive(archiveId)
    if archiveId > self:getArchiveNum() then
        return
    end

    if not self.block or not self.decoration or not self.archive then
        return
    end

    -- 1. 清空领地,回收方块和装饰
    -- 2. 检查存档需要多少个方块和装饰(要对特殊的方块转换)
    -- 3. 检测玩家够不够方块和装饰, 不够弹提示:缺少物品和金钱 ,等待玩家购买。
    -- 4. 在领地生成方块和装饰，扣除玩家身上的方块和装饰
    self:deleteEverythingInManor()
    self.loadArchiveBlock = {}
    self.loadArchiveDecoration = {}

    local blockData = self.archive:getArchiveBlockData(archiveId)
    local decorationData = self.archive:getArchiveDecorationData(archiveId)
    local isLackItems = false

    local tempBlockData = {}
    for _, v in pairs(blockData) do
        local value = StringUtil.split(v, ":")
        local id = tonumber(value[1])
        local meta = tonumber(value[2])

        -- 转换特殊的方块
        local needNum = 1
        if id == 64 then
            id = 324
            meta = 0
        elseif id == 75 then
            id = 76
        elseif id == 124 then
            id = 123
        elseif BlockConfig:isDoubleSlabBlock(id) then
            id = BlockConfig:getSingleSlabIdByDoubleSlabId(id)
            needNum = 2
        end

        local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(id, meta)
        local sign = id .. ":" .. masterMeta
        if not tempBlockData[sign] then
            tempBlockData[sign] = needNum
        else
            tempBlockData[sign] = tempBlockData[sign] + needNum
        end
    end

    local lackItemsInfo = {}
    local lackItemsPrice = 0

    if next(tempBlockData) then
        for key, num in pairs(tempBlockData) do
            local value = StringUtil.split(key, ":")
            local id = tonumber(value[1])
            local meta = tonumber(value[2])
            if id == 324 then
                num = num / 2
            end
            local data = {
                id = id,
                meta = meta,
                num = num
            }
            table.insert(self.loadArchiveBlock, data)
        end

        local lackBlock = self:checkHasBlocks(self.loadArchiveBlock)
        if next(lackBlock.items) then
            lackItemsInfo = lackBlock.items
            lackItemsPrice = lackItemsPrice + lackBlock.price
            isLackItems = true
        end
    end

    for _, v in pairs(decorationData) do
        if not self.loadArchiveDecoration[v] then
            self.loadArchiveDecoration[v] = 1
        else
            self.loadArchiveDecoration[v] = self.loadArchiveDecoration[v] + 1
        end
    end

    if next(self.loadArchiveDecoration) then
        for id, num in pairs(self.loadArchiveDecoration) do
            local deco = self:checkHasDecoration(id, num)
            if next(deco.items) then
                table.insert(lackItemsInfo, deco.items[1])
                lackItemsPrice = lackItemsPrice + deco.price
                isLackItems = true
            end
        end
    end

    if isLackItems and next(lackItemsInfo) then
        local items = {
            price = lackItemsPrice,
            type = 1,
            items = lackItemsInfo,
            priceCube = math.ceil(lackItemsPrice / 100)
        }
        self:setBuyLackItems(items)

        local itemInfo = {
            type = 4,       -- 4是读取文档时缺少的方块或装饰
            master = self,
            items = items.items,
            moneyPrice = items.price,
            archiveId = archiveId
        }

        self:setLackItems(itemInfo)
        return
    end

    if next(self.loadArchiveBlock) then
        for _, v in pairs(self.loadArchiveBlock) do
            self:subBlock(v.id, v.meta, v.num)
        end
    end

    if next(self.loadArchiveDecoration) then
        for d_id, d_num in pairs(self.loadArchiveDecoration) do
            self:subDecoration(d_id, d_num)
        end
    end

    self.block:initBlocksData(blockData)
    self.decoration:initDecorationData(decorationData)
    self:loadArchiveAnalytics()
end

function GamePlayer:checkHasPraiseNum()
    if not next(self.bagRoses) then
        HostApi.showBlockCityCommonTip(self.rakssid, {}, 0, "not_enough_praise_num")
        return false
    end

    for id, num in pairs(self.bagRoses) do
        if RoseConfig:isRoseItem(id) and num > 0 then
            return true
        end
    end
    
    HostApi.showBlockCityCommonTip(self.rakssid, {}, 0, "not_enough_praise_num")
    return false
end

function GamePlayer:checkCanPraiseByTargetId(targetId)
    if self.praiseDay ~= DateUtil.getYearDay() then
        self.praiseDay = DateUtil.getYearDay()
        self.praiseUserIds = {}
        return true
    end

    targetId = tostring(targetId)
    for _, userId in pairs(self.praiseUserIds) do
        if targetId == userId then
            return false
        end
    end

    return true
end

function GamePlayer:praiseManor(userId, status)
    -- status: 0是点赞.  1是回赞
    local reward = PraiseRewardConfig:randomReward()
    if not reward then
        return false
    end

    local roseId = 0
    local roseNum = 0
    local heldItemId = self:getHeldItemId()
    if RoseConfig:isRoseItem(heldItemId) then
        local num = self.bagRoses[heldItemId]
        if num and num > 0 then
            roseId = heldItemId
            roseNum = num
        end
    end

    if roseId == 0 or roseNum == 0 then
        for itemId, num in pairs(self.bagRoses) do
            if RoseConfig:isRoseItem(itemId) and num > 0 then
                roseId = itemId
                roseNum = num
                break
            end
        end
    end

    if roseId == 0 or roseNum == 0 then
        HostApi.showBlockCityCommonTip(self.rakssid, {}, 0, "not_enough_praise_num")
        return false
    end

    local praiseNum = RoseConfig:getPraiseNumByItemId(roseId)
    if praiseNum == 0 then
        return false
    end

    ReportManager:reportUserIntegral(userId, praiseNum)
    self:addPraiseUserId(userId)
    self:getRewardItem(reward)
    self:subRoseNum(roseId, 1)

    WebService:SendThumbUpRecord(self.userId, userId, status)
    HostApi.showBlockCityCommonTip(self.rakssid, {}, 0, "praise_manor_success")

    local targetPlayer = PlayerManager:getPlayerByUserId(userId)
    local content = {}
    if targetPlayer then
        HostApi.sendBroadcastMessage(targetPlayer.rakssid, 6, json.encode(content))
    else
        WebService:SendMsg({ userId }, json.encode(content), 6, "user", "g1047")
    end

    return true
end

function GamePlayer:getRewardItem(reward)
    if reward.type == 1 then
        local blocks = DrawingConfig:getBlockDataByDrawingId(reward.uniqueId)
        for _, v in pairs(blocks) do
            self:addBlock(v.blockId, v.meta, v.num * reward.num)
        end

        local draw = DrawingConfig:getDrawingById(reward.uniqueId)
        if not draw then
            return
        end

        if draw.isFake == 1 then
            HostApi.notifyGetItem(self.rakssid, draw.itemId, 0, reward.num)
        elseif draw.isFake == 0 then
            HostApi.notifyGetGoods(self.rakssid, draw.icon, reward.num)
        end
        return
    end

    if reward.type == 2 then
        local block = BlockConfig:getBlockById(reward.uniqueId)
        if block then
            self:addBlock(block.blockId, block.meta, reward.num)
            HostApi.notifyGetItem(self.rakssid, block.blockId, 0, reward.num)
        end
        return
    end

    if reward.type == 3 then
        local decoration = DecorationConfig:getDecorationInfo(reward.uniqueId)
        if decoration then
            self:addDecoration(decoration.id, reward.num)
            HostApi.notifyGetItem(self.rakssid, decoration.itemId, 0, reward.num)
        end
        return
    end

    if reward.type == 4 then
        self:setWingsRemainTimeById(reward.uniqueId, os.time())
        ShopHouseManager:syncSingleShop(self.rakssid, ShopItemType.Wings, reward.uniqueId)
        return
    end

    if reward.type == 5 then
        -- todo:
    end

    if reward.type == 6 then
        -- todo:
    end
end

function GamePlayer:setManorArea()
    local manorArea = {}
    for _, area in pairs(GameMatch.manorAreaInfo) do
        table.insert(manorArea, area)
    end
    self.entityPlayerMP:getBlockCity():setManorArea(manorArea)
end

function GamePlayer:setElevatorArea()
    local elevatorArea = {}
    for _, v in pairs(ElevatorConfig.elevator) do
        local area = BlockCityElevatorArea.new()
        area.id = v.id
        area.floor = v.floor

        local minX = math.min(v.x1, v.x2)
        local minY = math.min(v.y1, v.y2)
        local minZ = math.min(v.z1, v.z2)

        local maxX = math.max(v.x1, v.x2)
        local maxY = math.max(v.y1, v.y2)
        local maxZ = math.max(v.z1, v.z2)

        area.startPos = VectorUtil.newVector3(minX, minY, minZ)
        area.endPos = VectorUtil.newVector3(maxX, maxY, maxZ)
        area.floorIcon = v.floorIcon
        table.insert(elevatorArea, area)
    end

    self.entityPlayerMP:getBlockCity():setElevatorArea(elevatorArea)
end

function GamePlayer:setWeekReward()
    local weekRewards = {}
    local rewards = WeekRewardConfig:getRewardsByRankClassify()
    for _, v in pairs(rewards) do
        local items = {}
        for _, reward in pairs(v.items) do
            local item = BlockCityWeekRewardItem.new()
            if reward.type == 2 then
                local blockInfo = BlockConfig:getBlockById(reward.id)
                if blockInfo then
                    item.id = blockInfo.blockId
                    item.meta = blockInfo.meta
                end
            else
                item.id = reward.id
                item.meta = -1
            end
            table.insert(items, item)
        end  
        local reward = BlockCityWeekReward.new()
        reward.rank = v.rank
        reward.items = items 
        table.insert(weekRewards, reward)
    end

    self.entityPlayerMP:getBlockCity():setWeekReward(weekRewards)
end

function GamePlayer:checkWingsTimeEnd()
    if not next(self.wingsRemainTime) then
        return
    end

    local items = {}
    for wingId, startTime in pairs(self.wingsRemainTime) do
        local itemInfo = ShopHouseManager:getItemInfoByClassifyIdAndItemId(ShopItemType.Wings, wingId)
        if not itemInfo then
            return
        end

        local availableTime = itemInfo.availableTime
        if os.time() - startTime > availableTime then
            table.insert(items, wingId)
        end
    end

    if not next(items) then
        return
    end

    for _, wingId in pairs(items) do
        self:setWingsRemainTimeById(wingId, nil)
        self:setItemStatusInfos(wingId, ShopItemStatus.NotBuy)
        if self.wingId == wingId then
            self.wingId = nil
            self:setWingFly(self.windId, false)
            self:changeClothes("custom_wing", self.custom_wing)
        end
    end

    local mainShop = ShopHouseManager:getMainShopByClassifyId(ShopItemType.Wings)
    if mainShop then
        local packet = mainShop:syncMainShopInfo(self.rakssid)
        HostApi.sendBlockCityPackTab(self.rakssid, packet)
    end
end

function GamePlayer:setCDRewardData(data)
    local day = DateUtil.getYearDay()
    local level = 0
    local cDay = 0

    if data then
        cDay = data.cDay
        level = tonumber(data.level)
    end

    if cDay ~= day then
        cDay = day
        level = 0
    end

    self.CDRewardData.cDay = cDay
    self.CDRewardData.level = level
end

function GamePlayer:getCDRewardData()
    local data = {
        level = self.CDRewardData.level,
        cDay = self.CDRewardData.cDay,
    }
    return data
end

function GamePlayer:getCDRewardNextLevel()
    local curLevel = self.CDRewardData.level
    local maxLevel = CDRewardConfig.maxLevel
    local nextLevel = (curLevel + 1 > maxLevel and {maxLevel} or {curLevel + 1})[1]

    return nextLevel
end

function GamePlayer:syncCDRewardData()
    local nextLevel = self:getCDRewardNextLevel()
    local CDInfo = CDRewardConfig:getCDRewardInfoByLevel(nextLevel)
    if not CDInfo then
        HostApi.log("GamePlayer:syncCDRewardData  player.userId = " .. tostring(self.userId) .. ", nextLevel = " .. nextLevel .. ", CDInfo not found")
        return
    end

    local countDownTime = CDInfo.cdTime
    local params = {
        CDTime = countDownTime * 1000,
        CDImg = CDInfo.image
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(self.rakssid, "syncBlockCityCDRewardData", "", data)

    self.CDRewardStartTime = os.time()
    self.CDRewardAvailableTime = countDownTime
end

function GamePlayer:updateLocalCDRewardData()
    local day = DateUtil.getYearDay()
    local cDay = self.CDRewardData.cDay

    local nextLevel = self:getCDRewardNextLevel()
    local money = CDRewardConfig:getCashByLevel(nextLevel)

    if money then
        self:addMoney(money)
    end

    if cDay ~= day then
        cDay = day
        nextLevel = 0
    end

    self.CDRewardData.cDay = cDay
    self.CDRewardData.level = nextLevel

    local key = "g1047CDReward" .. nextLevel
    GameAnalytics:design(self.userId, 0, {key})
end

function GamePlayer:checkCDRewardTimeEnd()
    if self.CDRewardStartTime == 0 then
        return
    end

    local remainTime = self.CDRewardAvailableTime - os.time() + self.CDRewardStartTime
    if remainTime > 0 then
        return
    end
    self.CDRewardStartTime = 0
    self:updateLocalCDRewardData()
    self:syncCDRewardData()
end

function GamePlayer:setMoneyNum(money)
    self:setCurrency(tonumber(money))
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:addMoney(money)
    if money > 0 then
        self:addCurrency(tonumber(money))
    end
end

function GamePlayer:subMoney(money)
    if money > 0 then
        self:subCurrency(tonumber(money))
    end
end

function GamePlayer:setIsGotFreeR_Currency(state)
    self.isGotFreeR_Currency = state
end

function GamePlayer:getIsGotFreeR_Currency()
    return self.isGotFreeR_Currency
end

function GamePlayer:presentFreeR_Currency()
    if self:getIsGotFreeR_Currency() then
        return
    end
    self:setIsGotFreeR_Currency(true)
    self:addMoney(GameConfig.freeR_Currency)
end

function GamePlayer:checkPlayerRaceWin()
    if not self.race.isRacing then
        return
    end

    local isRacing = false
    local isSelfWin = false
    local data = {}

    local competitor = PlayerManager:getPlayerByUserId(self.race.competitorUId)
    if not competitor then
        isSelfWin = true
        data = {
            [self] = isSelfWin,
        }

    elseif self.race and self.race.isTakeOffVehicle or competitor.race and competitor.race.isTakeOffVehicle then
        isSelfWin = (self.race.isTakeOffVehicle and {false} or {true})[1]

    else
        local selfPos = self:getPosition()
        local otherPos = competitor:getPosition()
        local isSelfFinish = VehicleConfig:checkPlayerFinishRace(selfPos)
        local isCompetitorFinish = VehicleConfig:checkPlayerFinishRace(otherPos)

        if isSelfFinish and not isCompetitorFinish then
            isSelfWin = true

        elseif isSelfFinish and isCompetitorFinish then
            local yaw = tonumber(VehicleConfig.RaceVehicleInfo.EndLine.Yaw)
            if yaw == 90 or yaw == 270 then
                isSelfWin = checkWin * {(selfPos.x >= otherPos.x and {true} or {false})[1], yaw}
            else
                isSelfWin = checkWin * {(selfPos.z >= otherPos.z and {true} or {false})[1], yaw}
            end

        elseif not isSelfFinish and isCompetitorFinish then
            isSelfWin = false

        else
            isRacing = true
        end
    end

    if not isRacing then
        if not next(data) then
            data = {
                [self] = isSelfWin,
                [competitor] = not isSelfWin,
            }
        end

        local content = {
            winner = (isSelfWin and {self.name} or {self.race.competitorName})[1],
            loser = (isSelfWin and {self.race.competitorName} or {self.name})[1]
        }
        HostApi.sendBroadcastMessage(0, 10, json.encode(content))

        for driver, isWin in pairs(data) do
            if driver then
                driver.race = nil
                local packet = DataBuilder.new():addParam("isWin", isWin):getData()
                PacketSender:sendServerCommonData(driver.rakssid, "BlockCityRaceVehicleResult", "", packet)
            end
        end
    end

end

function GamePlayer:setRaceData(isRacing, competitorUId, competitorName, isRaceWin, vEntityId, isTakeOffVehicle, inviteTime, inviterUId)
    self.race = {
        isRacing = isRacing,
        competitorUId = competitorUId,
        isRaceWin = isRaceWin,
        vEntityId = vEntityId,
        isTakeOffVehicle = isTakeOffVehicle,
        inviteTime = inviteTime,
        inviterUId = inviterUId,
        competitorName = competitorName,
    }
end

function GamePlayer:closeGlide()
    self:setGlide(false)
    self:changeClothes("custom_wing", self.custom_wing)
end

return GamePlayer

--endregion


