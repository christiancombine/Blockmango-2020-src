---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 12:35
---
GameSkill = class()

function GameSkill:ctor(config, player, initPos, itemEntityId)
    self.itemId = config.itemId
    self.length = config.length
    self.width = config.width
    self.height = config.height
    self.duration = config.duration
    self.damage = config.damage
    self.player = player
    self.initPos = initPos
    self.itemEntityId = itemEntityId
    self.area = {}
    self:initArea()
    self:doAction()
end

function GameSkill:initArea()
    local matrix = {}
    matrix[1] = {}
    matrix[1][1] = self.initPos.x - self.length
    matrix[1][2] = self.initPos.y - self.height
    matrix[1][3] = self.initPos.z - self.width
    matrix[2] = {}
    matrix[2][1] = self.initPos.x + self.length
    matrix[2][2] = self.initPos.y + self.height
    matrix[2][3] = self.initPos.z + self.width
    self.area = Area.new(matrix)
end

function GameSkill:doAction()
    if self.itemId == ItemID.ENDER_PEARL then
        SkillUtil:useEnderPearl(self.initPos, self.player)
    end
    if self.itemId == ItemID.GRENADE_TREMOR then
        SkillUtil:createExplosion(self.initPos, self.player, self.damage)
    end
    if self.itemId == ItemID.FROZEN_BALL then
        SkillUtil:useFrozenBall(self.area, self.player, self.duration * 1000)
    end

    if self.itemId == ItemID.FLARE_BOMB then
        SkillUtil:useFlareBomb(self.initPos, self.player, self.damage, self.duration, self.itemEntityId)
    end

    if self.itemId == ItemID.RESCUE then
        SkillUtil:useRescue(self.initPos, self.player, self.duration)
    end

    if self.itemId == ItemID.WitchPotion then
        SkillUtil:useWitchPotion(self.player, self.area, self.initPos)
    end

    if self.itemId == ItemID.BackHomeReel then
        SkillUtil:useBackHomeReel(self.player)
    end

    if self.itemId == ItemID.IronPuppet then
        if MonsterManager:createMonster(self.initPos, MonsterType.IronGolem, self.player) == false then
            MessagesManage:sendSystemTipsToPlayer(self.player.rakssid, Messages:msgSummonsNumLimit())
        else
            self.player:removeItem(ItemID.IronPuppet, 1)
        end
    end

    if self.itemId == ItemID.SnowBallInsect then
        SkillUtil:useSnowBall(self.initPos, self.player, self.damage, self.duration, self.area)
    end
    self.player.taskController:packingProgressData(Define.TaskType.Fright.UseProp, self.itemId, 1)
    self:onRemove()
end

function GameSkill:onRemove()
    self = {}
end

return GameSkill