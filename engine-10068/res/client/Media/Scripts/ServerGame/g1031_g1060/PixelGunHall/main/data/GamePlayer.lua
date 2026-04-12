---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "util.RewardUtil"
require "data.GameSeason"
require "data.TaskController"
require "data.GiftBagController"
require "data.GameGuide"
require "data.GameSignIn"
require "data.GameTicket"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()
    self:initStaticAttr(0)
    self.level = 1
    self.money = 0
    self.cur_exp = -1
    self.yaoshi = 0
    self.chest_integral = 0
    self.unlock_map = {}
    self.fragment_jindu = {}
    self.chests = {}
    self.equip_guns = {}
    self.equip_props = {}
    self.equip_blocks = {}
    self.equip_modules = {}
    self.guns = {}
    self.props = {}
    self.blocks = {}
    self.modules = {}
    self.armor_level = 1
    self.armor_upTime = 0
    self.armor_value = 0
    self.rewards = {}
    self.cur_is_random = 0
    self.cur_map_id = ""
    self.mode_first = {}
    self.mode_first_day_id = 0
    self.init_exp = false
    self.vip_time = 0
    self.active_info = nil
    self.unlock_level = 0
    self.daily_tasks = {}
    self.mainline_tasks = {}
    self.task_progresses = {}
    self.running_guides = {}
    self.finish_ids = {}
    self.advert_record = nil
    self.signIn_id = 1
    self.signIn_dayKey = 0
    self.gift_bags = {}
    self.gift_bag_refresh_time = 0
    self.bought_gift_bag = {}
    self.gift_bag_day_key = 0

    self.gun_ticket = {}

    self.standByTime = 0
    self.moveHash = ""
    self.addHealth = 0
    self.addDefense = 0
    self.maxHealth = 0
    self.allProperties = {}
    self.taskController = TaskController.new(self)
    self.giftBagController = GiftBagController.new(self)
    self.ticketManager = GameTicket.new(self)

    self:teleInitPos()
end

function GamePlayer:initDataFromDB(data, subKey)
    if subKey == DbUtil.GAME_DATA then
        self:initGameDataFromDB(data)
    end
    if subKey == DbUtil.MODE_DATA then
        self:initModeDataFromDB(data)
    end
    if subKey == DbUtil.ARMORY_DATA then
        self:initArmoryDataFromDB(data)
    end
    if subKey == DbUtil.PROTECT_ARMOR then
        self:initProtectArmorDataFromDB(data)
    end
    if subKey == DbUtil.REWARD_DATA then
        self:initRewardDataFromDB(data)
    end
    if subKey == DbUtil.TASK_DATA then
        self:initTaskDataFromDB(data)
    end
    if subKey == DbUtil.TASK_PROGRESS_DATA then
        self:initTaskProgressDataFromDB(data)
    end
    if subKey == DbUtil.GUIDE_DATA then
        self:initGuideDataFromDB(data)
    end
    if subKey == DbUtil.ADVERT_DATA then
        self:initAdvertDataFromDB(data)
    end
    if subKey == DbUtil.SIGN_IN_DATA then
        self:initSignInDataFromDB(data)
    end
    if subKey == DbUtil.GIFT_BAG_DATA then
        self:initGiftBagDataFromDB(data)
    end
end

