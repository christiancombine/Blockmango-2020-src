---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "data.GameManor"
require "data.TaskController"
require "config.IslandAreaConfig"
require "config.ProductConfig"
require "Match"
require "data.GameSignIn"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self.manorIndex = 0
    self.curXChunkIndex = 0
    self.curYChunkIndex = 0
    self.curZChunkIndex = 0
    self.lastXChunkIndex = 0
    self.lastYChunkIndex = 0
    self.lastZChunkIndex = 0
    self.posOriginX = 0
    self.posOriginY = 0
    self.posOriginZ = 0
    self.mainline_tasks = {}
	self.progressesLevel = 0

    self.dare_tasks = {}
    self.money = 0
	self.score = 0
	self.respawnPos = nil
    self.genislandPos = GameConfig.genislandPos
    self.isGenisland = false
	self.target_user_id = nil
	self.oldExp = 0
	self.dareIsUnLock = false

	local manorIndex = 0
    local targetUserId = GameMatch:getTargetByUid(self.userId)
    if targetUserId ~= nil then
        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil then
            manorIndex = targetManorInfo.manorIndex
            local pos = GameConfig:getInitPosByManorIndex(targetManorInfo.manorIndex)
            if pos then
                if GameMatch:isMaster(self.userId) then
                    local manorInfo = GameMatch:getUserManorInfo(self.userId)
                    if manorInfo then
                        local rPos = manorInfo:aPosToRPos(pos)
                        if rPos then
                    	    self.respawnPos = rPos
                        	HostApi.log("respawnPos master aPos  " .. tostring(self.userId) .. " " .. pos.x .. " " .. pos.y .. " " .. pos.z )
                        	HostApi.log("respawnPos master rPos  " .. tostring(self.userId) .. " " .. self.respawnPos.x .. " " .. self.respawnPos.y .. " " .. self.respawnPos.z )
                        end
                    end
                else
                    HostApi.log("respawnPos vistor aPos  " .. tostring(self.userId) .. " " .. pos.x .. " " .. pos.y .. " " .. pos.z )
                end
                self:setRespawnPos(pos)
                self:teleportPos(pos)
                self.manorIndex = targetManorInfo.manorIndex
                self.target_user_id = targetUserId
                if not DBManager:isLoadDataFinished(targetUserId, DbUtil.TAG_SIGN) then
                    DbUtil.getPlayerData(targetUserId, DbUtil.TAG_SIGN)
                end
                if not DBManager:isLoadDataFinished(targetUserId, DbUtil.TAG_ACTOR) then
                    DbUtil.getPlayerData(targetUserId, DbUtil.TAG_ACTOR)
                end
            end
        end
    end
    self.isLife = true
    self.maxHp = GameConfig.initHP
    self.hp = self.maxHp
    self:changeMaxHealth(self.maxHp)
    self:setFoodLevel(20)
    self.foodLevel = 20
    self.isMaxFood = false
    self.backHomeTime = os.time() - GameConfig.homeCD
    self.backHallTime = os.time() - GameConfig.hallCD
    self.backMiningAreaTime = os.time() - GameConfig.miningAreaCD
    self.backProductTime = os.time() - GameConfig.productCD
    self.invinciblyTime = os.time()
    self.inventory = {}
    self.inHandItemId = 0
    self.exp = 0
    self.level = 1
	self.area = "20*20*20"
    self:initChunkIndex()
    self.taskController = TaskController.new(self)
	self.books = {}
	self.off_lineTime = 0
	self.products = {}

	self.maxDareTimes = tonumber(GameConfig.buyDaretask.initCount) or 0
	self.curDareTimes = 0
	self.nextTaskTime = os.time() + tonumber(GameConfig.refreshDareTask.nextTime)
	self.LastLoginDate = ""

	self.free_refresh_times = 0
	self.privilege = {}
    self.other_unlockTime = 0
	self.isSky = false
	self.cur_pri_item = {}
    self.has_ad = false
    self.stay_time = 0
    self.stay_hall_time = 0
    self.canSendCostDiamond = true
    self.initBlockCd = {}
    self.signIn_id = 1
    self.signIn_dayKey = "0"
    self.readyShowTreasure = false
    self.giveLoveLastNum = 1
    self.loginData = ""
    self.loveNum = 0
    self.loveNumType = 1
    self.buildGroupId = tostring(self.userId)
    self.buildGroupData = {}
    self.isNotInPartyCreateOrOver = true
    self.tips = "0"
    self.tipsUserId = "0"
    self.isNextGameStatus = "initGameStatus"
    self.taskIsLandLevel = 1
    self.getAllKeyScore = false
    self.allKeyScore = 0
    self.ownPicUrl = ""
    self.manorIsExistWhenReady = false
    self.msgTips = "0"
    self.recive_new_prive = false
    self.checkAreaNormalCd = 0
    self.signIn_start = "0"
    self.signIn_end = "0"
    self.havePriImgList = {}
    self.freeCreatePartyTimes = -1
    self.bulletin = {}
    self.getRewardTick = 0
    self.current_day_money = 0
    self.notShowSwitchRed = false
    self.notShowRichRed = false
end

function GamePlayer:initDataFromDB(data)
	if not data.__first then
        if data.respawnPos ~= nil and data.respawnPos.x and data.respawnPos.y and data.respawnPos.z then
            self.respawnPos = VectorUtil.newVector3(data.respawnPos.x, data.respawnPos.y, data.respawnPos.z)
            HostApi.log("respawnPos GamePlayer  " .. tostring(self.userId) .. " " .. self.respawnPos.x .. " " .. self.respawnPos.y .. " " .. self.respawnPos.z )
            if self.respawnPos.x < 0 or self.respawnPos.y < 0 or self.respawnPos.z < 0
             or self.respawnPos.x > 200 or self.respawnPos.y > 200 or self.respawnPos.z > 200 then
                self.respawnPos.x = 102
                self.respawnPos.y = 100
                self.respawnPos.z = 100
            end
        end
        self.oldExp = data.oldExp or 0
        self.loginData = data.loginData or ""
        self.giveLoveLastNum = data.giveLoveLastNum or 1
        self.bulletin = data.bulletin or {}
        self:canInitGiveLoveNum()
        self:teleInitPos()
    else
        self:teleportHall()
        HostApi.log("initDataFromDB first transport to hall userId: " .. tostring(self.userId))
	end
    GamePacketSender:sendBulletinData(self.rakssid, self.bulletin)
