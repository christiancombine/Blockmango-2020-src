---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "config.RespawnConfig"
require "data.TaskController"

GamePlayer = class("GamePlayer", BasePlayer)
GamePlayer.Integral = {
    HEADSHOT = 1,
    KILL = 2,
    DIE = 3
}
GamePlayer.MaxModules = 3

function GamePlayer:init()
    self:initStaticAttr(0)
    self:teleInitPos()

    self.armor_value = 0
    self.equip_guns = {}
    self.equip_props = {}
    self.equip_blocks = {}
    self.equip_modules = {}
    self.equip_item = 0
    self.honorId = 0
    self.honor = 0
    self.money = 0
    self.mode_first = {}
    self.level = 1
    self.cur_exp = 0
    self.yaoshi = 0
    self.vip_time = 0
    self.gun_ticket = {}
    self.rewards = {}
    self.armor_level = 0
    self.task_progresses = {}
    self.advert_record = {
        DayKey = "",
        GoldAdTimes = 0,
        ShopAdTimes = 0,
        GameAdTimes = 0
    }

    self.isFirstAward = false
    self.teamId = 0
    self.kills = 0
    self.dies = 0
    self.evenKills = 0
    self.onSkillDamageUid = 0
    self.onPosionDamageUid = 0
    self.onFireDamageUid = 0
    self.damageType = ""
    self.isHeadshot = false
    self.headshots = 0
    self.isLife = true
    self.curDefense = 0
    self.maxDefense = 0
    self.maxHealth = 100
    self.addSpeed = 0
    self.addDamage = 0
    self.addHeadShot = 0
    self.invincibleTime = 0
    self.isFlyInvincible = false  --飞行无敌
    self.standByTime = 0
    self.siteId = 0
    self.waitRespawnTime = 0
    self.revenge = false
    self.extraAward = {}
    self.rank = 0
    self.allProperties = {}
    self.curUseItemId = 0
    self.is_need_show_lv_up = false
    self.isReward = false
    self.respawnTaskKey = ""
    self.killType = ""
    self.killTick = 0
    self.cur_reward = nil
    self.taskController = TaskController.new(self)
    self.isParachute = false
    self.isInPoison = false
    self.killBySpike = false
    self.killByPoison = false
    self.original_equip = {}
    self.closestWeapon = {
        id = -1,
        distance = 5000
    }
    self.supplys = {
        health = 0,
        defense = 0,
        block = 0,
        bullet = 0
    }
    self.qualitySupplys = {
        rare = 0,
        epic = 0,
        legend = 0
    }
    self.curWeapon = 0
    self.changeWing = false
    self:getUserSeasonInfo(3)
    self:setFlyWing(true, GameConfig.glideMoveAddition, GameConfig.glideDropResistance)
    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initDataFromDB(data, subKey)
    if subKey == DbUtil.GAME_DATA then
        self:initGameDataFromDB(data)
    end
    if subKey == DbUtil.PROTECT_ARMOR then
        self:initProtectArmorDataFromDB(data)
    end
    if subKey == DbUtil.REWARD_DATA then
        self:initRewardDataFromDB(data)
    end
    if subKey == DbUtil.TASK_PROGRESS_DATA then
        self:initTaskProgressDataFromDB(data)
    end
    if subKey == DbUtil.ADVERT_DATA then
        self:initAdvertDataFromDB(data)
    end
end

function GamePlayer:initGameDataFromDB(data)
    if not data.__first then
        self.level = ExpConfig:getPlayerInitLv(data.level or 1)
        self.money = data.money or 0
        self.cur_exp = data.cur_exp or 0
        self.armor_value = data.armor_value or 0
        self.yaoshi = data.yaoshi or 0
        self.chest_integral = data.chest_integral or 0
        self.chests = data.chests or {}
        self.equip_guns = data.equip_guns or {}
        self.equip_props = data.equip_props or {}
        self.equip_blocks = data.equip_blocks or {}
        self.equip_modules = data.equip_modules or {}
        self.cur_is_random = data.cur_is_random or 0
        self.cur_map_id = data.cur_map_id or ""
        self.mode_first = data.mode_first or {}
        self.mode_first_day_id = data.mode_first_day_id or 0
        self.vip_time = data.vip_time or 0
        self.gun_ticket = data.gun_ticket or {}
    else
        self:setInitial()
    end
    self:setCurrency(self.money)
    self:initMaxHealthByLevel(self.level)
    self:initDefense()
    GameMatch.Process:setPlayerInfo(self)
    self:initEquipItemsAttr()
    self:initEquipItems()
    self:changeMaxHealth(self.maxHealth)
    self:initModuleBlockData()
    self:sendNameToOtherPlayers()
