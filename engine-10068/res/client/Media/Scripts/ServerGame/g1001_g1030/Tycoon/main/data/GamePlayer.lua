--GamePlayer.lua
require "config.OccupationConfig"
require "config.CommodityConfig"
require "config.RankConfig"
require "config.GameConfig"
require "data.GamePotionEffect"
require "util.RewardUtil"
require "util.VideoAdHelper"
require "Match"

local utils = require "utils"
local GameRank = require "data.GameRank"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(GameMatch.gameType, 0)

    if GameMatch:isGameRunning() then
        self:teleGamePos()
    else
        self:teleInitPos()
    end

    self.occupationId = 0
    self.config = nil
    self.defense = 0
    self.damage = 0
    self.attackX = 1
    self.addDefense = 0
    self.addAttack = 0
    self.p_addDefense = 0
    self.p_addAttack = 0

    self.s_addDefense = 0   -- add defense when stay at home
    self.s_addAttack = 0    -- add attack when stay at home

    self.occName = nil
    self.domain = nil
    self.portal = nil
    self.isDead = false

    self.lastHp = 0
    self.isChangeHp = false

    self.maxHealth = self:getHealth()
    self.stealPercent = 0
    self.stealPercentIncrement = 0
    self.dropMoneyPercent = GameConfig.dropMoneyPercent
    self.rewardMoneyPercent = GameConfig.rewardMoneyPercent
    self.isPotionEffect = false
    self.isHealPotion = false
    self.interval = 0
    self.healHp = 0
    self.timer = 0
    self.isTeleport = false
    self.resourceValue = 1      --资源产出倍增值
    self.ownLastClothesId = 0
    self.ownPotionIds = {}
    self.ownPotionEffects = {}
    self.ownEquipIds = {}
    self.ownWeaponDamage = {}
    self.TNTDamage = nil
    self.killOccIds = {}        --当前命 所击杀过的职业id
    self.getCdRecord = {}
    self.kills = 0
    self.lastKillTime = 0
    self.multiKill = 0
    self._rank_data = {}
    self._rank_record = {}
    self.tmepownEquipIds = {} --临时存储玩家 的装备柜  会在玩家创建出装备柜的时候 保存起来
    self.guardList = {}     --玩家的门卫列表
    self.position = VectorUtil.newVector3(GameConfig.initPos.x, GameConfig.initPos.y, GameConfig.initPos.z)

    self.lastAddMaxHp = 0

    self.baseOccIds = GameConfig.baseOccIds
    --self.extraCanOccIds = {}        --临时存储当局玩家可用职业
    ----data----
    self.supOccIds = { }       --玩家购买过的可用氪金职业id
    self.upgradeEquips = { }   --玩家购买升级过的职业装备柜  --{ {职业：occid-> id ,装备：type->{ } } , ...}
    self.walletInfo = { }      --保存玩家身上的货币信息    --{ { 物品id： coin-> id , 物品num： coin-> num } }
    self.privilege = { }       --保存玩家拥有的特权
    self.isSaveWallet = false

    self.videoAdRecord = {
        moneyTimes = 0,
        buildingTimes = 0,
        tntTimes = 0,
        ladderTimes = 0,
        dayKey = 0
    }
    ------------

    --gift packs -- 大礼包
    self.giftpackOccIds = false
    --occ gift packs -- 职业礼包
    self.occGiftPack_common = {}
    self.occGiftPack_money = {}
    --pubResource packs --野区资源包
    self.addResource = 0
    self.addsaveMax = 0
    --hp packs --血量礼包
    self.foreverHp = 0

    --记录玩家被那个玩家用技能杀死的
    self.killerBySkill = 0

    self.freeOccId = 0
    self.getTntAdReward = false
    self.getLadderAdReward = false
    self.showVideoMoneyAdTime = 0
    self.showVideoBuildingAdTime = 0

    self:initScorePoint()
    self:setCurrency(GameConfig.money)

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:CheckCollision()
    local pos = VectorUtil.newVector3i(self.position.x, self.position.y, self.position.z)
    local blockId = EngineWorld:getBlockId(pos)
    return blockId == 44
end

function GamePlayer:onTick(ticks)
    self:updatePotionEffect()
end

