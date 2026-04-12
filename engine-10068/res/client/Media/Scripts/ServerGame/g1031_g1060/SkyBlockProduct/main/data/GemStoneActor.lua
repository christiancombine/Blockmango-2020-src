---GemStoneActor
GemStoneActor = class()

function GemStoneActor:ctor(pos)
    self.radius = 2
    self.isAddItem = false
    self.pos = pos
    self:onCreate()
end

function GemStoneActor:onCreate()
    local entityId = EngineWorld:addActorNpc(self.pos, 0, "g1048_diamond.actor", function(entity)
        entity:setCanObstruct(false)
        entity:setCanCollided(true)
        entity:setHeadName("")
        entity:setSkillName("idle")
    end)
    self.entityId = entityId
end

function GemStoneActor:onDestroy()
    EngineWorld:removeEntity(self.entityId)
end

function GemStoneActor:onPlayerPickUp(player)
    if self.isAddItem or player == nil then
        return
    end
    local inv = player:getInventory()
    if inv and inv:getFirstEmptyStackInInventory() ~= -1 then
        self:onDestroy()
        self.isAddItem = true
        player:addItem(511, 1, 0)
        LuaTimer:scheduleTimer(function()
            player:checkTreasureInInvetory()
        end, 500, 1)
    end
end

function GemStoneActor:onPlayerMove(player, pos)
    if self:inArea(pos.x, pos.y, pos.z)  then
        self:onPlayerPickUp(player)
    end
    return self.isAddItem
end

function GemStoneActor:inArea(x, y, z)
    local pos = EngineWorld:getWorld():getPositionByEntityId(self.entityId)
    return math.pow((self.radius), 2) >= math.pow((x-pos.x), 2) + math.pow((z-pos.z), 2)
        and math.abs(y-pos.y) < 3
end

return GemStoneActor