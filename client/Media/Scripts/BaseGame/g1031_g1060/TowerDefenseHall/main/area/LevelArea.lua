LevelArea = class("LevelArea", BaseArea)

function LevelArea:init()
    self.levelConfig = LevelConfig:getConfigById(self.config.configId)
end

function LevelArea:checkPlayerLevel(player)
    if player.level <  self.levelConfig.levelLimit then
        self.isLocked = true
        self:openAirWall(true)
        if  self.notAllowEnterEffect == nil then
            self.notAllowEnterEffect = EngineWorld:addSimpleEffect(self.levelConfig.notEnter, self.config.centrePos, self.config.pYaw, -1, tonumber(self.config.scale[1]), tonumber(self.config.scale[2]), tonumber(self.config.scale[3]))
        end

        local LvPos = VectorUtil.newVector3(self.config.pos2.x,self.config.pos2.y,self.config.pos2.z)
        LvPos.y = LvPos.y + self.config.scale2 * 0.15
        WorldImageRender.Instance():addStaticImage("LevelText" .. self.config.Id, "tower_defense_world_sprite.json", "level", LvPos, 0.85 * self.config.scale2, false, self.config.pYaw)
        WorldImageRender.Instance():addStaticNumber("LevelNum" .. self.config.Id, self.levelConfig.levelLimit, self.config.pos3, -1, self.config.scale3, 6, self.config.pYaw)

    else
        self.isLocked = false
        self:openAirWall(false)
        if self.notAllowEnterEffect ~= nil then
            EngineWorld:removeSimpleEffect(self.notAllowEnterEffect.mEffectName)
            self.notAllowEnterEffect = nil
        end
        WorldImageRender.Instance():removeStaticImage("LevelText" .. self.config.Id)
        WorldImageRender.Instance():removeStaticNumber("LevelNum" .. self.config.Id)

        self.notAllowEnterEffect = EngineWorld:addSimpleEffect(self.levelConfig.allowEnter, self.config.centrePos, self.config.pYaw, -1, tonumber(self.config.scale[1]), tonumber(self.config.scale[2]), tonumber(self.config.scale[3]))
    end
end


