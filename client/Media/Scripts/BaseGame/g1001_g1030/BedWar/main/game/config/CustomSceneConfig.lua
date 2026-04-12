CustomSceneConfig = {}

CustomSceneConfig.Config = {}

CustomSceneConfig.Type = {
    Effect = 1,
    Image = 2,
    Actor = 3
}

local JsonMap = {}

function CustomSceneConfig:init(configs)
    for _, config in pairs(configs) do
        if tonumber(GameConfig.mapId) == tonumber(config.mapId) then
            local item = {}
            item.Id = tonumber(config.Id) or 0
            item.type = tonumber(config.type) or 0
            local pos = StringUtil.split(config.pos or "", ",")
            item.pos = VectorUtil.newVector3(pos[1] or 0, pos[2] or 0, pos[3] or 0)
            item.param1 = tostring(config.param1) or ""
            item.param2 = tostring(config.param2) or ""
            item.scale = tonumber(config.scale) or 0
            item.yaw = tonumber(config.yaw) or 0
            self.Config[item.Id] = item
            if item.type == CustomSceneConfig.Type.Image then
                JsonMap[item.param1] = 1
            end
        end
    end
end

function CustomSceneConfig:gameReady()
    for jsonName, _ in pairs(JsonMap) do
        TextureAtlasRegister.Instance():addTextureAtlas(jsonName, "")
    end

    self:createScene()
end

function CustomSceneConfig:createScene()
    for _, config in pairs(self.Config) do
        if config.type == self.Type.Effect then
            EngineWorld:addSimpleEffect(config.param1, config.pos, config.yaw, -1, config.scale, config.scale, config.scale)
        elseif config.type == self.Type.Image then
            WorldImageRender.Instance():addStaticImage("CustomImage#" .. config.Id, config.param1, config.param2, config.pos, config.scale, false, config.yaw)
        elseif config.type == self.Type.Actor then
            EngineWorld:addActorNpc(config.pos, config.yaw, config.param1, function(entity)
                entity:setSkillName("idle")
                entity:setScale(config.scale)
                entity:setCanCollided(false)
            end)
        end
    end
end

CustomSceneConfig:init(FileUtil.getConfigFromCsv("setting/CustomScene.csv"), nil, true)