function GamePlayer:initGameDataFromDB(data)
    if not data.__first then
        self.level = ExpConfig:getPlayerInitLv(data.level or 1)
        self.money = data.money or 0
        if self.cur_exp ~= 0 then
            self.cur_exp = data.cur_exp or 0
        end
        self.armor_value = ArmorConfig:getCurArmorValueByLevel(self.armor_level) or data.armor_value
        self.yaoshi = data.yaoshi or 0
        self.chest_integral = data.chest_integral or 0
        self.chests = data.chests or {}
        self.equip_guns = data.equip_guns or {}
        self.equip_props = data.equip_props or {}
        self.equip_blocks = data.equip_blocks or {}
        --self.equip_modules = result.equip_modules or {}
        if not GameModule:isNullEquipModule(self) then
            self.equip_modules = data.equip_modules or {}
        end
        self.cur_is_random = 0
        self.cur_map_id = data.cur_map_id or ""
        self.mode_first = data.mode_first or {}
        self.mode_first_day_id = data.mode_first_day_id or 0
        self.vip_time = data.vip_time or 0
        self.gun_ticket = data.gun_ticket or {}
    end
    local mode_first_day_id = DateUtil.getYearDay()
    if self.mode_first_day_id ~= mode_first_day_id then
        self.mode_first = {}
        self.mode_first_day_id = mode_first_day_id
    end

    if self.cur_exp < 0 then
        self.cur_exp = 0
    end

    if #self.chests == 0 then
        GameChestLottery:initChestLotteryData(self)
    end
    self:setShowName(self:buildShowName())
    self:teleInitPos()
    self:setCurrency(self.money)
    self:initEquipItems()
    self:syncHallInfo()
    self:initMaxHealthByLevel(self.level)
    self:syncMapModelInfo()
    GameSeason:getUserSeasonInfo(self.userId, false, 3)
    GameGunStore:syncGunStoreData(self)
    GameGunStore:updateEquipAttr(self)
    GameChestLottery:syncChestLotteryData(self)
    GameArmor:initArmorData(self)
    GameModule:syncEquipModuleData(self)
    self.ticketManager:init()
end

function GamePlayer:initModeDataFromDB(data)
    if not data.__first then
        self.init_exp = data.init_exp or false
        self.unlock_map = data.unlock_map or {}
        self.fragment_jindu = data.fragment_jindu or {}
        self.fragment_jindu_day_id = data.fragment_jindu_day_id or 0
        local fragment_jindu_day_id = DateUtil.getYearDay()
        if self.fragment_jindu_day_id ~= fragment_jindu_day_id then
            self.fragment_jindu = {}
            self.fragment_jindu_day_id = fragment_jindu_day_id
        end

        if not self.init_exp then
            self:addExp(GameConfig.initExp)
            self.init_exp = true
        end
    else
        if not self.init_exp then
            self:addExp(GameConfig.initExp)
            self.init_exp = true
        end
    end
    self:syncMapModelInfo()
end

function GamePlayer:initArmoryDataFromDB(data)
    if not data.__first then
        self.guns = data.guns or {}
        self.props = data.props or {}
        self.blocks = data.blocks or {}
        self.modules = data.modules or {}
    end
    self:initEquipItems()
    if #self.modules == 0 then
        GameModule:initModulesData(self)
    end
    GameGunStore:syncGunStoreData(self)
    GameModule:syncAllModuleData(self)
end

function GamePlayer:initProtectArmorDataFromDB(data)
    if not data.__first then
        self.armor_level = data.armor_level or 1
        self.armor_upTime = data.armor_upTime or 0
    end
    self:updateDefense()
    GameArmor:initArmorData(self)
end

function GamePlayer:initRewardDataFromDB(data)
    if not data.__first then
        self.rewards = data.rewards or {}
    end
end

function GamePlayer:initTaskDataFromDB(data)
    if not data.__first then
        self.daily_tasks = data.daily_tasks or {}
        self.mainline_tasks = data.mainline_tasks or {}
        self.unlock_level = data.unlock_level or 0
        self.active_info = data.active_info
    end
    self:syncHallInfo()
end

function GamePlayer:initTaskProgressDataFromDB(data)
    if not data.__first then
        self.task_progresses = data.task_progresses or {}
    end
end

function GamePlayer:initGuideDataFromDB(data)
    if not data.__first then
        GameGuide:recoverGuide(self, data)
    end
end

function GamePlayer:initAdvertDataFromDB(data)
    if not data.__first then
        self.advert_record = data.advert_record
    end
    AdvertConfig:initPlayerAds(self)
end

function GamePlayer:initSignInDataFromDB(data)
    if not data.__first then
        self.signIn_id = data.signIn_id or 1
        self.signIn_dayKey = data.signIn_dayKey or 0
        GameSignIn:initSignInData(self)
    end
    GameSignIn:sendToClient(self)
