GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self.team = nil
    self.teamId = self.dynamicAttr.team

    self.dress = {}
    self.equip_dress = {}
    self.custom_goods = {}
    self.block_goods = {}
    self.skin_order = {}
    self.skin_order = {}
    self.wall_order = {}
    self.equip_action = {}
    self.command_list = {}
    self.initItems = {}

    self:initStaticAttr(0)

    self.multiKill = 0
    self.lastKillTime = 0
    self.kills = 0
    self.killAIs = 0
    self.isLife = true
    self.realLife = true
    self.standTime = 0
    self.deathNum = 0
    self.initLevel = 1

    self.isKeepItem = false
    self.curApple = 0
    self.maxApple = 0
    self.inventory = {}
    self.enderInventory = {}

    self.rewards = {}

    self.level = 1
    self.yaoshi = 0
    self.cur_exp = 0
    self.honor = 0
    self.honorId = 0

    self.surviveTime = -1

    self.expDoubleTime = 0
    self.privilegeCard = {}
    self.chest_integral = 0
    self.chests = {}
    self.init_exp = false
    self.rune = {}
    self.equip_rune = {}

    self.limit_item = {}

    self.isRespawn = true
    self.isStartMinute = true
    self.isRewardDoubleKey = false
    self.isPartyUser = false
    self.unlock_level = {}

    self.play_days = 0
    self.play_day_key = 0

    self.license_season = 0
    self.license_buy = 0
    self.license_exp = 0
    self.license_level = 1
    self.license_isNew = true
    self.hadOpenLicense = false
    self.had_horn = 0
    self.get_free_reward = {}
    self.get_pay_reward = {}
    self.exchange_reward = {}
    self.refresh_times = 1
    self.license_week_exp = 0
    self.license_week = 0
    self.free_upgrade_time = 0
    self.show_effect_task = {}

    self.challenges = {}
    self.needNewChallengeTip = false
    self.changeTaskId = -1
    self.challenge_ad = false

    self.addSpeed = 0

    self.banCureBuff = false

    GameInfoManager.Instance():newPlayerInfo(self.userId)
    self.info = GameInfoManager.Instance():getPlayerInfo(self.userId)
    SettlementManager:addPlayer(self)

    ReportManager:reportUserEnter(self.userId)
    self:sendLotteryConfig()

    self:getUserSeasonInfo(3)
    self:setFoodLevel(20)
    self:initTeam()

    if GameMatch:isGameStart() then
        self:teleInitPos()
    else
        self:teleWaitPos()
    end
    self.taskController = GameTaskController.new(self)
end

function GamePlayer:initDBDataFinished()
    GamePacketSender:sendBedWarPlayerKeyNum(self.rakssid, self.yaoshi)
    Privilege.syncPrivilegeTime(self)
    RuneManager:onPlayerInitRune(self)
    self:initItem()
    TreasureChallenge:onPlayerReady(self)
    self.taskController:init()
    TreasureChallenge:refreshChallengeTask(self)
    GameLicense:updatePlayerData(self)
    if tonumber(self.play_day_key) < tonumber(DateUtil.getYearDay()) then
        self.play_days = self.play_days + 1
        self.play_day_key = DateUtil.getYearDay()
        self.taskController:packingProgressData(Define.TaskType.GrowUp.DailyLogin, nil, 1)
        ReportUtil.reportPlayerLoginLicenseLevel(self)
        ReportUtil.reportDailyPlayerLevel(self)
        TreasureChallenge:resetTreasureAd(self)
    end

    if self.hasReport == false then
        GameAnalytics:design(self.userId, 1, { "Player_Lv", self.level })
        self.hasReport = true
    end
    self.initLevel = self.level
    GamePacketSender:sendBedWarPlayerLevel(self.rakssid, self.level)
    if #self.dress == 0 then
        GameDressStore:initDressStoreData(self)
    end
    if #self.chests == 0 then
        GameChestLottery:initChestLotteryData(self)
    end
    GameChestLottery:syncChestLotteryData(self)
    GameDressStore:syncDressStoreData(self)

    CustomShop:onSyncPlayerAllGoods(self)
    BlockShop:onSyncPlayerAllGoods(self)
    GameCommandShop:onSyncPlayerCommandList(self)
    CommonPacketSender:sendTicketNum(self.rakssid, self.ticket_num)
    self:changeClothes("custom_hat", 0)
end

