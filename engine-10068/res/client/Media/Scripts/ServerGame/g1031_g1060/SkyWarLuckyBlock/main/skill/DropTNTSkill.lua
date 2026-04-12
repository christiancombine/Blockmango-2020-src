require "skill.BaseSkill"

DropTNTSkill = class("DropTNTSkill", BaseSkill)

function DropTNTSkill:createSkillEffect(name, time)
    local creator = self:getCreator()
    if not creator then
        return
    end
    local pos = VectorUtil.toBlockVector3(self.initPos.x, self.initPos.y - 2, self.initPos.z)
    if EngineWorld:getBlockId(pos) ~= BlockID.AIR then
        pos.y = pos.y + 1
    end
    EngineWorld:setBlock(pos, BlockID.TNT)
    EngineWorld:getWorld():fireTNT(pos, creator:getEntityId())
end

return DropTNTSkill