function GamePlayer:updatePotionEffect()
    if self.isHealPotion then
        self.timer = self.timer + 1
        if self.timer % self.interval == 0 then
            self:addHealth(self.healHp)
        end
    end
    if self.isPotionEffect then
        for _, potionId in pairs(self.ownPotionIds) do
            local config = self.ownPotionEffects[tostring(potionId)]
            if config then
                if GamePotionEffect.LOW_HP_HEIGTH_DEFENSE == config.effect.id then
                    self.p_addDefense = ((1 - self:getHealth() / self.maxHealth) * config.effect.value * 100)
                end
                if GamePotionEffect.LOW_HP_TELEPORT == config.effect.id then
                    if self:getHealth() > config.effect.value then
                        self.isTeleport = false
                    end
                    if self.isTeleport then
                        return
                    end
                    if self.isDead then
                        return
                    end
                    if self:getHealth() < config.effect.value then
                        if config.effect.pos then
                            self:teleportPos(config.effect.pos)
                            self.isTeleport = true
                        end
                    end
                end
                if GamePotionEffect.LOW_HP_HEIGTH_ATTACK == config.effect.id then
                    self.p_addAttack = ((1 - self:getHealth() / self.maxHealth) * config.effect.value * 100)
                end
            end
        end
    end
end

function GamePlayer:onUseCustomProp(propId, itemId)
    local occId = OccupationConfig:getOccupationIdByCommonItemId(itemId) or 0
    if occId ~= 0 then
        table.insert(self.occGiftPack_common, occId)
        return
    end

    local m_occId = OccupationConfig:getOccupationIdByMoneyItemId(itemId) or 0
    if m_occId ~= 0 then
        table.insert(self.occGiftPack_money, m_occId)
        return
    end

    if propId == "game.props.15" then
        self.giftpackOccIds = true
        return
    end

    if propId == "game.props.17" then
        self.foreverHp = 10
        return
    end

    if propId == "game.props.18" then
        self.addResource = 10
        self.addsaveMax = 100
        return
    end

    local config = OccupationConfig:getUnlockOccupationInfoById(itemId)
    if config ~= nil then
        self.freeOccId = itemId
        return
    end
end

function GamePlayer:attackEffect(damage)
    if self.isPotionEffect then
        for _, potionId in pairs(self.ownPotionIds) do
            local config = self.ownPotionEffects[tostring(potionId)]
            if config then
                if GamePotionEffect.LOW_HP_HEIGTH_LEECH == config.effect.id then
                    self:addHealth(damage * config.effect.value)
                end
            end
        end
    end
end

function GamePlayer:AddGrardToList(guard)
    if guard then
        table.insert(self.guardList, guard)
    end
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        --处理超级英雄特权时间
        local curTime = os.time()
        self.privilege = data.privilege or {}
        for occId, endTime in pairs((self.privilege or {})) do
            if tonumber(curTime) > tonumber(endTime) then
                --移除已过期的英雄特权
                self.privilege[occId] = nil
            end
        end
        --初始化超级英雄氪金职业id列表
        for _, occId in pairs((data.supOccIds or {})) do
            local isAdd = true
            for _, bId in pairs(self.baseOccIds) do
                if occId == bId then
                    isAdd = false
                    break
                end
            end
            if isAdd then
                table.insert(self.supOccIds, occId)
            end
        end
        --初始化玩家剩余货币信息
        for _, item in pairs((data.walletInfo or {})) do
            if item.coinNum > 0 then
                table.insert(self.walletInfo, item)
            end
        end
        --初始化玩家购买过的职业升级装备柜
        self.upgradeEquips = data.upgradeEquips or {}
        self.videoAdRecord = data.videoAdRecord or { dayKey = 0 }
    end
    for _, item in pairs(self.walletInfo) do
        self:addItem(item.coinId, item.coinNum, 0)
    end
    self:syncPrivilegeData()
    VideoAdHelper:initPlayerData(self)
end

--在玩家复活的时候 把玩家创建的装备柜 的装备给玩家
function GamePlayer:OnRespawn()
    for index = 1, #self.tmepownEquipIds do
        local equipbox = self.tmepownEquipIds[index]
        if equipbox then
            equipbox:getEquip(self)
        end
    end
end

function GamePlayer:OnLoginOut()
    if self.domain and self.domain.basicId ~= 0 and GameMatch.curStatus ~= 3 then
        self.domain:onRemoveOccMeta()
        self.domain:CreateRemainingEquipBox()
    end
    for index = 1, #self.guardList do
        local guard = self.guardList[index]
        guard:openDoor()
    end
    if self.occupationId ~= 0 and GameMatch.curStatus == 3 then
        self:resetOcc()
    end
    GameMatch:subPublicResources(self.addResource, self.addsaveMax)
