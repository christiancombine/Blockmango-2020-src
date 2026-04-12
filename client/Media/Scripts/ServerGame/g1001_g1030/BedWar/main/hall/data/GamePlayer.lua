---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self.level = 1
    self.cur_exp = 0
    self.yaoshi = 0
    self.chest_integral = 0
    self.fragment_jindu = {}
    self.chests = {}
    self.rewards = {}
    self.running_guides = {}
    self.finish_ids = {}
    self.init_exp = false

    self.standByTime = 0
    self.moveHash = ""
    self.addHealth = 0
    self.addDefense = 0
    self.maxHealth = 0
    self.spDays = 0
    self.spGetAway = false
    self.init_rune_trough = false
    self.unlock_level = {}
    self.rune = {}
    self.equip_rune = {}
    self.dress = {}
    self.equip_dress = {}
    self.custom_goods = {}
    self.block_goods = {}
    self.skin_order = {}
    self.wall_order = {}
    self.equip_action = {}
    self.command_list = {}

    self.expDoubleTime = 0
    self.privilegeCard = {}
    self.checkRuneUnlockList = {}
    self.init_reward_finished = false
    self.activity_data = {
        first_recharge = false,
        tiro_sign_in_id = 0,
        tiro_sign_in_key = 0,
        super_player_daily = 0,
    }
    self.daily_sign_in_data = {
        get_first = false,
        week_key = 0,
        record = {}
    }
    self.video_ad_data = {
        box_lottery_key = 0
    }

    self.play_days = 0
    self.play_day_key = 0
    self.props_rewards = {}

    self.free_cube_data = {
        day_key = 0,
        times = 0
    }
    self.challenges = {}
    self.needNewChallengeTip = false
    self.challenge_ad = false

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

    self.free_gift_refresh_time = DateUtil.getYearDay()
    self.ticket_num = 0
    self.free_gift = {}

    self.get_free_ticket_times = -1
    self.refresh_free_ticket = 0

    self.changeTaskId = -1

    self:teleInitPos()
    GameInfoManager.Instance():newPlayerInfo(self.userId)
    self.info = GameInfoManager.Instance():getPlayerInfo(self.userId)

    self.taskController = GameTaskController.new(self)
end

function GamePlayer:initDBDataFinished()
    if self.init_rune_trough == false then
        self:initRuneTrough()
        self.init_rune_trough = true
    end
    TreasureChallenge:onPlayerReady(self)
    self.taskController:init()
    TreasureChallenge:refreshChallengeTask(self)
    GameLicense:updatePlayerData(self)
    WatchADHelper:synWatchAdData(self)
    if tonumber(self.play_day_key) < tonumber(DateUtil.getYearDay()) then
        self.play_days = self.play_days + 1
        self.play_day_key = DateUtil.getYearDay()
        self.taskController:packingProgressData(Define.TaskType.GrowUp.DailyLogin, nil, 1)
        ReportUtil.reportPlayerLoginLicenseLevel(self)
        ReportUtil.reportDailyPlayerLevel(self)
        GamePacketSender:sendIndieDailyLogin(self.rakssid)
        TreasureChallenge:resetTreasureAd(self)
    end
    if #self.chests == 0 then
        GameChestLottery:initChestLotteryData(self)
    end
    self:setShowName(self:buildShowName())
    self:teleInitPos()
    self:syncHallInfo()
    self:addItemFromDB()
    GameSeason:getUserSeasonInfo(self.userId, false, 3)
    GameChestLottery:syncChestLotteryData(self)
    self:checkTroughLock()
    self:synFreeTicketInfo()
    if self.unlock_level and #self.unlock_level > 0 then
        if self.level ~= 2 then
            self:sendOpenRune(true)
        end
        self.unlock_level = {}
    end
    if self.hasReport == false then
        GameAnalytics:design(self.userId, 1, { "Player_Lv", self.level })
        self.hasReport = true
    end
    GameActivity:initPlayerData(self)
    DailySignInHelper:initPlayer(self)
    VideoAdHelper:syncBoxLotteryAd(self)
    InitRewardConfig:initPlayer(self)
    GameGuide:sendDataToClient(self)
    RewardUtil:tryConsumeRewards(self)
    local todayKey = DateUtil.getYearDay()
    if self.free_cube_data.day_key ~= todayKey then
        self.free_cube_data.day_key = todayKey
        self.free_cube_data.times = 0
    end
    if self.free_cube_data.times >= 2 then
        GamePacketSender:sendBedWarFreeCubeLimit(self.rakssid)
    end
    if #self.dress == 0 then
        GameDressStore:initDressStoreData(self)
    end
    GameDressStore:syncDressStoreData(self)
    if not self.init_exp then
        self:addExp(GameConfig.initExp)
        self.init_exp = true
    end
    GameLicense:getHadDress(self)
    CustomShop:onSyncPlayerAllGoods(self)
    BlockShop:onSyncPlayerAllGoods(self)
    Privilege.syncPrivilegeTime(self)
    CommonPacketSender:sendTicketNum(self.rakssid, self.ticket_num)
