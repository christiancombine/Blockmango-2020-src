---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "config.GunsConfig"
require "config.KillRewardConfig"
require "config.RespawnConfig"
require "config.ItemSettingConfig"
require "config.InitItemsConfig"
require "config.SpecialClothingConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.isReport = false
    self.team = nil
    self.teamId = 1
    self.kills = 0
    self.evenKills = 0
    self.dies = 0
    self.headshot = 0
    self.money = 0
    self.score = 0
    self.hallMoney = 0
    self.guns = {}
    self.isLife = true
    self.invincibleTime = 0
    self.curDefense = 0
    self.maxDefense = 0
    self.inventory = {}
    self.getInitPorps = false
    self.dieTick = 0
    self.isShowShop = false
    self.moveHash = ""
    self.standByTime = 0
    self.specialClothing = {}
    self.gunSprayPaints = {}

    self:initInventory()
    self:initScorePoint()
    self:setPlayerHealth()
    self:setCurrency(0)

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initInventory()
    self.inventory = {}
    self.inventory.item = {}
    self.inventory.gun = {}
    self.inventory.armor = {}
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.DIE] = 0
end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.guns = data.guns or {}
        self.hallMoney = data.money or 0
        self.teamId = data.teamId or 1
        self.gunSprayPaints = data.gunSprayPaints or {}
        self.specialClothing = data.specialClothing or {}
    end
    self:setPlayerTeam()
    self:initTeam()
    self:teleInitPos()
    self:setCurrency(self.money)
    self:initGunExtraAttr()
    self:updateCustomProps()
    self:sendNameToOtherPlayers()
    GameManager:onPlayerInGame(self)
end

function GamePlayer:setPlayerTeam()
    local teams = GameMatch.Teams
    if self:isTeamMax() then
        for i, team in pairs(teams) do
            if self.teamId ~= team.id then
                self.teamId = team.id
                break
            end
        end
    end
    self:changeTeamActor(self.teamId)
end

function GamePlayer:initTeam()
    local team = GameMatch.Teams[self.teamId]
    if team == nil then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.NoThisTeam)
        return
    end
    self.team = team
    team:addPlayer()
    self:setShowName(self:buildShowName())
    self:syncTeamId()
    GameMatch:sendAllPlayerTeamInfo()
    GameManager:updateTeamKills()
end

function GamePlayer:isTeamMax()
    local teams = GameMatch.Teams
    for i, c_team in pairs(teams) do
        if self.teamId == c_team.id and c_team.curPlayers >= GameConfig.teamMaxPlayers then
            return true
        end
    end
    return false
end

function GamePlayer:teleInitPos()
    if GameMatch:ifGameRunning() then
        local respawn = RespawnConfig:randomRespawn(self.teamId)
        self:teleportPos(respawn.position)
    else
        self:teleportPos(GameConfig.initPos)
    end
end

function GamePlayer:initGunExtraAttr()
    for gunId, gun in pairs(self.guns) do
        local guns = GunsConfig:getSameGanByInitialItemId(tonumber(gunId))
        for _, c_gun in pairs(guns) do
            self:addGunDamage(c_gun.itemId, (gun[1] or 0))
            self:addGunBulletNum(c_gun.itemId, (gun[2] or 0))
            self:subGunRecoil(c_gun.itemId, (gun[3] or 0))
        end
    end
end

function GamePlayer:updateCustomProps()
    if self.entityPlayerMP == nil then
        return
    end
    local data = self:getPropData(self.teamId)
    self.entityPlayerMP:changeCustomProps(json.encode(data))
end

function GamePlayer:getPropData(teamId)
    local data = {}
    for _, group in pairs(PropGroupConfig.groups) do
        local item = {}
        item.id = group.id
        item.name = group.name
        item.props = {}
        for _, prop in pairs(group.props) do
            if prop.teamId == 0 or prop.teamId == teamId then
                local _image = self:getGunImage(prop)
                table.insert(item.props, {
                    id = prop.id,
                    name = prop.name,
                    image = _image,
                    desc = prop.desc,
                    values = self:getGunValues(prop),
                    price = prop.price,
                })
            end
        end
        table.insert(data, item)
    end
    return data