end

function GamePlayer:initMaxHealthByLevel(level)
    local maxHealth = ExpConfig:getMaxHp(level)
    self.maxHealth = maxHealth
end

function GamePlayer:initModuleBlockData()
    if self.clientPeer == nil then
        return
    end
    local result = {}
    for pos = 1, GamePlayer.MaxModules do
        local module = self.equip_modules[tostring(pos)]
        if not module then
            table.insert(result, pos .. "-" .. "0")
        else
            table.insert(result, pos .. "-" .. module.Id)
        end
    end
    local data = table.concat(result, ",")
    self.clientPeer:setModuleBlockData(data)
end

function GamePlayer:initProtectArmorDataFromDB(data)
    if not data.__first then
        self.armor_level = data.armor_level or 1
    end
end

function GamePlayer:initRewardDataFromDB(data)
    if not data.__first then
        self.rewards = data.rewards or {}
    end
end

function GamePlayer:initTaskProgressDataFromDB(data)
    if not data.__first then
        self.task_progresses = data.task_progresses or {}
    end
end

function GamePlayer:initAdvertDataFromDB(data)
    if not data.__first then
        self.advert_record = data.advert_record or {
            DayKey = "",
            GoldAdTimes = 0,
            ShopAdTimes = 0,
            GameAdTimes = 0
        }
    end
    local key = DateUtil.getYearDay()
    if self.advert_record.DayKey ~= key then
        self.advert_record = {
            DayKey = key,
            GoldAdTimes = 0,
            ShopAdTimes = 0,
            GameAdTimes = 0
        }
    end
end

function GamePlayer:initEquipItemsAttr()
    for _, equip_gun in pairs(self.equip_guns) do
        self:onGetProperties(equip_gun.ItemId, equip_gun.PropertyIds)
        self:addGunDamage(equip_gun.ItemId, equip_gun.AddDamage)
    end
    for index, equip_prop in pairs(self.equip_props) do
        self:onGetProperties(equip_prop.ItemId, equip_prop.PropertyIds)
    end
    for type, equip_block in pairs(self.equip_blocks) do
        self:onGetProperties(equip_block.ItemId, equip_block.PropertyIds)
    end
end

function GamePlayer:initEquipItems()
    self:clearInv()
    for type, equip_gun in pairs(self.equip_guns) do
        self:replaceItem(equip_gun.ItemId, 1, 0, tonumber(type) - 1, 1000)
        local gun = GunConfig:getGunByItemId(equip_gun.ItemId)
        if gun ~= nil and GameMatch.gameType ~= "g1053" then
            self:addItem(gun.bulletId, gun.spareBullet, 0, true)
        end
    end
    for index, equip_prop in pairs(self.equip_props) do
        self:replaceItem(equip_prop.ItemId, 1, 0, 5 + index)
    end
    for index, equip_block in pairs(self.equip_blocks) do
        self:replaceItem(equip_block.ItemId, equip_block.Count, 0, tonumber(index) + 1)
    end
end

function GamePlayer:onGetProperties(itemId, propertyIds)
    local properties = PropertyConfig:getPropertyByIds(propertyIds)
    for _, property in pairs(properties) do
        property:onPlayerGet(self)
    end
    self.allProperties[tostring(itemId)] = properties
end

function GamePlayer:onRemoveProperties(itemId, propertyIds)
    local properties = PropertyConfig:getPropertyByIds(propertyIds)
    for _, property in pairs(properties) do
        property:onPlayerRemove(self)
    end
    self.allProperties[tostring(itemId)] = nil
end

function GamePlayer:onEquipItem(itemId)
    if self.curUseItemId == itemId then
        return
    end
    self:onUnloadItem(self.curUseItemId)
    self.curUseItemId = itemId
    local properties = self.allProperties[tostring(itemId)]
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerEquip(self)
        end
    end