end

function GamePlayer:addItemFromDB()
    for k, v in pairs(self.equip_dress) do
        self:addItemOrEquipItem(v)
    end
end

function GamePlayer:addItemOrEquipItem(itemId)
    local cfg = DressConfig:getDressByItemId(itemId)
    if cfg then
        if cfg.TabType == DressConfig.DressType.HELMET then
            self.entityPlayerMP:replaceArmorItemWithEnchant(itemId, 1, 0, {})
            self:changeClothes("custom_hat", 0)
        else
            self:addItem(itemId, 1)
        end
    end
end

function GamePlayer:buildShowName()
    local nameList = {}
    local namePrefix = TextFormat.colorGreen .. "LV" .. self.level .. "  "
    local disName = self.name
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName, namePrefix)
    return table.concat(nameList, "\n")
end

function GamePlayer:getHallInfo()
    local lv = self.level or 1
    local cur_exp = self.cur_exp or 0
    local max_exp, is_max = ExpConfig:getMaxExp(self.level)
    local yaoshi = self.yaoshi
    return lv, cur_exp, max_exp, yaoshi, is_max
end

function GamePlayer:syncHallInfo()
    local lv, cur_exp, max_exp, yaoshi, is_max = self:getHallInfo()
    GamePacketSender:sendBedWarHallInfo(self.rakssid, lv, cur_exp, max_exp, is_max, yaoshi)
end

function GamePlayer:addExp(exp)
    local cur_max, is_max = ExpConfig:getMaxExp(self.level)
    if not is_max then
        self.cur_exp = self.cur_exp + exp
    end
    while not is_max and self.cur_exp >= cur_max do
        self.level = self.level + 1
        self.cur_exp = self.cur_exp - cur_max
        cur_max, is_max = ExpConfig:getMaxExp(self.level)
        GameGuide:sendDataToClient(self)
    end

    if self.hasReport == true then
        GameAnalytics:design(self.userId, 1, { "Player_Lv", self.level })
        GameAnalytics:design(self.userId, -1, { "Player_Lv", self.level - 1 })
    end

    self:syncHallInfo()
    self:checkTroughLock()
    GameDressStore:syncDressStoreData(self)
    self:setShowName(self:buildShowName())
end

function GamePlayer:teleInitPos()
    self:teleportPosWithYaw(GameConfig.initPos, GameConfig.yaw)
    --HostApi.changePlayerPerspece(self.rakssid, 1)
end

function GamePlayer:onUseCustomProp(propId, itemId)
    local reward = RewardConfig:getRewardById(itemId)
    if not reward then
        if not DbUtil:IsGetAllDataFinished(self) then
            table.insert(self.props_rewards, reward)
            return
        end
        local rewardId = GameReward:onPlayerGetReward(self, reward)
        if not rewardId then
            return
        end
        GamePacketSender:sendBedWarRewardResult(self.rakssid, rewardId)
    end
end

function GamePlayer:changeRune(cur_trough_id, cur_trough_rune_id, target_rune_id)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    for _, trough in pairs(self.equip_rune) do
        if cur_trough_id == trough.t_id then
            cur_trough_rune_id = trough.id
        end
    end

    local trough = RuneTroughConfig:getTrough(cur_trough_id)
    local rune = RuneConfig:getTrough(target_rune_id)

    if trough and rune then
        if self:subRune(target_rune_id, 1) then
            if cur_trough_rune_id == 0 then
                self:replaceRuneInTrough(cur_trough_id, target_rune_id)
            else
                local old_rune_id = self:replaceRuneInTrough(cur_trough_id, target_rune_id)
                self:addRune(old_rune_id, 1)
            end
            self:sendOpenRune()
            GamePacketSender:sendBedWarRuneNextTrough(self.rakssid)
        end
    end
end

