require "Match"
require "config.TeamConfig"
require "config.RespawnConfig"
require "config.JobPondConfig"
require "data.GameTeam"
require "data.GameJob"
require "data.GameJobPond"
require "data.Area"
require "config.HonourConfig"

GamePlayer = class("GamePlayer", BasePlayer)
GamePlayer.Integral = {
    HEADSHOT = 1,
    KILL = 2,
    DIE = 3
}
GamePlayer.MaxModules = 3

GameResult = {
    WIN = 0, --赢
    LOSE = 1, --输
    DRAW = 2   --平局
}

function GamePlayer:init()
    self:initStaticAttr(0)
    self:teleInitPos()
    self:initScorePoint()
    self.team = {}
    self.money = 0
    self.teamId = self.dynamicAttr.team
    self:initTeam()
    self.kills = 0
    self.dies = 0
    self.evenKills = 0
    self.isLife = true

    self.rank = 0
    self.winner = 0
    self.addDamage = 0
    self.invincibleTime = 0
    self.standByTime = 0

    self.cur_jobId = 0    --当前职业id
    self.jobTimes = 0   --次数
    self.lastJobId = 0  --上一次职业id
    self.init_prop = {}

    self.jobs_data = {}     --玩家的职业数据
    self.Jobs_Pond = {}     --玩家的奖池数据
    self.jobName = ""
    self.pondRefreshTimes = 1 --记录玩家刷新奖池的次数
    self.maxHealth = self:getMaxHealth()
    self.health = self:getHealth()
    self.bloodReturn = {}
    self.hasGoldApple = false
    self.hasBloodApple = false
    self.bloodAppleTimer = 0
    self.bloodReturnTimer = 0
    self.bloodFreshTimer = 0
    self.canAddMaxApple = true
    self.commonProperty = {}
    self.canChangeIce = false
    self.curUseItem = {
        id = 0,
        meta = 0
    }
    self.iceArea = {}
    self.curArmor = {}
    self.isHaveTnt = false
    self.tntTimer = 0
    self.tntTime = 0
    self.tntDamage = 0
    self.hotPotato = {}
    self.useHollowBlock = false
    self.isHiding = false
    self.signDevilPack = false
    self.damageType = ""
    self.curApple = 0
    self.maxApple = 0

    self.surviveTime = -1    --存活时间
    self.inDevilPact = false
    self.backPearTimer = 0

    self.result_add_integral = 0 --添加的积分
    self.choiceJob = 0
    self.hurtInfo = {
        type = 0,
        id = 0,
        meta = 0,
        time = 0,
        attacker = 0
    }
    self.honorId = 0
    self.honor = 0
    self.level = 1
    self.GameJob = GameJob.new(self)
    self.GameJobPond = GameJobPond.new(self)
    self:setFlyWing(true, GameConfig.glideMoveAddition, GameConfig.glideDropResistance)
    self:getUserSeasonInfo(3)
end

function GamePlayer:onTick(tick)

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

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end
function GamePlayer:initGameDataFromDB(data)
    HostApi.log("initGameDataFromDB" .. json.encode(data))
    if not data.first then
        self.lastJobId = data.lastJobId or 0
        self.jobs_data = data.jobs_data or {}
        self.init_prop = data.init_prop or {}
    end
end

function GamePlayer:setCurJobId(id)
    self.cur_jobId = id
    self.lastJobId = id
end

function GamePlayer:sendNameToOtherPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        local sameTeam = (player:getTeamId() == self:getTeamId())
        self.entityPlayerMP:changeNamePerspective(player.rakssid, false)
        player.entityPlayerMP:changeNamePerspective(self.rakssid, false)
        self:setShowName(GameMatch:buildShowName(self, sameTeam), player.rakssid)
    end
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:initTeam()
    if self.teamId == 0 then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.NoThisTeam)
        return
    end
    print("GamePlayer:initTeam")
    local team = GameMatch.teams[self.dynamicAttr.team]
    if team == nil then
        print("self.dynamicAttr.team" .. self.dynamicAttr.team)
        local config = TeamConfig:getTeam(self.dynamicAttr.team)
        team = GameTeam.new(config)
        GameMatch.teams[config.teamId] = team
    end
    self.team = team
    --self.teamId = team.teamId
end

