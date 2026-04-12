--GamePlayer.lua
require "config.TeamConfig"
require "config.ScoreConfig"
require "config.RespawnConfig"
require "data.GameTeam"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.team = {}

    self:initTeam()

    self.multiKill = 0
    self.lastKillTime = 0
    self.kills = 0
    self.score = 0
    self.isLife = true
    self.realLife = true
    self.standTime = 0
    self.keepItemTimes = 1
    self.isKeepItem = false
    self.keepItemSource = 0
    self.inventory = {}
    self.isSingleReward = false

    self:teleInitPos()
    self:initScorePoint()

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.DIE] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
    self.scorePoints[ScoreID.DESTROY_BED] = 0
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:getDisplayName()
    if self.team ~= nil then
        return self.team:getDisplayName() .. TextFormat.colorWrite .. self.name
    else
        return self.name
    end
end

function GamePlayer:initTeam()
    local team = GameMatch.Teams[self.dynamicAttr.team]
    if team == nil then
        local config = TeamConfig:getTeam(self.dynamicAttr.team)
        team = GameMatch.Teams[config.id]
        if team == nil then
            team = GameTeam.new(config, TeamConfig:getTeamColor(config.id))
        end
        GameMatch.Teams[config.id] = team
    end
    team:addPlayer()
    self.team = team
end

function GamePlayer:getTeamId()
    return self.dynamicAttr.team
end

function GamePlayer:teleInitPos()
    self.realLife = true
    HostApi.resetPos(self.rakssid, self.team.initPos.x, self.team.initPos.y + 0.5, self.team.initPos.z)
end

function GamePlayer:onLogout()
    if self.isLife then
        self.isLife = false
        self.team:subPlayer()
    end
end

function GamePlayer:showBuyRespawn()
    self.realLife = false
    local isRespawn = RespawnConfig:showBuyRespawn(self.rakssid, self.respawnTimes)
    if isRespawn == false then
        self:onDie()
        GameMatch:ifGameOver()
    end
end

function GamePlayer:onDieByPlayer()
    self.score = self.score + ScoreConfig.DIE_SCORE
    self.scorePoints[ScoreID.DIE] = self.scorePoints[ScoreID.DIE] + 1
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1

    if self.multiKill == 0 then
        self.multiKill = 1
    else
        if os.time() - self.lastKillTime <= 30 then
            self.multiKill = self.multiKill + 1
            self.scorePoints[ScoreID.SERIAL_KILL] = self.scorePoints[ScoreID.SERIAL_KILL] + 1
        else
            self.multiKill = 1
        end
    end

    self.lastKillTime = os.time()

    if self.multiKill >= 5 then
        self.score = self.score + ScoreConfig.KILL_5_SCORE
        MsgSender.sendMsg(IMessages:msgFiveKill(self:getDisplayName()))
    elseif self.multiKill >= 4 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 3 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 2 then
        self.score = self.score + ScoreConfig.KILL_2_SCORE
        MsgSender.sendMsg(IMessages:msgDoubleKill(self:getDisplayName()))
    end

end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN_SCORE
    self.appIntegral = self.appIntegral + 10
end

function GamePlayer:onDestroyBed()
    self.score = self.score + ScoreConfig.DESTROY_BED_SCORE
    self.scorePoints[ScoreID.DESTROY_BED] = self.scorePoints[ScoreID.DESTROY_BED] + 1
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.realLife = false
        self.team:subPlayer()
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
        HostApi.sendPlaySound(self.rakssid, 10002)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
    settlement.name = self.name
    settlement.isWin = 0
    settlement.gold = self.gold
    settlement.points = self.scorePoints
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
end

function GamePlayer:onGameEnd(win)
    self:reward(win, GameMatch:getPlayerRank(self), true)
end

function GamePlayer:overGame(win)
    self:onLogout()
    if win and self.isLife then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:reward(isWin, rank, isEnd)

    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin, 4)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    if isEnd then
        return
    end
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch.hasEndGame == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end