function GamePlayer:replaceRuneInTrough(rune_trough_id, rune_id)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    local old_rune_id = 0
    local has = false
    for _, trough in pairs(self.equip_rune) do
        if trough.t_id == rune_trough_id then
            old_rune_id = trough.id
            trough.id = rune_id
            has = true
            break
        end
    end
    if not has then
        local item = {}
        item.t_id = rune_trough_id
        item.id = rune_id
        item.lock = true
        table.insert(self.equip_rune, item)
    end
    return old_rune_id
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

function GamePlayer:subRune(rune_id, num)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    if rune_id == 0 then
        return false
    end
    for key, rune in pairs(self.rune) do
        if rune.runeId == rune_id then
            rune.num = rune.num - num
            if rune.num <= 0 then
                table.remove(self.rune, key)
                rune.num = 0
            end
            return true
        end
    end
    return false
end

function GamePlayer:reclaim(rune_id)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    local runeConfig = RuneConfig:getTrough(rune_id)
    if runeConfig.canSell == 0 then
        return
    end

    if self:subRune(rune_id, 1) then
        local yaoshi = runeConfig.SellKey
        self:addYaoShi(yaoshi)
    end
    self:sendOpenRune()
end

function GamePlayer:reclaimAll(rune_type)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    local remove_table = {}
    for _, rune in pairs(self.rune) do
        local runeConfig = RuneConfig:getTrough(rune.runeId)
        if runeConfig and ((runeConfig.Type == rune_type) or (rune_type == 0)) then
            if runeConfig.Level == 1 then
                local item = {}
                item.runeId = rune.runeId
                item.num = rune.num
                table.insert(remove_table, item)
            end
        end
    end

    for _, rune in pairs(remove_table) do
        if self:subRune(rune.runeId, rune.num) then
            local runeConfig = RuneConfig:getTrough(rune.runeId)
            if runeConfig then
                local yaoshi = rune.num * runeConfig.SellKey
                self:addYaoShi(yaoshi)
            end
        end
    end

    self:sendOpenRune()
end

function GamePlayer:oneKeyUnload(cur_rune_trough_type)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    for key, trough in pairs(self.equip_rune) do
        local troughItem = RuneTroughConfig:getTrough(trough.t_id)
        if troughItem then
            if troughItem.Type == cur_rune_trough_type then
                self:addRune(trough.id, 1)
                trough.id = 0
            end
        end
    end
    self:sendOpenRune(false, RuneTroughConfig.AOT_RUNE_LIST)
end

function GamePlayer:oneKeyAssemble(cur_rune_trough_type)
    if not DbUtil:IsGetAllDataFinished(self) then
        return
    end

    local empty_trough_list = {}
    for key, trough in pairs(RuneTroughConfig.Trough) do
        if cur_rune_trough_type == trough.Type then
            for _, troughItem in pairs(self.equip_rune) do
                if trough.Id == troughItem.t_id and troughItem.id == 0 and troughItem.lock == false then
                    table.insert(empty_trough_list, trough.Id)
                end
            end
        end
    end

    for _, troughId in pairs(empty_trough_list) do
        local rune_list = self:getSortRuneCopy()
        for _, rune in pairs(rune_list) do
            local runeItem = RuneConfig:getTrough(rune.rune_id)
            if runeItem and runeItem.Type == cur_rune_trough_type then
                self:replaceRuneInTrough(troughId, rune.rune_id)
                self:subRune(rune.rune_id, 1)
                break
            end
        end
    end
    self:sendOpenRune(false, RuneTroughConfig.AOT_PRO)
end

function GamePlayer:getSortRuneCopy()
    local rune_list = {}
    for _, rune in pairs(self.rune) do
        local item = {}
        item.rune_id = rune.runeId or 0
        item.rune_num = rune.num
        table.insert(rune_list, item)
    end

    table.sort(rune_list, function(a, b)
        local rune_a = RuneConfig:getTrough(a.rune_id)
        local rune_b = RuneConfig:getTrough(b.rune_id)

        if rune_a and rune_b then
            if rune_a.Level ~= rune_b.Level then
                return rune_a.Level > rune_b.Level
            end
            if rune_a.Type ~= rune_b.Type then
                return rune_a.Type > rune_b.Type
            end
            if rune_a.Weight ~= rune_b.Weight then
                return rune_a.Weight > rune_b.Weight
            end
            return a.rune_id < b.rune_id
        else
            return a.rune_id < b.rune_id
        end
    end)
    return rune_list
end

