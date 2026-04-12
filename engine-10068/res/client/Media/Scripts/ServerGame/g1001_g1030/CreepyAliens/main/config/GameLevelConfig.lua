---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 14:45
---
require "config.MonsterNumConfig"
require "config.MonsterConfig"
require "data.GameMonster"

GameLevelConfig = {}
GameLevelConfig.levels = {}

function GameLevelConfig:init(levels)
    self:initGameLevels(levels)
end

function GameLevelConfig:initGameLevels(levels)
    for lv, level in pairs(levels) do
        local item = {}
        item.level = tonumber(lv)
        item.kills = tonumber(level.kills)
        item.monsterLv = tonumber(level.monsterLv)
        item.monsterNumIds = StringUtil.split(level.monsterNumIds, "#")
        item.basementId = level.basementId
        item.gold = tonumber(level.gold)
        item.exp = tonumber(level.exp)
        self.levels[tonumber(lv)] = item
    end
end

function GameLevelConfig:getMaxLevel()
    return #self.levels
end

function GameLevelConfig:getLevel(level)
    local level = self.levels[level]
    if level then
        return level
    end
    return self.levels[1]
end

function GameLevelConfig:getKillsByLevel(level)
    local level = self.levels[level]
    if level then
        return level.kills
    end
    return 0
end

function GameLevelConfig:getMonstersByLevel(level)
    local monsters = {}
    local level = self.levels[level]
    local kills = 0
    if level then
        for i, monsterNumId in pairs(level.monsterNumIds) do
            local monster = {}
            monster.monsterNum = MonsterNumConfig:getMonsterNumById(monsterNumId)
            monster.generated = 0
            kills = kills + monster.monsterNum.num
            monsters[i] = monster
        end
        if kills < level.kills then
            level.kills = kills
        end
    end
    return monsters, kills
end

function GameLevelConfig:prepareMonster(monsters, num, monsterLv)
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local times = 10
    while num > 0 and times > 0 do
        local last = 0
        for i, monster in pairs(monsters) do
            if num <= 0 then
                break
            end
            local count = GameMatch:getCurGameScene():getMonsterCount()
            if count >= GameConfig.maxMonsters then
                break
            end
            if num < 1 then
                num = 1
            end
            num = NumberUtil.getIntPart(num)
            local random = math.random(1, num)
            local max = math.min(random, monster.monsterNum.num - monster.generated)
            if max > 0 then
                last = last + monster.monsterNum.num - monster.generated
                for c = 1, max do
                    local config = MonsterConfig:getMonsterById(monster.monsterNum.monsterId)
                    if config ~= nil then
                        num = num - 1
                        monster.generated = monster.generated + 1
                        GameMonster.new(config, monsterLv)
                    end
                end
            end
        end
        if last == 0 then
            break
        end
        times = times - 1
    end
end

return GameLevelConfig