function GamePlayer:onUseCustomProp(propId)
    local maxhp = self:getMaxHealth()
    local extraHp = AppPropConfig:getRewardHp(propId)
    maxHp = maxhp + extraHp
    self:changeMaxHealth(maxHp)
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, self.kills, GameMatch:getPlayerRank(self), isCount)
end

function GamePlayer:sendOpenChantment(type)
    local result = {}
    result.equipmentId = 0
    result.cosumeId = 0
    result.cosumeNum = 0
    result.bag = {}
    result.effect_info = {}
    result.haveItemEnchantment = self:haveItemEnchantment()
    result.refreshType = type
    result.diamond = GameConfig.enchantmentQuickMoney
    result.consumeBg = "set:enchantment.json image:consume_bg"

    local inv = InventoryUtil:getPlayerInventoryInfo(self)
    if inv and inv.item then
        for _, stack in pairs(inv.item) do
            if stack.id == GameConfig.enchantmentConsume then
                result.cosumeId = GameConfig.enchantmentConsume
                result.cosumeNum = stack.num
            end

            if GameConfig:canEnchantment(stack.id) then
                local invPlayer = self:getInventory()
                if invPlayer then
                    local isItemEnchanted = invPlayer:isEnchantment(stack.stackIndex)

                    if not isItemEnchanted then
                        local item = {}
                        item.itemId = stack.id
                        table.insert(result.bag, item)

                        -- HostApi.log("stack " .. stack.id .. " " .. stack.num .. " " .. stack.stackIndex)
                    end
                end
            end
        end
    end

    for i = 1, #GameConfig.enchantmentEffect do
        local item = {}
        item.title, item.des = GameConfig:getEnchantmentEffectDes(i)
        table.insert(result.effect_info, item)
    end
    HostApi.log("result ")

    HostApi.sendOpenChantment(self.rakssid, json.encode(result))
end

function GamePlayer:fillInvetory(inventory)
    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    for i, armor in pairs(armors) do
        if armor.id > 0 and armor.id < 10000 then
            self:equipArmor(armor.id, armor.meta)
        end
    end
    
    local items = inventory.item
    for i, item in pairs(items) do
        if item.id > 0 and item.id < 10000 then
            if MerchantConfig.equipment[item.id] and MerchantConfig.equipment[item.id] == 1 then
                if item.enchantId and item.enchantLv then
                    self:addEnchantmentItem(item.id, item.num, 0, item.enchantId, item.enchantLv)
                else
                    self:addItem(item.id, item.num, item.meta)
                end
            end
        end
    end
end


function GamePlayer:haveItemEnchantment()
    local hasConsume = false
    local hasEquip = false

    local inv = InventoryUtil:getPlayerInventoryInfo(self)
    if inv and inv.item then
        for _, stack in pairs(inv.item) do
            if stack.id == GameConfig.enchantmentConsume then
                hasConsume = true
            end

            if GameConfig:canEnchantment(stack.id) then
                local invPlayer = self:getInventory()
                if invPlayer then
                    local isItemEnchanted = invPlayer:isEnchantment(stack.stackIndex)

                    if not isItemEnchanted then
                        hasEquip = true
                    end
                end
            end
        end
    end

    if hasConsume == false or hasEquip == false then
        return false
    end
    return true
end

function GamePlayer:setInventory()
    self.inventory = InventoryUtil:getPlayerInventoryInfo(self)
    local invPlayer = self:getInventory()
    local inv = InventoryUtil:getPlayerInventoryInfo(self)
    if self.inventory and  self.inventory.item then
        for k, stack in pairs(self.inventory.item) do
            if invPlayer then
                local isItemEnchanted = invPlayer:isEnchantment(stack.stackIndex)
                if isItemEnchanted then
                    self.inventory.item[k].enchantId = invPlayer:getItemStackEnchantmentId(stack.stackIndex, 1)
                    self.inventory.item[k].enchantLv = invPlayer:getItemStackEnchantmentLv(stack.stackIndex, 1)
                end
            end
        end
    end 
end


return GamePlayer

--endregion
