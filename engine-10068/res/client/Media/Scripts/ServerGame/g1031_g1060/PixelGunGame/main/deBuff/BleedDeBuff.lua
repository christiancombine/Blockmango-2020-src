require "deBuff.BaseDeBuff"

BleedDeBuff = class("BleedDeBuff", BaseDeBuff)

function BleedDeBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    local creator = self:getCreator()
    if not creator then
        return
    end
    if not player.isLife then
        self:onRemove()
        return
    end
    player.onSkillDamageUid = creator.userId
    player:subHealth(self.value)
    player.onSkillDamageUid = 0
end

return BleedDeBuff