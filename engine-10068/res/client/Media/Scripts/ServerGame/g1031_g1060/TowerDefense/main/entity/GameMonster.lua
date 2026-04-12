GameMonster = class("GameMonster", BaseMonster)

function GameMonster:ctor(config, path, processId)
    self.entityId = 0
    self:onCreate(config, path, processId)
    ManagerPacketSender:sendMonsterCreate(self.entityId, config, path, processId)
    self.lastSynHp = config.Hp
end

function GameMonster:killed(attackerId)
    local player = PlayerManager:getPlayerByUserId(attackerId)
    if player then
        player:killMonster(self.goldReward)
    else
        self.path:equallyDistributedMoney(self.goldReward)
    end
    self.path:onMonsterDie(self)
end

function GameMonster:subHp(damage, attackerId, dieCallback)
    self.super.subHp(self, damage, attackerId, dieCallback)
    if true and self.lastSynHp - self.Hp > self.config.Hp * Define.SyncBloodByPercent then
        self.lastSynHp = self.Hp
        self:synHp()
    end
end

function GameMonster:synHp()
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if process then
        local builder = DataBuilder.new()
        builder:addParam("monsterId", self.entityId)
        builder:addParam("hp", self.Hp)
        process:broadCastPacket("SynHp", builder:getListData())
    end
end

function GameMonster:attackHome()
    ProcessManager:getProcessByProcessId(self.processId):onMissMonster()
    self.path:onHomeBeAttack(self)
end

function GameMonster:onDie()
    ManagerPacketSender:sendMonsterDead(self.entityId, self.processId)
    self.super.onDie(self)
end
