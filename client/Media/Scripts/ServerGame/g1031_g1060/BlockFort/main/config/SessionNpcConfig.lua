SessionNpcConfig = {}
SessionNpcConfig.sessionNpc = {}
SessionNpcConfig.MULTI_TIP_INTERACTION_NPC = 9

function SessionNpcConfig:initNpc(config)
    for i, npc in pairs(config) do
        local data = {}
        data.id = tonumber(npc.id)
        data.name = tostring(npc.name)
        data.actor = tostring(npc.actor)
        data.position = VectorUtil.newVector3(npc.x, npc.y, npc.z)
        data.yaw = tonumber(npc.yaw)
        data.title = tostring(npc.title)
        data.sureContent = tostring(npc.sureContent)
        data.message = StringUtil.split(npc.message, "#")
        self.sessionNpc[data.id] = data
    end
end

function SessionNpcConfig:prepareNpc()
    for _, v in pairs(self.sessionNpc) do
        local message = {}
        message.title = v.title
        message.tips = {}
        message.interactionType = v.interactionType
        message.sureContent = v.sureContent
        for i, m in pairs(v.message) do
            message.tips[i] = m
        end
        local entityId = EngineWorld:addSessionNpc(v.position, v.yaw, SessionNpcConfig.MULTI_TIP_INTERACTION_NPC, v.actor, function(entity)
            entity:setNameLang(v.name or "")
            entity:setSessionContent(json.encode(message) or "")
        end)
        v.entityId = entityId
    end
end

return SessionNpcConfig