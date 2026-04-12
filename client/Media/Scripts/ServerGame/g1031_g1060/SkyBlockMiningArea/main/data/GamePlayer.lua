---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.ProductConfig"
require "data.TaskController"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
	self.isLife = true
    self.maxHp = GameConfig.initHP 
    self.hp = self.maxHp
    self:changeMaxHealth(self.maxHp)
    self.foodLevel = 20
    self:setFoodLevel(self.foodLevel)
    self.isMaxFood = false
    self.inventory = {}
    self.money = 0
    self.level = 1
    self.exp = 0
    self.score = 0
    self.area = "20*20*20"
    self.books = {}
    self.off_lineTime = 0
    self.products = {}
    self.other_unlockTime = 0
    self.backMiningAreaTime = os.time()
    self.backProductTime = os.time()
    self.backHomeTime = os.time() - GameConfig.backHomeCD
	self.privilege = {}
    self.cur_pri_item = {}
    self.taskController = TaskController.new(self)
    
    self.mainline_tasks = {}
	self.progressesLevel = 0
    self.dare_tasks = {}
    self.maxDareTimes = tonumber(GameConfig.buyDaretask.initCount) or 0
	self.curDareTimes = 0
	self.nextTaskTime = os.time() + tonumber(GameConfig.refreshDareTask.nextTime)
	self.LastLoginDate = ""

	self.free_refresh_times = 0
	self.dareIsUnLock = false
    self.pvpTime = 0
    self.isInPvp = false
    self.inHandItemId = 0
    self.diedPos = VectorUtil.ZERO
    self:initStaticAttr(0)
    self:teleInitPos()
    self.stay_time = 0
    self.has_ad = false
    self.canSendCostDiamond = true
    self.signIn_id = 1
    self.signIn_dayKey = "0"
    self.needRecivePri = false
    self.readyShowTreasure = false
    self.taskIsLandLevel = 1
    self.isNeedGuideArrow = false
    self.signIn_start = "0"
    self.signIn_end = "0"
    self.havePriImgList = {}
    self.notShowSwitchRed = false
    self.notShowRichRed = false
end

function GamePlayer:initShareDataFromDB(data)
    if not data.__first then
    	self.hp = data.hp or self.maxHp
    	self.foodLevel = data.foodLevel or 20
        self.inventory = data.inventory or {}
        self.money = data.money or 0
        self.current_day_money = data.current_day_money or 0
        self.exp = data.exp or 0
        self.level = data.level or 1
        self.area = data.area or "20*20*20"
        self.getRewardTick = data.getRewardTick or 0
        self.books = data.books or {}
        self.off_lineTime = data.off_lineTime or 0
        self.products = data.products or {}
        self.other_unlockTime = data.other_unlockTime or 0
        self.backMiningAreaTime = data.backMiningAreaTime or os.time()
        self.backProductTime = data.backProductTime or os.time()
		self.privilege = data.privilege or {}
    	self.isSky = data.isSky or false
        self.signIn_id = data.signIn_id or 1
        self.signIn_dayKey = data.signIn_dayKey or "0"
        self.signIn_start = data.signIn_start or "0"
        self.signIn_end = data.signIn_end or "0"
        self.havePriImgList = data.havePriImgList or {}
        self.notShowSwitchRed = data.notShowSwitchRed or false
        self.notShowRichRed = data.notShowRichRed or false
    else
    	HostApi.log("initShareDataFromDB the data.__first is true ")
    end
    
    self:fillInvetoryBySlot(self.inventory)
    self.readyShowTreasure = false
    self:checkTreasureInInvetory()
    if not data.__first then
        self:setHealth(data.hp or self.maxHp)
        self:setFoodLevel(data.foodLevel or 20)
    end
    PrivilegeConfig:setplayerPrivilege(self)
    PrivilegeConfig:setPrivilegePriImg(self)
    PrivilegeConfig:setInventoryPriImg(self)
    self:syncListInfo()
    self:setCurrency(self.money)
    PrivilegeConfig:checkUnlockThreeNpc(self)
    self:sendSkyBlockShopInfo()
    self:sendSkyBlockPriImg()
    self:sendShowSwitchRed()
    self:sendShowRichRed()

    if self.money and self.exp and self.level and self.area and self.off_lineTime and self.other_unlockTime then

        HostApi.log("initShareDataFromDB userId: " .. tostring(self.userId) .. " money: " .. self.money .. " exp: " .. self.exp .. " level: " .. self.level
        .. " area: " .. self.area .. " off_lineTime: " .. self.off_lineTime .. " other_unlockTime: " .. self.other_unlockTime)
    end

    if self.products then
        for _, npc in pairs(self.products) do
            if npc.Id and npc.level and npc.Time then
                HostApi.log("initShareDataFromDB product: npcId: " .. npc.Id .. " npcLevel: " .. npc.level .. " npcTime: " .. npc.Time)
            end
        end
    end

    self:LogInventoryInfo()
    self:checkShowPriSign()
    self:sendGiftNum()