end

function GamePlayer:initDialogTipsDataFromDB(data)
    if not data.__first then
        self.tips = data.tips or "0"
        self.tipsUserId = data.tipsUserId or "0"
        self:showDbDialogTip()
    end
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
		self.privilege = data.privilege or {}
        self.other_unlockTime = data.other_unlockTime or 0
        self.backMiningAreaTime = data.backMiningAreaTime or (os.time() - GameConfig.miningAreaCD)
    	self.backProductTime = data.backProductTime or (os.time() - GameConfig.productCD)
    	self.isSky = data.isSky or false
        self.signIn_id = data.signIn_id or 1
        self.signIn_dayKey = data.signIn_dayKey or "0"
        self.signIn_start = data.signIn_start or "0"
        self.signIn_end = data.signIn_end or "0"
        self.havePriImgList = data.havePriImgList or {}
        self.notShowSwitchRed = data.notShowSwitchRed or false
        self.notShowRichRed = data.notShowRichRed or false
    else
        if self.enableIndie then
            GameAnalytics:design(self.userId, 1, {"g1048explevel_indie", 1})
            GameAnalytics:design(self.userId, 1, {"g1048area_indie", 1})
        else
            GameAnalytics:design(self.userId, 1, {"g1048explevel", 1})
            GameAnalytics:design(self.userId, 1, {"g1048area", 1})
        end

    	HostApi.log("initShareDataFromDB the data.__first is true ")
	end

	if self.isSky then
		HostApi.setCustomSceneSwitch(self.rakssid, 2, 2, true, true, true)
	else
		HostApi.setCustomSceneSwitch(self.rakssid, 2, 2, true, false, true)
	end

    GameSignIn:initSignInData(self)
    self:fillInvetoryBySlot(self.inventory)
    self.readyShowTreasure = false
    self:checkTreasureInInvetory()
    if not data.__first then
        self:setHealth(data.hp or self.maxHp)
        self:setFoodLevel(data.foodLevel or 20)
    end
	self:syncListInfo()
	self:setCurrency(self.money)
    PrivilegeConfig:setplayerPrivilege(self)
    PrivilegeConfig:setPrivilegePriImg(self)
    PrivilegeConfig:setInventoryPriImg(self)
    PrivilegeConfig:checkUnlockThreeNpc(self)
    GameSignIn:sendToClient(self)
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
    self:getMoneyChange()
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

function GamePlayer:getMoneyChange()
    local Today = DateUtil.getDate()
    if Today ~= self.loginData then
        if self.current_day_money == 0 then
            self.current_day_money = self.money
            return
        end
        local day_money = tonumber(self.money - self.current_day_money)
        if day_money >= 0 then
            GameAnalytics:design(self.userId, 0, { "g1048getMoneyEveryDay", tostring(Today), day_money})
        else
            GameAnalytics:design(self.userId, 0, { "g1048ConsumeMoneyEveryDay", tostring(Today), day_money})
        end
        self.current_day_money = self.money
    end
end

function GamePlayer:canInitGiveLoveNum()
    local data = DateUtil.getDate()
    if data ~= self.loginData then
        self.loginData = data
        self.giveLoveLastNum = 1 
    end
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

function GamePlayer:setInventory()
	if not DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
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

function GamePlayer:fillInvetoryBySlot(inventory)
	if not DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
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
    if items then
	    for i, item in pairs(items) do
	        if item.id > 0 and item.id < 10000 then
	            self:addItem(item.id, item.num, item.meta)
	        end
	    end
	end

end

function GamePlayer:teleportHall()
    if not self.entityPlayerMP then
        return
    end
    self.entityPlayerMP:setPositionAndUpdate(self.genislandPos.x, self.genislandPos.y, self.genislandPos.z)
    self:setPositionAndRotation(self.genislandPos, GameConfig.genislandYaw, 0)
    self:setRespawnPos(self.genislandPos)
    self.isGenisland = true
    HostApi.log("self.genislandPos x : "..self.genislandPos.x.." y : "..self.genislandPos.y.." z : "..self.genislandPos.z)
    self.backHallTime = os.time()
    self:bucketStateChange(false)
    GameParty:getLoveNum(self.userId)
    self:changeShowData()
    PrivilegeConfig:checkSpePri(self)
end

function GamePlayer:teleInitPos()
    if self.isGenisland == true then
        self:teleportHall()
        return
    end
    PrivilegeConfig:checkSpePri(self)
	if not GameMatch:isMaster(self.userId) then
        local targetUserId = GameMatch:getTargetByUid(self.userId)
	    if targetUserId ~= nil then
	        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
	        if targetManorInfo ~= nil then
	            local pos = GameConfig:getInitPosByManorIndex(targetManorInfo.manorIndex)
	            if pos then
                    self:teleportPos(pos)
				    self:setRespawnPos(pos)
                    self:bucketStateChange()
                    self:changeShowData()
	            	HostApi.log("teleInitPos " .. pos.x .. " " .. pos.y .. " " .. pos.z)
	            end
	        end
	    end
    else
    	self:checkBackHome()
    end
end