end

function GamePlayer:getResourcesMoney()
    if self.domain ~= nil then
        local money = self.domain:getAllPrivateStock()
        return self:ConversionNum(money)
    end
    return tostring(0)
end

function GamePlayer:getPlayerMoney()
    local money = self:getCurrency()
    return self:ConversionNum(money)
end

function GamePlayer:updateRealRankData()
    GameMatch:sendRealRankData(self)
end

function GamePlayer:ConversionNum(money)
    local res = math.floor(money / 1000)
    if res > 0 and res < 10 then
        return res .. 'k +'
    elseif res >= 10 then
        return math.floor(res / 10) .. 'W +'
    end
    return tostring(math.floor(money))
end

function GamePlayer:onRemoveHealHp()
    self.isHealPotion = false
    self.interval = 0
    self.healHp = 0
    self.timer = 0
end

function GamePlayer:respawnInGame()
    GamePotionEffect:onRemovePotionEffect(self)
    self.isDead = false
    self.isPotionEffect = false
    self.isTeleport = false
    self.resourceValue = 1
    self.ownLastClothesId = 0
    self.ownPotionIds = {}
    self.ownEquipIds = {}
    self.killOccIds = {}
    self.killerBySkill = 0
    if self.domain ~= nil then
        if self.config ~= nil then
            self:onSelectOccuption(self.config)
        end
        self:teleportPos(self.domain:getRespwanPos())
        self:OnRespawn()
    else
        self:teleGamePos()
    end
end

function GamePlayer:dorpMoneyByPercent(killer, dier)
    local money = self:getCurrency()
    local sub = math.floor(money * self.dropMoneyPercent)
    local curMoney = math.max(0, money - sub)
    local reward = math.floor(money * self.rewardMoneyPercent)
    if money > 0 and sub > 0 then
        MsgSender.sendCenterTips(
                2,
                Messages:killPlayer(killer.occName or "▢FFFFFFFFnobody", dier.occName or "▢FFFFFFFFnobody", sub)
        )
        killer:rewardPlayerKill(reward, dier)
    end
    self:setCurrency(curMoney)
end

function GamePlayer:rewardPlayerKill(rewrad, dier)
    self:addCurrency(rewrad)
    if self.isPotionEffect then
        for _, potionId in pairs(self.ownPotionIds) do
            local config = self.ownPotionEffects[tostring(potionId)]
            if config then
                if GamePotionEffect.ADD_MAXHP_BY_KILL == config.effect.id then
                    self:changeMaxHealth(self.maxHealth + config.effect.value)
                    self.maxHealth = self:getHealth()
                end
                if GamePotionEffect.GET_MONEY_BY_KILL == config.effect.id then
                    if dier.domain then
                        local steal = dier.domain:getAllPrivateStock() * config.effect.value
                        self:addCurrency(steal)
                        dier.domain:subResourceStock(steal)
                    end
                end
                if GamePotionEffect.ADD_ATTCK_BY_KILL == config.effect.id then
                    local isAdd = false
                    for _, id in pairs(self.killOccIds) do
                        if dier.occupationId == id then
                            isAdd = true
                        end
                    end
                    if isAdd == false then
                        table.insert(self.killOccIds, dier.occupationId)
                        self:setAddAttack(self.addAttack + config.effect.value)
                    end
                end
            end
        end
    end
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:teleGamePos()
    self.isPotionEffect = false
    self.isTeleport = false
    self.resourceValue = 1
    self.ownLastClothesId = 0
    self.ownPotionIds = {}
    self.ownEquipIds = {}
    self.killOccIds = {}
    self.killerBySkill = 0
    self:saveWalletData()
    self:clearInv()
    AppProps:useItemAppProps(self)
    VideoAdHelper:checkShowShopAd(self)
    if self.domain ~= nil then
        self:teleportPos(self.domain:getRespwanPos())
        return
    end
    self:teleportPos(GameConfig.gamePos)
end

function GamePlayer:teleSelectRolePos()
    self:teleportPos(GameConfig.selectRolePos)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end

function GamePlayer:resetOcc()
    self.domain:resetDomain(self)
end

function GamePlayer:onSelectDomain(domain)
    self.domain = domain
    self.gamePos = domain:getRespwanPos()
end