function GamePlayer:initRuneTrough()
    local all_trough = RuneTroughConfig:getAllTrough()
    if all_trough and #self.equip_rune == 0 then
        for _, trough in pairs(all_trough) do
            local item = {}
            item.t_id = trough.Id
            item.id = 0
            item.lock = true
            table.insert(self.equip_rune, item)
        end
    end
end

function GamePlayer:checkTroughLock()
    self.checkRuneUnlockList = {}
    if self.equip_rune and #self.equip_rune == 0 then
        self:initRuneTrough()
    end

    for _, trough in pairs(self.equip_rune) do
        if trough.lock == true then
            trough.lock = RuneTroughConfig:getTroughLock(trough.t_id, self.level)
            if trough.lock == false then
                table.insert(self.checkRuneUnlockList, trough.t_id)
            end
        end
    end
end

function GamePlayer:runeUnlock(type)
    for _, trough in pairs(self.equip_rune) do
        local troughItem = RuneTroughConfig:getTrough(trough.t_id)
        if troughItem.Type == type and trough.lock == true then
            trough.lock = false
            self:sendOpenRune()
            break
        end
    end
end

function GamePlayer:addYaoShi(yaoshi)
    self.yaoshi = self.yaoshi + yaoshi
    self:syncHallInfo()
end

function GamePlayer:addTicket(count)
    self.ticket_num = self.ticket_num + count
    CommonPacketSender:sendTicketNum(self.rakssid, self.ticket_num)
end

function GamePlayer:addHorn(count)
    self.had_horn = self.had_horn + count
    CommonPacketSender:sendBedWarPlayerHornNum(self.rakssid, self.had_horn)
end

function GamePlayer:subYaoShi(yaoshi, source, itemId)
    if self.yaoshi - yaoshi >= 0 then
        self.yaoshi = self.yaoshi - yaoshi
        self.taskController:packingProgressData(Define.TaskType.GrowUp.CostCoin, Define.Money.KEY, yaoshi)
        ReportUtil.reportPlayerCostKey(self, yaoshi, source, itemId)
        self:syncHallInfo()
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

function GamePlayer:checkNeedShowEffect(troughId)
    if self.checkRuneUnlockList then
        for _, t_id in pairs(self.checkRuneUnlockList) do
            if troughId == t_id then
                return true
            end
        end
    end
    return false
end

function GamePlayer:sendOpenRune(unlock_effect, assemble_type)
    local data = {}

    data.unlock_effect = unlock_effect or false
    data.assemble_type = assemble_type or RuneTroughConfig.AOT_DEFAULT
    data.unlock_trough_list = {}
    data.unlock_first_type = 1

    if self.checkRuneUnlockList and #self.checkRuneUnlockList == 0 then
        data.unlock_effect = false
    end

    if data.unlock_effect and self.unlock_level and #self.unlock_level > 0 then
        for _, level in pairs(self.unlock_level) do
            local item = {}
            item.level = level
            table.insert(data.unlock_trough_list, item)
        end
        -- data.unlock_first_type = RuneTroughConfig:getFirstUnlockTroughType(self.unlock_level[1])
        data.unlock_first_type = RuneTroughConfig:getTroughTypeById(self.checkRuneUnlockList[1])
    end
    data.player_level = self.level
    data.unlock_price = GameConfig.unlockRuneTroughPrice
    data.rune_trough_list = {}
    for _, trough in pairs(self.equip_rune) do
        local item = {}
        item.rune_trough_id = trough.t_id
        item.rune_id = trough.id
        item.rune_lock = trough.lock
        item.need_show_effect = self:checkNeedShowEffect(trough.t_id)
        table.insert(data.rune_trough_list, item)
    end

    data.rune_list = self:getSortRuneCopy()
    GamePacketSender:sendBedWarRuneData(self.rakssid, json.encode(data))
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:onTick()
    self.standByTime = self.standByTime + 1

    if self.standByTime == GameConfig.standByTime - 10 then
        -- MsgSender.sendBottomTipsToTarget(self.rakssid, 3, Messages:msgNotOperateTip())
    end
    if self.standByTime >= GameConfig.standByTime then
        self:doPlayerQuit()
    end
end

function GamePlayer:doPlayerQuit()
    BaseListener.onPlayerLogout(self.userId)
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:synFreeTicketInfo()
    if tonumber(self.refresh_free_ticket) < tonumber(DateUtil.getYearDay())  then
        self.get_free_ticket_times = -1
        self.refresh_free_ticket = DateUtil.getYearDay()
    end
    GamePacketSender:sendCubeStoreGetTicketTimes(self.rakssid, self.get_free_ticket_times)
end

return GamePlayer

--endregion