function GamePlayer:teleHomeWithYaw()
    local manorInfo = GameMatch:getUserManorInfo(self.userId)
    if not manorInfo then
        return
    end
    local pos = manorInfo:rPosToAPos(self.respawnPos)
	local aPos = VectorUtil.newVector3(pos.x + 0.5, pos.y, pos.z + 0.5)
	HostApi.log("teleHomeWithYaw  self.userId is : "..tostring(self.userId).." aPos is : " .. aPos.x .. " " .. aPos.y .. " " .. aPos.z .. " self.manorIndex is : " .. self.manorIndex)
    self:setFlying(false)
    self:teleportPos(aPos)
    self:setRespawnPos(aPos)
    self.isGenisland = false
    self.backHomeTime = os.time()
    self.isNextGameStatus = "initGameStatus"
    PrivilegeConfig:checkSpePri(self)
    GameMultiBuild:upDataBuildGroupData(self.userId, Define.visitMasterTips.notShow)
    GameParty:partySubPlayer(self.userId)
    MsgSender.sendCenterTipsToTarget(self.rakssid, 1, Messages:msgBackHomeSuccess())

    -- out of vistors list of current manor
    self.manorIndex = manorInfo.manorIndex
    self.target_user_id = manorInfo.userId
    self:initChunkIndex()
    GameMatch:leaveCurrentManor(self.userId)
    GameMatch:initCallOnTarget(self.userId, self.userId)

    -- must be self manor
    local targetUserId = GameMatch:getTargetByUid(self.userId)
    if targetUserId ~= nil then
        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil then
        	self.manorIndex = targetManorInfo.manorIndex
            self.target_user_id = targetManorInfo.userId
            self:initChunkIndex()

            -- player's manor is not exist when ready
            -- go home need to get data and init data
            if not self.manorIsExistWhenReady and self:checkReady() then
                targetManorInfo:masterLogin()
                targetManorInfo:refreshTime()
                targetManorInfo:initAllTree()
                targetManorInfo:initAllDirt()
                if not DBManager:isLoadDataFinished(targetManorInfo.userId, DbUtil.TAG_CHEST) then
                    DbUtil.getPlayerData(targetManorInfo.userId, DbUtil.TAG_CHEST)
                end
                if not DBManager:isLoadDataFinished(targetManorInfo.userId, DbUtil.TAG_FURNACE) then
                    DbUtil.getPlayerData(targetManorInfo.userId, DbUtil.TAG_FURNACE)
                end
                if not DBManager:isLoadDataFinished(targetManorInfo.userId, DbUtil.TAG_MANOR) then
                    DbUtil.getPlayerData(targetManorInfo.userId, DbUtil.TAG_MANOR)
                end
                self.manorIsExistWhenReady = true
            end

        end
    end
    self:showDbDialogTip()
    GameParty:getLoveNum(self.userId)
    self:changeShowData()
end

function GamePlayer:checkBackHome()
    --check home is in this server and party is over
    local manorInfo = GameMatch:getUserManorInfo(self.userId)
    if manorInfo == nil or manorInfo.isNotInPartyCreateOrOver == false then
        self.isNextGameStatus = Define.Npc_Func[1]
        MsgSender.sendTopTipsToTarget(self.rakssid, 1000, Messages:msgTransmitting())
        EngineUtil.sendEnterOtherGame(self.rakssid, "g1048", self.userId)
        if manorInfo then
            HostApi.log("checkBackHome self.userId : " .. tostring(self.userId).."  manorInfo.isNotInPartyCreateOrOver : "..tostring(manorInfo.isNotInPartyCreateOrOver))
        else
            HostApi.log("checkBackHome self.userId : " .. tostring(self.userId).."  home is in this server")
        end
        return true
    else
        local data = {}
        data.targetName = self.name
        data.show = false
        GamePacketSender:sendWelcomeVisitorTip(self.rakssid, json.encode(data))
        self:backHome()
        return false
    end
end

function GamePlayer:backHome()
    --check respawnPos is safe
    if self.respawnPos and self.respawnPos.x and self.respawnPos.y and self.respawnPos.z then
        local manorInfo = GameMatch:getUserManorInfo(self.userId)
        if manorInfo then
            local aPos = manorInfo:rPosToAPos(self.respawnPos)
            local d_Pos = VectorUtil.toBlockVector3(aPos.x, aPos.y, aPos.z)
            local d_BlockId = EngineWorld:getBlockId(d_Pos)
            local u_Pos = VectorUtil.toBlockVector3(aPos.x, aPos.y + 1, aPos.z)
            local u_BlockId = EngineWorld:getBlockId(u_Pos)
            if u_BlockId ~= BlockID.AIR then
                 self:moveHome(u_Pos, true)
            elseif d_BlockId ~= BlockID.AIR then
                 self:moveHome(d_Pos, true)
            end
            local newPos = manorInfo:rPosToAPos(self.respawnPos)
            local underBlockPos = VectorUtil.toBlockVector3(newPos.x, newPos.y - 1, newPos.z)
            local blockId = EngineWorld:getBlockId(underBlockPos)
            if blockId == BlockID.LAVA_MOVING or blockId == BlockID.LAVA_STILL or blockId == BlockID.WATER_MOVING or blockId == BlockID.WATER_STILL then
                self:moveHome(underBlockPos, false)
            end
            self:teleHomeWithYaw()
        end
    end
end