function GamePlayer:onSelectOccuption(config)
    if config ~= nil then
        self.config = config
        self.stealPercent = config.stealPercent
        self.occupationId = self.config.id
        self.occName = config.displayName
        self.defense = config.defense
        self.damage = config.damage
        self.attackX = config.attackX
        self.maxHealth = config.health + self.foreverHp
        self.lastAddMaxHp = 0
        self:changeMaxHealth(self.maxHealth)
        for i, chothes in pairs(self.config.actors) do
            self:changeClothes(chothes.name, chothes.id)
        end
        for i, item in pairs(self.config.inventory) do
            if item.isEquip then
                self:equipArmor(item.id, item.meta)
            else
                self:addItem(item.id, item.num, item.meta)
            end
        end
        self:sendSkillData()
        self:setOccupation()
        self:setShowName(self:buildShowName())
        MsgSender.sendMsgToTarget(self.rakssid, Messages:selectRoleHint(self.config.displayName))
    end
end

function GamePlayer:setOccupation()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:setOccupation(self.occupationId)
    end
end

function GamePlayer:setAddDefense(addDefense)
    self.addDefense = addDefense
end

function GamePlayer:resetDefense()
    self.addDefense = 0
end

function GamePlayer:setAddAttack(addAttack)
    self.addAttack = addAttack
end

function GamePlayer:resetAttack()
    self.addAttack = 0
end

function GamePlayer:changePlayerStealPercent(increment)
    self.stealPercentIncrement = increment
    self.stealPercent = self.stealPercent + self.stealPercentIncrement
end

function GamePlayer:onRemoveStealPercentPotion()
    self.stealPercent = math.max(0, self.stealPercent - self.stealPercentIncrement)
    self.stealPercentIncrement = 0
end
--得到玩家的建筑进度
function GamePlayer:getBuildProgress()
    if self.domain ~= nil then
        return self.domain:getBuildProgress()
    end
    return 0
end

function GamePlayer:getBuildTime()
    if self.domain ~= nil then
        return self.domain:getBuildTime()
    end
    return 0
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    if self.config and self.config.displayName then
        table.insert(nameList, self.config.displayName)
    end

    PlayerIdentityCache:buildNameList(self.userId, nameList, self.name)
    return table.concat(nameList, "\n")
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
end

function GamePlayer:overGame(win)
    if win then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:sendSkillData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = {}
    data.title = "tycoon.war.shop.tab.skill"
    data.props = {}
    local skills = SkillConfig:getSkillsByOccupationId(tonumber(self.occupationId))
    for i, skill in pairs(skills) do
        local item = {}
        item.id = skill.id
        item.name = skill.name
        item.image = skill.image
        item.desc = skill.desc
        item.level = -1
        item.price = 0
        item.value = 0
        item.nextValue = 0
        table.insert(data.props, item)
    end
    self.entityPlayerMP:changeUpgradeProps(json.encode(data))
end

function GamePlayer:onQuit()
    local isCount = 0
    if GameMatch:isGameOver() then
        isCount = 1
    else
        self.appIntegral = self:settlementBuildProgress()
    end
    ReportManager:reportUserData(self.userId, self.kills, RewardUtil:getPlayerRank(self), isCount)
end

--判断自身领取cd是否在cd中
function GamePlayer:isGetCd(cd, resourceId)
    local geptime = os.time() - self:getResourceRecord(resourceId)
    if tonumber(geptime) >= cd then
        return true
    end
    MsgSender.sendCenterTipsToTarget(self.rakssid, 2, Messages:hintGetResourceCd(cd - geptime))
    return false
end

function GamePlayer:setResourceRecord(resourceId)
    self.getCdRecord[resourceId] = os.time()
end

function GamePlayer:getResourceRecord(resourceId)
    return self.getCdRecord[resourceId] or 0
end

--rank
----------------------------------------------------------------
function GamePlayer:settlementBuildProgress()
    if self.domain == nil then
        return 0
    end
    if self.domain:getBuildProgress() > 0 then
        local progress = math.floor(self.domain:getBuildProgress() / 100)
        return progress
    end
    return 0
end

function GamePlayer:getScore()
    return self.domain:getBuildProgress() or 0
end

--修改排行榜数据
function GamePlayer:modify_rank_score()
    local diff = self:settlementBuildProgress()
    self.appIntegral = diff
    local wkey = utils.week_rank_id()
    if diff > 0 then
        HostApi.ZIncrBy(wkey, tostring(self.userId), diff)
    end
    local dkey = utils.day_rank_id()

    if diff > 0 then
        HostApi.ZIncrBy(dkey, tostring(self.userId), diff)
    end
end

