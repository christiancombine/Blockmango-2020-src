MonsterManager = {}
MonsterManager.monsters = {}
MonsterManager.countDown = {}

function MonsterManager:onTick()
    self:generateMonster()
end

function MonsterManager:generateMonster(vec3)

    for key, monsterGenerateConfig in pairs(MonsterGenerateConfig.monsters) do
        local monsterInfo = MonsterConfig:getMonsterInfoById(monsterGenerateConfig.mosterId)

        if monsterInfo then
            local pos = monsterGenerateConfig.monsterPos

            local sign = pos[1] .. ":" .. pos[2] .. ":" .. pos[3]
            local monster = self:getMonsterBySign(sign)
            if monster == nil then
                local monsterPos = VectorUtil.newVector3(pos[1], pos[2], pos[3])
                local countDown = self:getCountDown(sign)
                if countDown == nil then
                    self:addMonsterToWorld(sign, monsterGenerateConfig.mosterId, monsterPos, monsterInfo)
                else
                    local cd = monsterGenerateConfig.refreshCD
                    if os.time() - countDown.time >= cd then

                        self:addMonsterToWorld(sign, monsterGenerateConfig.mosterId, monsterPos, monsterInfo)
                    end
                end
            end
        end

    end

end

function MonsterManager:addMonsterToWorld(sign, monsterId, monsterPos, monsterInfo)
    local entityId = EngineWorld:addCreatureNpc(monsterPos, monsterId, 0, monsterInfo.actorName, function(entity)
        entity:setNameLang(monsterInfo.name or "")
    end)
    local homePos = VectorUtil.newVector3i(monsterPos.x, monsterPos.y, monsterPos.z)
    EngineWorld:getWorld():setCreatureHome(entityId, homePos)
    self:addMonster(sign, entityId, monsterId, monsterInfo)
    EngineWorld:getWorld():setEntityHealth(entityId, tonumber(monsterInfo.hp), tonumber(monsterInfo.hp))
end

function MonsterManager:addMonster(sign, entityId, monsterId, monsterInfo)
    if entityId ~= 0 then
        self.monsters[sign] = {}
        self.monsters[sign].sign = sign
        self.monsters[sign].entityId = tonumber(entityId)
        self.monsters[sign].monsterId = tonumber(monsterId)
        self.monsters[sign].genre = tonumber(monsterInfo.genre)
        self.monsters[sign].hp = tonumber(monsterInfo.hp)
        self.monsters[sign].maxHp = tonumber(monsterInfo.hp)
        self.monsters[sign].attack = tonumber(monsterInfo.attack)
        self.monsters[sign].attackCD = tonumber(monsterInfo.attackCD)
        self.monsters[sign].moveSpeed = tonumber(monsterInfo.moveSpeed)
        self.monsters[sign].defence = tonumber(monsterInfo.defence)
    end
end

function MonsterManager:getMonsterBySign(sign)
    if self.monsters[sign] ~= nil then
        return self.monsters[sign]
    end

    return nil
end

function MonsterManager:getMonsterByEntityId(entityId)
    for i, v in pairs(self.monsters) do
        if v.entityId == entityId then
            return v
        end
    end

    return nil
end

function MonsterManager:onPlayerAttackCreature(player, entityId, vec3, damage, currentItemId)
    if currentItemId == ItemID.STAR_GUN then
        -- HostApi.log("onPlayerAttackCreature star gun")
        return
    end
    for key, monster in pairs(self.monsters) do
        if monster.entityId == entityId then
            HostApi.log("onPlayerAttackCreature " .. monster.hp)

            local attacker_attack = damage
            local defencer_defence = monster.defence
            local hurtValue = attacker_attack * (1 - (defencer_defence * 0.04))

            HostApi.log("onPlayerAttackCreature " .. attacker_attack .. " " .. defencer_defence .. " " .. hurtValue)

            monster.hp = monster.hp - hurtValue
            if monster.hp <= 0 then
                monster.hp = 0
                player.taskController:packingProgressData(player, Define.TaskType.kill_monster, GameMatch.gameType, monster.genre, 1, 0)
                self:setMonsterDead(entityId, vec3, player)
            end
            EngineWorld:getWorld():setEntityHealth(entityId, tonumber(monster.hp), tonumber(monster.maxHp))
        end
    end
end

function MonsterManager:setMonsterDead(entityId, vec3, player)
    local monster = self:getMonsterByEntityId(entityId)
    if monster ~= nil then
        self.monsters[monster.sign] = nil
        EngineWorld:getWorld():killCreature(entityId)
        self:setCountDown(monster.sign, os.time())
        local monsterInfo = MonsterConfig:getMonsterInfoById(monster.monsterId)
        if monsterInfo ~= nil then
            local radius = monsterInfo.dropRadius or 1
            for _, v in pairs(monsterInfo.rewardItems) do
                local items = StringUtil.split(v, ":")
                if items[1] ~= nil and items[2] ~= nil and items[3] ~= nil and items[4] ~= nil then
                    local itemId = tonumber(items[1])
                    local itemMeta = tonumber(items[2])
                    local num = tonumber(items[3])
                    local weight = tonumber(items[4])
                    GameAnalytics:design(player.userId, 0, { "g1049PlayerKillMonster", tostring(player.userId), monster.monsterId })
                    GameAnalytics:design(player.userId, 0, { "g1049MonsterDied", monster.monsterId, tostring(player.userId) })
                    if itemId > 0 and num > 0 then
                        local randNum = HostApi.random("rewardItems", 1, 100)
                        if randNum <= weight then
                            local motion = VectorUtil.newVector3(0, 0, 0)
                            local pos = GameConfig:getRandomPosAZ(vec3, radius)
                            --HostApi.log("setMonsterDead getRandomPosAZ x : "..pos.x.." y : "..pos.y.." z : "..pos.z.." ItemID : "..itemId)
                            EngineWorld:addEntityItem(itemId, num, itemMeta, 20, pos, motion, false, false)
                        end
                    end
                end
            end
        end
    end
end

function MonsterManager:attackPlayer(entityId, player)
    for i, v in pairs(self.monsters) do
        if v.entityId == entityId then
            local attacker_attack = v.attack
            local defencer_defence = player:getTotalArmorValue()
            local hurtValue = attacker_attack * (1 - (defencer_defence * 0.04))

            HostApi.log("attackPlayer " .. attacker_attack .. " " .. defencer_defence .. " " .. hurtValue)

            player:subHealth(hurtValue)
        end
    end
end

function MonsterManager:getCountDown(sign)
    if self.countDown[sign] ~= nil then
        return self.countDown[sign]
    end

    return nil
end

function MonsterManager:setCountDown(sign, time)
    if self.countDown[sign] == nil then
        self.countDown[sign] = {}
    end

    self.countDown[sign].time = time
end