function GamePlayer:moveHome(vec3, isBlock)
    -- check Home has been occupied or destroyed
	if not DbUtil.CanSavePlayerData(self, DbUtil.TAG_PLAYER) then
        return
    end
    local manorInfo = GameMatch:getUserManorInfo(self.userId)
    if not manorInfo then
        return
    end
    local aPos = manorInfo:rPosToAPos(self.respawnPos)
	local pos1 = VectorUtil.toVector3i(vec3)--has been BlockVector3i
	local pos2 = VectorUtil.toBlockVector3(aPos.x, aPos.y, aPos.z)
	local pos3 = VectorUtil.toBlockVector3(aPos.x, aPos.y - 1, aPos.z)
	local pos4 = VectorUtil.toBlockVector3(aPos.x, aPos.y + 1, aPos.z)
	local basePos = VectorUtil.newVector3i(0,0,0)
   	if isBlock then
   		if VectorUtil.equal(pos1, pos2) or VectorUtil.equal(pos1, pos4) then
	        basePos = pos1
	        HostApi.log("Home has been occupied self.userId : " .. tostring(self.userId))
 		else
			return
	    end
	else
	    if VectorUtil.equal(pos1, pos3) then
        	local originPos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
            if originPos then
                basePos = VectorUtil.newVector3i(originPos.x, originPos.y - 5 , originPos.z)--起始点和基岩的高度差为5
            end
        	HostApi.log("Home has been destroyed self.userId : " .. tostring(self.userId))
		else
			return
    	end
    end
	local posY = basePos.y
    local bodyPos_1 = VectorUtil.toBlockVector3(basePos.x, posY + 1, basePos.z)
    local bodyPos_2 = VectorUtil.toBlockVector3(basePos.x, posY + 2, basePos.z)
    local blockId_1 = EngineWorld:getBlockId(bodyPos_1)
    local blockId_2 = EngineWorld:getBlockId(bodyPos_2)
    local is_airWall = false
    local posY_upperLimit = GameConfig.airWall_MaxY - 2--252--basePos.y + math.ceil(GameConfig.manorMax)
    --正常情况用不到（万一复活点腿部被空气墙占用）
    if blockId_1 == 916 then
    	posY_upperLimit = bodyPos_1.y
    	bodyPos_2 = VectorUtil.toBlockVector3(bodyPos_1.x, bodyPos_1 - 1, bodyPos_1.z)
    	bodyPos_1 = VectorUtil.toBlockVector3(bodyPos_1.x, bodyPos_1 - 2, bodyPos_1.z)
    	EngineWorld:setBlockToAir(bodyPos_1)
    	EngineWorld:setBlockToAir(bodyPos_2)
    	is_airWall = true
    end

    while is_airWall == false and (blockId_1 ~= BlockID.AIR or blockId_2 ~= BlockID.AIR) and posY <= posY_upperLimit do
        posY = posY + 1
	    bodyPos_1 = VectorUtil.toBlockVector3(basePos.x, posY + 1, basePos.z)
	  	bodyPos_2 = VectorUtil.toBlockVector3(basePos.x, posY + 2, basePos.z)
	    blockId_1 = EngineWorld:getBlockId(bodyPos_1)
	    blockId_2 = EngineWorld:getBlockId(bodyPos_2)
        if not isBlock then
        	if VectorUtil.equal(pos1, bodyPos_1) then
        		blockId_1 = BlockID.AIR
        	end
        	if VectorUtil.equal(pos1, bodyPos_2) then
        		blockId_2 = BlockID.AIR
        	end
        end
        --正常情况用不到（万一复活点头部被空气墙占用）
	    if blockId_2 == 916 then
	    	posY_upperLimit = bodyPos_2.y
	    	bodyPos_2 = bodyPos_1
	    	bodyPos_1 = VectorUtil.toBlockVector3(bodyPos_1.x, bodyPos_1 - 1, bodyPos_1.z)
	    	EngineWorld:setBlockToAir(bodyPos_1)
	    	EngineWorld:setBlockToAir(bodyPos_2)
	    	break
	    end
    end
    local pos = manorInfo:aPosToRPos(VectorUtil.newVector3(bodyPos_1.x + 0.5, bodyPos_1.y, bodyPos_1.z + 0.5))
    if pos then
	   self.respawnPos = pos
    end
end

function GamePlayer:addExp(exp)
    local last_level = IslandLevelConfig:getLevelByExp(self.exp)
    self.exp = self.exp + exp
    if self.exp < 0 then
        self.exp = 0
    end
	self.level = IslandLevelConfig:getLevelByExp(self.exp)
	if self.level >= 2  and self.dareIsUnLock == false then
		self.dareIsUnLock = true
		self.taskController:randomDareTask()
		self.taskController:sendMainAndDareTaskToClient()
	end
    if last_level ~= self.level then
        if self.enableIndie then
            GameAnalytics:design(self.userId, 1, {"g1048explevel_indie", self.level})
            GameAnalytics:design(self.userId, -1, {"g1048explevel_indie", last_level})
        else
            GameAnalytics:design(self.userId, 1, {"g1048explevel", self.level})
            GameAnalytics:design(self.userId, -1, {"g1048explevel", last_level})
        end

        self:sendSkyBlockShopInfo()
        GameParty:sendPlayerPartyData(self.userId)
    end
	self:syncListInfo()
    if self.exp and self.level then
        HostApi.log("GamePlayer:addExp " .. tostring(self.userId) .. " exp: " .. self.exp .. " level: " .. self.level)
    end
end

function GamePlayer:update()
    if self.initBlockCd then
        for hash, time in pairs(self.initBlockCd) do
            if time > 0 then
                self.initBlockCd[hash] = self.initBlockCd[hash] - 1
            end
        end
    end
end

function GamePlayer:initChunkIndex()
	local target_pos = GameConfig:getInitPosByManorIndex(self.manorIndex)
	local origin_pos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
	if origin_pos and target_pos then
		self.posOriginX = origin_pos.x - GameConfig.manorMax / 2
		self.posOriginY = origin_pos.y - GameConfig.manorMax / 2
		self.posOriginZ = origin_pos.z - GameConfig.manorMax / 2

		self.curXChunkIndex = math.floor((target_pos.x - self.posOriginX) / GameConfig.manorChunk)
		self.curYChunkIndex = math.floor((target_pos.y - self.posOriginY) / GameConfig.manorChunk)
		self.curZChunkIndex = math.floor((target_pos.z - self.posOriginZ) / GameConfig.manorChunk)
	end

    self.lastXChunkIndex = 0
    self.lastYChunkIndex = 0
    self.lastZChunkIndex = 0
end

function GamePlayer:generateSubKey(hash, value)
	local posX, posY, posZ = GameConfig:decodeHash(hash)
	local blockId, blockMeta = GameConfig:decodeValue(value)

	local curXChunkIndex = math.floor(posX / GameConfig.manorChunk)
	local curYChunkIndex = math.floor(posY / GameConfig.manorChunk)
	local curZChunkIndex = math.floor(posZ / GameConfig.manorChunk)

	return DbUtil.TAG_BLOCK_PRE .. "_" .. curXChunkIndex .. "_" .. curYChunkIndex .. "_" .. curZChunkIndex .. "v2"
end

