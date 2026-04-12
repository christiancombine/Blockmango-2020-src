WorldInfo = {}

WorldInfo.playerTeam = {}
WorldInfo.AITeam = {}
WorldInfo.GameStage = 1
WorldInfo.playerTeamNum = 0
WorldInfo.AITeamNum = 0
WorldInfo.WaitDeadTeam = nil
WorldInfo.AIKiller = nil

function WorldInfo:init()
    for _, team in pairs(GameMatch.Teams) do
        if #AIManager:getAIsByTeamId(team.id) ~= 0 then
            table.insert(self.AITeam, team)
        else
            table.insert(self.playerTeam, team)
        end
    end

    self.playerTeamNum = #self.playerTeam
    self.AITeamNum = #self.AITeam

    self:initPublicResourcePoint()
    self:updateTeamLifeInfo()
    self:initAITeamTarget()
    self:initGameType()
end

function WorldInfo:onTick(ticks)
    self:checkAITeamNum(ticks)
    --所有玩家的信息
    local players = PlayerManager:getPlayers()

    for teamId, team in pairs(GameMatch.Teams) do
        local dangerEnemy
        for _, player in pairs(players) do
            if player.lifeStatus == true and player.entityPlayerMP ~= nil and player.entityPlayerMP:isPotionActive(14) == false and player.teamId ~= teamId then
                local distance = BtUtil.calculateDistance(team.bedPos[1], player:getPosition())
                if dangerEnemy == nil then
                    dangerEnemy = { userId = player.userId, dis = distance }
                elseif distance < dangerEnemy.dis then
                    dangerEnemy = { userId = player.userId, dis = distance }
                end
            end
        end

        BlackBoard:setValue(tostring(teamId) .. "dangerEnemy", dangerEnemy)

        for _, AI in pairs(AIManager:getAIsByTeamId(teamId)) do
            if AI.isLife == true and dangerEnemy and dangerEnemy.dis < AI.aiConfig.activityRange and (not AIManager:isAIByUserId(dangerEnemy.userId) or not AI.WaitDead) then
                AI.isActivity = true
                if dangerEnemy.dis < AI.aiConfig.followRange then
                    AI.needDefense = true
                else
                    AI.needDefense = false
                end
            else
                AI.isActivity = false
                AI.needDefense = false
            end
        end
    end
end

function WorldInfo:updateTeamLifeInfo()
    if self.AITeamNum >= 1 then
        for index = #self.AITeam, 1, -1 do
            if self.AITeam[index]:isDeath() == true then
                self.AITeamNum = self.AITeamNum - 1
                if self.AIKillerTarget == self.AITeam[index].id then
                    self.AIKillerTarget = nil
                end
                table.remove(self.AITeam, index)
                self:initAITeamTarget()
            end
        end
    end

    if self.playerTeamNum >= 1 then
        for index = #self.playerTeam, 1, -1 do
            if self.playerTeam[index]:isDeath() == true then
                self.playerTeamNum = self.playerTeamNum - 1
                table.remove(self.playerTeam, index)
                self:initAITeamTarget()
            end
        end
    end
end

function WorldInfo:initAITeamTarget()
    if self.AITeamNum == 0 then
        return
    end

    if self.AITeamNum == 3 and self.playerTeamNum == 1 then
        local furthest = 0
        local attackPlayerAI
        ---AI互相攻击
        for index, team in pairs(self.AITeam) do
            local teamId = index + 1
            if teamId == 4 then
                teamId = 1
            end
            for _, AI in pairs(AIManager:getAIsByTeamId(team.id)) do
                AI.targetTeamId = self.AITeam[teamId].id
            end
        end
        ---选出最近的AI去攻击玩家
        for _, team in pairs(self.AITeam) do
            local distance = BtUtil.calculateDistance(team.bedPos[1], self.playerTeam[1].bedPos[1])
            if furthest == 0 or distance < furthest then
                furthest = distance
                attackPlayerAI = team
            end
        end
        for _, AI in pairs(AIManager:getAIsByTeamId(attackPlayerAI.id)) do
            AI.targetTeamId = self.playerTeam[1].id
        end
    end

    if self.AITeamNum == 2 and self.playerTeamNum == 2 then
        ---两队AI时，AI都去进攻离自己最近的玩家
        if BtUtil.calculateDistance(self.AITeam[1].bedPos[1], self.playerTeam[1].bedPos[1]) < BtUtil.calculateDistance(self.AITeam[1].bedPos[1], self.playerTeam[2].bedPos[1]) then
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[1].id)) do
                AI.targetTeamId = self.playerTeam[1].id
            end
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[2].id)) do
                AI.targetTeamId = self.playerTeam[2].id
            end
        else
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[1].id)) do
                AI.targetTeamId = self.playerTeam[2].id
            end
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[2].id)) do
                AI.targetTeamId = self.playerTeam[1].id
            end
        end
    end

    if self.AITeamNum == 2 and self.playerTeamNum == 1 then
        ---两队AI时，1队玩家时
        for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[1].id)) do
            AI.targetTeamId = self.playerTeam[1].id
        end

        if math.random(1, 2) == 1 then
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[2].id)) do
                AI.targetTeamId = self.playerTeam[1].id
            end
        else
            for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[2].id)) do
                AI.targetTeamId = self.AITeam[1].id
            end
        end
    end

    if self.AITeamNum == 1 and self.playerTeamNum >= 1 then
        local randomNum = math.random(1, self.playerTeamNum)
        for _, AI in pairs(AIManager:getAIsByTeamId(self.AITeam[1].id)) do
            AI.targetTeamId = self.playerTeam[randomNum].id
        end
    end

    if self.playerTeamNum == 0 and self.AITeamNum > 1 then
        for index, team in pairs(self.AITeam) do
            local teamId = index + 1
            if teamId == self.AITeamNum + 1 then
                teamId = 1
            end
            for _, AI in pairs(AIManager:getAIsByTeamId(team.id)) do
                AI.targetTeamId = self.AITeam[teamId].id
            end
        end
    end
