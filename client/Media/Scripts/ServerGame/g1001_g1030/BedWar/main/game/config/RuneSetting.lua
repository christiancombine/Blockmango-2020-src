
require "game.rune.SwordRune"
require "game.rune.ArrowRune"
require "game.rune.KnockBackRune"
require "game.rune.SuckBloodRune"
require "game.rune.CritRune"
require "game.rune.SwordFireRune"
require "game.rune.MaxHpRune"
require "game.rune.AutoAddHpRune"
require "game.rune.ReduceHurtRune"
require "game.rune.MaxHpWithTimeRune"
require "game.rune.MaxHpWithBeginRune"
require "game.rune.OnKillHpRune"
require "game.rune.OnKillGoldRune"
require "game.rune.OnKillIronRune"
require "game.rune.OnKillWoolRune"
require "game.rune.OnDieCountDownRune"
require "game.rune.InitItemRune"
require "game.rune.RespawnItemRune"

RuneSetting = {}

RuneSetting.runeMap = {
    ["SwordRune"] = SwordRune,
    ["ArrowRune"] = ArrowRune,
    ["KnockBackRune"] = KnockBackRune,
    ["SuckBloodRune"] = SuckBloodRune,
    ["CritRune"] = CritRune,
    ["SwordFireRune"] = SwordFireRune,
    ["MaxHpRune"] = MaxHpRune,
    ["AutoAddHpRune"] = AutoAddHpRune,
    ["ReduceHurtRune"] = ReduceHurtRune,
    ["MaxHpWithTimeRune"] = MaxHpWithTimeRune,
    ["MaxHpWithBeginRune"] = MaxHpWithBeginRune,
    ["OnDieCountDownRune"] = OnDieCountDownRune,
    ["OnKillHpRune"] = OnKillHpRune,
    ["OnKillGoldRune"] = OnKillGoldRune,
    ["OnKillIronRune"] = OnKillIronRune,
    ["OnKillWoolRune"] = OnKillWoolRune,
    ["InitItemRune"] = InitItemRune,
    ["RespawnItemRune"] = RespawnItemRune
}
RuneSetting.runesConfig = {}

function RuneSetting:init(runeConfig)
    for _, config in pairs(runeConfig) do
        local rune = self.runeMap[config.Name].new(config)
        rune.id = tonumber(config.Id)
        rune.name = tostring(config.Name)
        self.runesConfig[tonumber(config.Id)] = rune
    end
end
return RuneSetting

