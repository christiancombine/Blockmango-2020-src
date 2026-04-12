---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:10
---
require "config.ProduceConfig"
require "config.InventoryConfig"

Scene = class()

function Scene:ctor(config)
    self.scenePos = {}
    self.zombiePos = {}
    self.soldierPos = {}
    self.zombieCount = {}
    self.gameTime = tonumber(config.gameTime)
    self.produceConfig = {}
    self.zombieConfig = {}
    self.soldierConfig = {}
    self.inventoryConfig = {}
    self:init(config)
end

function Scene:init(config)
    self:initPos(config)
    self:initProduceConfig(config.produce)
    self:initInventoryConfig(config.inventory)
    self:initZombieCount(config.zombieCount)
    self:initSoldierConfig(config.soldier)
    self:initZombieConfig(config.zombie)

end

function Scene:initPos(config)
    self.scenePos = VectorUtil.newVector3(config.scenePos[1], config.scenePos[2], config.scenePos[3])
    self:initZombiePos(config)
    self:initSoldierPos(config)
end

function Scene:initZombiePos(config)
    if config.zombiePos ~= nil then
        for i, v in pairs(config.zombiePos) do
            self.zombiePos[i] = {}
            self.zombiePos[i].x = v.x
            self.zombiePos[i].y = v.y
            self.zombiePos[i].z = v.z
        end
    end
end

function Scene:getZombiePos()
    if self.zombiePos ~= nil then
        if #self.zombiePos > 0 then
            local random = math.random(1, #self.zombiePos)
            for i, v in pairs(self.zombiePos) do
                if i == random then
                    return v
                end
            end
        end
    end
    return nil
end

function Scene:initSoldierPos(config)
    if config.soldierPos ~= nil then
        for i, v in pairs(config.soldierPos) do
            self.soldierPos[i] = {}
            self.soldierPos[i].x = v.x
            self.soldierPos[i].y = v.y
            self.soldierPos[i].z = v.z
        end
    end
end

function Scene:getSoldierPos()
    if self.soldierPos ~= nil then
        if #self.soldierPos > 0 then
            local random = math.random(1, #self.soldierPos)
            for i, v in pairs(self.soldierPos) do
                if i == random then
                    return v
                end
            end
        end
    end
    return nil
end

function Scene:initProduceConfig(produce)
    self.produceConfig = ProduceConfig.new(produce)
end

function Scene:initInventoryConfig(inventory)
    self.inventoryConfig = InventoryConfig.new(inventory)
end

function Scene:initZombieConfig(zombie)
    for i, v in pairs(zombie) do
        local z = {}
        z.id = tonumber(v.id)
        z.model = v.model
        z.hp = tonumber(v.hp)
        z.bornOdds = tonumber(v.bornOdds)
        z.convertOdds = tonumber(v.convertOdds)
        z.damage = tonumber(v.damage)
        z.poisonTime = tonumber(v.poisonTime)
        z.skillType = tonumber(v.skillType)
        z.skillCDTime = tonumber(v.skillCDTime)
        z.skillDurationTime = tonumber(v.skillDurationTime)
        z.defenseCoefficient = tonumber(v.defenseCoefficient or "1")
        z.knockBackCoefficient = tonumber(v.knockBackCoefficient or "1")
        z.skillSpeed = tonumber(v.skillSpeed or "1")
        z.skillPoisonRange = tonumber(v.skillPoisonRange or "0")
        z.skillPoisonDamage = tonumber(v.skillPoisonDamage or "0")
        z.skillPoisonOnTime = tonumber(v.skillPoisonOnTime or "0")
        z.skillPoisonDurationTime = tonumber(v.skillPoisonDurationTime or "0")
        z.skillPoisonColor ={}
        z.skillPoisonColor.b = tonumber(v.skillPoisonColor[1] or "0")
        z.skillPoisonColor.g = tonumber(v.skillPoisonColor[2] or "0")
        z.skillPoisonColor.r = tonumber(v.skillPoisonColor[3] or "0")

        z.buff = {}
        for j, vv in pairs(v.buff) do
            if vv.id ~= 0 then
                local buff = {}
                buff.id = tonumber(vv.id)
                buff.lv = tonumber(vv.lv)
                z.buff[j] = buff
            end
        end
        --站立回血
        z.standRecHp = {}
        z.standRecHp.standTime = tonumber(v.standRecHp.standTime)
        z.standRecHp.hp = tonumber(v.standRecHp.hp)
        self.zombieConfig[z.id] = z
    end
end

function Scene:initSoldierConfig(soldier)
    self.soldierConfig.inventory = {}
    for i, v in pairs(soldier.inventory) do
        local itemCf = {}
        itemCf.id = tonumber(v.id)
        itemCf.num = tonumber(v.num)
        if v.fm then
            itemCf.fm = {}
            itemCf.fm.id = tonumber(v.fm.id)
            itemCf.fm.lv = tonumber(v.fm.lv)
        end
        self.soldierConfig.inventory[i] = itemCf
    end
    self.soldierConfig.hp = tonumber(soldier.hp)
end

function Scene:initZombieCount(zombieCount)
    for i, v in pairs(zombieCount) do
        self.zombieCount[i] = v
    end
end

function Scene:getZombieCount(number)
    local count = self.zombieCount[number]
    if count then
        return count
    else
        return (number - number % 8) + 1
    end
end

function Scene:getBornRandomZombie()
    local zombie
    local chance = math.random(1, 100)
    local zbOdds = 0
    for i, v in pairs(self.zombieConfig) do
        if chance >= zbOdds and chance <= zbOdds + v.bornOdds then
            zombie = v
            break
        end
        zbOdds = zbOdds + v.bornOdds
    end
    return zombie
end

function Scene:getConvertRandomZombie()
    local zombie
    local chance = math.random(1, 100)
    local zbOdds = 0
    for i, v in pairs(self.zombieConfig) do
        if chance >= zbOdds and chance <= zbOdds + v.convertOdds then
            zombie = v
            break
        end
        zbOdds = zbOdds + v.convertOdds
    end
    return zombie
end

function Scene:getZbStandRecConfig(zombieId)
    for i, v in pairs(self.zombieConfig) do
        if v.id == zombieId then
            return v.standRecHp
        end
    end
    return nil
end

function Scene:getZombieDamage(zombieId)
    for i, v in pairs(self.zombieConfig) do
        if v.id == zombieId then
            return v.damage
        end
    end
    return 0
end

function Scene:getZombieByZombieId(zombieId)
    for i, v in pairs(self.zombieConfig) do
        if v.id == zombieId then
            return v
        end
    end
    return nil
end

return Scene