function GamePlayer:onSpawnInGame()
    self.isRespawn = true
    self.damageType = nil
    self.lifeStatus = true
    self.lastGroundPos = nil
    self:teleInitPos()
    self:fillEnderInvetory(self.enderInventory)
    if self.isKeepItem then
        self:fillInvetory(self.inventory)
    else
        self:initItem()
    end
    local ai = AIManager:getAIByRakssid(self.rakssid)
    if ai ~= nil then
        ai:onRespawn()
    end
    AddMaxHPPrivilege:onPlayerRespawn(self)
    CustomShopCloth:onPlayerRespawn(self)
    self:addCustomEffect("InvincibleEffect" .. tostring(self.userId), GameConfig.InvincibleEffect, GameConfig.invincibleTime * 1000)
    self.isInvincible = true
    LuaTimer:schedule(function()
        self.isInvincible = false
    end, GameConfig.invincibleTime * 1000)
end

function GamePlayer:addLvUpReward()
    local yaoshi, dress_id_list, dress_num_list, rune_id_list, rune_num_list, box_id, box_num = ExpConfig:getLevelReward(self.level)
    self.yaoshi = self.yaoshi + yaoshi
    --add dress fragment
    if dress_id_list and dress_num_list and #dress_id_list == #dress_num_list then
        for i = 1, #dress_id_list do
            local item = {}
            item.Id = dress_id_list[i]
            item.Count = dress_num_list[i]
            item.Type = 1
            table.insert(self.rewards, item)
        end
    end
    --add rune fragment
    if rune_id_list and rune_num_list and #rune_id_list == #rune_num_list then
        for i = 1, #rune_id_list do
            local item = {}
            item.Id = rune_id_list[i]
            item.Count = rune_num_list[i]
            item.Type = 2
            table.insert(self.rewards, item)
        end
    end
    --add box
    if box_id > 0 and box_num > 0 then
        local item = {}
        item.Id = box_id
        item.Count = box_num
        item.Type = 3
        table.insert(self.rewards, item)
    end

    if self.hasReport == true then
        GameAnalytics:design(self.userId, 1, { "Player_Lv", self.level })
        GameAnalytics:design(self.userId, -1, { "Player_Lv", self.level - 1 })
    end
end

function GamePlayer:addExp(exp)
    local oldLv = self.level
    local cur_max, is_max = ExpConfig:getMaxExp(self.level)
    if not is_max then
        self.cur_exp = self.cur_exp + exp
    end
    while not is_max and self.cur_exp >= cur_max do
        self.level = self.level + 1
        self.cur_exp = self.cur_exp - cur_max
        self:addLvUpReward()
        cur_max, is_max = ExpConfig:getMaxExp(self.level)
        table.insert(self.unlock_level, self.level)
    end
    if oldLv ~= self.level then
        GamePacketSender:sendBedWarLevelUp(self.rakssid, oldLv, self.level)
    end
end

function GamePlayer:getUserSeasonInfo(retryTime)
    if tonumber(tostring(self.rakssid)) <= 100 then
        --AI return
        return
    end
    WebService:GetUserSeasonInfo(self.userId, false, function(data)
        if not data then
            if retryTime > 0 then
                self:getUserSeasonInfo(retryTime - 1)
            end
            return
        end
        self.honorId = data.segment
        self.honor = data.integral
    end)
end

function GamePlayer:checkParty()
    local manager = GameServer:getRoomManager()
    if manager:isUserAttrExisted(self.userId) then
        local attr = manager:getUserAttrInfo(self.userId)
        self.isPartyUser = attr.privateParty
    end
    if self.isPartyUser then
        GamePacketSender:sendBedWarKeyAdDeadSummary(self.rakssid, 0)
    end
end

function GamePlayer:buildShowName()
    local nameList = {}
    local disName = self.name
    if self.team ~= nil then
        if BaseMain:isChina() then
            disName = disName .. self.team:getDisplayName("zh_CN")
        else
            disName = disName .. self.team:getDisplayName("en_US")
        end
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:getDisplayName()
    if self.team ~= nil then
        return self.team:getDisplayName(self:getLanguage()) .. TextFormat.colorWrite .. self.name
    else
        return self.name
    end
end

function GamePlayer:getTeamColorName()
    if self.team ~= nil then
        return self.team:getTeamColor() .. self.name
    else
        return self.name
    end
end