end

function GamePlayer:initMainLineDataFromDB(data)
    HostApi.log("initMainLineDataFromDB")
	if not data.__first then
		self.progressesLevel = data.progressesLevel or 0
		self.mainline_tasks = data.mainline_tasks or {}
		self.dare_tasks = data.dare_tasks or {}
		self.maxDareTimes = data.maxDareTimes or 0
		self.curDareTimes = data.curDareTimes or 0
		self.nextTaskTime = data.nextTaskTime or 0
		self.LastLoginDate = data.LastLoginDate or ""
		self.free_refresh_times = data.free_refresh_times or 0
		self.dareIsUnLock = data.dareIsUnLock or false
        self.taskIsLandLevel = data.taskIsLandLevel or 1
	end
	self:IsTodayFirstLogin()
	self:sendDareTaskInfo()
end

function GamePlayer:IsTodayFirstLogin()
    local data = DateUtil.getDate()
	if data ~= self.LastLoginDate then
		self.LastLoginDate = data
		self:initDareTaskInfo()
    end
end

function GamePlayer:initDareTaskInfo()
	self.maxDareTimes = tonumber(GameConfig.buyDaretask.initCount) or 0
	self.curDareTimes = self.maxDareTimes
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
    if self.money then
        HostApi.log("onMoneyChange " .. tostring(self.userId) .. " money: " .. self.money)
    end
end

function GamePlayer:refreshDareTask()
	HostApi.log("refreshDareTask")
	local lackNum = tonumber(GameConfig.refreshDareTask.coinNum) or 0
	if self:checkMoney(Define.Money.DIAMOND, lackNum) then
        if self.canSendCostDiamond == false then
            return
        end
        self.canSendCostDiamond = false
		self:consumeDiamonds(1000, lackNum, Define.dareTaskTip.Refresh.. "#" .. tostring(self.rakssid))
	end
end

function GamePlayer:refreshDareTaskSuccess()
	HostApi.log("refreshDareTaskSuccess")
    GameAnalytics:design(self.userId, 0, { "g1048refresh_task", "g1048refresh_task"})
	self.taskController:ResetDareTask()
	self.taskController:sendDareTaskToClient()
	self:sendDareTaskInfo()
end

function GamePlayer:addDareTaskTimes()
	HostApi.log("addDareTaskTimes")
	local lackNum = tonumber(GameConfig.buyDaretask.coinNum) or 0
	if self:checkMoney(Define.Money.DIAMOND, lackNum) then
        if self.canSendCostDiamond == false then
            return
        end
        self.canSendCostDiamond = false
		self:consumeDiamonds(1001, lackNum, Define.dareTaskTip.AddTimes.. "#" .. tostring(self.rakssid))
	end
end

function GamePlayer:addDareTaskTimesSuccess()
	HostApi.log("addDareTaskTimesSuccess")
	self.curDareTimes = self.curDareTimes + 1
	self:sendDareTaskInfo()
end

function GamePlayer:checkTaskTimes()
	return self.curDareTimes > 0
end

function GamePlayer:checkTaskTimesIsFull()
	return self.curDareTimes >= self.maxDareTimes
end