end

function GamePlayer:initGiftBagDataFromDB(data)
    if not data.__first then
        self.gift_bag_refresh_time = data.gift_bag_refresh_time or os.time() + GameMatch.refreshTime
        self.bought_gift_bag = data.bought_gift_bag or {}
        self.gift_bags = data.gift_bags or {}
        self.gift_bag_day_key = data.gift_bag_day_key or 0
        if self.gift_bag_refresh_time - os.time() <= 0 then
            self.gift_bags = {}
        end
    end
    self.giftBagController:init()
end

function GamePlayer:updateDefense()
    self.armor_value = ArmorConfig:getCurArmorValueByLevel(self.armor_level)
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeDefense(self.armor_value + self.addDefense, self.armor_value + self.addDefense)
    end
end

function GamePlayer:updateHealth()
    self:changeMaxHealth(self.maxHealth + self.addHealth)
end

function GamePlayer:initEquipItems()
    if not DbUtil:CanSavePlayerData(self, DbUtil.GAME_DATA) or
            not DbUtil:CanSavePlayerData(self, DbUtil.ARMORY_DATA) then
        return
    end
    if #self.guns == 0 and #self.props == 0 then
        GameGunStore:initGunStoreData(self)
    end
    GameGunStore:initGunStoreEquipData(self)
    for type, equip_gun in pairs(self.equip_guns) do
        self:onGetProperties(equip_gun.ItemId, equip_gun.PropertyIds)
        local gun = GunConfig:getGun(equip_gun.Id)
        if gun then
            self:replaceItem(gun.ItemId, 1, 0, tonumber(gun.TabType) - 1, 1000)
            self:addItem(gun.BulletId, gun.SpareBullet, 0, true)
        end
    end
    local props_count = 0
    for _, equip_prop in pairs(self.equip_props) do
        self:onGetProperties(equip_prop.ItemId, equip_prop.PropertyIds)
        local prop_tab = PropConfig:getPropsTab(equip_prop.ItemId)
        if prop_tab then
            self:replaceItem(equip_prop.ItemId, 1, 0, prop_tab + props_count)
        end
        props_count = props_count + 1
    end
    for type, equip_block in pairs(self.equip_blocks) do
        self:onGetProperties(equip_block.ItemId, equip_block.PropertyIds)
        self:replaceItem(equip_block.ItemId, equip_block.Count, 0, tonumber(type) + 1)
    end
end

function GamePlayer:initMaxHealthByLevel(level)
    local maxHealth = ExpConfig:getMaxHp(level)
    self.maxHealth = maxHealth
    self:changeMaxHealth(self.maxHealth + self.addHealth)
end

function GamePlayer:onGetProperties(itemId, propertyIds)
    local properties = PropertyConfig:getPropertyByIds(propertyIds)
    for _, property in pairs(properties) do
        property:onPlayerGet(self)
    end
    self.allProperties[tostring(itemId)] = properties
end

function GamePlayer:onEquipItem(itemId, propertyIds)
    local properties = self.allProperties[tostring(itemId)]
    if not properties then
        properties = PropertyConfig:getPropertyByIds(propertyIds)
        self.allProperties[tostring(itemId)] = properties
    end
    if #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerEquip(self)
        end
    end
end

function GamePlayer:onUnloadItem(itemId, propertyIds)
    local properties = self.allProperties[tostring(itemId)]
    if not properties then
        properties = PropertyConfig:getPropertyByIds(propertyIds)
        self.allProperties[tostring(itemId)] = properties
    end
    if #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerUnload(self)
        end
    end
end

function GamePlayer:isGameVip()
    return self.vip_time - os.time() > 0
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    local disName = self.name
    if self:isGameVip() then
        if BaseMain:isChina() then
            disName = TextFormat.colorGold .. "[会员]" .. disName
        else
            disName = TextFormat.colorGold .. "[VIP]" .. disName
        end
    end

    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:getHallInfo()
    local lv = self.level or 1
    local cur_exp = self.cur_exp or 0
    local max_exp, is_max = ExpConfig:getMaxExp(self.level)
    local yaoshi = self.yaoshi
    local vip_time = math.max(0, self.vip_time - os.time())
    return lv, cur_exp, max_exp, yaoshi, is_max, vip_time
