---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "data.GameManor"
require "data.ProductNpc"
require "config.ProductConfig"
require "Match"
require "data.TaskController"

GamePlayer = class("GamePlayer", BasePlayer)
GamePlayer.PriceType = {
    LeftDiamond= 1,
    LeftMoney = 2,
    RightDiamond = 3
}
GamePlayer.DropNum = {
    FirstStage = 64,
    SecondStage = 5
}
function GamePlayer:init()
    self:initStaticAttr(0)
    self.manorIndex = 0
    self.money = 0
    self.score = 0
	self.respawnPos = VectorUtil.newVector3(0,105,0)
	self.yaw = self:getYaw()
	self.isHome = true
    local target = GameMatch.callOnTarget[tostring(self.userId)]
    if target ~= nil then
    	local manorInfo = GameMatch.userManorInfo[tostring(target.userId)]
    	if manorInfo ~= nil then
    		local pos = GameConfig:getInitPosByManorIndex(manorInfo.manorIndex)
    		if pos then
                self.respawnPos = pos 
                self:setRespawnPos(self.respawnPos)
                print("the respawnPos is :  "..pos.x.."   "..pos.y.."   "..pos.z.."   "..self.manorIndex)
                self:teleHomeWithYaw()
    			self.manorIndex = manorInfo.manorIndex
    		end
    	end
    end
    GameMatch.callOnTarget[tostring(self.userId)] = nil
    self.isLife = true
    self.maxHp = GameConfig.initHP 
    self.hp = self.maxHp
    self:changeMaxHealth(self.maxHp)
    self.foodLevel = 20
    self:setFoodLevel(self.foodLevel)
    self.isMaxFood = false
    self.inventory = {}
    self.exp = 0
    self.level = 1
    self.products = {}
    self.prices = 0
    self.coins = 0
    self.adType = 0
    self.adPrice = 0
    self.current = 0
    self.next = 0
    self.npcid = 0
    self.npclevel = 0
    self.times = 0
    self.use_time = 0
    self.on_lineTime = os.time()
    self.update_Time = 0
    self.unlockTime = 0
    self.books = {}
    self.off_lineTime = 0
    self.other_unlockTime = 0
    self.backMiningAreaTime = os.time()
    self.backProductTime = os.time()
    self.cur_product_num = 0
	self.privilege = {}
    self.has_ad = false
    self.cur_pri_item = {}
    self.mainline_tasks = {}
	self.progressesLevel = 0
    self.dare_tasks = {}
    self.maxDareTimes = tonumber(GameConfig.buyDaretask.initCount) or 0
	self.curDareTimes = 0
	self.nextTaskTime = os.time() + tonumber(GameConfig.refreshDareTask.nextTime)
	self.LastLoginDate = ""
	self.free_refresh_times = 0
    self.taskController = TaskController.new(self)
    self.isSky = false
    self.watchAd = false
	self.dareIsUnLock = false
    self.world_products = {}
    self.isUpdateAllNpc = false
    self.isUpdateAllName = false
    self:initSelfArea()
    self.stay_time = 0
    self.signIn_id = 1
    self.signIn_dayKey = "0"
    self.canSendCostDiamond = true
    self.readyShowTreasure = false
    self.taskIsLandLevel = 1
    self.signIn_start = "0"
    self.signIn_end = "0"
    self.havePriImgList = {}
    self.notShowSwitchRed = false
    self.notShowRichRed = false
    self:changeGuideArrow(true)
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
        self.products = data.products or {}
        self.off_lineTime = data.off_lineTime or 0
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
    if self.isSky then
        HostApi.setCustomSceneSwitch(self.rakssid, 2, 2, true, true, true)
    else
        HostApi.setCustomSceneSwitch(self.rakssid, 2, 2, true, false, true)
    end
    
    self:fillInvetoryBySlot(self.inventory)
    self.readyShowTreasure = false
    self:checkTreasureInInvetory()
    if not data.__first then
        self:setHealth(data.hp or self.maxHp)
        self:setFoodLevel(data.foodLevel or 20)
    end
    self:productListTransfer()
    self:offlineSpwanItem()
    self:spawnOnceLargeItem()
    PrivilegeConfig:setplayerPrivilege(self)
    PrivilegeConfig:setPrivilegePriImg(self)
    PrivilegeConfig:setInventoryPriImg(self)
    self:syncListInfo()
	self:setCurrency(self.money)
    self:initSelfNpc()
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
            self:updateOreNum(npc)
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