end

function GamePlayer:onUnloadItem(itemId)
    local properties = self.allProperties[tostring(itemId)]
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerUnload(self)
        end
    end
end

function GamePlayer:onUsedItem(target, damage)
    local properties = self.allProperties[tostring(self.curUseItemId)]
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerUsed(self, target, damage)
        end
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:onPlayerRespawn()
    GamePacketSender:sendPixelGunReviveData(self.rakssid, false, "{}")
    LuaTimer:cancel(self.respawnTaskKey)
    self.invincibleTime = GameConfig.invincibleTime
    self:getResetItem()
    self:resetCurrentItem()
end

function GamePlayer:getResetItem()
    self.curDefense = self.maxDefense
    self:updateDefense()
    self:initEquipItems()
    self:changeMaxHealth(self.maxHealth)
end

function GamePlayer:resetCurrentItem()
    for type, equip_gun in pairs(self.equip_guns) do
        if equip_gun.ItemId == self.equip_item then
            self.entityPlayerMP:changeCurrentItem(tonumber(type) - 1)
            return
        end
    end
end

function GamePlayer:getTeamId()
    return self.teamId
end

function GamePlayer:setTeamId(teamId)
    self.teamId = teamId
    HostApi.changePlayerTeam(0, self.userId, self.teamId)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        HostApi.changePlayerTeam(self.rakssid, player.userId, player.teamId)
    end
end

function GamePlayer:isInvincible()
    return self.invincibleTime > 0
end

function GamePlayer:initDefense()
    self.curDefense = self.armor_value
    self.maxDefense = self.armor_value
    self:updateDefense()
end

function GamePlayer:addMaxDefense(defense, changeCur)
    self.maxDefense = self.maxDefense + defense
    if changeCur then
        self.curDefense = self.maxDefense
    end
    self:updateDefense()
end

function GamePlayer:subMaxDefense(defense, changeCur)
    self.maxDefense = self.maxDefense - defense
    if changeCur or self.maxDefense < self.curDefense then
        self.curDefense = self.maxDefense
    end
    self:updateDefense()
end

function GamePlayer:addDefense(defense)
    self.curDefense = self.curDefense + defense
    self.curDefense = math.min(self.curDefense, self.maxDefense)
    self:updateDefense()
end

function GamePlayer:subDefense(damage)
    if self.curDefense == nil or self.curDefense <= 0 then
        return damage
    end
    if self.curDefense - damage >= 0 then
        self.curDefense = self.curDefense - damage
        self:updateDefense()
        return 0
    else
        local result = damage - self.curDefense
        self.curDefense = 0
        self:updateDefense()
        return result
    end
end

function GamePlayer:updateDefense()
    if self.entityPlayerMP == nil then
        return
    end
    self.entityPlayerMP:changeDefense(self.curDefense, self.maxDefense)
end

function GamePlayer:addKey(key)
    self.yaoshi = self.yaoshi + key
end

function GamePlayer:onKill(dier)
    self.kills = self.kills + 1
    if GameMatch.curTick - self.killTick > GameConfig.comboKillTime then
        self.evenKills = 0
    end
    self.killTick = GameMatch.curTick
    self.evenKills = self.evenKills + 1
    self.appIntegral = self.appIntegral + 1
    local headshot = 0
    local comboKill = self:getCombo()
    local gun = GunConfig:getGunByItemId(self:getHeldItemId())
    local prop = PropConfig:getPropByItemId(self:getHeldItemId())
    local weaponType = 0
    local spike = 0
    if gun then
        weaponType = gun.gunType
    elseif prop then
        weaponType = 6
    end
    if self.isHeadshot then
        headshot = 1
        self.headshots = self.headshots + 1
        self.isHeadshot = false
        self.taskController:packingProgressData(self, Define.TaskType.Fright.BlowUpKill, GameMatch.gameType, weaponType)
    end
    self.taskController:packingProgressData(self, Define.TaskType.Fright.GeneralKill, GameMatch.gameType, weaponType)
    self.taskController:packingProgressData(self, Define.TaskType.Fright.WinKill, GameMatch.gameType, weaponType)
    local combo = ComboKillConfig:getInfoById(comboKill)
    if self.evenKills > 1 then
        if self.evenKills > 5 then
            self.taskController:packingProgressData(self, Define.TaskType.Fright.ComboKill, GameMatch.gameType, 5)
        else
            self.taskController:packingProgressData(self, Define.TaskType.Fright.ComboKill, GameMatch.gameType, self.evenKills)
        end
    end
    HostApi.notifyKills(self.rakssid, combo.icon, combo.sound, GameConfig.stayTime, GameConfig.animateTime)
    if dier.killBySpike then
        spike = 1
        dier.killBySpike = false
    end
    MsgSender.sendKillMsg(self.name, dier.name, self:getHeldItemId(), self.evenKills, headshot, TextFormat.colorAqua, 0, spike)