function GamePlayer:initTeam()
    self.teamId = math.max(1, self.teamId)
    local team = GameMatch.Teams[self.teamId]
    team:addPlayer(self.userId)
    self.team = team
    local respawnPos = VectorUtil.newVector3i(self.team.initPos.x, self.team.initPos.y, self.team.initPos.z)
    self:setRespawnPos(respawnPos)
    self.info:setValue("teamId", self.teamId)
end

function GamePlayer:getTeamId()
    if self.team ~= nil then
        return self.team.id
    end
    return self.teamId
end

function GamePlayer:teleInitPos()
    self.realLife = true
    if self.team ~= nil and self.team.initPos ~= nil then
        self:teleportPos(self.team.initPos)
    end
end

function GamePlayer:teleWaitPos()
    if self:checkReady() then
        self:teleportPos(GameConfig.waitPos)
    else
        self:teleportPosWithYaw(GameConfig.waitPos, GameConfig.waitYaw)
    end
end

function GamePlayer:teleWatchPos()
    self:setAllowFlying(true)
    HostApi.sendSetFlying(self.rakssid)
    local vec = VectorUtil.newVector3(GameConfig.watchPoint[1], GameConfig.watchPoint[2], GameConfig.watchPoint[3])
    self:setPositionAndRotation(vec, tonumber(GameConfig.watchPoint[4]), tonumber(GameConfig.watchPoint[5]))

    if self.entityPlayerMP then
        self.entityPlayerMP:setPositionAndUpdate(tonumber(GameConfig.watchPoint[1]), tonumber(GameConfig.watchPoint[2]), tonumber(GameConfig.watchPoint[3]))
    end
end

function GamePlayer:onLogout()
    SettlementManager:subPlayer(self)

    if self.surviveTime == -1 then
        self.surviveTime = os.time() - GameMatch.startGameTime
    end

    if self.isLife then
        self.isLife = false
        if self.team ~= nil then
            self.team:subPlayer()
        end
    end

    if GameMatch.hasStartGame then
        BaseInfoManager:getPlayerInfo(self.userId):setValue("ExitGame", "1")
        if AIManager:isAI(self.rakssid) == false then
            ReportUtil.reportKillNum(self, self.initLevel, self.kills)
            ReportUtil.reportDeathNum(self, self.initLevel, self.deathNum)
            ReportUtil.reportPlayerKillAI(self, self.honor, self.killAIs)
            ReportUtil.reportLogout(self)
        end
    end
end

function GamePlayer:onKill(dier)
    self.kills = self.kills + 1
    self.appIntegral = self.appIntegral + 1

    if AIManager:isAI(dier.rakssid) == true then
        self.killAIs = self.killAIs + 1
    end

    if self.multiKill == 0 then
        self.multiKill = 1
    else
        if os.time() - self.lastKillTime <= 30 then
            self.multiKill = self.multiKill + 1
        else
            self.multiKill = 1
        end
    end
    SettlementManager:onPlayerDied(dier.userId, self.userId, self.multiKill)
    self.lastKillTime = os.time()
    RuneManager:onKill(self)
end

function GamePlayer:onDestroyBed(team)
    MessagesManage:sendSystemTipsToTeam(team.id, Messages:msgBedHasBreak())
    local players = PlayerManager:getPlayers()
    for _, _player in pairs(players) do
        MessagesManage:sendSystemTipsToPlayer(_player.rakssid, Messages:breakBedTip(self:getTeamColorName(), team:getDisplayName(_player:getLanguage())))
        if _player.teamId == self.teamId then
            GamePacketSender:sendPlaySound(_player.rakssid, SoundConfig.breakOtherBed)
        end
    end

    local destroy_bed = self.info:getValue("destroy_bed")
    self.taskController:packingProgressData(Define.TaskType.Fright.DestroyBed, nil, 1)
    self.info:setValue("destroy_bed", destroy_bed .. team.id)
end

function GamePlayer:onDie()
    if self.isLife then
        self.surviveTime = os.time() - GameMatch.startGameTime
        self.isLife = false
        self.realLife = false
        self.team:subPlayer()
        SettlementManager:onPlayerLose(self)
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
        HostApi.sendPlaySound(self.rakssid, 10002)
        UpdateTipsUtil:sendFeedback(self)
        if AIManager:isAI(self.rakssid) == true then
            AITeamManager:onAIDie(self)
        end
    end
end

function GamePlayer:overGame()
    BackHallManager:addPlayer(self)
end

function GamePlayer:addYaoShi(yaoshi)
    self.yaoshi = self.yaoshi + yaoshi
    GamePacketSender:sendBedWarPlayerKeyNum(self.rakssid, self.yaoshi)