function GamePlayer:getOfflineTime()
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end

    for k, data in pairs(self.products) do
        local maxLv = ProductConfig:getMaxLevel(data.Id)
        if maxLv > 0 then
            if data.level <= maxLv and data.Time then
                local Time = data.Time
                if Time == -1 then
                    if self.off_lineTime == 0 and self.other_unlockTime ~= 0 then
                        self.times = self.on_lineTime - self.other_unlockTime -- on other_unlockTime
                    else
                        self.times = self.on_lineTime - self.off_lineTime
                    end
                else
                    if self.other_unlockTime ~= 0 then
                        local m = self.on_lineTime - self.other_unlockTime -- on other_unlockTime
                        if m >= Time then
                            self.times = Time
                            data.Time = 0
                            data.pre_product_list = {}
                            data.had_product_list = {}
                        else
                            self.times = m
                            data.Time = Time - m
                        end
                    else
                        local h = self.on_lineTime - self.off_lineTime
                        if h >= Time then
                            self.times = Time
                            data.Time = 0
                            data.pre_product_list = {}
                            data.had_product_list = {}
                        else
                            self.times = h
                            data.Time = Time - h
                        end
                    end
                end
            else
                if data.level > maxLv then
                    data.level = maxLv
                end
                if data.Time == nil then
                    data.Time = ProductConfig:getTimeByNpcLevel(data.Id, data.level)
                end
                HostApi.log("products data.level or Time error " .. tostring(self.userId) .. "level: " .. data.level .. " maxLv:" .. maxLv)
            end
        end
    end
    return self.times
end

function GamePlayer:insertItemToNpcHadList(npc, product_item, itemNum)
    local list = npc.had_product_list
    if list then
        local has_key = false
        for key, product in pairs(list) do
            if product.item_id == product_item.item_id then
                product.item_num = product.item_num + itemNum
                has_key = true
                break
            end
        end
        if has_key == false then
            local item = {}
            item.item_id = product_item.item_id
            item.item_num = itemNum
            item.entityId = 0
            table.insert(list , item)
        end
    end
end

function GamePlayer:insertItemToNpcPreList(npc, product_item, type)
    local list = npc.pre_product_list
    local id = self:getIdByLevel(npc.Id, npc.level)
    if list then
        local has_key = false
        for key, product in pairs(list) do
            if product.item_id == product_item.item_id then
                local MaxNum = ProductResourceConfig:getLimitNumByRatio(id, product.item_id)
                if product.item_num >= MaxNum then
                    product.item_num = MaxNum
                end
                if product.item_num < MaxNum and type ~= ProductLimitConfig.Transfer then
                    product.item_num = product.item_num + 1
                end
                has_key = true
                break
            end
        end
        if has_key == false then
            local item = {}
            item.item_id = product_item.item_id
            item.item_num = 1
            item.entityId = 0
            table.insert(list , item)
        end
    end
end

function GamePlayer:removeItemFromNpcList(list, item_id, itemNum)
    if list then
        for key, product in pairs(list) do
            if product.item_id == item_id then
                product.item_num = product.item_num - itemNum
                if product.item_num <= 0 then
                    table.remove(list, key)
                end
                break
            end
        end
    end
end

