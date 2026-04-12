SessionNpcConfig = {}
SessionNpcConfig.sessionNpc = {}
SessionNpcConfig.SessionType_TIP_NPC = 4
SessionNpcConfig.MULTI_TIP_NPC = 5
SessionNpcConfig.PERSONAL_SHOP = 6
SessionNpcConfig.STATUE_NPC = 7

function SessionNpcConfig:initNpc(npcs)
    for i, npc in pairs(npcs) do
        local data = {}
        data.type = tonumber(npc.type)
        data.name = tostring(npc.name)
        data.actor = tostring(npc.actor)
        data.position = VectorUtil.newVector3(npc.x, npc.y, npc.z)
        data.yaw = tonumber(npc.yaw)
        data.title = tostring(npc.title)
        data.message = StringUtil.split(npc.message, "#")
        self.sessionNpc[i] = data
    end
end

function SessionNpcConfig:prepareNpc()
    for _, v in pairs(self.sessionNpc) do
        if v.type == 1 then
            local entityId = EngineWorld:addSessionNpc(v.position, v.yaw, SessionNpcConfig.SessionType_TIP_NPC, v.actor, function(entity)
                entity:setNameLang(v.name or "")
                entity:setSessionContent(v.message[1] or "")
            end)
            v.entityId = entityId
        elseif v.type == 2 then
            local message = {}
            message.title = v.title
            message.tips = {}
            for i, v in pairs(v.message) do
                message.tips[i] = v
            end
            local entityId = EngineWorld:addSessionNpc(v.position, v.yaw, SessionNpcConfig.MULTI_TIP_NPC, v.actor, function(entity)
                entity:setNameLang(v.name or "")
                entity:setSessionContent(json.encode(message) or "")
            end)
            v.entityId = entityId
        end
    end
end

return SessionNpcConfig