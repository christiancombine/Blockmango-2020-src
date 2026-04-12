CageActorConfig = {}

--CageActorConfig.settings = {}

CageActorConfig.actors = {}

function CageActorConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.actorName = vConfig.s_actorName or "" --模型名称
        data.birthPos = StringUtil.split(vConfig.s_birthPos, ",", false, true) --出生点
        --table.insert(CageActorConfig.settings, data)

        local pos = VectorUtil.newVector3(data.birthPos[1], data.birthPos[2], data.birthPos[3])
        local entityId = EngineWorld:addCreatureNpc(pos, data.id, data.birthPos[4], data.actorName)
        table.insert(CageActorConfig.actors, entityId)
    end
end

function CageActorConfig:removeAllCageActor()
    for i = 1, #CageActorConfig.actors do
        local entityId = CageActorConfig.actors[i]
        EngineWorld:getWorld():killCreature(entityId)
        --EngineWorld:removeEntity(entityId)
    end
end

return CageActorConfig
