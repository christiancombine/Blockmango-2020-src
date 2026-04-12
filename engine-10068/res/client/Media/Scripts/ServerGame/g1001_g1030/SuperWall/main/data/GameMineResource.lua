---
--- Created by Jimmy.
--- DateTime: 2018/8/24 0024 10:41
---
GameMineResource = class()

function GameMineResource:ctor(config)
    self.blockId = config.blockId
    self.itemId = config.itemId
    self.num = config.num
    self.damage = config.damage
    self.score = config.score
    self.config = config
    self:onCreate()
    self:initArea()
    self:addMineHaloEffect()
end

function GameMineResource:onCreate()
    GameManager:onAddMineResource(self)
end

function GameMineResource:initArea()
    self.minX = math.min(self.config.startPos.x, self.config.endPos.x)
    self.minY = math.min(self.config.startPos.y, self.config.endPos.y)
    self.minZ = math.min(self.config.startPos.z, self.config.endPos.z)
    self.maxX = math.max(self.config.startPos.x, self.config.endPos.x)
    self.maxY = math.max(self.config.startPos.y, self.config.endPos.y)
    self.maxZ = math.max(self.config.startPos.z, self.config.endPos.z)
end

function GameMineResource:addMineHaloEffect()
    if self.config.effect == "#" then
        return
    end
    local position = VectorUtil.newVector3((self.minX + self.maxX) / 2, (self.minY + self.maxY) / 2, (self.minZ + self.maxZ) / 2)
    EngineWorld:addSimpleEffect(self.config.effect, position, self.config.yaw)
end

function GameMineResource:inArea(vec3)
    return vec3.x >= self.minX and vec3.x <= self.maxX
    and vec3.y >= self.minY and vec3.y <= self.maxY
    and vec3.z >= self.minZ and vec3.z <= self.maxZ
end

function GameMineResource:breakBlock(player, blockPos, blockId)
    if self:inArea(blockPos) == false then
        return
    end
    if self.blockId ~= blockId then
        return
    end
    local moneyPercent = ToolAttrConfig:getMoneyPercent(player:getHeldItemId())
    if self.itemId == 10000 then
        player:addMoney(math.floor(self.num * moneyPercent))
    else
        player:addItem(self.itemId, math.floor(self.num * moneyPercent), 0)
    end
    local digScore = ToolAttrConfig:getDigScore(player:getHeldItemId())
    player.resourceScore = player.resourceScore + self.score * digScore
    player:onBreakBlock(self.damage)
end

return GameMineResource