--花钱可复活
function GamePlayer:showBuyRespawn()
    print("GamePlayer:showBuyRespawn")
    --判断是否可以复活
    self.isLife = false
    local isRespawn = RespawnConfig:showBuyRespawn(self.rakssid, self.respawnTimes)
    if isRespawn == false then
        self:onDie()
    end
end

function GamePlayer:onDie()
    self.surviveTime = GameMatch.curTick - GameMatch.RunningTick
    self.isLife = false
    self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
    self:teleWatchPos()
    GameMatch:updatePlayerInfo(0)
    GameMatch:updateGameInfo()
    HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
end

function GamePlayer:reward(isWin, rank, isEnd)
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    if isEnd then
        return
    end
    self.rank = GameMatch:getPlayerRank()
    self.result_add_integral = HonourConfig:getHonourByRank(self.rank)
    SeasonManager:addUserHonor(self.userId, self.result_add_integral, self.level)
    RewardManager:getUserReward(self.userId, rank, function()
        self:sendPlayerSettlement()
    end)
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = self.rank
    settlement.gold = self.gold
    settlement.kills = self.kills
    settlement.result_add_integral = self.result_add_integral
    settlement.honorId = self.honorId
    GamePacketSender:sendDeadSummaryData(self.rakssid, json.encode(settlement))
end

function GamePlayer:getTeamId()
    return self.teamId
end

function GamePlayer:onLogout()
    if self.surviveTime == -1 then
        self.surviveTime = GameMatch.curTick - GameMatch.RunningTick
    end
    if self.isLife == true and self:isCountKill() then
        local attacker = PlayerManager:getPlayerByUserId(self.hurtInfo.attacker)
        if attacker then
            attacker:onKill(self)
        end
    end
    self.isLife = false
    self:takeOffMount()
end

function GamePlayer:onWin()

end

function GamePlayer:onKill(dier)
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.kills = self.kills + 1
    GameMatch:updatePlayerInfo(0)
    GameMatch:updateGameInfo()
    MsgSender.sendKillMsg(GameMatch:nameAndTeamId(self), GameMatch:nameAndTeamId(dier), self:getHeldItemId(), 0, 0, TextFormat.colorAqua, 0, 0)
end

function GamePlayer:checkBloodReturn()
    local players = PlayerManager:getPlayers()
    local apple, health, appleDuration = self.bloodReturn:getBloodReturn()
    for _, player in pairs(players) do
        if player.teamId == self.teamId and self.bloodReturn:isInArea(player:getPosition()) then
            player:setBloodReturn(apple, health, appleDuration)
        end
    end
end

function GamePlayer:setBloodReturn(apple, health, appleDuration)
    if self.hasBloodApple == false or self:canAddApple() then
        self.hasBloodApple = true
        if self.bloodAppleTimer ~= 0 then
            self:addApple(apple)
        else
            self:addMaxApple(apple)
        end
    end
    if appleDuration ~= -1 then
        if self.bloodAppleTimer ~= 0 then
            LuaTimer:cancel(self.bloodAppleTimer)
            self.bloodAppleTimer = 0
        end
        self.bloodAppleTimer = LuaTimer:schedule(function(userId, apple)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if not c_player then
                return
            end
            c_player:subMaxApple(apple)
            c_player.hasBloodApple = false
            c_player.bloodAppleTimer = 0
        end, appleDuration * 1000, nil, self.userId, apple)
    end
    self:addHealth(health)
end

function GamePlayer:propertyOnUse(hurter, damage)
    local properties = ItemConfig:getPropertiesByIdAndMeta(self.curUseItem.id, self.curUseItem.meta)
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            local pProperty = PropertyConfig:getPropertyById(property)
            pProperty:onPlayerUsed(self, hurter, damage)
        end
    end
end

function GamePlayer:propertyOnHit(hurter, damage)
    local properties = self.commonProperty
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            property:onPlayerUsed(self, hurter, damage)
        end
    end
end

function GamePlayer:propertyBeAttack(attacker, damage)
    for _, armor in pairs(self.curArmor) do
        local properties = ItemConfig:getPropertiesByIdAndMeta(armor.id, armor.meta)
        if properties and #properties > 0 then
            for _, property in pairs(properties) do
                local pProperty = PropertyConfig:getPropertyById(property)
                pProperty:onPlayerBeAttack(self, attacker, damage)
            end
        end
    end
