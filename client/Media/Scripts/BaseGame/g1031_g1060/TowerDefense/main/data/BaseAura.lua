BaseAura = class("BaseAura")

function BaseAura:ctor(pos, auraId,config)
    self.pos = pos
    self.auraId = auraId
    self.config = config
    self.targetList = {}
    self.targetType = self.config.TargetType

    self.effectPosition = VectorUtil.newVector3(pos.x, pos.y + 0.1, pos.z)
    self.auraEffectName = nil

    self:onBuild()
end

function BaseAura:onBuild()
    --创建特效
    self:addAuraEffect()
    --根据配置决定是否要设置定时器自毁
    if self.config.duration ~= -1 then
        LuaTimer:schedule(function()
            self:onDestroy()
        end, self.config.duration * 1000)
    end
end

function BaseAura:addAuraEffect()
    if isClient then
        if self.config.AuraEffect then
            local effect = EngineWorld:addSimpleEffect(self.config.AuraEffect, self.effectPosition, 0, -1, self.config.AuraRange, 1, self.config.AuraRange)
            self.auraEffectName = effect.mEffectName
        end
    end
end

function BaseAura:removeAuraEffect()
    if isClient and self.auraEffectName then
        EngineWorld:removeSimpleEffect(self.auraEffectName)
    end
    self.auraEffectName = nil
end

function BaseAura:addGridToList(grid)
    table.insert(self.targetList, grid)
end

function BaseAura:onDestroy()
    --移除特效
    self:removeAuraEffect()
    --根据targetGridList, 从对应的格子的GridList中将自己移除
    for _, target in pairs(self.targetList) do
        target:removeAura(self)
    end
end

return BaseAura