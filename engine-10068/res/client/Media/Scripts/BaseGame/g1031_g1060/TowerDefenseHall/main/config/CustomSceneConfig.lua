CustomSceneConfig = {}

CustomSceneConfig.Config = {}

CustomSceneConfig.Type = {
    Effect = 1,
    Image = 2,
    Actor = 3
}

function CustomSceneConfig:init(Areas)
    for _, area in pairs(Areas) do
        local item = {}
        item.Id = tonumber(area.Id) or 0
        item.type = tonumber(area.type) or 0
        local pos = StringUtil.split(area.pos or "", ",")
        item.pos = VectorUtil.newVector3(pos[1] or 0, pos[2] or 0, pos[3] or 0)
        item.param = tostring(area.param) or ""
        item.scale = tonumber(area.scale) or 0
        item.yaw = tonumber(area.yaw) or 0
        self.Config[item.Id] = item
    end
    self:createScene()
end

function CustomSceneConfig:createScene()
    for _, config in pairs(self.Config) do
        if config.type == self.Type.Effect then
            EngineWorld:addSimpleEffect(config.param, config.pos, config.yaw, -1, config.scale, config.scale, config.scale)
        elseif config.type == self.Type.Image then
            WorldImageRender.Instance():addStaticImage("CustomImage#" .. config.Id, "tower_defense_map.json", config.param, config.pos, config.scale, false, config.yaw)
        elseif config.type == self.Type.Actor then
            EngineWorld:addActorNpc(config.pos, config.yaw, config.param, function(entity)
                entity:setSkillName("idle")
                entity:setScale(config.scale)
                entity:setCanCollided(false)
            end)
        end
    end
end

CustomSceneConfig:init(FileUtil.getConfigFromCsv("setting/CustomScene.csv"))