end

function GamePlayer:addGameItem(itemId, num, itemMeta, isInit)
    if self.entityPlayerMP == nil then
        return
    end
    local item = ItemConfig:getItemByIdAndMeta(itemId, itemMeta or 0)
    if item ~= nil then
        if #item.enchantments > 0 then
            if isInit then
                if item.type == ItemConfig.ItemType.Defense then
                    self.entityPlayerMP:replaceArmorItemWithEnchant(itemId, num, itemMeta, item.enchantments)
                else
                    self:addEnchantmentsItem(itemId, num, itemMeta, item.enchantments)
                end
            else
                self:addEnchantmentsItem(itemId, num, itemMeta, item.enchantments)
            end
        else
            if GunSetting.isGunItem(itemId) then
                self:addGunItem(itemId, num, 0, 1000)
            else
                if isInit then
                    if item.type == ItemConfig.ItemType.Defense then
                        self:equipArmor(itemId, itemMeta)
                    else
                        self:addItem(itemId, num, itemMeta or 0)
                    end
                else
                    self:addItem(itemId, num, itemMeta or 0)
                end
            end
        end
    else
        if GunSetting.isGunItem(itemId) then
            self:addGunItem(itemId, num, 0, 1000)
        else
            self:addItem(itemId, num, itemMeta or 0)
        end
    end
    self:onGetItem(itemId, itemMeta)
end

function GamePlayer:onGetItem(itemId, meta)
    local properties = ItemConfig:getPropertiesByIdAndMeta(itemId, meta or 0)
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            local pProperty = PropertyConfig:getPropertyById(property)
            pProperty:onPlayerGet(self)
        end
    end
end

function GamePlayer:onDropItem(itemId, meta)
    local properties = ItemConfig:getPropertiesByIdAndMeta(itemId, meta or 0)
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            local pProperty = PropertyConfig:getPropertyById(property)
            pProperty:onPlayerRemove(self)
        end
    end
end

function GamePlayer:onEquipItemChange(itemId, meta)
    local item = { id = itemId, meta = meta }
    local type = ItemConfig:getTypeByIdAndMeta(itemId, meta)
    if type == ItemConfig.ItemType.Defense then
        self.curUseItem = item
        return
    end
    if self.curUseItem.id == itemId then
        return
    end
    self:onUnloadItem(self.curUseItem.id, self.curUseItem.meta)
    self.curUseItem = item
    self:onEquipItem(itemId, meta)
end

function GamePlayer:onEquipItem(itemId, meta)
    local properties = ItemConfig:getPropertiesByIdAndMeta(itemId, meta)
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            local pProperty = PropertyConfig:getPropertyById(property)
            pProperty:onPlayerEquip(self)
        end
    end
end

function GamePlayer:onUnloadItem(itemId, meta)
    local properties = ItemConfig:getPropertiesByIdAndMeta(itemId, meta)
    if properties and #properties > 0 then
        for _, property in pairs(properties) do
            local pProperty = PropertyConfig:getPropertyById(property)
            pProperty:onPlayerUnload(self)
        end
    end
end

function GamePlayer:getLuckyBlock(blockId, pos)
    local block = BlockConfig:getBlockById(blockId)
    if block == nil then
        print("can't find luck block [blockId]:" .. blockId)
        return
    end
    local items = BlockGoodsManager:randomGoods(block.poolId[BoxManager.Stage], 1)
    if TableUtil.getTableSize(items) == 0 then
        print("random block empty [stage]:" .. BoxManager.Stage .. " [blockId]:" .. blockId)
    end
    pos = VectorUtil.newVector3(pos.x + 0.5, pos.y, pos.z + 0.5)
    for _, item in pairs(items) do
        if block.type == Define.BlockType.EventBlock then
            EventConfig:onTriggered(item.itemId, self)
        else
            local pItem = ItemConfig:getItemById(item.itemId)
            if item.itemId == Define.HotPotato then
                if self:getInventory():isCanAddItem(item.itemId, 0, 1) then
                    self:becomeTntPlayer({ id = item.itemId, meta = pItem.meta or 0 })
                else
                    EngineWorld:addEntityItem(item.itemId, 1, 0, 10000, pos)
                end
            else
                local function addEntityItem(itemId, count, meta, pos)
                    EngineWorld:addEntityItem(itemId, count, meta, 10000, pos)
                    local config = BowConfig:getBowConfig(itemId)
                    if not config then
                        return
                    end
                    EngineWorld:addEntityItem(config.arrowId, config.arrowNum, 0, 10000, pos)
                end
                if pItem == nil then
                    addEntityItem(item.itemId, item.count, 0, pos)
                else
                    addEntityItem(pItem.itemId, item.count, pItem.meta, pos)
                end
            end
        end
    end