function GamePlayer:onTick()
	if self.nextTaskTime - os.time() <= 0 and DbUtil:CanSavePlayerData(self, DbUtil.TAG_MIAN_LINE) and DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
		if self.free_refresh_times < 0 then
			self.free_refresh_times = 0
		end

		if self.free_refresh_times < 3 then
			self.free_refresh_times = self.free_refresh_times + 1
		end
		
		self.nextTaskTime = os.time() + tonumber(GameConfig.refreshDareTask.nextTime)
		self:sendDareTaskInfo()
	end

    if self.isInPvp then
        self.pvpTime = self.pvpTime + 1
    end
    self.stay_time = self.stay_time + 1
end

function GamePlayer:sendDareTaskInfo()
	local data = { }
	local addcfg = GameConfig.buyDaretask
	local reCfg =  GameConfig.refreshDareTask
	data.curTimes = self.curDareTimes
	data.maxTimes = self.maxDareTimes
	data.addCoinId = addcfg.coinId
	data.addCoinNum = addcfg.coinNum
	data.refreshCoinId = reCfg.coinId
	data.refreshCoinNum = reCfg.coinNum
	data.curFreeTimes = self.free_refresh_times
	data.maxFreeTimes = reCfg.freeTimes
	data.nextTime = self.nextTaskTime - os.time()
	HostApi.sendSkyBlockDareTaskInfo(self.rakssid, json.encode(data))
end

function GamePlayer:setInventory()
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end

    self.inventory = InventoryUtil:getPlayerInventoryInfo(self, true) 
    local hotBar = {}
    local invPlayer = self:getInventory()
    if self.inventory and  self.inventory.item then
        for k, stack in pairs(self.inventory.item) do
            if invPlayer then
                self.inventory.item[k].hotBarIndex = 1000
                for i = 0, 8 do
                    local data = invPlayer:getHotBarInventory(i)
                    if data and hotBar[i] == nil and data.id == stack.id  and data.num == stack.num and data.meta == stack.meta then
                        hotBar[i] = true
                        self.inventory.item[k].hotBarIndex = i
                        break
                    end
                end
            end
        end

        table.sort(self.inventory.item, function (a, b)
            if a.hotBarIndex ~= b.hotBarIndex then
                return a.hotBarIndex < b.hotBarIndex
            end
        end)
    end 
end

function GamePlayer:fillInvetoryBySlot(inventory, isRespawn)
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end
    isRespawn = isRespawn or false
    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    if armors then
        for i, armor in pairs(armors) do
            if armor.id > 0 and armor.id < 10000 then
                if self.entityPlayerMP then
                    self.entityPlayerMP:replaceArmorItemWithEnchant(armor.id, 1, armor.meta, {})
                end
            end
        end
    end

    local guns = inventory.gun
    if guns then
        for i, gun in pairs(guns) do
            if gun.id > 0 then
                self:addGunItem(gun.id, gun.num, gun.meta, gun.bullets)
            end
        end
    end

    local items = inventory.item
    if items and next(items) then
        for i, item in pairs(items) do
            if item.id > 0 and item.id < 10000 then
                if isRespawn then
                    local itemConfig = DiedDropConfig:getItemConfig(item.id, item.meta, item.num)
                    if itemConfig then
                        if itemConfig.num > 0 then
                            self:addItem(itemConfig.itemId, itemConfig.num, itemConfig.itemMeta)
                        end
                        if itemConfig.died_num > 0 then
                            for i=1,itemConfig.died_num do
                                local pos = GameConfig:getRandomPosAZ(self.diedPos, GameConfig.drop_radius)
                                if itemConfig.itemId == 511 and itemConfig.itemMeta == 0 then
                                    pos = GameConfig:getRandomPosAZ(self.diedPos, 0.5)
                                    GemStoneConfig:initGemStone(pos, 0, 0)
                                else
                                    EngineWorld:addEntityItem(itemConfig.itemId, 1, itemConfig.itemMeta, GameConfig.map_resetCD, pos, motion, false, true)
                                end
                                --HostApi.log("DropMoney getRandomPosAZ x : "..pos.x.." y : "..pos.y.." z : "..pos.z.." ItemID : "..ItemID.CURRENCY_MONEY)
                            end
                        end
                    else
                        self:addItem(item.id, item.num, item.meta)
                    end
                else
                    self:addItem(item.id, item.num, item.meta)
                end
            end
        end
        if isRespawn then
            DiedDropConfig:revertDropConfig()
        end
    end