end

function GamePlayer:getCombo()
    local combo = self.evenKills
    if combo == 1 then
        if self.killType == "SnowstormSkill" then
            combo = 6
        elseif self.isHeadshot then
            combo = 7
        elseif self.killType == "GrenadeSkill" then
            combo = 8
        end
    end
    if self.evenKills > 5 then
        combo = 5
    end
    return combo
end

function GamePlayer:onDie(isEnd, isPlayer)
    self.isLife = false
    self.dies = self.dies + 1
    self.evenKills = 0
    self.equip_item = self:getHeldItemId()
    if isPlayer == true then
        self.respawnTaskKey = LuaTimer:schedule(function(rakssid)
            RespawnHelper.sendRespawnCountdown(self, 0)
        end, GameConfig.autoRespawnTime * 1000, nil, self.rakssid)
    end
    if isEnd then
        self.waitRespawnTime = 1
    end
end

function GamePlayer:sendReviveData(killer)
    local data = {}
    local killer_weapon = GunConfig:getGunByItemId(killer:getHeldItemId()) or PropConfig:getPropByItemId(killer:getHeldItemId())
    if killer_weapon ~= nil then
        data.killer_use_gun_img = killer_weapon.image
        data.killer_use_gun_name = killer_weapon.name
    else
        data.killer_use_gun_img = ""
        data.killer_use_gun_name = ""
    end
    data.killer_name = killer.name
    --five guns
    data.arms = {}
    local guns = {}
    for type, equip_gun in pairs(killer.equip_guns) do
        table.insert(guns, {
            type = tonumber(type),
            gun = equip_gun
        })
    end
    table.sort(guns, function(gun1, gun2)
        return gun1.type < gun2.type
    end)
    for _, c_gun in pairs(guns) do
        local equip_gun = c_gun.gun
        local arms_item = {}
        local gun = GunConfig:getGunByItemId(equip_gun.ItemId) or PropConfig:getPropByItemId(equip_gun.ItemId)
        if gun ~= nil then
            arms_item.img = gun.image
            arms_item.name = gun.name
        else
            arms_item.img = ""
            arms_item.name = ""
        end
        table.insert(data.arms, arms_item)
    end

    --two skills
    data.gadgets = {}
    for i, equip_prop in pairs(killer.equip_props) do
        local gadgets_item = {}
        local prop = PropConfig:getPropByItemId(equip_prop.ItemId)
        if prop ~= nil then
            gadgets_item.img = prop.image
            gadgets_item.name = prop.name
        else
            gadgets_item.img = ""
            gadgets_item.name = ""
        end
        table.insert(data.gadgets, gadgets_item)
    end
    --one block
    data.killer_block_img = ""
    data.killer_block_name = ""
    for _, equip_block in pairs(killer.equip_blocks) do
        local prop = PropConfig:getPropByItemId(equip_block.ItemId)
        if prop ~= nil then
            data.killer_block_img = prop.image
            data.killer_block_name = prop.name
        end
    end

    data.battle_time = GameConfig.waitRespawnTime
    data.killer_entityId = killer:getEntityId()
    data.killer_itemId = killer:getHeldItemId()

    data.cur_hp = math.max(math.floor(killer:getHealth()), 1)
    data.max_hp = math.max(math.floor(killer:getMaxHealth()), 1)
    data.cur_defence = math.floor(killer.curDefense)
    data.max_defence = math.floor(killer.maxDefense)
    GamePacketSender:sendPixelGunReviveData(self.rakssid, true, json.encode(data))
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:onTick()
    self.invincibleTime = self.invincibleTime - 1
    self.standByTime = self.standByTime + 1
    if self.standByTime == GameConfig.standByTime - 5 then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 2, Messages:msgNotOperateTip())
    end
    if self.standByTime >= GameConfig.standByTime then
        BaseListener.onPlayerLogout(self.userId)
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:respawnRightNow()
    RespawnHelper.sendRespawnCountdown(self, 0)
    LuaTimer:cancel(self.respawnTaskKey)
