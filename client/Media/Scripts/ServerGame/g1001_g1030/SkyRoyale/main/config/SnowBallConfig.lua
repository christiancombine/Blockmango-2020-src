SnowBallConfig = {}
SnowBallConfig.damage = 0
SnowBallConfig.snowBall = {}
SnowBallConfig.isStartGenerate = false

function SnowBallConfig:initDamage(damage)
    self.damage = damage
end

function SnowBallConfig:initSnowBall(config)
    for i, v in pairs(config) do
        local data = {}
        data.life = tonumber(v.life)
        data.pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        data.speed = {}
        for j, speed in pairs(v.speed) do
            local item = {}
            item.time = tonumber(speed.time)
            item.cd = tonumber(speed.cd)
            item.num = tonumber(speed.num)
            data.speed[j] = item
        end
        self.snowBall[i] = data
    end
end

function SnowBallConfig:generateSnowBallByTime(time)
    local result
    for i, snowBall in pairs(self.snowBall) do
        for j, speed in pairs(snowBall.speed) do
            if result == nil then
                result = speed
            end
            if time >= speed.time then
                result = speed
            end
        end
        
        if time % result.cd == 0 and result.num > 0 then
            EngineWorld:addEntityItem(332, result.num, 0, snowBall.life, snowBall.pos)
        end
    end

end



return SnowBallConfig