end

function GamePlayer:LogInventoryInfo()
    local logStr = "LogInventoryInfo userId: " .. tostring(self.userId) .. " "
    if self.inventory then
        if self.inventory.armor then
            logStr = logStr .. "armor: [id, meta]"
            for _, armor in pairs(self.inventory.armor) do
                logStr = logStr .. "[" .. armor.id .. ", " .. armor.meta .. "]"
            end
        end

        if self.inventory.gun then
            logStr = logStr .. "gun: [id]"
            for _, gun in pairs(self.inventory.gun) do
                logStr = logStr .. "[" .. gun.id .. "]"
            end
        end

        if self.inventory.item then
            logStr = logStr .. "item: [id, meta, num]"
            for _, item in pairs(self.inventory.item) do
                logStr = logStr .. "[" .. item.id .. ", " .. item.meta .. ", " .. item.num .. "]"
            end
        end
        HostApi.log(logStr)
    end
end

function GamePlayer:DropMoney()
    local money, money_heap = DiedDropConfig:getCurrencyConfig(self.money)
    if money_heap > 0 and money < self.money then
        GameAnalytics:design(self.userId, 0, { "g1048subMoney", tostring(self.userId), money_heap})
        HostApi.log("g1049subMoney money_heap : "..money_heap)
        money_heap = math.floor(money_heap / GameConfig.monetaryUnit)
        if money_heap > 0 then
            for i=1,money_heap do
                local pos = GameConfig:getRandomPosAZ(self.diedPos, GameConfig.drop_radius)
                --HostApi.log("DropMoney getRandomPosAZ x : "..pos.x.." y : "..pos.y.." z : "..pos.z.." ItemID : "..ItemID.CURRENCY_MONEY)
                EngineWorld:addEntityItem(ItemID.CURRENCY_MONEY, 1, 0, GameConfig.map_resetCD, pos, motion, false, true)
                self:setCurrency(money)
            end
        end
    end
end

function GamePlayer:teleInitPos()
    self:setRespawnPos(GameConfig.initPos)
    -- HostApi.log("teleInitPos x : "..GameConfig.initPos.x.." y : "..GameConfig.initPos.y.." z : "..GameConfig.initPos.z)
    self:setPositionAndRotation(GameConfig.initPos, GameConfig.yaw, 0)
    if self.entityPlayerMP then
        self.entityPlayerMP:setPositionAndUpdate(GameConfig.initPos.x, GameConfig.initPos.y, GameConfig.initPos.z)
    end
    -- HostApi.changePlayerPerspece(self.rakssid, 1)
end

function GamePlayer:getListInfo()
    local lv = self.level or 1
    local cur_exp = self.exp or 0
    local max_exp, is_max = IslandLevelConfig:getMaxExp(self.level)
    local area = self.area or "20*20*20"
    local sortType =  AppShopConfig:getSortTypeByScore(self.level)
    local island_lv = IslandAreaConfig:getIdByArea(self.area)
    return lv, cur_exp, max_exp, island_lv, area, is_max, sortType
end

function GamePlayer:syncListInfo()
    local lv, cur_exp, max_exp, island_lv, area, is_max, sortType = self:getListInfo()
    HostApi.sendShowSkyBlockInfo(self.rakssid, lv, cur_exp, max_exp, island_lv, area, is_max, sortType)
	self:sendSkyBlockShopInfo()
end

function GamePlayer:sendSkyBlockShopInfo()
	local data = {}
	data.priData = self:getpriData()
	data.npcData = self:getnpcData()
	data.priItemData = self:getPriItemData()

	local sortType =  AppShopConfig:getSortTypeByScore(self.level)
	PrivilegeConfig:checkSpePri(self)
	HostApi.sendSkyBlockShopInfo(self.rakssid, sortType, self.level, json.encode(data))
end


function GamePlayer:getPriItemData()
	local priItemData = {}
	for k, v in pairs(self.cur_pri_item) do
		local data = {}
		data.id = v.Id
		data.priType = v.PriType
		data.detail = v.Detail
		data.have = v.Forever
		if v.Time >= os.time() then
			data.have = true
		end

		table.insert(priItemData, data)
	end
	return priItemData