end

function GamePlayer:onPickUpBullet(coe)
    for i, v in pairs(self.equip_guns) do
        local gun = GunConfig:getGunByItemId(v.ItemId)
        if gun ~= nil then
            local num = gun.pickupBulletNum * coe
            self:addItem(gun.bulletId, num, 0, true)
        end
    end
end

function GamePlayer:onPickUpBlock(value)
    for index, equip_block in pairs(self.equip_blocks) do
        local num = self:getItemNumById(equip_block.ItemId) + value
        self:replaceItem(equip_block.ItemId, num, 0, tonumber(index) + 1)
    end
end

function GamePlayer:isGameVip()
    return self.vip_time - os.time() > 0
end
--
function GamePlayer:checkFirstAward()
    if self.mode_first == "" then
        table.insert(self.mode_first, GameMatch.gameType)
        return FirstAwardCheck:isFirstAward(GameMatch.gameType)
    end
    for i, v in pairs(self.mode_first) do
        if v == GameMatch.gameType then
            return false
        end
    end
    table.insert(self.mode_first, GameMatch.gameType)
    return FirstAwardCheck:isFirstAward(GameMatch.gameType)
end

function GamePlayer:getRewardInfo(reward, isWin)
    local data = {}
    data.exp = reward.exp
    data.money = reward.money
    data.yaoshi = reward.yaoshi
    data.integral = reward.integral
    if self:isGameVip() then
        data.exp = data.exp * ExtraAwardConfig:getVipAward("exp")
        data.money = data.money * ExtraAwardConfig:getVipAward("money")
        data.yaoshi = data.yaoshi * ExtraAwardConfig:getVipAward("yaoshi")
        if isWin then
            data.integral = data.integral * ExtraAwardConfig:getVipAward("integral")
        end

    end
    if self:checkFirstAward() then
        data.exp = data.exp * ExtraAwardConfig:getFirstAward("exp")
        data.money = data.money * ExtraAwardConfig:getFirstAward("money")
        data.yaoshi = data.yaoshi * ExtraAwardConfig:getFirstAward("yaoshi")
        if isWin then
            data.integral = data.integral * ExtraAwardConfig:getFirstAward("integral")
        end
    end
    if self.cur_is_random ~= 0 then
        data.exp = data.exp + ExtraAwardConfig:getRandomMapAward("exp")
        data.money = data.money + ExtraAwardConfig:getRandomMapAward("money")
        data.yaoshi = data.yaoshi + ExtraAwardConfig:getRandomMapAward("yaoshi")
        if isWin then
            data.integral = data.integral + ExtraAwardConfig:getRandomMapAward("integral")
        end
    end
    data.integral = self:getFinalIntegral(data.integral)
    if self.isReward == false then
        self:doReward(data)
    end
    return data
end

function GamePlayer:getFinalIntegral(integral)
    local headshot = HonorExponentConfig:getRateById(GamePlayer.Integral.HEADSHOT)
    local kill = HonorExponentConfig:getRateById(GamePlayer.Integral.KILL)
    local die = HonorExponentConfig:getRateById(GamePlayer.Integral.DIE)
    integral = integral + (self.headshots * headshot) + (self.kills * kill) + (self.dies * die)
    return integral
end

function GamePlayer:doReward(reward)
    self.isReward = true
    self.cur_reward = reward
    self.honor = self.honor + reward.integral
    self:addCurrency(reward.money)
    self:addExp(reward.exp)
    self:addKey(reward.yaoshi)
    self.taskController:packingProgressData(self, Define.TaskType.GrowUp.GainCoin, GameMatch.gameType, Define.Money.MONEY, reward.money)
    self.taskController:packingProgressData(self, Define.TaskType.GrowUp.GainCoin, GameMatch.gameType, Define.Money.KEY, reward.yaoshi)
    if reward.integral > 0 then
        self.taskController:packingProgressData(self, Define.TaskType.GrowUp.GainScore, GameMatch.gameType, nil, reward.integral)
    end
    self.taskController:packingProgressData(self, Define.TaskType.Fright.PerDiedCount, GameMatch.gameType, self.dies)
    self:addFragment()
    SeasonManager:addUserHonor(self.userId, reward.integral, self.level)
    if self.cur_is_random ~= 0 then
        self.taskController:packingProgressData(self, Define.TaskType.Fright.CompleteGame, GameMatch.gameType, "m1043_0")
    end
