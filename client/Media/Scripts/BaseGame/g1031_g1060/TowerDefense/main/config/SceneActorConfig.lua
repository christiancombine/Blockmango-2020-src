SceneActorConfig = {}

function SceneActorConfig:init(configs)
    for _, vConfig in pairs(configs) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.actor = tostring(vConfig.actor) or ""
        data.pos = StringUtil.split(vConfig.pos, ",")
        data.yaw = tonumber(vConfig.yaw) or 0
        data.skill = tostring(vConfig.skill) or "#"
        self:createActor(data)
    end
end

function SceneActorConfig:createActor(config)
    local pos = VectorUtil.newVector3(config.pos[1], config.pos[2], config.pos[3])
    EngineWorld:addActorNpc(pos, config.yaw, config.actor,function (entity)
        if config.skill ~= "#" then
            entity:setSkillName(config.skill)
        end
        entity:setCanCollided(false)
    end)
end

SceneActorConfig:init(FileUtil.getConfigFromCsv("setting/SceneActor.csv"))