function GamePlayer:spawnOnceLargeItem()
    local count = 0
    while self.cur_product_num < GameConfig.maxNum do
        if count > 5000 then
            break
        end

        for _, npc in pairs(self.products) do
            local item = self:getPreProductListItemByRandom(npc.pre_product_list)
            if item then

                if item.item_num > GamePlayer.DropNum.FirstStage then
                    local n = math.floor(item.item_num / GamePlayer.DropNum.FirstStage)
                    for i = 1, n do
                        self:addNpcItemToWordOffline(npc, item, 64)
                    end
                else
                    if item.item_num > GamePlayer.DropNum.SecondStage then
                        for i = 1, GamePlayer.DropNum.SecondStage do
                            self:addNpcItemToWordOffline(npc, item, 1)
                        end
                    else
                        self:addNpcItemToWordOffline(npc, item, 1)
                    end
                end

            end
        end
        count = count + 1
    end
end

function GamePlayer:addNpcItemToWordOffline(npc, item ,itemNum)
    if self.cur_product_num < GameConfig.maxNum then
        -- 1. spawn the item
        local motion = VectorUtil.newVector3(0, 0, 0)
        local Vector3i = GameConfig:getOriginPosByManorIndex(self.manorIndex)
        local radius = GameConfig.randomRadius
        local pos = self:getRandomPos(Vector3i, radius)
        if pos then
            local entityId = EngineWorld:addEntityItem(item.item_id, itemNum, 0, item.life, pos, motion, false, true)
            table.insert(self.world_products, entityId)
            self.cur_product_num = self.cur_product_num + 1

            -- 2. remove item from pre_product_list
            self:removeItemFromNpcList(npc.pre_product_list, item.item_id, itemNum)

            -- 3. add item to had_product_list
            local product_item = {}
            product_item.item_id = item.item_id
            product_item.item_num = item.item_num
            product_item.entityId = entityId
            self:insertItemToNpcHadList(npc, product_item, itemNum)
        end
    end
end