end

function GamePlayer:doDoubleReward()
    if self.advert_record.GameAdTimes >= GameConfig.maxGameAdTimes then
        return
    end
    if self.cur_reward == nil then
        return
    end
    self.advert_record.GameAdTimes = self.advert_record.GameAdTimes + 1
    self:addCurrency(self.cur_reward.money)
    self:addExp(self.cur_reward.exp)
    self:addKey(self.cur_reward.yaoshi)
    self.cur_reward = nil
end

function GamePlayer:addFragment()
    local item = {}
    item.Id = self.cur_map_id
    item.Count = 1
    item.Type = 4
    table.insert(self.rewards, item)
end

function GamePlayer:addExp(exp)
    local cur_max, is_max = ExpConfig:getMaxExp(self.level)
    if not is_max then
        self.cur_exp = self.cur_exp + exp
    end
    while not is_max and self.cur_exp >= cur_max do
        self.level = self.level + 1
        self.cur_exp = self.cur_exp - cur_max
        self:addLvUpReward()
        cur_max, is_max = ExpConfig:getMaxExp(self.level)
        --Report Upgrade Even
        ReportUtil:reportUpgrade(self)
    end
end

function GamePlayer:addLvUpReward()
    local money, yaoshi, gun_id, frag_num = ExpConfig:getLevelReward(self.level)
    self.money = self.money + money
    self.yaoshi = self.yaoshi + yaoshi

    -- add gun fragment
    if gun_id > 0 and frag_num > 0 then
        local item = {}
        item.Id = gun_id
        item.Count = frag_num
        item.Type = 1
        table.insert(self.rewards, item)
    end
    self.is_need_show_lv_up = true
end

function GamePlayer:getLvUpData()
    local data = {}
    local money, yaoshi, gun_id, frag_num = ExpConfig:getLevelReward(self.level)

    data.rewards = {}
    data.level = self.level

    local money_item = {}
    money_item.img = "set:pixelgungamebig.json image:jinbi"
    money_item.num = money
    table.insert(data.rewards, money_item)

    local yaoshi_item = {}
    yaoshi_item.img = "set:pixelgungamebig.json image:yaoshi_kuang"
    yaoshi_item.num = yaoshi
    table.insert(data.rewards, yaoshi_item)

    if gun_id > 0 and frag_num > 0 then
        local frag_item = {}
        frag_item.img = ExpConfig:getLevelFragImg(self.level)
        frag_item.num = frag_num
        table.insert(data.rewards, frag_item)
        -- show gun when gun and hp conflict 
        return data
    end

    local diffHp = ExpConfig:getDiffhp(self.level)

    if diffHp > 0 then
        local hp_item = {}
        hp_item.img = "set:pixelgungamebig.json image:hp_max"
        hp_item.num = diffHp
        table.insert(data.rewards, hp_item)
    end

    return data
end

function GamePlayer:showLvUpReward()
    if self.is_need_show_lv_up then
        self.is_need_show_lv_up = false
        local lvUpData = self:getLvUpData()
        GamePacketSender:sendPixelGunLevelUpData(self.rakssid, json.encode(lvUpData))
    end
end

function GamePlayer:getUserSeasonInfo(retryTime)
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

function GamePlayer:sendNameToOtherPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        local sameTeam = (player:getTeamId() == self:getTeamId())
        if not sameTeam then
            self.entityPlayerMP:changeNamePerspective(player.rakssid, true)
            player.entityPlayerMP:changeNamePerspective(self.rakssid, true)
        else
            self.entityPlayerMP:changeNamePerspective(player.rakssid, false)
            player.entityPlayerMP:changeNamePerspective(self.rakssid, false)
        end
        self:setShowName(GameMatch.Process:buildShowName(self, sameTeam), player.rakssid)
    end