end

function GamePlayer:sendNameToOtherPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player:getTeamId() ~= self:getTeamId() then
            self:setShowName(" ", player.rakssid)
            player:setShowName(" ", self.rakssid)
        else
            self:setShowName(self:buildShowName(), player.rakssid)
            player:setShowName(player:buildShowName(), self.rakssid)
        end
    end
end

function GamePlayer:changeTeamActor(teamId)
    if self.entityPlayerMP == nil then
        return
    end
    local actor = self:getPlayerActor(teamId)
    if actor ~= nil and #actor > 0 then
        EngineWorld:changePlayerActor(self, actor)
    end
end

function GamePlayer:getPlayerActor(teamId)
    if self.specialClothing == nil then
        return GameConfig.redTeamActor
    end
    local actor
    local actors = SpecialClothingConfig:getSameTypeActor(teamId)
    for i, v in pairs(actors) do
        if self.specialClothing[v.clothingId] ~= nil then
            if self.specialClothing[v.clothingId] == tostring(teamId) then
                actor = v.actor
                break
            end
        end
    end
    if actor == nil then
        if teamId == 1 then
            actor = GameConfig.redTeamActor
        else
            actor = GameConfig.blueTeamActor
        end
    end
    return actor
end

function GamePlayer:syncTeamId()
    HostApi.changePlayerTeam(0, self.userId, self.teamId)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player ~= self then
            HostApi.changePlayerTeam(self.rakssid, player.userId, player.teamId)
        end
    end
end

function GamePlayer:teleTeamPos()
    if self.team ~= nil then
        self:teleportPos(self.team.pos)
        self:setRespawnPos(self.team.pos)
    end
end

function GamePlayer:playerRespawn()
    local respawn = RespawnConfig:randomRespawn(self.teamId)
    self:fillInvetory(self.inventory)
    self:setPlayerHealth()
    self.invincibleTime = respawn.invincibleTime
    self.isLife = true
end

function GamePlayer:setPlayerHealth()
    self:changeMaxHealth(GameConfig.initPlayerHealth)
end

function GamePlayer:fillInvetory(inventory)
    self:clearInv()
    if inventory == nil then
        return
    end
    local armors = inventory.armor
    for _, armor in pairs(armors) do
        if armor.id > 0 and armor.id < 10000 then
            self:equipArmor(armor.defense)
        end
    end
    local guns = inventory.gun
    for _, gun in pairs(guns) do
        self:addGunItem(gun.id, gun.num, gun.meta, gun.bullets)
    end
    local items = inventory.item
    for _, item in pairs(items) do
        if item.id > 0 and item.id < 10000 then
            local c_item = ItemsConfig:getItemByItemId(item.id)
            if c_item ~= nil then
                self:addItem(item.id, item.num, item.meta, c_item.isLast)
            else
                self:addItem(item.id, item.num, item.meta)
            end
        end
    end
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:isInvincible()
    return self.invincibleTime > 0
end

function GamePlayer:getTeamId()
    if self.team ~= nil then
        return self.team.id
    end
    return 0
end

function GamePlayer:getGunImage(prop)
    local guns = {}
    for i, item in pairs(prop.items) do
        if item.isGun then
            guns = GunsConfig:getSameGanByInitialItemId(item.itemId)
        end
    end
    if guns == nil then
        return prop.image
    end
    for i, gun in pairs(guns) do
        if self.gunSprayPaints[tostring(gun.itemId)] == 1 then
            return gun.image
        end
    end
    return prop.image
end