function GamePlayer:getPreProductListItemByRandom(pre_product_list)
    local result_item = nil
    if pre_product_list then
        local randomPos = HostApi.random("getPreProductListItem", 1, #pre_product_list + 1)
        result_item = pre_product_list[randomPos]
    end
    return result_item
end

function GamePlayer:offlineSpwanItem()
    for _, npc in pairs(self.products) do
        local id = self:getIdByLevel(npc.Id, npc.level)
        local times = self:getOfflineTime()
        local item = ProductResourceConfig:getItem(id)
        for i , k in pairs(item) do
            if k.itemId == 0 then
                break
            end
            k.num = math.floor(times / k.time)

            if self:checkIsHAvePri(PrivilegeConfig.priType.npc_eff_double) then
                 k.num = k.num * 2
            end
            local MaxNum = ProductResourceConfig:getLimitNumByRatio(id , k.itemId)
            if k.num >= MaxNum then
                   k.num = MaxNum
            end
            for j = 1, k.num do
                    local product_item = {}
                    product_item.item_id = k.itemId
                    product_item.item_num = 1
                    product_item.entityId = 0
                    self:insertItemToNpcPreList(npc, product_item, ProductLimitConfig.Offline)
            end
        end
    end
end

function GamePlayer:productListTransfer()
    for _, npc in pairs(self.products) do
        for key, product in pairs(npc.had_product_list) do
            local product_item = {}
            product_item.item_id = product.item_id
            product_item.item_num = product.item_num
            product_item.entityId = 0
            self:insertItemToNpcPreList(npc, product_item, ProductLimitConfig.Transfer)
        end

        npc.had_product_list = {}
    end

end

function GamePlayer:teleHomeWithYaw()
    self:setPositionAndRotation(self.respawnPos, self.yaw, 0)
    if self.entityPlayerMP then
        self.entityPlayerMP:setPositionAndUpdate(self.respawnPos.x, self.respawnPos.y, self.respawnPos.z)
    end
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
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end
    self.unlockTime = os.time()
    for i, v in pairs(self.products) do
        if v.Id == Id then
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
            self:updateOreNum(npc)
        end
    end
end

function GamePlayer:getRandomPos(Vector3i, radius)
    if Vector3i and Vector3i.x and Vector3i.y and Vector3i.z then
        local x = HostApi.random("getRandomPos", (Vector3i.x - (radius - 0.5)) * 10000, (Vector3i.x + (radius - 0.5)) * 10000)
        local y = Vector3i.y + 1
        local z = HostApi.random("getRandomPos", (Vector3i.z - (radius - 0.5)) * 10000, (Vector3i.z + (radius - 0.5)) * 10000)
        local pos = VectorUtil.newVector3(x / 10000 , y, z / 10000)
        return pos
    end
    return nil
end

function GamePlayer:initSelfArea()
    local centralPos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
    if centralPos then
        local radius = GameConfig.randomRadius
        local tab = {{},{}}
        tab[1][1] = tonumber(centralPos.x - radius)
        tab[1][2] = tonumber(centralPos.y)
        tab[1][3] = tonumber(centralPos.z - radius)
        tab[2][1] = tonumber(centralPos.x + radius)
        tab[2][2] = tonumber(centralPos.y)
        tab[2][3] = tonumber(centralPos.z + radius)
        GameConfig.ProductArea[tostring(self.manorIndex)] = Area.new(tab)
    end
end

function GamePlayer:unInitSelfArea()
    for k,v in pairs(GameConfig.ProductArea) do
        if tonumber(k) == self.manorIndex then
            if GameConfig.ProductArea[k] then
                GameConfig.ProductArea[k] = nil
            end
        end
    end
end

function GamePlayer:initSelfNpc()
    local pos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
    if pos and pos.x and pos.y and pos.z then
        local centralPos = VectorUtil.newVector3(pos.x, pos.y, pos.z) 
        if centralPos then
            print("initSelfNpc: centralPos is "..centralPos.x.."   "..centralPos.y.."   "..centralPos.z.."   "..self.manorIndex)
            local npcs = {}
            for _, producInfo in pairs(ProductConfig.products) do
                local info = self:getUpdateNpcInfo(producInfo.id)
                local curInterval = info.curInterval or 0
                local curLevel = info.level or -2
                local npc = ProductNpc.new(producInfo, centralPos, curInterval, curLevel)
                npcs[npc.entityId] = npc
            end
            GameConfig.ProductNpc[tostring(self.manorIndex)] = npcs
        end
    end
    
end

function GamePlayer:unInitSelfNpc()
    for index, npcs in pairs(GameConfig.ProductNpc) do
        if tonumber(index) == self.manorIndex then
            for entityId, npc in pairs(npcs) do
                if npc then
                    npc:onDestroy()
                    npcs[entityId] = nil
                end
            end
        end
    end
end

function GamePlayer:updateNpcLevel(ticks, npc)
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end
    local Id = 0
    if npc.Time == -1 then
        Id =  ProductConfig:getIdByNpcLevel(npc.Id, npc.level)
        return Id
    else
        if self.unlockTime ~= 0 and npc.Time ~= 0 then
            self.use_time = npc.Time + (self.unlockTime - self.on_lineTime)
        else
            self.use_time = npc.Time
        end
        if npc.First == false then
            if self.use_time >= ticks then
                Id =  ProductConfig:getIdByNpcLevel(npc.Id, npc.level)
                return Id
            else
                npc.Time = 0
                npc.had_product_list = {}
                npc.pre_product_list = {}
            end
        else
            local NewTime = self.use_time + (self.update_Time - self.unlockTime)
            if NewTime >= ticks then
                Id =  ProductConfig:getIdByNpcLevel(npc.Id, npc.level)
                return Id
            else
                npc.Time = 0
                npc.had_product_list = {}
                npc.pre_product_list = {}
            end
        end
    end
    return Id
end

function GamePlayer:getIdByLevel(Id, level)
    return ProductConfig:getIdByNpcLevel(Id, level)
end

function GamePlayer:clickUpdateLevel(id)
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end
    local unlock = false
    local maxLv = ProductConfig:getMaxLevel(id)
    for i, v in pairs(self.products) do
        if v.Id == id then
            self.npcid = v.Id
            self.npclevel = v.level
            self.current = ProductConfig:getIdByNpcLevel(id, v.level)
            self.next = ProductConfig:getIdByNpcLevel(id, v.level + 1)
            self.adType = ProductConfig:getAdTypeByNpcLevel(id, v.level)
            self.adPrice = ProductConfig:getAdPriceByNpcLevel(id, v.level)
            self.coins = ProductConfig:getCoinByNpcLevel(id, v.level)
            self.prices = ProductConfig:getPriceByNpcLevel(id, v.level)
            if v.level >= maxLv then
                local current = ProductResourceConfig:getItemIdAndOutput(self.current)
                local current_capacity = ProductResourceConfig:getHighestNumById(self.current)
                local data = {}
                -- value = 0 or 2 for Hidden display
                data.adPrice = 0
                data.adType = 2
                data.coins = 2
                data.prices = 0
                data.current_item1 = current[1].itemId
                data.current_num1 = current[1].num
                data.current_item2 = current[2].itemId
                data.current_num2 = current[2].num
                data.current_item3 = current[3].itemId
                data.current_num3 = current[3].num
                data.next_item1 = 0
                data.next_num1 = 0
                data.next_item2 = 0
                data.next_num2 = 0
                data.next_item3 = 0
                data.next_num3 = 0
                data.npcid = self.npcid
                data.npclevel = self.npclevel
                data.isMax = 1
                data.current_capacity = current_capacity
                data.next_capacity = 0
                GamePacketSender:sendShowUpdateProductInfo(self.rakssid, json.encode(data))
                unlock = true
                break
            end

            if self:checkIsHAvePri(PrivilegeConfig.priType.up_npc_half) then
                self.adPrice = math.floor(self.adPrice / 2)
                self.prices = math.floor(self.prices / 2)
            end

            if self.adPrice <= 1 then
                self.adPrice = 1
            end

            if self.prices <= 1 then
                self.prices = 1
            end

            self:showUpdateProductInfo()
            unlock = true
        end
    end

    if not unlock then
        local data = ProductUnlockConfig:getItemCfgById(id)
        GamePacketSender:sendShowUnlockTipMsgEvent(self.rakssid, json.encode(data))
    end
end

function GamePlayer:showUpdateProductInfo()
    self:getCurrentProductData(1)
    self.watchAd = false
end

function GamePlayer:showWatchAdUpdateProductInfo()
    self:getCurrentProductData(2)
    self.watchAd = true
    if self.enableIndie then
        GameAnalytics:design(self.userId, 0, {"g1050product_watch_Ad_finish_indie", self.npcid, self.npclevel})
    else
        GameAnalytics:design(self.userId, 0, {"g1050product_watch_Ad_finish", self.npcid, self.npclevel})
    end

end

function GamePlayer:getCurrentProductData(type)
    local current = ProductResourceConfig:getItemIdAndOutput(self.current)
    local next = ProductResourceConfig:getItemIdAndOutput(self.next)
    local current_capacity = ProductResourceConfig:getHighestNumById(self.current)
    local next_capacity = ProductResourceConfig:getHighestNumById(self.next)
    if current and next and current_capacity and next_capacity then
        local data = {}
        data.adPrice = self.adPrice
        if type == 1 then
            data.adType = self.adType
            data.prices = self.prices
        else
            data.adType = 2
            data.prices = self.adPrice
        end
        data.coins = self.coins
        data.current_item1 = current[1].itemId
        data.current_num1 = current[1].num
        data.current_item2 = current[2].itemId
        data.current_num2 = current[2].num
        data.current_item3 = current[3].itemId
        data.current_num3 = current[3].num
        data.next_item1 = next[1].itemId
        data.next_num1 = next[1].num
        data.next_item2 = next[2].itemId
        data.next_num2 = next[2].num
        data.next_item3 = next[3].itemId
        data.next_num3 = next[3].num
        data.npcid = self.npcid
        data.npclevel = self.npclevel
        data.isMax = 0
        data.current_capacity = current_capacity
        data.next_capacity = next_capacity
        GamePacketSender:sendShowUpdateProductInfo(self.rakssid, json.encode(data))
    end
end

function GamePlayer:updateLevel(type, id)
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end

    local cur_level = self:getCurrentLevelById(id)
    local prices = ProductConfig:getPriceByNpcLevel(id, cur_level)
    local adPrice = ProductConfig:getAdPriceByNpcLevel(id, cur_level)

    HostApi.log("cal before prices == "..prices.."   adPrice == "..adPrice .. " cur_level" .. cur_level
    .. " type: " .. type .. " id: " .. id .. " userId: " .. tostring(self.userId))

    if self:checkIsHAvePri(PrivilegeConfig.priType.up_npc_half) then
        prices = math.floor(prices / 2)
        adPrice = math.floor(adPrice / 2)
    end

    if self.watchAd == true then
        prices = adPrice
        self.watchAd = false
    end

    if prices <= 1 then
        prices = 1
    end

    if adPrice <= 1 then
        adPrice = 1
    end

    HostApi.log("cal after prices == "..prices.."   adPrice == "..adPrice .. " cur_level" .. cur_level
            .. " type: " .. type .. " id: " .. id .. " userId: " .. tostring(self.userId))

    if type == GamePlayer.PriceType.LeftDiamond then
        if self:checkMoney(Define.Money.DIAMOND, prices) then
            self:consumeDiamonds(1000, prices, "UpdateLevel".. "#" .. tostring(self.rakssid))
        end
    end
    if type == GamePlayer.PriceType.RightDiamond then
        if self:checkMoney(Define.Money.DIAMOND, adPrice) then
            self:consumeDiamonds(1000, adPrice, "UpdateLevel".. "#" .. tostring(self.rakssid))
        end
    end
    if type == GamePlayer.PriceType.LeftMoney then
        if self:checkMoney(Define.Money.MONEY, prices) then
            -- self.money = self.money - prices
            self:subCurrency(prices)
            GameAnalytics:design(self.userId, 0, { "g1048subMoney", tostring(self.userId), prices})
            self:updateLevelSuccess()
        end
    end
end

function GamePlayer:getCurrentLevelById(id)
    for i, v in pairs(self.products) do
        if v.Id == id then
            return v.level
        end
    end
end

function GamePlayer:updateLevelSuccess()
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end
    for i, v in pairs(self.products) do
        local maxLv = ProductConfig:getMaxLevel(v.Id)
        if v.Id == self.npcid then
            v.level = v.level + 1
            if v.level >= maxLv then
                v.level = maxLv
            end
            v.Time = ProductConfig:getTimeByNpcLevel(v.Id, v.level)
            if self.enableIndie then
                GameAnalytics:design(self.userId, 0, {"g1050product_indie", v.Id, v.level})
            else
                GameAnalytics:design(self.userId, 0, {"g1050product", v.Id, v.level})
            end

            v.First = true
            self.update_Time = os.time()
            self:updateNpcLevel(0, v)
            self:sendUpdateSuccessTip(v.Id, v.level)
        end
    end
    
	local level = 0
	for i, v in pairs(self.products) do
		level = level + v.level
    end
    
    self.taskController:packingProgressData(self, Define.TaskType.products_level, GameMatch.gameType, 0, level, 0)
    self.taskController:packingProgressData(self, Define.TaskType.up_products, GameMatch.gameType, 0, 1, 0)
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgUpdateSuccessTip())

    if self.products then
        for _, npc in pairs(self.products) do
            if npc.Id and npc.level and npc.Time then
                HostApi.log("updateLevelSuccess product: userId " .. tostring(self.userId) .. " npcId: " .. npc.Id .. " npcLevel: " .. npc.level .. " npcTime: " .. npc.Time)
            end
            self:updateOreNum(npc)
        end
    end
