BuffManager = {}
BuffManager.BuffType = {
    Attack = "1", --攻击力
    Bleed = "2", --流血
    Defense = "3", --防御力
    Drowning = "4", --溺水
    Fracture = "5", --骨折
    Healing = "6", --治疗
    Hungry = "7", --饥饿
    Infection = "8", --感染
    Satiety = "9", --饱腹
    Speed = "10", --速度
    Thirst = "11", --饥渴
    Poisoning = "12", --中毒
    Fire = "13", --燃烧
    AddWater = "14", --补水
}
BuffManager.BuffMapping = {
    [BuffManager.BuffType.Attack] = AttackBuff,
    [BuffManager.BuffType.Bleed] = BleedBuff,
    [BuffManager.BuffType.Defense] = DefenseBuff,
    [BuffManager.BuffType.Drowning] = DrowningBuff,
    [BuffManager.BuffType.Fracture] = FractureBuff,
    [BuffManager.BuffType.Healing] = HealingBuff,
    [BuffManager.BuffType.Hungry] = HungryBuff,
    [BuffManager.BuffType.Infection] = InfectionBuff,
    [BuffManager.BuffType.Satiety] = SatietyBuff,
    [BuffManager.BuffType.Speed] = SpeedBuff,
    [BuffManager.BuffType.Thirst] = ThirstBuff,
    [BuffManager.BuffType.Poisoning] = PoisoningBuff,
    [BuffManager.BuffType.Fire] = FireBuff,
    [BuffManager.BuffType.AddWater] = AddWaterBuff,
}

BuffManager.Buffs = {}

function BuffManager:createBuff(creator, target, buffId)
    local config = BuffConfig:getBuffById(buffId)
    local targetPlayer = PlayerManager:getPlayerByUserId(target)
    if config == nil or targetPlayer == nil or not targetPlayer.isLife then
        return false
    end
    local cache = self.Buffs[tostring(target) .. "#" .. config["id"]]
    local Buff = BuffManager.BuffMapping[config.type]
    if cache ~= nil then
        if config.overlayRule == 1 then
            return
        end
        if config.overlayRule == 2 then
            cache[tostring(target)]:addBuffTime(tostring(creator))
            return
        end
        if config.overlayRule == 3 then
            Buff.new(tostring(creator), tostring(target), config)
            return
        end
    end
    if Buff ~= nil then
        Buff.new(tostring(creator), tostring(target), config)
    end
end

function BuffManager:addBuff(Buff)
    local uniqueId = tostring(Buff.targetUid) .. "#" .. Buff.id
    Buff.uniqueId = uniqueId
    if self.Buffs[uniqueId] == nil then
        self.Buffs[uniqueId] = {}
    end
    if Buff.overlayRule == 1 then
        self.Buffs[uniqueId][Buff.creatorUid] = Buff
        local player = PlayerManager:getPlayerByUserId(Buff.targetUid)
        GamePacketSender:sendWalkingDeadBuffInfo(player.rakssid, Buff.type, Buff.duration - Buff.lifetime)
    end
    if Buff.overlayRule == 2 then
        self.Buffs[uniqueId][Buff.creatorUid] = Buff
        return
    end
    if Buff.overlayRule == 3 then
        self.Buffs[uniqueId][Buff.creatorUid] = Buff
        local player = PlayerManager:getPlayerByUserId(Buff.targetUid)
        GamePacketSender:sendWalkingDeadBuffInfo(player.rakssid, Buff.type, Buff.duration - Buff.lifetime)
        return
    end
end

function BuffManager:onTick(tick)
    for _, buffs in pairs(self.Buffs) do
        for _, buff in pairs(buffs) do
            buff:onTick(tick)
        end
    end
end

function BuffManager:cleanBuff(uniqueId)
    if self.Buffs[uniqueId] ~= nil then
        for _, buff in pairs(self.Buffs[uniqueId]) do
            buff:onRemove()
        end
    end
end

function BuffManager:removeBuff(creator, uniqueId, buffId)
    local buff = BuffConfig:getBuffById(tonumber(buffId))
    if buff == nil then
        return false
    end
    local overlayRule = (BuffConfig:getBuffById(buffId)).overlayRule
    if overlayRule ~= 3 then
        self.Buffs[uniqueId] = nil
        return
    end
    self.Buffs[uniqueId][tostring(creator)]:onDestroy()
    self.Buffs[uniqueId][tostring(creator)] = nil
end

function BuffManager:clearBuff()
    for i, Buff in pairs(self.Buffs) do
        Buff = nil
    end
    self.Buffs = {}
end

function BuffManager:getPlayerBuffData(userId)
    local BuffData = {}
    local allBuffId = BuffConfig:getAllBuffId()
    for _, id in pairs(allBuffId) do
        local cache = self.Buffs[tostring(userId) .. "#" .. tonumber(id)]
        if cache ~= nil then
            for _, buff in pairs(cache) do
                local data = buff:getBuffInfo()
                table.insert(BuffData, data)
            end
        end
    end
    return BuffData
end

function BuffManager:ReloadPlayerBuffData(data, userId)
    if data ~= nil then
        for _, buff in pairs(data) do
            local Buff = BuffManager.BuffMapping[buff.type]
            Buff:onReLoad(buff, tostring(userId))
        end
    end
end

function BuffManager:removePlayerAllBuff(userId)
    local allBuffId = BuffConfig:getAllBuffId()
    for _, id in pairs(allBuffId) do
        local cache = self.Buffs[tostring(userId) .. "#" .. id]
        if cache ~= nil then
            for _, buff in pairs(cache) do
                buff:onRemove()
            end
        end
    end
end

return BuffManager