function GamePlayer:getGunValues(prop)
    for i, item in pairs(prop.items) do
        if item.isGun then
            local gun = self.guns[tostring(item.itemId)]
            if gun ~= nil then
                return tostring(gun[1] .. "#" .. gun[2] .. "#" .. (gun[3] * 100))
            else
                return "0#0#0"
            end
        end
    end
    return ""
end

function GamePlayer:addMoney(money)
    self:addCurrency(money)
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:subDefense(damage)
    if self.curDefense <= 0 then
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

function GamePlayer:equipArmor(defense)
    if self.curDefense >= defense then
        return false
    end
    self.curDefense = math.max(self.curDefense, defense)
    self.maxDefense = math.max(self.maxDefense, defense)
    self:updateDefense()
    return true
end

function GamePlayer:buyProp(prop)
    if prop.price > self.money then
        return false
    end
    if prop.teamId ~= 0 and prop.teamId ~= self.teamId then
        return false
    end
    local canAddProp = true
    for _, item in pairs(prop.items) do
        if canAddProp then
            canAddProp = ItemSettingConfig:canAddItem(self, item.itemId, item.num)
        else
            return false
        end
    end
    local notify = false
    for _, item in pairs(prop.items) do
        if item.isGun then
            local canAdd, num = ItemSettingConfig:canAddItem(self, item.itemId, item.num)
            if canAdd then
                local itemId = self:canBuySprayPaintGun(item.itemId)
                local bullets = self:getGunBullets(item.itemId) + item.bullets
                if self.isLife then
                    self:addGunItem(itemId, num, 0, bullets, item.isLast)
                else
                    table.insert(self.inventory.gun, {
                        id = itemId,
                        meta = 0,
                        num = 1,
                        bullets = bullets
                    })
                end
                if not notify then
                    HostApi.notifyGetItem(self.rakssid, itemId, 0, 1)
                    notify = true
                end
            end
        end
        if item.isArmor then
            canAddProp = self:equipArmor(item.defense)
            if not notify and canAddProp then
                HostApi.notifyGetItem(self.rakssid, item.itemId, 0, 1)
                notify = true
            end
        end
        if item.isGun == false and item.isArmor == false then
            local canAdd, num = ItemSettingConfig:canAddItem(self, item.itemId, item.num)
            if canAdd then
                if self.isLife then
                    self:addItem(item.itemId, item.num, 0, item.isLast)
                else
                    table.insert(self.inventory.item, {
                        id = item.itemId,
                        meta = 0,
                        num = item.num
                    })
                end
                if not notify then
                    HostApi.notifyGetItem(self.rakssid, item.itemId, 0, item.num)
                    notify = true
                end
            end
        end
    end
    if canAddProp then
        self:subMoney(prop.price)
    end
end

function GamePlayer:canBuySprayPaintGun(itemId)
    local guns = GunsConfig:getSameGanByInitialItemId(itemId)
    for i, gun in pairs(guns) do
        if self.gunSprayPaints[tostring(gun.itemId)] ~= nil and self.gunSprayPaints[tostring(gun.itemId)] == 1 then
            return gun.itemId
        end
    end
    return itemId
end

function GamePlayer:getGunBullets(itemId)
    for gunId, gun in pairs(self.guns) do
        if tonumber(gunId) == itemId then
            return gun[2]
        end
    end
    return 0
end

function GamePlayer:onPrepare()
    self:teleTeamPos()
    self:prepareInitPorps()
end

function GamePlayer:prepareInitPorps()
    if self.getInitPorps == false then
        local items = InitItemsConfig:getGameStartItems()
        for _, item in pairs(items) do
            self:addItem(item.itemId, item.num, 0)
        end
        self.getInitPorps = true
    end
end