function GamePlayer:onMove(x, y, z)
    if self:checkReady() == false then
        return
    end

    if self.isGenisland == true then
        return
    end

    if self.isNotInPartyCreateOrOver == false then
		return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(self.target_user_id)
    if targetManorInfo == nil then
        return
    end

	local playerPos = VectorUtil.newVector3i(math.floor(self:getPosition().x), math.floor(self:getPosition().y), math.floor(self:getPosition().z))
	self.curXChunkIndex = math.floor((playerPos.x - self.posOriginX) / GameConfig.manorChunk)
	self.curYChunkIndex = math.floor((playerPos.y - self.posOriginY) / GameConfig.manorChunk)
	self.curZChunkIndex = math.floor((playerPos.z - self.posOriginZ) / GameConfig.manorChunk)
    
	if self.curXChunkIndex ~= self.lastXChunkIndex
		or self.curYChunkIndex ~= self.lastYChunkIndex
		or self.curZChunkIndex ~= self.lastZChunkIndex then
            self:checkNeedLoadData()
	end
	self.lastXChunkIndex = self.curXChunkIndex
	self.lastYChunkIndex = self.curYChunkIndex
	self.lastZChunkIndex = self.curZChunkIndex
end

function GamePlayer:checkChunkLoad()
    local targetManorInfo = GameMatch:getUserManorInfo(self.target_user_id)
    if targetManorInfo == nil then
        return false
    end

    if self.curXChunkIndex and self.curYChunkIndex and self.curZChunkIndex and self.target_user_id then
        local subKey = DbUtil.TAG_BLOCK_PRE .. "_" .. self.curXChunkIndex .. "_" .. self.curYChunkIndex .. "_" .. self.curZChunkIndex
        local newKey = subKey .. "v2"
        if DBManager:isLoadDataFinished(self.target_user_id, newKey) and not targetManorInfo.newKeyLock[newKey] then
        -- if DBManager:isLoadDataFinished(self.target_user_id, newKey) or DBManager:isLoadDataFinished(self.target_user_id, subKey) then
            return true
        end
    end

    return false
end

function GamePlayer:checkNeedLoadData()
	local manorNear = GameConfig.manorNear
	local x_down = GameConfig:checkChunk(self.curXChunkIndex - manorNear)
	local x_up = GameConfig:checkChunk(self.curXChunkIndex + manorNear)
	local y_down = GameConfig:checkChunk(self.curYChunkIndex - manorNear)
	local y_up = GameConfig:checkChunk(self.curYChunkIndex + manorNear)
	local z_down = GameConfig:checkChunk(self.curZChunkIndex - manorNear)
	local z_up = GameConfig:checkChunk(self.curZChunkIndex + manorNear)

	for x = x_down, x_up do
		for y = y_down, y_up do
			for z = z_down, z_up do
				local subKey = DbUtil.TAG_BLOCK_PRE .. "_" .. x .. "_" .. y .. "_" .. z
                local newKey = subKey .. "v2"
                if not DBManager:isLoadDataFinished(self.target_user_id, newKey) then    
                    HostApi.log("GamePlayer:checkNeedLoadData " .. tostring(self.userId) .. " " .. tostring(self.target_user_id) .. " " .. newKey)
					if self.target_user_id ~= nil then
                        if self.initBlockCd[newKey] == nil or self.initBlockCd[newKey] == 0 then
    						DbUtil.getPlayerData(self.target_user_id, newKey)
                            self.initBlockCd[newKey] = 30
                        end
					end
				end
			end
		end
	end
end

function GamePlayer:getExtendArea()
    local cur_player = self:getShowDataPlayer()
	local prices = IslandAreaConfig:getPriceByArea(cur_player.area)
	if (cur_player:checkIsHAvePri(PrivilegeConfig.priType.extend_area_half)) then
		prices = math.floor(prices / 2)
        if prices < 1 then
            prices = 1
        end
	end
	local number = IslandAreaConfig:getIdByMasterArea(self)
	if number < 5 then
        if prices > 0 then
            if self:checkMoney(Define.Money.MONEY, prices) then
                self:subCurrency(prices)
                GameAnalytics:design(self.userId, 0, { "g1048subMoney", tostring(self.userId), prices})
                cur_player.area = IslandAreaConfig:getAreaById(number + 1)
                MsgSender.sendCenterTipsToTarget(self.rakssid, 1, Messages:msgExtendAreaTip())
                cur_player.taskController:unLockMainlineTask()

                cur_player:checkAllChangeDataToMember()
                cur_player.taskController:packingProgressData(cur_player, Define.TaskType.expand_islands, GameMatch.gameType, 0, number + 1, 0)
                if self.enableIndie then
                    GameAnalytics:design(self.userId, 1, {"g1048area_indie", number + 1})
                    GameAnalytics:design(self.userId, -1, {"g1048area_indie", number})
                else
                    GameAnalytics:design(self.userId, 1, {"g1048area", number + 1})
                    GameAnalytics:design(self.userId, -1, {"g1048area", number})
                end
            end
        end
	end
	cur_player:syncListInfo()
	return self.area
end

function GamePlayer:checkExtendArea()
    local cur_player = self:getShowDataPlayer()
	local tips = IslandAreaConfig:getTipByArea(cur_player.area)
	local prices = IslandAreaConfig:getPriceByArea(cur_player.area)
	if (cur_player:checkIsHAvePri(PrivilegeConfig.priType.extend_area_half)) then
		prices = math.floor(prices / 2)
        if prices < 1 then
            prices = 1
        end
	end

	if IslandAreaConfig:getIdByMasterArea(self) == 5 then
		HostApi.sendCustomTip(self.rakssid, tips, "BuyExArea".. "#" .. tostring(self.rakssid))
	else
		HostApi.showConsumeCoinTip(self.rakssid, tips, 3, prices, "BuyExArea".. "#" .. tostring(self.rakssid))
	end
end

