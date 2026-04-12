require "messages.Messages"
require "manager.StoreHouseManager"
require "manager.ChestManager"
require "manager.TimerManager"
require "manager.PersonalStoreManager"
require "manager.DrawLuckyManager"
require "manager.TaskManager"
require "manager.NovicePackage"
require "manager.EggChestManager"
GameMatch = {}

GameMatch.gameType = "g1041"

GameMatch.curTick = 0
GameMatch.curSecondTime = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    StoreHouseManager:Init()
    PersonalStoreManager:Init()
    DrawLuckyManager:Init()
    TaskManager:Init()
    ChestManager:Init()
    NovicePackage:Init()
    EggChestManager:Init()
    self.curSecondTime = tonumber(os.date("%S", os.time()))
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    TimerManager.Update()
    self:recoverHp()
    self:generateMonster()
    self:generateTree()
    self:updateTreeHp()
    self:removeMonster()
    self:removeTree()
    self:addPlayerScore()
    SceneManager:onTick(ticks)
    self.curSecondTime = self.curSecondTime + 1
    self:updateADRefreshTime()
end

function GameMatch:onPlayerQuit(player)
    player:reward()
    player:onQuit()
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(10)
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick == 0 or self.curTick % 14400 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    RanchersRankManager:updateRankData()
    for i, v in pairs(players) do
        v:getPlayerRanks()
    end
end

function GameMatch:recoverHp()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInBattle == false
        and os.time() - v:getHpChangeTime() >= GameConfig.hpRecoverCD
        and v:getHealth() < v.maxHp then
            v:addHealth(GameConfig.hpRecover)
            v:setHpChangeTime(os.time())
        end
    end
end

function GameMatch:generateMonster()
    local players = PlayerManager:getPlayers()
    for _, v in pairs(players) do
        if v.enterFieldIndex ~= 0 then
            v:generateMonster()
        end
    end
end

function GameMatch:generateTree()
    if self.curTick == 0 or self.curTick % GameConfig.treeGenerateCD ~= 0 then
        return
    end

    SceneManager:addTreeInScene()
end

function GameMatch:removeMonster()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.userMonster ~= nil then
            for _, v in pairs(player.userMonster.monsters) do
                if v.isLostTarget == true then
                    player.userMonster:removeMonsterByEntityId(v.entityId)
                end
            end
        end
    end
end

function GameMatch:removeTree()
    for fieldId, tree in pairs(SceneManager.tree) do
        if tree.remainTime > 0 then
            tree.remainTime = tree.remainTime - 1
        end

        if tree.remainTime <= 0 then
            SceneManager:removeTree(fieldId)

            local fieldInfo = FieldConfig:getFieldInfo(fieldId)
            if fieldInfo == nil then
                return
            end

            local id = FieldConfig:getGenerateFieldId(fieldInfo.level - 1)
            if id == 0 then
                return
            end

            local treeId = TreeConfig:generateTreeId()
            if treeId == 0 then
                return
            end

            LuaTimer:scheduleTimer(SceneManager.callBackAddTreeInScene, GameConfig.treeRespawnCD * 1000, 1)
        end
    end
end

function GameMatch:updateTreeHp()
    if self.curTick % 2 ~= 0 then
        return
    end

    for fieldId, tree in pairs(SceneManager.tree) do
        if tree.changeHp == true then
            tree.changeHp = false
            local treeInfo = TreeConfig:getTreeInfo(tree.treeId)
            local fieldInfo = FieldConfig:getFieldInfo(fieldId)
            if treeInfo ~= nil and fieldInfo ~= nil then
                local players = PlayerManager:getPlayers()
                for _, player in pairs(players) do
                    local currentHp = Tools:formatNumberThousands(tree.currentHp)
                    EngineWorld:getWorld():updateSessionNpc(tree.entityId, player.rakssid, currentHp, treeInfo.actor, treeInfo.body, tree.bodyId, "", "", 0, false)
                end
            end
        end
    end
end

function GameMatch:updateADRefreshTime()
    if self.curSecondTime % 60 ~= 0 then
        return
    end

    self.curSecondTime = tonumber(os.date("%S", os.time()))
    local time = os.date("%H:%M", os.time())
    for _, v in pairs(GameConfig.vipChestADRefreshTime) do
        if v == time then
            local players = PlayerManager:getPlayers()
            for i, player in pairs(players) do
                VideoAdHelper:checkVipChestAd(player)
            end
        end
    end
end

return GameMatch