end

function GamePlayer:addHorn(count)
    self.had_horn = self.had_horn + count
    CommonPacketSender:sendBedWarPlayerHornNum(self.rakssid, self.had_horn)
end

function GamePlayer:addTicket(count)
    self.ticket_num = self.ticket_num + count
    CommonPacketSender:sendTicketNum(self.rakssid, self.ticket_num)
end

function GamePlayer:subYaoShi(yaoshi, source, itemId)
    if self.yaoshi - yaoshi >= 0 then
        self.yaoshi = self.yaoshi - yaoshi
        GamePacketSender:sendBedWarPlayerKeyNum(self.rakssid, self.yaoshi)
        self.taskController:packingProgressData(Define.TaskType.GrowUp.CostCoin, Define.Money.KEY, yaoshi)
        ReportUtil.reportPlayerCostKey(self, yaoshi, source, itemId)
        return true
    end
    return false
end

function GamePlayer:subTicket(count)
    if self.ticket_num - count >= 0 then
        self.ticket_num = self.ticket_num - count
        CommonPacketSender:sendTicketNum(self.rakssid, self.ticket_num)
        return true
    end
    return false
end

function GamePlayer:onUseCustomProp(propId)
    local maxhp = self:getMaxHealth()
    local extraHp = AppPropConfig:getRewardHp(propId)
    self:changeMaxHealth(maxhp + extraHp)
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, tonumber(self.info:getValue("kill")), tonumber(self.info:getValue("rank")), isCount)
    ReportUtil.reportPlayerRank(self, tonumber(self.info:getValue("rank")))
    ReportUtil.reportNewPlayerHadOpenLicense(self)
end

function GamePlayer:fillInvetory(inventory)
    self:clearInv()
    if inventory == nil then
        return
    end
    local items = inventory.item or {}
    for _, item in pairs(items) do
        if item.id > 0 and item.id < 10000 then
            self:tryAddItem(item.id, item.num)
        end
    end
    local armors = inventory.armor or {}
    for _, armor in pairs(armors) do
        if armor.id > 0 and armor.id < 10000 then
            self:tryAddItem(armor.id, armor.num)
        end
    end
end

function GamePlayer:setInventory()
    local inv = self:getInventory()
    if not inv then
        return
    end
    self.inventory = InventoryUtil:getPlayerInventoryInfo(self)
    local hotbarFlag = {}
    for index, stack in pairs(self.inventory.item) do
        self.inventory.item[index].hotBarIndex = 1000
        for hotbarIndex = 0, 8 do
            local data = inv:getHotBarInventory(hotbarIndex)
            if data and hotbarFlag[hotbarIndex] == nil and data.id == stack.id
                    and data.num == stack.num and data.meta == stack.meta then
                hotbarFlag[hotbarIndex] = true
                self.inventory.item[index].hotBarIndex = hotbarIndex
                break
            end
        end
    end
    table.sort(self.inventory.item, function(a, b)
        if a.hotBarIndex ~= b.hotBarIndex then
            return a.hotBarIndex < b.hotBarIndex
        end
    end)
end

function GamePlayer:fillEnderInvetory(enderInv)
    if enderInv == nil then
        return
    end
    local slot = 0
    local items = enderInv.item or {}
    for _, item in pairs(items) do
        if item.id > 0 and item.id < 10000 then
            self:addItemToEnderChest(slot, item.id, item.num, item.meta, item.enchantList)
            slot = slot + 1
        end
    end
end

function GamePlayer:setEnderInventory()
    local inv = self:getEnderInventory()
    if not inv then
        return
    end
    self.enderInventory = InventoryUtil:getPlayerEnderInventoryInfo(self)
    for index, stack in pairs(self.enderInventory.item) do
        if inv then
            self.enderInventory.item[index].enchantList = ItemEnchantment:getItemEnchantmentList(inv, stack.stackIndex)
        end
    end
end

function GamePlayer:addMaxApple(apple)
    self.maxApple = self.maxApple + apple
    self.curApple = self.maxApple
    self:updateApple()
end

function GamePlayer:addApple(apple)
    self.curApple = self.curApple + apple
    self.curApple = math.min(self.curApple, self.maxApple)
    self:updateApple()
end

function GamePlayer:subApple(damage)
    if self.curApple == nil or self.curApple <= 0 then
        return damage
    end
    if self.curApple - damage > 0 then
        self.curApple = self.curApple - damage
        self:updateApple()
        return 0
    else
        local result = damage - self.curApple
        self.curApple = 0
        self.maxApple = 0
        self:updateApple()
        return result
    end