end

function WorldInfo:checkAITeamNum(ticks)
    self:updateTeamLifeInfo()
    if self.WaitDeadTeam ~= nil then
    end
    local totalTeamNum = AITeamConfig:getConfig(ticks)
    if self.playerTeamNum + self.AITeamNum > totalTeamNum and self.AITeamNum >= 2 and (self.WaitDeadTeam == nil or self.WaitDeadTeam:isDeath() == true) then
        local maxDeadWeight = 0
        local minDeadWeight = 10000
        for _, team in pairs(self.AITeam) do
            local deadWeight = AIDeadConfig:getConfig(team.id)
            if deadWeight > maxDeadWeight then
                maxDeadWeight = deadWeight
                self.WaitDeadTeam = team
            end
            if deadWeight < minDeadWeight then
                minDeadWeight = deadWeight
                self.AIKiller = team
            end
        end

        for _, AI in pairs(AIManager:getAIsByTeamId(self.WaitDeadTeam.id)) do
            AI.WaitDead = true
        end

        for _, AI in pairs(AIManager:getAIsByTeamId(self.AIKiller.id)) do
            AI.targetTeamId = self.WaitDeadTeam.id
        end
    end
end

function WorldInfo:initPublicResourcePoint()
    for _, team in pairs(self.AITeam) do
        local resourcePoint = {}
        local bedPos = team.bedPos[1]
        for _, pointConfig in pairs(ResourcePointConfig:getAllConfig()) do
            if pointConfig.pointType == 1 then
                local Point = {
                    pos = nil,
                    haveAI = false,
                }
                Point.pos = VectorUtil.newVector3i(pointConfig.producePos1.x, pointConfig.producePos1.y, pointConfig.producePos1.z)
                --todo
                ---存在死循环的可能,临时处理
                local time = 0
                while EngineWorld:getBlockId(VectorUtil.newVector3i(Point.pos.x, Point.pos.y - 1, Point.pos.z)) == BlockID.AIR and time < 10 do
                    Point.pos = VectorUtil.newVector3i(Point.pos.x, Point.pos.y - 1, Point.pos.z)
                    time = time + 1
                end

                local dis = BtUtil.calculateDistance(bedPos, Point.pos)
                if #resourcePoint > 0 then
                    for i = 1, #resourcePoint do
                        if dis < BtUtil.calculateDistance(bedPos, resourcePoint[i].pos) then
                            table.insert(resourcePoint, i, Point)
                            break
                        end
                        if i == #resourcePoint then
                            table.insert(resourcePoint, Point)
                        end
                    end
                else
                    table.insert(resourcePoint, Point)
                end
            end
        end

        table.remove(resourcePoint, 6)
        table.remove(resourcePoint, 5)

        BlackBoard:setValue(tostring(team.id) .. "resourcePoint", resourcePoint)
    end
end

function WorldInfo:initGameType()
    local GameType = "NoPlayer"

    if self.AITeamNum == 0 then
        GameType = "PlayerGame"
    elseif self.AITeamNum == 1 then
        GameType = "AIGame_1"
    elseif self.AITeamNum == 2 then
        GameType = "AIGame_2"
    elseif self.AITeamNum == 3 then
        GameType = "AIGame_3"
    end
    BlackBoard:setValue("GameType", GameType)
end