function GamePlayer:onTick()
    self.invincibleTime = self.invincibleTime - 1
    if self.dieTick > 0 and self.isShowShop == true then
        self.dieTick = self.dieTick - 1
    elseif self.dieTick <= 0 and self.isShowShop == true then
        HostApi.sendShowPersonalShop(self.rakssid)
        self.isShowShop = false
    end
    self.standByTime = self.standByTime + 1
    if self.standByTime == GameConfig.standByTime - 10 then
        MsgSender.sendBottomTipsToTarget(self.rakssid, 2, Messages:msgNotOperateTip())
    end
    if self.standByTime >= GameConfig.standByTime then
        BaseListener.onPlayerLogout(self.userId)
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:onKill(dier)
    self.kills = self.kills + 1
    self.evenKills = self.evenKills + 1
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.score = self.score + 1
    self.appIntegral = self.appIntegral + 1
    local color = TextFormat.colorWrite
    if self.team ~= nil then
        self.team:onKill()
        color = self.team:getKillMsgColor()
    end
    local reward = KillRewardConfig:getReward(dier.evenKills)
    if reward ~= nil then
        self:addMoney(reward.baseMoney + reward.killMoney)
    end
    if self.evenKills > 1 then
        MsgSender.sendKillMsg(self.name, dier.name, self:getHeldItemId(), self.evenKills, self.headshot, color)
    end
end

function GamePlayer:filterInventory()
    if self.inventory == nil then
        return
    end
    if self.inventory.item ~= nil then
        local items = {}
        for _, c_item in pairs(self.inventory.item) do
            local setting = ItemSettingConfig:getItemSetting(c_item.id)
            if setting and setting.type == "4" then
                table.insert(items, c_item)
            end
        end
        self.inventory.item = items
    end
    if self.inventory.gun ~= nil then
        local guns = {}
        for _, c_item in pairs(self.inventory.gun) do
            local setting = ItemSettingConfig:getItemSetting(c_item.id)
            if setting and setting.type == "4" then
                table.insert(guns, c_item)
            end
        end
        self.inventory.gun = guns
    end
end

function GamePlayer:onMove(x, y, z)
    local hash = x .. ":" .. y .. ":" .. z
    if self.moveHash ~= hash then
        self.moveHash = hash
        self.standByTime = 0
    end
end

function GamePlayer:onDie(isEnd)
    self.isLife = false
    self.dies = self.dies + 1
    self.scorePoints[ScoreID.DIE] = self.scorePoints[ScoreID.DIE] + 1
    self.curDefense = 0
    self.maxDefense = 0
    self:updateDefense()
    self.inventory = InventoryUtil:getPlayerInventoryInfo(self)
    self:filterInventory()
    self:setSpeedAddition(0)
    local reward = KillRewardConfig:getReward(self.evenKills)
    self.evenKills = 0
    if reward ~= nil then
        self:addMoney(reward.dieMoney)
    end
    local respawn = RespawnConfig:randomRespawn(self.teamId)
    local waitTime = respawn.waitTime
    if isEnd then
        waitTime = 1
    end
    self:setRespawnPos(respawn.position)
    RespawnHelper.sendRespawnCountdown(self, waitTime)
    self.dieTick = GameConfig.showShopTick
    self.isShowShop = true
end

function GamePlayer:onGameEnd()
    self:clearInv()
    local money = math.min(self.kills, 50)
    self.hallMoney = self.hallMoney + money
    local items = InitItemsConfig:getGameEndItems()
    for _, item in pairs(items) do
        self:addItem(item.itemId, item.num, 0)
    end
    RankNpcConfig:savePlayerRankScore(self)
end

function GamePlayer:setWeekRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerWeekRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:setDayRank(id, rank, score)
    local npc = RankNpcConfig:getRankNpc(id)
    if npc then
        RankManager:addPlayerDayRank(self, npc.key, rank, score, function(npc)
            npc:sendRankData(self)
        end, npc)
    end
end

function GamePlayer:overGame()
    if self.team ~= nil then
        if self.team:isWin() then
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
        else
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
        end
    else
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

return GamePlayer

--endregion