end

function GamePlayer:syncHallInfo()
    local lv, cur_exp, max_exp, yaoshi, is_max, vip_time = self:getHallInfo()
    GamePacketSender:sendPixelGunHallInfo(self.rakssid, lv, cur_exp, max_exp, is_max, yaoshi, vip_time)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:onUnloadGun(type, itemId)
    self:removeItem(itemId, 1)
    local gun = GunConfig:getGunByItemId(itemId)
    if gun then
        self:removeItem(gun.BulletId, gun.SpareBullet)
    end
end

function GamePlayer:onEquipGun(type, itemId)
    self:replaceItem(itemId, 1, 0, tonumber(type) - 1, 1000)
    local gun = GunConfig:getGunByItemId(itemId)
    if gun then
        self:addItem(gun.BulletId, gun.SpareBullet, 0, true)
    end
end

function GamePlayer:onUnloadProp(itemId)
    self:removeItem(itemId, 1)
end

function GamePlayer:onEquipProp(itemId)
    for index, equip_prop in pairs(self.equip_props) do
        self:replaceItem(equip_prop.ItemId, 1, 0, 5 + index)
        if index >= 2 then
            break
        end
    end
end

function GamePlayer:onUnloadBlock(itemId)
    self:removeItem(itemId, 1000)
end

function GamePlayer:onEquipBlock(itemId, count)
    self:replaceItem(itemId, count, 0, 8)
end

function GamePlayer:syncMapModelInfo()
    if not DbUtil:CanSavePlayerData(self, DbUtil.GAME_DATA)
            or not DbUtil:CanSavePlayerData(self, DbUtil.MODE_DATA) then
        return
    end
    local data = self:getModeInfo()
    GamePacketSender:sendPixelGunSelectModeData(self.rakssid, json.encode(data))
end

function GamePlayer:getModeInfo()
    return ModeSelectConfig:getModeInfo(self)
end

function GamePlayer:unlockMap(map_id)
    if self.unlock_map then
        for _, map in pairs(self.unlock_map) do
            if tonumber(map) == tonumber(map_id) then
                return
            end
        end
        table.insert(self.unlock_map, map_id)
        self:syncMapModelInfo()
    end
end

function GamePlayer:isUnlockMap(map_id)
    if self.unlock_map then
        for _, map in pairs(self.unlock_map) do
            if tonumber(map) == tonumber(map_id) then
                return true
            end
        end
    end
    return false
end

function GamePlayer:getFragmentJindu(map_id)
    for _, frag in pairs(self.fragment_jindu) do
        if tostring(frag.map_id) == tostring(map_id) then
            return tonumber(frag.jindu)
        end
    end

    return 0
end

function GamePlayer:getModeFirst(gameType)
    for k, v in pairs(self.mode_first) do
        if tostring(v) == tostring(gameType) then
            return false
        end
    end
    return true
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
        self.taskController:packingProgressData(self, Define.TaskType.Achievement.PlayerLevel, GameMatch.gameType, nil, self.level)
        self.taskController:unlockMainLineTask(self)
        self:syncMapModelInfo()
        GamePacketSender:sendPixelGunShowLevelUp(self.rakssid)
        ReportUtil:reportUpgrade(self)
        GameGuide:sendDataToClient(self)
    end

    self:syncHallInfo()
end

function GamePlayer:teleInitPos()
    self:teleportPosWithYaw(GameConfig.initPos, 90)
    HostApi.changePlayerPerspece(self.rakssid, 1)
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

function GamePlayer:onBuyGiftBag(giftId)
    self.giftBagController:onGiftBagBought(tonumber(giftId))
end

function GamePlayer:doPlayerQuit()
    BaseListener.onPlayerLogout(self.userId)
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

return GamePlayer

--endregion