end

function GamePlayer:removeHotPotato()
    if self.HotPotato ~= nil then
        self:removeItem(self.hotPotato.id, 1, self.hotPotato.meta)
        self:onDropItem(self.hotPotato.id, self.hotPotato.meta)
    elseif self:getItemNumById(Define.HotPotato) > 0 then
        self:removeItem(Define.HotPotato, 1)
    end
    self.tntTime = 0
    self.tntDamage = 0
    self.hotPotato = {}
    if self.tntTeleTimer ~= 0 then
        LuaTimer:cancel(self.tntTeleTimer)
        self.tntTeleTimer = 0
    end
    if self.tntTimer ~= 0 then
        LuaTimer:cancel(self.tntTimer)
        self.tntTimer = 0
    end
    self.isHaveTnt = false
end

function GamePlayer:becomeTntPlayer(item)
    self:addGameItem(item.id, 1, item.meta)
    self:holdHotPotato(item.id, item.meta)
    self.isHaveTnt = true
    self.hotPotato = item
    local effect = SkillEffectConfig:getSkillEffect(item.id)
    if effect == nil then
        return
    end
    self:changeClothes("custom_hair", 30)
    self.tntDamage = tonumber(effect.value)
    local time = StringUtil.split(effect.param, "#")
    self.tntTime = tonumber(time[1])
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHotPotatoTelePos())
    self.tntTeleTimer = LuaTimer:schedule(function(userId)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return
        end
        player:tntTelePos()
    end, tonumber(time[2]) * 1000, nil, self.userId)
    self.tntTimer = LuaTimer:schedule(function(userId)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return
        end
        if player.isHaveTnt and player.isLife then
            player:tntExplode()
        end
    end, self.tntTime * 1000, nil, self.userId)
end

function GamePlayer:holdHotPotato(id, meta)
    local inv = self:getInventory()
    for i = 1, InventoryUtil.MAIN_INVENTORY_COUNT + InventoryUtil.ARMOR_INVENTORY_COUNT do
        local info = inv:getItemStackInfo(i - 1)
        if info.id == tonumber(id) and info.meta == tonumber(meta) then
            GamePacketSender:sendLuckySkyHoldHotPotato(self.rakssid, i - 1)
            self.entityPlayerMP:changeCurrentItem(0)
            return
        end
    end
end

function GamePlayer:tntTelePos()
    self.curPos = self:getPosition()
    self.tntTeleTimer = 0
    local targetId = GameMatch:randomPlayer(self:getTeamId(), true)
    local target = PlayerManager:getPlayerByRakssid(targetId)
    if target == nil then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgTeleportFailedCantFindTarget())
        return
    end
    self:teleportPos(target:getPosition())
end

function GamePlayer:tntExplode()
    EngineWorld:getWorld():createExplosion(self:getEntityPlayer(), self:getEntityPlayer(), self:getPosition(), 0, false)
    EngineWorld:addSimpleEffect("explosion.effect", self:getPosition(), self:getYaw(), 1000)
    self:subHealth(self.tntDamage)
    self:becomeNormalPlayer()
end

function GamePlayer:becomeNormalPlayer()
    self:resetClothes()
    self:removeHotPotato()
end

function GamePlayer:fireTNT()
    local pos = self:getPosition()
    local v3i = VectorUtil.toBlockVector3(pos.x, pos.y, pos.z)
    EngineWorld:setBlock(v3i, BlockID.TNT)
    EngineWorld:getWorld():fireTNT(v3i, self:getEntityId())
end

function GamePlayer:addInitItems()
    local result = JobConfig:getJobById(self.cur_jobId)
    if result == nil then
        return
    end
    local items = StringUtil.split(result.initializeItem, "#")
    local amounts = StringUtil.split(result.num, "#")
    for index, id in pairs(items) do
        local pItem = ItemConfig:getItemById(id)
        if pItem == nil then
            self:addItem(id, amounts[index])
        else
            self:addGameItem(pItem.itemId, amounts[index], pItem.meta, true)
        end
    end
