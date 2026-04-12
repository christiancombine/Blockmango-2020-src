GameMonster = class("GameMonster", BaseMonster)

function GameMonster:ctor(serverId, configId, pathId, processId)
    self.serverId = serverId
    self.entityId = 0
    local config = MonsterConfig:getConfigById(configId)
    local Path = MapManager:Instance():getPath(pathId)
    self:onCreate(config, Path, processId)
    self.actorSize = ActorSizeSetting.getActorSize(self.config.Actor)
    self:addImmunityBuffIcon()
    self.lockBloodLine = self.Hp - self.config.Hp * Define.SyncBloodByPercent
end

function GameMonster:addImmunityBuffIcon()
    local IconIds = self.config.IconIds
    local count = #IconIds
    local offset = (count - 1) * (0.4 * self.actorSize.scale)
    local curOffset = -((count - 1) / 2) * offset
    for _, buffId in pairs(IconIds) do
        local iconConfig = EntityIconConfig:getConfigById(buffId)
        if iconConfig then
            local builder = DataBuilder.new()
            builder:addParam("atlasName", iconConfig.AtlasName)
            builder:addParam("spriteName", iconConfig.SpriteName)
            local offsetY = (self.actorSize.height + 0.15) * self.actorSize.scale
            builder:addVector3Param("offset", VectorUtil.newVector3(curOffset, offsetY, curOffset))
            self.entity:addCustomImage(buffId, builder:getData(), -1, iconConfig.Scale)
            curOffset = curOffset + offset
        end
    end
end

function GameMonster:synHp(hp)
    local needSubHp = self.Hp - hp
    self.futureHp = hp
    self:subHp(needSubHp, nil, nil, true)
end

function GameMonster:subHp(damage, attackerId, dieCallback, isSync)
    if not isSync and (self.lockBloodLine > self.Hp - damage or self.Hp - damage <= 0) then
        return
    end
    self.super.subHp(self, damage, attackerId, dieCallback)
    if isSync then
        self.lockBloodLine = self.Hp - self.config.Hp * Define.SyncBloodByPercent
    end
end

function GameMonster:showFloatNumber(damage, color, scale)
    if damage == 0 or not GUIMain.openFloat then
        return
    end
    local pos = self:getPosition()
    pos.x = pos.x + math.random(-40, 40) / 80
    pos.y = pos.y + self.actorSize.height * self.actorSize.scale + 0.4
    pos.z = pos.z + math.random(-40, 40) / 80
    WorldImageRender.Instance():addFloatNumber(tostring(damage), pos, scale or 0.5, color or Define.FloatNumberColor.Red)
end

function GameMonster:onDie()
    LuaTimer:schedule(function()
        self.super.onDie(self)
    end, 100)
end