function GamePlayer:checkExtendAreaTip()
    local cur_player = self:getShowDataPlayer()
    local tips = IslandAreaConfig:getTipByArea(cur_player.area)
    local prices = IslandAreaConfig:getPriceByArea(cur_player.area)
    if (cur_player:checkIsHAvePri(PrivilegeConfig.priType.extend_area_half)) then
        prices = math.floor(prices / 2)
        if prices < 1 then
            prices = 1
        end
    end

    if IslandAreaConfig:getIdByMasterArea(self) == 5 then
        HostApi.sendCustomTip(self.rakssid, tips, "BuyExArea".. "#" .. tostring(self.rakssid))
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:msgNotEnlargeBlockPlaceTip())
    else
        HostApi.showConsumeCoinTip(self.rakssid, tips, 3, prices, "BuyExArea".. "#" .. tostring(self.rakssid))
        MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:msgEnlargeBlockPlaceTip())
    end
end

function GamePlayer:getListInfo()
	local lv = self.level or 1
	local cur_exp = self.exp or 0
	local max_exp, is_max = IslandLevelConfig:getMaxExp(self.level)
	local area = self.area or "20*20*20"
	local island_lv = IslandAreaConfig:getIdByMasterArea(self)
	return lv, cur_exp, max_exp, island_lv, area, is_max
end

function GamePlayer:syncListInfo()
    self:checkShareDataToMember()
    local lv, cur_exp, max_exp, island_lv, area, is_max = self:getListInfo()
    HostApi.sendShowSkyBlockInfo(self.rakssid, lv, cur_exp, max_exp, island_lv, area, is_max, true)
end

function GamePlayer:checkAllChangeDataToMember()
    if DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) and DbUtil.CanSavePlayerData(self, DbUtil.TAG_MIAN_LINE) then
        self:checkShareDataToMember()
        self:changeShowData()
        self:checkTaskDataToMember()
        self:changeTaskData()
    end

end

function GamePlayer:checkShareDataToMember()
    if tostring(self.userId) == tostring(self.buildGroupId) and #self.buildGroupData > 1 then
        for k, v in pairs(self.buildGroupData) do
            local player = PlayerManager:getPlayerByUserId(v.memberId)
            if tostring(v.memberId) ~= tostring(v.masterId) and tostring(v.memberId) ~= tostring(self.userId) and player then
                if DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) then
                    player:changeShowData()
                end
            end
        end
    end 
end

function GamePlayer:changeShowData()
    if DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) and DbUtil.CanSavePlayerData(self, DbUtil.TAG_MIAN_LINE) then
        local cur_player, isSelfData = self:getShowDataPlayer()
        local lv, cur_exp, max_exp, island_lv, area, is_max = cur_player:getListInfo()
        HostApi.sendShowSkyBlockInfo(self.rakssid, lv, cur_exp, max_exp, island_lv, area, is_max, isSelfData)
    end
end

function GamePlayer:quitAskMemeberChangeShow()
    GameMultiBuild:checkMasterQuitTips(self)
    self:checkShareDataToMember()
    GameMultiBuild:checkMemberBucketState(self)
end


function GamePlayer:checkTaskDataToMember()
    if tostring(self.userId) == tostring(self.buildGroupId) and #self.buildGroupData > 1 then
        for k, v in pairs(self.buildGroupData) do
            local player = PlayerManager:getPlayerByUserId(v.memberId)
            if tostring(v.memberId) ~= tostring(v.masterId) and tostring(v.memberId) ~= tostring(self.userId) and player then
                if DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) then
                    player:changeTaskData()
                end
            end
        end
    end 
end

function GamePlayer:changeTaskData()
    if DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) and DbUtil.CanSavePlayerData(self, DbUtil.TAG_MIAN_LINE) then
        local masterIdPlayer = PlayerManager:getPlayerByUserId(self.buildGroupId)
        if masterIdPlayer then
            local masterLevel = IslandAreaConfig:getIdByArea(masterIdPlayer.area) or 1
            if self.taskIsLandLevel < masterLevel then
                self.taskIsLandLevel = masterLevel
            end
        end

        self.taskController:unLockMainlineTask()
    end
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

function GamePlayer:getShowDataPlayer()
    if GameMultiBuild:isShowMasterLevel(self.userId, self.buildGroupId) and not self.isGenisland then
        local masterIdPlayer = PlayerManager:getPlayerByUserId(self.buildGroupId)
        if masterIdPlayer then
            return masterIdPlayer, false
        end
    end
    return self, true
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

function GamePlayer:checkBookInfo(itemId)
	for i, v in pairs(self.books) do
		if v == itemId then
			return
		end
	end
	table.insert(self.books, itemId)
	for i, v in pairs(self.books) do
		if i == GameConfig.maxBook then
			-- HostApi.setCustomSceneSwitch(self.rakssid, 2, 2, true, true, true)
			-- self.isSky = true
			HostApi.log("the star has come on !!!")
		end
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

    if self.enableIndie then
        GameAnalytics:design(self.userId, 0, { "g1048refresh_task_indie", "g1048refresh_task_indie"})
    else
        GameAnalytics:design(self.userId, 0, { "g1048refresh_task", "g1048refresh_task"})
    end
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
	if self.nextTaskTime - os.time() <= 0 and DbUtil.CanSavePlayerData(self, DbUtil.TAG_MIAN_LINE) and DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA) then
		if self.free_refresh_times < 0 then
			self.free_refresh_times = 0
		end

		if self.free_refresh_times < GameConfig.refreshDareTask.freeTimes then
			self.free_refresh_times = self.free_refresh_times + 1
		end

		self.nextTaskTime = os.time() + tonumber(GameConfig.refreshDareTask.nextTime)
		self:sendDareTaskInfo()
	end
    self.stay_time = self.stay_time + 1
    if isGenisland then
        self.stay_hall_time = self.stay_hall_time + 1
    end
    self.checkAreaNormalCd = self.checkAreaNormalCd + 1
    if self.checkAreaNormalCd > 10 then
        self.checkAreaNormalCd = 0
        if not self:checkAreaNormal() then
            local playerPos = VectorUtil.newVector3i(math.floor(self:getPosition().x), math.floor(self:getPosition().y), math.floor(self:getPosition().z))
            HostApi.log("player may be cheat checkAreaNormal error" .. tostring(self.userId) .. " " .. tostring(self.target_user_id) .. " pos: " .. playerPos.x .. " " .. playerPos.y .. " " .. playerPos.z)
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        end
    end
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