end

function GamePlayer:sendUpdateSuccessTip(id, level)
    local tip = ProductUnlockConfig:getItemCfgById(id)
    if tip and #tip.NotificationContent ~= 0 then
        local data = {
            message = tip.NotificationContent,
            top = true
        }
        if #tip.NotificationParams ~= 0 then
            data.params = self:getDisplayName() ..  "#" .. tip.NotificationParams  .. "#" .. level
        end
        WebService:SendMsg(nil, json.encode(data), 4, "game", GameMatch.gameType)
    end
end

function GamePlayer:getUpdateNpcInfo(npcId)
    local info = {}
    for i, v in pairs(self.products) do
        if type(v.Id) == "number" and v.Id == npcId then
            info.level = v.level 
            info.curInterval = v.Time
        end
    end
    return info
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

function GamePlayer:fillInvetoryBySlot(inventory)
    if not DbUtil:CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
        return
    end

    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    if armors then
        for i, armor in pairs(armors) do
            if armor.id > 0 and armor.id < 10000 then
                if self.entityPlayerMP then
                    self.entityPlayerMP:replaceArmorItemWithEnchant(armor.id, 1, armor.meta)
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
    if items then
        for i, item in pairs(items) do
            if item.id > 0 and item.id < 10000 then
                self:addItem(item.id, item.num, item.meta)
            end
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