function GamePlayer:request_rank()
    self._rank_data = {}
    local keys = {}
    for _, rank in ipairs(RankConfig.ranks) do
        self._rank_record[tostring(rank.id)] = 0
        table.insert(keys, utils.week_rank_id(rank.id))
        table.insert(keys, utils.day_rank_id(rank.id))
    end

    for _, key in ipairs(keys) do
        HostApi.ZScore(key, tostring(self.userId))
    end
end

function GamePlayer:receive_rank(key, rank, score)
    self._rank_data[key] = {
        rank = rank,
        userId = tonumber(tostring(self.userId)),
        score = score,
        name = self.name,
        vip = self.vip
    }
    local id = utils:get_id_from_key(key)
    if self._rank_record[id] == nil then
        return
    end
    self._rank_record[id] = self._rank_record[id] + 1
    if self._rank_record[id] % 2 == 0 then
        self:sync_rank(id)
    end
end

function GamePlayer:sync_rank(id)
    for _, rank in ipairs(RankConfig.ranks) do
        if not id or id == tostring(rank.id) then
            local wkey = utils.week_rank_id(id)
            local dkey = utils.day_rank_id(id)

            local ranks_week = GameRank:fetch_rank(wkey)
            local ranks_day = GameRank:fetch_rank(dkey)
            local own_week = self._rank_data[wkey]
            local own_day = self._rank_data[dkey]

            if ranks_week and ranks_day and own_week and own_day then
                local data = {
                    ranks = {
                        week = GameRank:fetch_rank(wkey),
                        day = GameRank:fetch_rank(dkey)
                    },
                    own = {
                        week = self._rank_data[wkey],
                        day = self._rank_data[dkey]
                    },
                    title = rank.name
                }
                HostApi.sendRankData(self.rakssid, GameRank:fetch_rank_npc(tonumber(id)), json.encode(data))
            end
        end
    end
end

--data
---------------------------------------------
function GamePlayer:BuyCommodity(mType, mGoodsInfo, mAttr, mMsg)
    local iType = CommodityConfig:getItemTypeByType(mType) or 5
    local occId = OccupationConfig:getOccupationIdByCommonItemId(mGoodsInfo.goodsId) or 0
    if tonumber(occId) == 0 then
        occId = OccupationConfig:getOccupationIdByMoneyItemId(mGoodsInfo.goodsId) or 0
    end
    if tonumber(occId) == 0 then
        return false
    end
    --if self.giftpackOccIds then
    --    return false
    --end
    --if #self.occGiftPack_money ~= 0 then
    --    for _, id in pairs(self.occGiftPack_money) do
    --        if id == occId then
    --            return false
    --        end
    --    end
    --end
    if tonumber(iType) == 0 then
        --解锁职业
        local isBuy = true
        for _, id in pairs(self.baseOccIds) do
            if tonumber(id) == tonumber(occId) then
                isBuy = false
                break
            end
        end
        for _, id in pairs(self.supOccIds) do
            if tonumber(id) == tonumber(occId) then
                isBuy = false
                break
            end
        end
        if isBuy then
            self:addsupOccIds(occId)
            mAttr.isAddGoods = false
            return true
        end
        mMsg.value = Messages:areUnlocked()
        return false
    end
    if tonumber(iType) ~= 0 then
        for _, bId in pairs(self.baseOccIds) do
            if tonumber(bId) == tonumber(occId) then
                if self:hasUpgradeEquip(occId, iType) then
                    mMsg.value = Messages:areUnlocked()
                    return false
                end
                self:addUpgradeEquips(occId, iType)
                mAttr.isAddGoods = false
                return true
            end
        end
        for _, sId in pairs(self.supOccIds) do
            if tonumber(sId) == tonumber(occId) then
                if self:hasUpgradeEquip(occId, iType) then
                    mMsg.value = Messages:areUnlocked()
                    return false
                end
                self:addUpgradeEquips(occId, iType)
                mAttr.isAddGoods = false
                return true
            end
        end
        return false
    end
    return false
end

function GamePlayer:addsupOccIds(occid)
    local isAdd = true
    for _, sid in pairs(self.supOccIds) do
        if tonumber(sid) == tonumber(occid) then
            isAdd = false
            break
        end
    end
    if isAdd then
        table.insert(self.supOccIds, occid)
        self:syncPrivilegeData()
    end
end