function GamePlayer:setRank(id, curRank, curScore)
    local npc = RankConfig:getRankNpc(id)
	if npc then
		local data = {
			rank = tonumber(curRank),
			name = tostring(self.name),
			userId = tonumber(tostring(self.userId)),
			score = tonumber(curScore),
            picUrl = self.ownPicUrl or ""
		}
		npc:sendRankData(self, data)
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

function GamePlayer:checkIsCanUnlockByOther(priId)
    for k, pri in pairs(self.privilege) do
        if pri.itemId == priId then
            return false
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
    return DbUtil.CanSavePlayerData(self, DbUtil.TAG_SHAREDATA)
end

function GamePlayer:checkTreasureInInvetory(targetRakssid)
    -- 有targetRakssid就把自己宝石状态单独发给targetRakssid
    -- 没有targetRakssid就把自己宝石状态广播给所有人(包括自己)
    targetRakssid = targetRakssid or false
    local num = self:getItemNumById(511, 0)
    if num >= 1 then
        if self.readyShowTreasure == false then --自己已经显示有宝石了，就不重复显示
            self:changeClothes("custom_glasses", 12)
            if self.entityPlayerMP then
                local builder = DataBuilder.new()
                builder:addParam("text", "skyblock_has_treasure")
                builder:addVector3Param("offset", VectorUtil.newVector3(0, 3.5, 0))
                self.entityPlayerMP:addCustomText("TreasureHint", builder:getData(), -1, 1)
                self.readyShowTreasure = true
            end

        end
    else
        self:resetClothes()
        self:checkShowPriSign()
        if self.entityPlayerMP then
            self.entityPlayerMP:clearEntityFeature("TreasureHint")
            self.readyShowTreasure = false
        end
    end
end


function GamePlayer:giveLoveSuccess(targetUserId)
   self.giveLoveLastNum = self.giveLoveLastNum - 1
   if self.giveLoveLastNum < 0 then
        self.giveLoveLastNum = 0
   end

   GameParty:sendPlayerPartyData(self.userId)
end

function GamePlayer:updateLoveNum(loveNum, targetUserId)
    local cur_targetUserId = GameMatch:getTargetByUid(self.userId)

    if GameMatch:isMaster(self.userId) then
        self.loveNumType = Define.PartyLoveNumType.mySelf
    else
        self.loveNumType = Define.PartyLoveNumType.otherPlayer
    end

    if tostring(cur_targetUserId) == tostring(targetUserId) then
        self.loveNum = loveNum
        GameParty:sendPlayerPartyData(self.userId)
        GameParty:sendUpDateLoveNum(self.userId, targetUserId, loveNum)
    else
        GameParty:sendUpDateLoveNum(self.userId, targetUserId, loveNum)
    end
end

function GamePlayer:checkCanGiveLove(targetUserId)
    if self.giveLoveLastNum <= 0 then
        HostApi.sendCommonTip(self.rakssid, "gui_give_love_is_max")
        return false
    end

    if self.level < GameConfig.giveLoveNumLevel then
        HostApi.sendCommonTip(self.rakssid, "gui_level_too_low")
        return false
    end

    if tostring(targetUserId) == tostring(self.userId) then
        HostApi.sendCommonTip(self.rakssid, "gui_not_give_love_to_self")
        return false
    end

    return true
end

function GamePlayer:visitorCheckCanGetBlockData()
    local targetUserId = GameMatch:getTargetByUid(self.userId)
    if targetUserId then
        local targetPlayer = PlayerManager:getPlayerByUserId(targetUserId)
        if targetPlayer then
            if targetPlayer.isNotInPartyCreateOrOver == false then
                return false
            end
        end
    end

    return true
end

function GamePlayer:showDbDialogTip()
    print("showDbDialogTip == "..self.tips.." userId == "..tostring(self.userId))
    if self.tips ~= "0" and self.tipsUserId ~= "0" and tostring(self.target_user_id) == tostring(self.tipsUserId) then
        HostApi.sendCommonTip(self.rakssid, self.tips)
    end
    self.tips = "0"
    self.tipsUserId = "0"
end

function GamePlayer:checkShowPriSign()
    if not PrivilegeConfig:checkPlayerShowPri(self) then
        return
    end
    self:changeClothes("custom_crown", 10)
end

function GamePlayer:getOwnPicUrl()
    local userIds = {}
    userIds[1] = tonumber(self.userId)
    UserInfoCache:GetCacheByUserIds(userIds, function()
        local info = UserInfoCache:GetCache(tostring(self.userId))
        if info ~= nil then
            self.ownPicUrl = info.picUrl or ""
        end
    end, "")
end

function GamePlayer:logPlayerBuildGroup(infoHead)
    if infoHead == nil then
        return
    end

    local logAll = infoHead .. " | "
    logAll = logAll .. " self logPlayerBuildGroup userId : " .. tostring(self.userId)
    if self.buildGroupId then
        logAll = logAll .. " groupId : " .. tostring(self.buildGroupId)
    end
    if self.buildGroupData then
        logAll = logAll .. " memberId : "
        for _, member in pairs(self.buildGroupData) do
            logAll = logAll .. tostring(member.memberId) .. " "
        end
    end

    if self.target_user_id then
        local master = PlayerManager:getPlayerByUserId(self.target_user_id)
        if master then
            logAll = logAll .. " master logPlayerBuildGroup userId : " .. tostring(master.userId)
            if master.buildGroupId then
                logAll = logAll .. " groupId : " .. tostring(master.buildGroupId)
            end
            if master.buildGroupData then
                logAll = logAll .. " memberId : "
                for _, member in pairs(master.buildGroupData) do
                    logAll = logAll .. tostring(member.memberId) .. " "
                end
            end
        end
    end

    HostApi.log(logAll)
end