function GamePlayer:removeItemFromHadProductList(itemId, entityId, itemNum)
    for _, npc in pairs(self.products) do
        for key, product in pairs(npc.had_product_list) do
            if itemId == product.item_id then
                self:removeItemFromNpcList(npc.had_product_list, itemId, itemNum)
                self.cur_product_num = self.cur_product_num - 1
                if self.cur_product_num < 0 then
                    self.cur_product_num = 0
                end
                return
            end
        end
        self:updateOreNum(npc)
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
    print("self:getEntityId().."..self:getEntityId())
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

function GamePlayer:clickSellAllOre()
    local NPCMoney = 0
    for _, npc in pairs(self.products) do
        for key, product in pairs(npc.had_product_list) do
            local money = SellItemConfig:getMoneyByItemId(product.item_id)
            NPCMoney = NPCMoney + (money * product.item_num)
        end
        npc.had_product_list = {}
        for key, product in pairs(npc.pre_product_list) do
            local money = SellItemConfig:getMoneyByItemId(product.item_id)
            NPCMoney = NPCMoney + (money * product.item_num)
        end
        npc.pre_product_list = {}
        self:updateOreNum(npc)
    end
    for k , entity in pairs(self.world_products) do
        EngineWorld:removeEntity(entity)
    end
    self.world_products = {}
    self.cur_product_num = 0
    self:addItem(10000, NPCMoney, 0)
    if self.enableIndie then
        GameAnalytics:design(self.userId, 0, { "g1050onekey_sell_indie", "g1050onekey_sell_indie" })
    else
        GameAnalytics:design(self.userId, 0, { "g1050onekey_sell", "g1050onekey_sell" })
    end