end

function GamePlayer:getpriData()
	local priData = {}
	for k, v in pairs(self.privilege) do
		local data ={}
		data = {
			id = v.id,
			itemId = v.itemId,
			forever = v.forever,
			time = v.time,
		}
		table.insert(priData, data)
	end

	for k, v in pairs(priData) do
		v.time = v.time - os.time()
		if v.time < 0 then
			v.time = 0
		end
	end
	return  priData
end

function GamePlayer:getnpcData()
	local npcdata = {}
	for k, v in pairs(self.products) do
		local data ={}
		data = {
			id = v.ShopId,
			itemId = v.Id,
			forever = v.Time == -1 and true or false,
			time = v.EndTime
		}
		table.insert(npcdata, data)
	end

	for k, v in pairs(npcdata) do
		v.time = v.time - os.time()
		if v.time < 0 then
			v.time = 0
		end
	end
	return npcdata
end

function GamePlayer:insertProductsInfo(Id, shopId, num)
    self.other_unlockTime = os.time()
    for i, v in pairs(self.products) do
        if v.Id == Id then
            --v.Time = ProductConfig:getTimeByNpcLevel(v.Id, v.level)
			local npcTime = ProductConfig:getTimeByNpcLevel(v.Id, v.level)
			if npcTime == nil then
				return 
			end

            local buyTime = npcTime * num
            v.Time = v.Time + buyTime

            if v.EndTime < os.time() then
				v.EndTime = os.time()
			end

			v.EndTime = v.EndTime + buyTime
            return
        end
    end
    --local Time = ProductConfig:getTimeByNpcLevel(Id, 1)
    local Time = ProductConfig:getTimeByNpcLevel(Id, 1) * num
    table.insert(self.products, {
        Id = tonumber(Id),
        ShopId = tonumber(shopId),
        level = 1,
        Time = tonumber(Time),
        First = false,
        EndTime = os.time() + Time,
        pre_product_list = {},
        had_product_list = {}
    })
	
	local level = 0
	for i, v in pairs(self.products) do
		level = level + v.level
	end
    self.taskController:packingProgressData(self, Define.TaskType.products_level, GameMatch.gameType, 0, level, 0)

    if self.products then
        for _, npc in pairs(self.products) do
            if npc.Id and npc.level and npc.Time then
                HostApi.log("insertProductsInfo product: userId " .. tostring(self.userId) .. " npcId: " .. npc.Id .. " npcLevel: " .. npc.level .. " npcTime: " .. npc.Time)
            end
        end
    end
end

function GamePlayer:checkIsHAvePri(priId)
	for k, pri in pairs(self.cur_pri_item) do
		if priId == pri.PriType then
			if pri.Forever then
				return true
			elseif pri.Time >= os.time() then
				return true
			end
		end
	end
	
	return false
end

function GamePlayer:checkIsCanBuyPri(priId)
	for k, pri in pairs(self.privilege) do
		if pri.itemId == priId then
			if pri.forever then
				return false
			end
		end
	end
	return true
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
end

function GamePlayer:CanSavePlayerShareData()
    return DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA)
end

function GamePlayer:checkTreasureInInvetory(targetRakssid)
    -- 有targetRakssid就把自己宝石状态单独发给targetRakssid
    -- 没有targetRakssid就把自己宝石状态广播给所有人(包括自己)
    targetRakssid = targetRakssid or false
    local num = self:getItemNumById(511, 0)
    if num >= 1 then
        if self.readyShowTreasure == false then --自己已经显示有宝石了，就不重复显示
            self:changeClothes("custom_glasses", 12)
            local builder = DataBuilder.new()
            builder:addParam("text", "skyblock_has_treasure")
            builder:addVector3Param("offset", VectorUtil.newVector3(0, 3.5, 0))
            self.entityPlayerMP:addCustomText("TreasureHint", builder:getData(), -1, 1)
            self.readyShowTreasure = true
        end
    else
        self:resetClothes()
        self:checkShowPriSign()
        self.entityPlayerMP:clearEntityFeature("TreasureHint")
        self.readyShowTreasure = false
    end
end

function GamePlayer:checkShowPriSign()
    if not PrivilegeConfig:checkPlayerShowPri(self) then
        return
    end
    self:changeClothes("custom_crown", 10)