end

function GamePlayer:addInitProps()
    for _, prop in pairs(self.init_prop) do
        local pItem = ItemConfig:getItemById(prop.id)
        if pItem == nil then
            self:addItem(prop.id, prop.num)
        else
            self:addGameItem(pItem.itemId, prop.num, pItem.meta, true)
        end
    end
    self.init_prop = {}
end

function GamePlayer:onEquipArmorChange(itemId, meta, armorType)
    if self.curArmor[armorType] ~= nil then
        local old = self.curArmor[armorType]
        self:onUnloadItem(old.id, old.meta)
    end
    local item = {
        id = itemId,
        meta = meta
    }
    self.curArmor[armorType] = item
    self:onEquipItem(itemId, meta)
end

function GamePlayer:canUseHollowBlock()
    local num = self:getItemNumById(2436)
    if num == 0 then
        return
    end
    self.useHollowBlock = true
    self:usingHollowBlock()
end

function GamePlayer:subHealth(damage)
    local value = self:subApple(damage)
    if value == 0 then
        return
    end
    return self.entityPlayerMP:heal(-damage)
end

function GamePlayer:subHealthWithDefense(damage)
    local defense = self:getTotalArmorValue()
    local value = damage - damage * defense * 0.04
    self:subHealth(value)
end

function GamePlayer:usingHollowBlock()
    local position = self:getPosition()

    self:setGlide(false)
    local dest = GameConfig.startGlideHeight - position.y + 10
    local totle = 0
    local motion = 0
    while (true) do
        motion = ((motion / 0.98) + 0.08) / 0.98

        totle = totle + motion
        if totle >= dest then
            break
        end
    end

    GamePacketSender:sendLuckySkyPlayerMotion(self.rakssid, VectorUtil.newVector3(0, motion, 0))

    self:removeItem(2436, 1)
    self:onDropItem(2436)
end

function GamePlayer:getFateDice(id)
    if id == 1 then
        self:addGameItem(2801, 1)
        return
    end
    if id == 2 then
        local event = EventConfig:getEventByType(EventConfig.EventType.QuicklyBuild)
        if event then
            EventConfig:onTriggered(event.config.id, self)
        end
        return
    end
    if id == 3 then
        self:addGameItem(ItemID.FLARE_BOMB, 10, 0)
        return
    end
    if id == 4 then
        self:addItem(2436, 5)
        return
    end
    if id == 5 then
        if self:getInventory():isCanAddItem(Define.HotPotato, 0, 1) then
            local item = { id = Define.HotPotato, meta = 0 }
            self:becomeTntPlayer(item)
        end
        return
    end
    if id == 6 then
        self:fireTNT()
        return
    end
end

function GamePlayer:onHurt(damage)
    if self.isHiding and damage > 0 then
        self:cancelHiding()
    end
end

function GamePlayer:onAttack(damage)
    if self.isHiding and damage > 0 then
        self:cancelHiding()
    end
end

function GamePlayer:cancelHiding()
    self:removeEffect(PotionID.INVISIBILITY)
    self.isHiding = false
    self.HidingTimer = 0
    self.HidingSkillTimer = 0
    LuaTimer:cancel(self.HidingTimer)
    LuaTimer:cancel(self.HidingSkillTimer)
end

function GamePlayer:useInvulnerabilityPotion()
    local effect = SkillEffectConfig:getSkillEffect(PotionItemID.POTION_NIGHT_VISION)
    if effect == nil then
        return
    end
    local param = StringUtil.split(effect.param, "#")
    local value = StringUtil.split(effect.value, "#")
    for index, item in pairs(param) do
        self:addEffect(tonumber(item), GameConfig.poisonDuration, tonumber(value[index]))
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

function GamePlayer:ifOnMount()
    if not self.entityPlayerMP then
        return false
    end
    return self.entityPlayerMP:isOnFlyingMount() or self.entityPlayerMP:isOnFloatingMount()
end

function GamePlayer:getRidingMount()
    if not self:ifOnMount() then
        return nil
    end
    local mount = EntityManager:getMount(self.entityPlayerMP:getFlyingMountId())
    if not mount then
        mount = EntityManager:getMount(self.entityPlayerMP:getFloatingMountId())
    end
    return mount