end

function GamePlayer:updateApple()
    if self.entityPlayerMP == nil then
        return
    end
    self.entityPlayerMP:changeApple(self.curApple, self.maxApple)
end

function GamePlayer:resetApple()

    for hash, timeId in pairs(LogicListener.goldAppleAddHpTimeId) do
        local findHash = string.find(hash, tostring(self.rakssid))
        if findHash ~= nil and findHash > 0 then
            if timeId ~= nil then
                LuaTimer:cancel(timeId)
                timeId = nil
            end
            table.remove(LogicListener.goldAppleAddHpTimeId, hash)
            break
        end
    end

    for hash, timeId in pairs(LogicListener.goldAppleTimeId) do
        local findHash = string.find(hash, tostring(self.rakssid))
        if findHash ~= nil and findHash > 0 then
            if timeId ~= nil then
                LuaTimer:cancel(timeId)
                timeId = nil
            end
            table.remove(LogicListener.goldAppleTimeId, hash)
            break
        end
    end

    for hash, timeId in pairs(LogicListener.goldAppleFreshTimeId) do
        local findHash = string.find(hash, tostring(self.rakssid))
        if findHash ~= nil and findHash > 0 then
            if timeId ~= nil then
                LuaTimer:cancel(timeId)
                timeId = nil
            end
            table.remove(LogicListener.goldAppleFreshTimeId, hash)
            break
        end
    end
end

function GamePlayer:hasSameArmor(itemId)
    local inv = self:getInventory()
    local index = inv:getArmorSlotById(itemId)
    if index >= 100 and index < 104 then
        local info = inv:getItemStackInfo(index)
        return info.id ~= 0
    end
    return false
end

function GamePlayer:hasSameTool(itemId)
    local item = Item.getItemById(itemId)
    if not item then
        return false
    end
    local inv = self:getInventory()
    local class = item:getClassName()
    if class == "ItemPickaxe" or class == "ItemAxe" or class == "ItemShears" then
        for index = 1, 36 do
            local info = inv:getItemStackInfo(index - 1)
            if info.id ~= 0 then
                local _item = Item.getItemById(info.id)
                if _item then
                    local _class = _item:getClassName()
                    if _class == class then
                        return true
                    end
                end
            end
        end
    end
    return false
end

function GamePlayer:initItem()
    if GameMatch:isGameStart() then
        self:removeWoodSword()
    else
        self:tryAddItem(ItemID.SWORD_WOOD, 1)
    end
    for _, itemId in ipairs(self.initItems) do
        if not self:hasSameArmor(itemId) and not self:hasSameTool(itemId) then
            self:tryAddItem(itemId, 1)
        end
    end
end

function GamePlayer:addItemOrEquipItem()
end

function GamePlayer:tryItemId2DressId(itemId)
    local oldItemId = ItemMapping:getOldItemId(itemId) ---获取原始ItemId
    local dressId = self.equip_dress[tostring(oldItemId)] or itemId ---原始ItemId映射成装扮Id
    itemId = ItemMapping:getItemIdByDressId(itemId, dressId) ---装扮ID转换成真正的ItemId
    return itemId
end

function GamePlayer:addEnchantItem(itemId, count, damage, enchantList)
    if enchantList and #enchantList > 0 then
        self:addEnchantmentsItem(itemId, count, damage, enchantList)
    else
        self:addItem(itemId, count, damage)
    end
end

function GamePlayer:removeWoodSword()
    self:removeItem(ItemID.SWORD_WOOD, 1)
    local itemId = self:tryItemId2DressId(ItemID.SWORD_WOOD)
    self:removeItem(itemId, 1)
end