function GamePlayer:addUpgradeEquips(occId, type)
    local isOccId = false
    for id, item in pairs(self.upgradeEquips) do
        if tonumber(item.occid) == tonumber(occId) then
            local isAdd = false
            isOccId = true
            for _, tId in pairs(item.type) do
                if tonumber(tId) == tonumber(type) then
                    isAdd = true
                    break
                end
            end
            if not isAdd then
                table.insert(item.type, type)
            end
        end
    end
    if not isOccId then
        local item = {}
        item.occid = occId
        item.type = {}
        table.insert(item.type, type)
        table.insert(self.upgradeEquips, item)
    end
end

function GamePlayer:hasUpgradeEquip(id, type)
    for _, item in pairs(self.upgradeEquips) do
        if tonumber(item.occid) == tonumber(id) then
            for _, tId in pairs(item.type) do
                if tonumber(tId) == tonumber(type) then
                    return true
                end
            end
            return false
        end
    end
    return false
end

function GamePlayer:getDBDataRequire()
    DBManager:getPlayerData(self.userId, 1)
end

function GamePlayer:saveWalletData()
    if self.isSaveWallet then
        return
    end
    self.walletInfo = {}
    for _, coin in pairs(GameConfig.coinMapping) do
        local num = self:getItemNumById(coin.itemId)
        if num ~= 0 then
            local item = {}
            item.coinId = coin.itemId
            item.coinNum = num
            table.insert(self.walletInfo, item)
        end
    end
    self.isSaveWallet = true
end

function GamePlayer:savePlayerData()
    local data = {}
    data.supOccIds = {}
    data.supOccIds = self.supOccIds
    data.upgradeEquips = {}
    data.upgradeEquips = self.upgradeEquips
    data.walletInfo = self.walletInfo
    data.privilege = self.privilege
    DBManager:savePlayerData(self.userId, 1, data, true)
end

function GamePlayer:unlockOccupation(extra)
    local extras = StringUtil.split(extra, "=")
    if #extras ~= 2 then
        return
    end

    local npc = GameManager:getNpcByEntityId(tonumber(extras[2]))
    if npc == nil then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    if npc.curSelectNum >= npc.selectedMaxNum then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    local occupationInfo = OccupationConfig:getUnlockOccupationInfoById(npc.config.id)
    if occupationInfo == nil then
        MsgSender.sendCenterTipsToTarget(self.rakssid, 3, Messages:msgHeroHasBeenChosen())
        return
    end

    if extras[1] == "Single" then
        local moneyType = tonumber(occupationInfo.singleCurrencyType)
        local money = tonumber(occupationInfo.singlePrice)
        self:payUnlockOccupation(moneyType, 10000 + npc.config.id, money, extra, npc)
        return
    end

    if extras[1] == "Perpetual" then
        local moneyType = tonumber(occupationInfo.perpetualCurrencyType)
        local money = tonumber(occupationInfo.perpetualPrice)
        self:payUnlockOccupation(moneyType, 20000 + npc.config.id, money, extra, npc)
        return
    end

end

function GamePlayer:payUnlockOccupation(moneyType, id, money, extra, npc)
    local extras = StringUtil.split(extra, "=")
    if #extras ~= 2 then
        return
    end

    if tonumber(moneyType) == 1 then
        self:consumeDiamonds(id, money, extra, false)
    elseif tonumber(moneyType) == 3 then
        if tonumber(self:getCurrency()) < money then
            MsgSender.sendCenterTipsToTarget(self.rakssid, 2, Messages:noMoney())
            return
        end

        self:subCurrency(money)
        if extras[1] == "Single" then
            npc:selectOccupation(self)
        end

        if extras[1] == "Perpetual" then
            self:addsupOccIds(npc.config.id)
            npc:selectOccupation(self)
        end
    end

end

function GamePlayer:restoreActor()
    EngineWorld:restorePlayerActor(self)
end

function GamePlayer:syncPrivilegeData()
    if TableUtil.getTableSize(self.privilege) == 0 then
        return
    end
    local result = {}
    local curTime = os.time()
    for occId, endTime in pairs((self.privilege or {})) do
        local hasOccupation = false
        for _, id in pairs(self.supOccIds) do
            if tonumber(id) == tonumber(occId) then
                hasOccupation = true
            end
        end
        if not hasOccupation then
            result[tostring(occId)] = endTime - curTime
        end
    end

    PacketSender:sendLuaCommonData(self.rakssid, "Privilege", json.encode(result))
end

function GamePlayer:hidePrivilege()
    PacketSender:sendLuaCommonData(self.rakssid, "HidePrivilege")
end

return GamePlayer

--endregion