end

function GamePlayer:onMountHunt(damage)
    local mount = self:getRidingMount()
    if not mount then
        return false
    end
    mount:onHurt(damage)
    return true
end

function GamePlayer:takeOffMount()
    local mount = self:getRidingMount()
    if not mount then
        return
    end
    mount:onDead()
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
        end
    end
end

function GamePlayer:resetBloodReturn(ifResetApple)
    self.bloodReturn = {}
    if self.bloodAppleTimer ~= 0 and ifResetApple then
        self.hasBloodApple = false
        LuaTimer:cancel(self.bloodAppleTimer)
        self.bloodAppleTimer = 0
    end
    if self.bloodReturnTimer ~= 0 then
        LuaTimer:cancel(self.bloodReturnTimer)
        self.bloodReturnTimer = 0
    end
    if self.bloodFreshTimer ~= 0 then
        LuaTimer:cancel(self.bloodFreshTimer)
        self.bloodFreshTimer = 0
    end
end

function GamePlayer:addMaxApple(apple)
    self.maxApple = self.maxApple + apple
    self:addApple(apple)
end

function GamePlayer:subMaxApple(apple)
    self.maxApple = self.maxApple - apple
    if self.curApple > self.maxApple then
        self:subApple(apple)
    end
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
    if self.curApple - damage >= 0 then
        self.curApple = self.curApple - damage
        self:updateApple()
        return 0
    else
        local result = damage - self.curApple
        self.curApple = 0
        self:updateApple()
        return result
    end
end

function GamePlayer:updateApple()
    if self.entityPlayerMP == nil then
        return
    end
    if self.curApple == 0 then
        self.entityPlayerMP:changeApple(0, 0)
        return
    end
    self.entityPlayerMP:changeApple(self.curApple, self.maxApple)
end

function GamePlayer:canAddApple()
    return self.curApple < self.maxApple
end

function GamePlayer:removeCurItem(num)
    num = num or 1
    local inv = self:getInventory()
    if not inv then
        return
    end
    local slot = inv:getInventoryIndexOfCurrentItem()
    if slot ~= -1 then
        self:decrStackSize(slot, num)
    end
end

function GamePlayer:removeTimer()
    if self.backPearTimer ~= 0 then
        LuaTimer:cancel(self.backPearTimer)
        self.backPearTimer = 0
    end
    self:resetBloodReturn(true)
    self:resetApple()
    self.curApple = 0
    self.maxApple = 0
    self:updateApple()
end

function GamePlayer:resetStatus()
    self.useHollowBlock = false
    self.canChangeIce = false
    self.signDevilPack = false
    self.hasGoldApple = false
    self.canAddMaxApple = true
    if self.inDevilPact == true then
        self.inDevilPact = false
        GamePacketSender:sendLuckySkyRopeDate(self.rakssid, 0)
    end
    self.hurtInfo = {
        type = 0,
        id = 0,
        meta = 0,
        time = 0,
        attacker = 0
    }
    PacketSender:getSender():sendChangeQuicklyBuildBlockStatus(self.rakssid, false, false)
end

function GamePlayer:isEquipArmor(itemId, meta)
    local ArmorInv = InventoryUtil:getPlayerArmorInventoryInfo(self)
    for _, item in pairs(ArmorInv) do
        if itemId == item.id and meta == item.meta then
            return true
        end
    end
    return false
end

function GamePlayer:nextGameUseProp()
    if self.init_prop ~= nil then
        for i, item in pairs(self.init_prop) do
            GamePacketSender:sendUsePropData(self.rakssid, item.id, item.num)
        end
    end
end

function GamePlayer:isCountKill()
    if self.hurtInfo ~= nil and os.time() - self.hurtInfo.time <= 10 then
        if self.hurtInfo.attacker ~= self.userId then
            return true
        end
    end
    return false
end

function GamePlayer:updateHurtInfo(type, id, meta, attackerUserId)
    self.hurtInfo.type = type
    self.hurtInfo.id = id
    self.hurtInfo.meta = meta

    if attackerUserId ~= 0 then
        self.hurtInfo.time = os.time()
        self.hurtInfo.attacker = attackerUserId
    end
end

return GamePlayer

--endregion