end

function GamePlayer:changeGuideArrow(isShow)
    if self.isNeedGuideArrow == isShow then
        return
    end
    local pos = GameConfig.ArrowPos
    if pos and pos.x and pos.y and pos.z then
        local data = {}
        data.isShow = isShow
        data.posX = pos.x
        data.posY = pos.y + 1
        data.posZ = pos.z
        GamePacketSender:sendShowGuideArrowEvent(self.rakssid, data)
        self.isNeedGuideArrow = isShow
        local data = {
            isShow = isShow
        }
        GamePacketSender:sendShowGuideArrowBtn(self.rakssid, json.encode(data))
    end
end

function GamePlayer:initHintText()
    for __,v in pairs(GameConfig.hintText) do
        local pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        HostApi.addWallText(self.rakssid, 0, 0, tostring(v.text), pos, v.scale, v.yaw, 0, {1, 1, 1, 1})
    end
end

function GamePlayer:checkIsCanUnlockByOther(priId)
    for k, pri in pairs(self.privilege) do
        if pri.itemId == priId then
            return false
        end
    end
    return true
end

function GamePlayer:sendSkyBlockPriImg()    
    GamePacketSender:sendSkyBlockPriImg(self.rakssid, self.havePriImgList)
end

function GamePlayer:checkMoney(priceType, price)
    print("self.userId == "..tostring(self.userId).." priceType == "..tostring(priceType).."price == "..tostring(price))
    if priceType == Define.Money.DIAMOND or priceType == Define.Money.GOLD_DIAMOND then
        if self:getDiamond() >= price then
            return true
        else
            GamePacketSender:sendShowLackDiamond(self.rakssid)
            return false
        end
    end

    if priceType == Define.Money.MONEY then
        if self:getCurrency() >= price then
            return true
        else
            GamePacketSender:sendShowLackMoney(self.rakssid)
            return false
        end
    end

    GamePacketSender:sendShowLackMoney(self.rakssid)
    return false
end

function GamePlayer:buyMoney(level)
    print("buyMoney userId == "..tostring(self.userId))
    local maxLevel = GameConfig.maxBuyMoneyCost / GameConfig.onceBuyMoneyCost
    if level <= 0 and level > maxLevel then
        return
    end

    local price = level * GameConfig.onceBuyMoneyCost
    if self:checkMoney(Define.Money.DIAMOND, price) then
        if self.canSendCostDiamond == false then
            return
        end
        self.canSendCostDiamond = false
        self:consumeDiamonds(1100, price, Define.buyMoney.. "#" .. tostring(level))
    end
end

function GamePlayer:buyMoneySuccess(level)
    print("buyMoneySuccess userId == "..tostring(self.userId))
    local money = level * GameConfig.onceMoneyNum
    HostApi.notifyGetItem(self.rakssid, 10000, 0, money)
    GameAnalytics:design(self.userId, 0, { "g1048addMoney", tostring(self.userId), tonumber(money)})
    self:addCurrency(money)
end


function GamePlayer:sendGiftNum()
    local numList = {}

    local cfg = ChristmasConfig:getGiftdata()

    for _, gift in pairs(cfg) do
        local cur_num = self:getItemNumById(gift.ItemId, 0)

        local isHave = false
        for _, cur_data in pairs(numList) do
            if tonumber(cur_data.ItemId) == tonumber(gift.ItemId) then
                isHave = true
                break
            end
        end

        if isHave == false then
            local data = 
            {
                ItemId = gift.ItemId,
                Num = cur_num,
            }
            table.insert(numList, data)
        end
    end

    GamePacketSender:sendGiftNumList(self.rakssid, numList)
end

function GamePlayer:sendShowSwitchRed()
    if not self.notShowSwitchRed then
        local data  = {}
        data.RedIsShow = true
        GamePacketSender:sendShowSwitchRed(self.rakssid, data)
    end
end

function GamePlayer:sendShowRichRed()
    if not self.notShowRichRed then
        local data  = {}
        data.RedIsShow = true
        GamePacketSender:sendShowRichRed(self.rakssid, data)
    end
end

return GamePlayer

--endregion


