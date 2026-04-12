
ProduceConfig = class()

function ProduceConfig:ctor(config)
    self.produce = {}
    self.record = {}
    self.ProduceMaxTimes = 5
    self:init(config)
end

function ProduceConfig:init(config)
    math.randomseed(tostring(os.clock()):reverse():sub(1, 6))
    if config == nil then
        HostApi.log("ProduceConfig is nil")
        return
    end
    for i, v in pairs(config) do
        self.record[i] = {}
        self.record[i].produceInterval = -1
        self.record[i].produceLastTime = 0
        self.record[i].hasProduce = {}

        self.produce[i] = {}
        self.produce[i].id = tonumber(v.id)
        self.produce[i].num = tonumber(v.num)
        self.produce[i].life = tonumber(v.life)
        self.produce[i].initLoop = tonumber(v.loop)
        self.produce[i].loop = self.produce[i].initLoop

        self.produce[i].blockType = {}
        for n, v in pairs(v.blockType) do
            self.produce[i].blockType[n] = tonumber(v)
        end

        self.produce[i].interval = {}
        for j, v in pairs(v.interval) do
            self.produce[i].interval[j] = tonumber(v)
        end

        self.produce[i].x = {}
        for k, v in pairs(v.x) do
            self.produce[i].x[k] = tonumber(v)
        end

        self.produce[i].y = {}
        for l, v in pairs(v.y) do
            self.produce[i].y[l] = tonumber(v)
        end

        self.produce[i].z = {}
        for m, v in pairs(v.z) do
            self.produce[i].z[m] = tonumber(v)
        end
    end
end

function ProduceConfig:addPropToWorld(ticks)
    for i, prop in pairs(self.produce) do
        if prop ~= nil and prop.loop > 0 then
            local interval = self.record[i].produceInterval
            local lastTime = self.record[i].produceLastTime
            if interval == -1 then
                self.record[i].produceLastTime = ticks
                self.record[i].produceInterval = math.random(prop.interval[1], prop.interval[2])
            elseif ticks - lastTime == interval then
                local propPos
                local flag = 0

                for j = 1, self.ProduceMaxTimes do

                    local x = math.random(math.min(prop.x[1], prop.x[2]), math.max(prop.x[1], prop.x[2]))
                    local y = math.random(math.min(prop.y[1], prop.y[2]), math.max(prop.y[1], prop.y[2]))
                    local z = math.random(math.min(prop.z[1], prop.z[2]), math.max(prop.z[1], prop.z[2]))
                    propPos = VectorUtil.newVector3(x, y, z)

                    if self.record[i].hasProduce[VectorUtil.hashVector3(propPos)] == nil then
                        local blockPos = VectorUtil.newVector3i(x, y, z);
                        local id = EngineWorld:getBlockId(blockPos)
                        local canBlock = false
                        for k, v in pairs(prop.blockType) do
                            if v == id then
                                canBlock = true
                                break
                            end
                        end
                        if #prop.blockType <= 0 or canBlock then
                            self.record[i].hasProduce[VectorUtil.hashVector3(propPos)] = propPos
                            flag = 1
                            break
                        end
                    else
                        flag = 1
                        break
                    end
                end
                if flag == 1 then
                    prop.loop = prop.loop - 1
                    EngineWorld:addEntityItem(prop.id, prop.num, 0, prop.life, propPos)
                end
                self.record[i].produceInterval = -1
            end
        end
    end
end

function ProduceConfig:reset()
    for i, v in pairs(self.produce) do
        self.record[i].produceInterval = -1
        self.record[i].produceLastTime = 0
        self.record[i].hasProduce = {}
        self.produce[i].loop = self.produce[i].initLoop
    end
end

return ProduceConfig 