function GamePlayer:tryAddItem(itemId, count, damage, enchantList)
    damage = damage or 0
    local item = Item.getItemById(itemId)
    if not item then
        ---尝试添加自定义方块
        itemId = self:tryItemId2DressId(itemId)
        self:addEnchantItem(itemId, count, damage, enchantList)
        return
    end
    if itemId == BlockID.CLOTH then
        itemId = self:getClothBlockId()
    end
    local class = item:getClassName()
    if class == "ItemBlock" and itemId > 2000 then
        ---自定义方块需要-1000
        itemId = self:tryItemId2DressId(itemId - 1000)
    else
        itemId = self:tryItemId2DressId(itemId)
    end
    if class == "ItemArmor" then
        self:tryEquipArmor(itemId, damage, enchantList)
    elseif class == "ItemPickaxe" or class == "ItemAxe" then
        enchantList = ToolItemGain:onPlayerGetTool(self, enchantList)
        self:tryAddToolItem(itemId, count, damage, enchantList, class)
    elseif class == "ItemSword" then
        self:removeWoodSword()
        ---删除木剑
        enchantList = FireSwordPrivilege:onPlayerGetSword(self, enchantList)
        enchantList = SwordItemGain:onPlayerGetSword(self, enchantList)
        self.info:setValue(class, itemId)
        self:addEnchantItem(itemId, count, damage, enchantList)
    elseif class == "ItemBow" then
        self.info:setValue(class, itemId)
        self:addEnchantItem(itemId, count, damage, enchantList)
    elseif class == "ItemShears" then
        ---检测是否有剪刀
        if self:getInventory():getItemNum(itemId) <= 0 then
            enchantList = ToolItemGain:onPlayerGetTool(self, enchantList)
            self:addEnchantItem(itemId, count, damage, enchantList)
        end
    else
        self:addEnchantItem(itemId, count, damage, enchantList)
    end
end

function GamePlayer:tryAddToolItem(itemId, count, damage, enchantList, class)
    ---工具类型的道具需要替换原来旧的道具
    local oldItemId = tonumber(self.info:getValue(class))
    if oldItemId ~= 0 then
        self:removeItem(oldItemId, count)
    end
    self.info:setValue(class, itemId)
    self:addEnchantItem(itemId, count, damage, enchantList)
end

function GamePlayer:tryEquipArmor(itemId, damage, enchantList)
    local inv = self:getInventory()
    local index = inv:getArmorSlotById(itemId)
    enchantList = enchantList or {}
    if index > 100 then
        ---只针对衣服，裤子，鞋子
        self.info:setValue("ItemArmor" .. index, itemId)
    end
    enchantList = ArmorItemGain:onPlayerGetArmor(self, enchantList)
    self.entityPlayerMP:replaceArmorItemWithEnchant(itemId, 1, damage, enchantList)
end

function GamePlayer:addInitItems(newItemId)
    for _, itemId in pairs(self.initItems) do
        if itemId == newItemId then
            return
        end
    end
    table.insert(self.initItems, newItemId)
end

function GamePlayer:clearArmorInfo()
    self.info:setValue("ItemSword", 0)
    self.info:setValue("ItemBow", 0)
    self.info:setValue("ItemArmor101", 0)
    self.info:setValue("ItemArmor102", 0)
    self.info:setValue("ItemArmor103", 0)
end

function GamePlayer:getClothBlockId()
    if self.team then
        return self.team.clothBlockId
    end
    return BlockID.CLOTH
end

function GamePlayer:sendLotteryConfig()
    GameLottery:RefreshLotteryPool(self.userId)
end

function GamePlayer:onSendCommand(commandConfig)
    local effectConfig = MessageEffectConfig:getConfigById(commandConfig.effectId)
    if not effectConfig then
        return
    end
    local getFountPosition = function(footPos, yaw)
        local leftYaw = yaw
        local x = -math.sin(leftYaw * MathUtil.DEG2RAD)
        local z = math.cos(leftYaw * MathUtil.DEG2RAD)
        return VectorUtil.newVector3(footPos.x + x * 0.3, footPos.y, footPos.z + z * 0.3)
    end

    -----设置特效
    local leftHandPos = getFountPosition(self:getPosition(), self:getYaw())
    GamePacketSender:sendSignalBomb(self:getEntityId(), leftHandPos, effectConfig.id, self.teamId)
end

function GamePlayer:subHealth(damage)
    local value = self:subApple(damage)
    if value == 0 then
        return
    end
    return self.entityPlayerMP:heal(-damage)
end

function GamePlayer:addRune(rune_id, num)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    if rune_id == 0 then
        return
    end
    local has = false
    for _, rune in pairs(self.rune) do
        if rune.runeId == rune_id then
            rune.num = rune.num + num
            has = true
            break
        end
    end
    if not has then
        local item = {}
        item.runeId = rune_id
        item.num = num
        table.insert(self.rune, item)
    end
end

function GamePlayer:changeSpeed(speed)
    self.addSpeed = self.addSpeed + speed
    self.entityPlayerMP:setSpeedAdditionLevel(self.addSpeed)
end

return GamePlayer

--endregion
