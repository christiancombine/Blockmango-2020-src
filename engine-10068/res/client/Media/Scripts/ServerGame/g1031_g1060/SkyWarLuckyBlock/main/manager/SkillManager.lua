require "skill.WaterBallSkill"
require "skill.DevilPactSkill"
require "skill.FateDiceSkill"
require "skill.BridgeEggSkill"
require "skill.GoldenKnightSkill"
require "skill.WitheredSkill"
require "skill.BackPearlSkill"
require "skill.FireBombSkill"
require "skill.EnderPearlSkill"
require "skill.HidingSkill"
require "skill.DropTNTSkill"
require "skill.DropArrowSkill"

SkillManager = {}
SkillManager.SkillType = {
    WaterBall = "1", --水球
    DevilPact = "2", --恶魔的契约
    FateDice = "5", --命运骰子
    BridgeEgg = "6", --搭桥蛋
    GoldenKnight = "7", --黄金骑士
    Withered = "9", --骑乘凋零
    BackPearl = "10", --回溯珍珠
    FireBomb = "11", --火焰弹
    EnderPearl = "12", --末影珍珠
    HidingSkill = "13", --隐身刀
    DropTNTSkill = "15", --掉落TNT
    DropArrowSkill = "16", --掉落弓箭
}
SkillManager.SkillMapping = {}
SkillManager.SkillMapping[SkillManager.SkillType.WaterBall] = WaterBallSkill
SkillManager.SkillMapping[SkillManager.SkillType.DevilPact] = DevilPactSkill
SkillManager.SkillMapping[SkillManager.SkillType.FateDice] = FateDiceSkill
SkillManager.SkillMapping[SkillManager.SkillType.BridgeEgg] = BridgeEggSkill
SkillManager.SkillMapping[SkillManager.SkillType.GoldenKnight] = GoldenKnightSkill
SkillManager.SkillMapping[SkillManager.SkillType.Withered] = WitheredSkill
SkillManager.SkillMapping[SkillManager.SkillType.BackPearl] = BackPearlSkill
SkillManager.SkillMapping[SkillManager.SkillType.FireBomb] = FireBombSkill
SkillManager.SkillMapping[SkillManager.SkillType.EnderPearl] = EnderPearlSkill
SkillManager.SkillMapping[SkillManager.SkillType.HidingSkill] = HidingSkill
SkillManager.SkillMapping[SkillManager.SkillType.DropTNTSkill] = DropTNTSkill
SkillManager.SkillMapping[SkillManager.SkillType.DropArrowSkill] = DropArrowSkill

SkillManager.HashCode = 1
SkillManager.Skills = {}

function SkillManager:createSkill(config, player, position)
    if config.type == SkillManager.SkillType.DevilPact and player.signDevilPack == false then
        HostApi.sendCustomTip(player.rakssid, "if_sign_devil_pact_txt", "UseDevilPact")
        player.devilPactInfo = {
            config = config,
            position = position
        }
        return
    end
    local skill = SkillManager.SkillMapping[config.type]
    if skill ~= nil then
        skill.new(config, player, position)
    else
        print("skill is nil, type: " .. skill.type)
    end
end

function SkillManager:addSkill(skill)
    local uniqueId = tostring(self.HashCode)
    skill.uniqueId = uniqueId
    self.Skills[uniqueId] = skill
    self.HashCode = self.HashCode + 1
end

function SkillManager:onTick(tick)
    for uniqueId, skill in pairs(self.Skills) do
        skill:onTick(tick)
    end
end

function SkillManager:removeSkill(skill)
    if skill.uniqueId then
        self.Skills[skill.uniqueId] = nil
    end
end

function SkillManager:clearSkill()
    for i, skill in pairs(self.Skills) do
        skill = nil
    end
    self.Skills = {}
    self.HashCode = 1
end

function SkillManager:checkTopAreaIsAir(position)
    local first = VectorUtil.newVector3i(position.x, position.y + 1, position.z)
    local second = VectorUtil.newVector3i(position.x, position.y + 2, position.z)
    if EngineWorld:getBlockId(first) == BlockID.AIR and
            EngineWorld:getBlockId(second) == BlockID.AIR then
        return true, first
    end
    return false, first
end

return SkillManager