EnchantMentNpcConfig = {}
EnchantMentNpcConfig.npc = {}

function EnchantMentNpcConfig:init(config)
    self:initNpc(config)
end

function EnchantMentNpcConfig:initNpc(config)
    for i, v in pairs(config) do
        local item = {}
        item.position = VectorUtil.newVector3(v.PosX, v.PosY, v.PosZ)
        item.yaw = tonumber(v.PosYaw) or 0
        item.actorName = tostring(v.ActorName) or ""
        item.name = tostring(v.Name) or ""
        item.type = tonumber(v.Type) or 0
        self.npc[i] = item
    end
end

function EnchantMentNpcConfig:prepareNpc()
    for i, v in pairs(self.npc) do
        if v.actorName ~= nil then
            local entityId = EngineWorld:addSessionNpc(v.position, v.yaw, v.type, v.actorName, function(entity)
                entity:setNameLang(v.name or "")
            end)
            v.entityId = entityId
        end
    end
end

function EnchantMentNpcConfig:addLightColumnEffect()
    for k, v in pairs(EnchantMentNpcConfig.npc) do
        local pos = VectorUtil.newVector3(v.position.x, v.position.y - 3, v.position.z)
        EngineWorld:addActorNpc(pos, v.yaw, "ranchers_door.actor", function(entity)
            entity:setCanCollided(false)
        end)
    end
end

return EnchantMentNpcConfig