end

function GamePlayer:updateOreNum(npc)
    local oreNum = 0
    local Id =  ProductConfig:getIdByNpcLevel(npc.Id, npc.level)
    local maxMillNum = ProductResourceConfig:getHighestNumById(Id)
    for key, product in pairs(npc.pre_product_list) do
        oreNum = oreNum + product.item_num
    end
    if oreNum >= maxMillNum then
        oreNum = maxMillNum
    end
    --print("666updateOreNum" .. " level is " .. npc.Id .. " maxMillNum is " .. maxMillNum)
    for index, npcs in pairs(GameConfig.ProductNpc) do
        if tonumber(index) == self.manorIndex then
            for entityId, name in pairs(npcs) do
                if name.id == npc.Id then
                    GamePacketSender:sendSkyBlockProductProcess(self.rakssid, tonumber(oreNum), tonumber(maxMillNum), entityId)
                end
            end
        end
    end
end

function GamePlayer:changeGuideArrow(isShow)
    local pos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
    if pos and pos.x and pos.y and pos.z then
        local data = {}
        data.isShow = isShow
        data.posX = pos.x
        data.posY = pos.y + 1
        data.posZ = pos.z
        GamePacketSender:sendShowGuideArrowEvent(self.rakssid, data)
    end
end

function GamePlayer:checkShowPriSign()
    if not PrivilegeConfig:checkPlayerShowPri(self) then
        return
    end
    self:changeClothes("custom_crown", 10)
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