function GamePlayer:gotoNextGame(nextGameType, isButton)
    local isBtn = isButton or false
    if not type(nextGameType) == "string" then
        return
    end
    local interval = 0
    if  nextGameType == "g1048" then
        interval =  GameConfig.homeCD + self.backHomeTime - os.time()
        if  interval <= 0 then
            if not DbUtil.CanSavePlayerData(self, DbUtil.TAG_PLAYER) then
                return
            end
            self.backHomeTime = os.time()
            self:checkBackHome()
            if not isBtn then
                if self.enableIndie then
                    GameAnalytics:design(self.userId, 0, { "g1048door_indie", 1})
                else
                    GameAnalytics:design(self.userId, 0, { "g1048door", 1})
                end
            end
        elseif isBtn then
            MsgSender.sendTopTipsToTarget(self.rakssid, 1, Messages:msgUIButtonCountDown(interval))
        end
        return
    elseif nextGameType == "g1049" then
        interval =  GameConfig.miningAreaCD + self.backMiningAreaTime - os.time()
        if  interval <= 0 then
            self.backMiningAreaTime = os.time()
            self.isNextGameStatus = Define.Npc_Func[2]
            if not isBtn then
                if self.enableIndie then
                    GameAnalytics:design(self.userId, 0, { "g1048door_indie", 2})
                else
                    GameAnalytics:design(self.userId, 0, { "g1048door", 2})
                end
            end
            HostApi.log("gotoNextGame goto g1049 and self.userId is : "..tostring(self.userId))
        end
    elseif nextGameType == "g1050" then
        interval =  GameConfig.productCD + self.backProductTime - os.time()
        if  interval <= 0 then
            self.backProductTime = os.time()
            self.isNextGameStatus = Define.Npc_Func[3]
            if not isBtn then
                if self.enableIndie then
                    GameAnalytics:design(self.userId, 0, { "g1048door_indie", 3})
                else
                    GameAnalytics:design(self.userId, 0, { "g1048door", 3})
                end
            end
            HostApi.log("gotoNextGame goto g1050 and self.userId is : "..tostring(self.userId))
        end
    else
        return
    end
    BackHallManager.nextGameType = nextGameType
    BackHallManager:addPlayer(self)
    MsgSender.sendTopTipsToTarget(self.rakssid, 1000, Messages:msgTransmitting())
end

function GamePlayer:initWelcomeText()
    for __,v in pairs(GameConfig.welcomeText) do
        local pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        HostApi.addWallText(self.rakssid, 0, 0, tostring(v.text), pos, v.scale, v.yaw, 0, {1, 1, 1, 1})
    end
end

function GamePlayer:bucketStateChange(force)
    if force == nil then
        local targetManorInfo = GameMatch:getUserManorInfo(self.target_user_id)
        if targetManorInfo then
            HostApi.sendCanUseItemBucket(self.rakssid, targetManorInfo:checkPower(self.userId))
        end
    else
        HostApi.sendCanUseItemBucket(self.rakssid, force)
    end
end

function GamePlayer:designStayTime()
    local mod = 0
    if self.stay_hall_time >= 0 and self.stay_hall_time < 15 then
        mod = 1
    elseif self.stay_hall_time >= 15 and self.stay_hall_time < 60 then
        mod = 2
    else
        mod = 3
    end
    if self.enableIndie then
        GameAnalytics:design(self.userId, 0, { "g1048StayHall_indie", mod} )
    else
        GameAnalytics:design(self.userId, 0, { "g1048StayHall", mod} )
    end
end

function GamePlayer:checkAreaNormal()

    if self:checkReady() == false then
        return true
    end

    local targetManorInfo = GameMatch:getUserManorInfo(self.target_user_id)
    if targetManorInfo then
        local playerPos = VectorUtil.newVector3i(math.floor(self:getPosition().x), math.floor(self:getPosition().y), math.floor(self:getPosition().z))
        if playerPos then
            if self.isGenisland then
                if self:checkIsInHallArea(playerPos) then
                    return true
                end
            else

                if targetManorInfo:isNormal(playerPos) then
                    return true
                end
            end
        end
    end
    return false
end

function GamePlayer:checkIsInHallArea(pos)
    return pos.x >= 801 and pos.x <= 1267
        and pos.y <= 254
        and pos.z >= 249 and pos.z <= 718
end

function GamePlayer:sendSkyBlockPriImg()
    GamePacketSender:sendSkyBlockPriImg(self.rakssid, self.havePriImgList)
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

function GamePlayer:updatefreeCreateNum(data)
    print("updatefreeCreateNum data == "..data.." self.userId == "..tostring(self.userId))
    self.freeCreatePartyTimes = tonumber(data) or -1
    GameParty:sendPlayerPartyData(self.userId)
end

function GamePlayer:CheckClickBulletin(curTable)
    print(" CheckClickBulletin userId == "..tostring(self.userId))
    for _, v in pairs(self.bulletin) do
        if v.TabId == curTable.TabId and v.GroupId == curTable.GroupId  then
            v.UpdateTime = curTable.UpdateTime
            GamePacketSender:sendBulletinData(self.rakssid, self.bulletin)
            return
        end
    end

    table.insert(self.bulletin, curTable)
    GamePacketSender:sendBulletinData(self.rakssid, self.bulletin)
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
    GameAnalytics:design(self.userId, 0, { "g1048buy_money_suc", level, tonumber(price)})
end

function GamePlayer:buyMoneySuccess(level)
    print("buyMoneySuccess userId == "..tostring(self.userId))
    local money = level * GameConfig.onceMoneyNum
    HostApi.notifyGetItem(self.rakssid, 10000, 0, money)
    GameAnalytics:design(self.userId, 0, { "g1048addMoney", tostring(self.userId), tonumber(money)})
    self:addCurrency(money)
end

function GamePlayer:checkCanGetRewardTime()
    local rewardTime = 0

    if self.getRewardTick > 0 then
        rewardTime = self.getRewardTick - os.time()
    end

    if rewardTime <= 0 then
        rewardTime = 0
    end
    return rewardTime
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

function GamePlayer:removeCurItem(num)
    num = num or 1
    local inv = self:getInventory()
    if not inv then
        return false
    end
    local slot = inv:getInventoryIndexOfCurrentItem()
    if slot ~= -1 then
        self:decrStackSize(slot, num)
        return true
    end
    return false
end

return GamePlayer

--endregion
