require "data.GameJewel"

JewelConfig = {}
JewelConfig.jewels = {}
JewelConfig.generation = {}
JewelConfig.isResourceFull = false
JewelConfig.isDiscover = false

function JewelConfig:initJewel(config)
    for _, jewel in pairs(config) do
        local data = {}
        data.id = tonumber(jewel.id)
        data.actor = jewel.actor
        data.name = jewel.name
        data.itemId = tonumber(jewel.itemId)
        self.jewels[data.id] = data
    end
end

function JewelConfig:Generation()
    for _, jewel in pairs(self.jewels) do
        self.generation[jewel.id] = GameJewel.new(jewel, false)
        jewel.entityId = self.generation[jewel.id].entityId
        jewel.position = self.generation[jewel.id].position
    end
end

function JewelConfig:GenerateById(id)
    local jewel = JewelConfig:getJewelById(id)
    if jewel ~= nil then
        self.generation[id] = GameJewel.new(jewel, true)
        self.jewels[id].entityId = self.generation[id].entityId
        self.jewels[id].position = self.generation[id].position
        MsgSender.sendOtherTips(GameConfig.gameTime, Messages:jewelRecreate())
    else
        HostApi.log("==== LuaLog: GenerateById: jewel config not found.")
    end
end

function JewelConfig:getJewelByEntityId(entityId)
    for _, jewel in pairs(self.jewels) do
        if entityId == jewel.entityId then
            return jewel
        end
    end
    return nil
end

function JewelConfig:getJewelById(id)
    for _, jewel in pairs(self.jewels) do
        if id == jewel.id then
            return jewel
        end
    end
    return nil
end

function JewelConfig:showName()
    for _, jewel in pairs(self.generation) do
        if jewel ~= nil then
            EngineWorld:removeEntity(jewel.entityId)
            local entityId = EngineWorld:addActorNpc(jewel.position, 0, jewel.actor, function(entity)
                entity:setHeadName(jewel.name or "")
                entity:setCanObstruct(false)
            end)
            jewel.entityId = entityId
            self.jewels[jewel.id].entityId = jewel.entityId
        end
    end
end

return JewelConfig