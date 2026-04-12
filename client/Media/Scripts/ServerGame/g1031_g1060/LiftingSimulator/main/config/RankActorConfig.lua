RankActorConfig = {}

RankActorConfig.settings = {}

function RankActorConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.actorName = vConfig.s_actorName or "" --模型名称
        data.birthPos = StringUtil.split(vConfig.s_birthPos, ",", false, true) --出生点
        table.insert(RankActorConfig.settings, data)

        local pos = VectorUtil.newVector3(data.birthPos[1], data.birthPos[2], data.birthPos[3])
        EngineWorld:addActorNpc(pos, data.birthPos[4], data.actorName, function(entity)
            entity:setWeight("-1")
        end)
    end
end

return RankActorConfig