end

function GamePlayer:onPickupGun(gunInfo)
    if not self.isLife then
        return
    end
    local gun = GunConfig:getGunByItemId(gunInfo.gun)
    local oldGun = 0
    local oldBulletNum = 0
    if gun ~= nil then
        for type, equip_gun in pairs(self.equip_guns) do
            if tonumber(type) == gun.gunType then
                oldGun = equip_gun.ItemId
                oldBulletNum = self.entityPlayerMP:getBulletNum(tonumber(type) - 1)
                break
            end
        end

        self.equip_guns[tostring(gun.gunType)] = {
            Id = gun.id,
            ItemId = gun.itemId,
            PropertyIds = gun.propertyIds,
            AddDamage = 0
        }
        if oldGun ~= 0 then
            self:removeItem(oldGun, 1)
            local gunConfig = {
                id = os.time(),
                pos = self:getPosition()
            }
            local newWeapon = WeaponSupply.new(gunConfig, oldGun, oldBulletNum)
            local oldWeapon = GunConfig:getGunByItemId(oldGun)
            self:onRemoveProperties(oldWeapon.itemId, oldWeapon.propertyIds)
            if (oldWeapon.quality > gun.quality) then
                newWeapon:setBan(self.userId)
            end
        end

        self:replaceItem(gun.itemId, 1, 0, tonumber(gun.gunType) - 1, gunInfo.bullet)
        self:onGetProperties(gun.itemId, gun.propertyIds)
        self:onPickUpWeapon(gun.quality)
    end
end

function GamePlayer:onOpenParachute()
    if self.isParachute then
        return
    end
    HostApi.sendChangeAircraftUI(self.rakssid, false)
    self.isParachute = true
    self:setGlide(true)
    self:changeClothes("custom_wing", 105302)
    HostApi.changePlayerPerspece(self.rakssid, 1)
end

function GamePlayer:resetClosestWeapon(id, distance)
    self.closestWeapon.id = id
    self.closestWeapon.distance = distance
    if id == -1 then
        GamePacketSender:sendPixelGunHideChickenWarning(self.rakssid, 3)
    end
end

function GamePlayer:setInitial()
    local initial = TableUtil.copyTable(PlayerInitialConfig:getInitialConfig())
    self.equip_guns = initial.gun
    self.equip_props = initial.prop
    self.equip_blocks = initial.block
    self.equip_modules = initial.module
    self.armor_value = initial.armor_value
end

function GamePlayer:onPickUpWeapon(type)
    if type == GunConfig.Quality.Rare then
        self.qualitySupplys.rare = self.qualitySupplys.rare + 1
    elseif type == GunConfig.Quality.Epic then
        self.qualitySupplys.epic = self.qualitySupplys.epic + 1
    elseif type == GunConfig.Quality.Legend then
        self.qualitySupplys.legend = self.qualitySupplys.legend + 1
    end
end

function GamePlayer:setWeaponInfo()
    if self:getHeldItemId() == self.curWeapon then
        return
    end
    self.curWeapon = self:getHeldItemId()
    if self:getHeldItemId() == 0 then
        GamePacketSender:sendPixelGunHideChickenWarning(self.rakssid, 4)
        return
    end
    local gun = GunConfig:getGunByItemId(self.curWeapon)
    if gun == nil then
        GamePacketSender:sendPixelGunHideChickenWarning(self.rakssid, 4)
        return
    end
    local data = {}
    data.img = gun.image
    data.damage = tostring(gun.attack)
    data.speed = tostring(gun.shootSpeed)
    data.properties = {}
    local properties = PropertyConfig:getPropertyByIds(gun.propertyIds)
    if properties ~= nil then
        for _, property in pairs(properties) do
            local info = {
                img = property.config.icon,
                name = property.config.name
            }
            table.insert(data.properties, info)
        end
    end
    GamePacketSender:sendPixelGunWeaponInfo(self.rakssid, data)
end

function GamePlayer:resetEquip()
    self.equip_blocks = self.original_equip.equip_blocks
    self.equip_props = self.original_equip.equip_props
    self.equip_guns = self.original_equip.equip_guns
end

return GamePlayer

--endregion


