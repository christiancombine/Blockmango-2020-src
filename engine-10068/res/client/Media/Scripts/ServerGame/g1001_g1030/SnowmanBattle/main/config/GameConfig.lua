-- GameConfig.lua
require "config.SnowBallConfig"

GameConfig = { }

GameConfig.teams = { }

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0
GameConfig.gameHintTime = 0
GameConfig.DIYTime = 0

GameConfig.snowballSpeed = { }
GameConfig.snowball = { }

GameConfig.moneyGrowth = { };

GameConfig.startPlayers = 1
GameConfig.maxPlayers = 8

GameConfig.attackRewradMoney = 0
GameConfig.killRewradMoney = 0

GameConfig.winCondition = 0

GameConfig.pickupDuration = 0

GameConfig.respawn = 0



GameConfig.winScore = 0


GameConfig.sppItems = { }
GameConfig.clzItems = { }

GameConfig.enableBreak = false
GameConfig.obstacle = { }
GameConfig.TNT = { }

GameConfig.sites = { }
GameConfig._site_idx = 0

local function checkPos(x, y, z)
    return {
        tonumber(x),
        tonumber(y),
        tonumber(z)
    }
end


local function print_table(root)
    assert(type(root) == "table")
    local cache = { [root] = "." }
    local ret = ""

    local function _new_line(level)
        local ret = ""
        ret = ret .. "\n"
        for i = 1, level do
            ret = ret .. "\t"
        end
        return ret
    end

    local function _format(value)
        if (type(value) == "string") then
            return "\"" .. value .. "\""
        else
            return tostring(value)
        end
    end

    local function _enter_level(t, level, name)
        local ret = ""
        local _keycache = { }
        for k, v in ipairs(t) do
            ret = ret .. _new_line(level + 1)
            if (cache[v]) then
                ret = ret .. "[" .. tostring(k) .. "]" .. " = " .. cache[v] .. ","
            elseif (type(v) == "table") then
                local newname = name .. "." .. tostring(k)
                cache[v] = newname
                ret = ret .. "[" .. tostring(k) .. "]" .. " = " .. "{"
                ret = ret .. _enter_level(v, level + 1, newname)
                ret = ret .. _new_line(level + 1)
                ret = ret .. "},"

            else
                if (type(v) == "string") then
                    ret = ret .. "[" .. tostring(k) .. "]" .. " = " .. "\"" .. v .. "\"" .. ","
                else
                    ret = ret .. "[" .. tostring(k) .. "]" .. " = " .. tostring(v) .. ","
                end
            end
            _keycache[k] = true
        end

        for k, v in pairs(t) do
            if (not _keycache[k]) then
                ret = ret .. _new_line(level + 1)
                if (cache[v]) then
                    ret = ret .. "[" .. _format(k) .. "]" .. " = " .. cache[v] .. ","
                elseif (type(v) == "table") then
                    local newname = name .. "." .. tostring(k)
                    cache[v] = newname
                    ret = ret .. "[" .. _format(k) .. "]" .. " = " .. "{"
                    ret = ret .. _enter_level(v, level + 1, newname)
                    ret = ret .. _new_line(level + 1)
                    ret = ret .. "},"
                else
                    if (type(v) == "string") then
                        ret = ret .. "[" .. _format(k) .. "]" .. " = " .. "\"" .. v .. "\"" .. ","
                    else
                        ret = ret .. "[" .. _format(k) .. "]" .. " = " .. tostring(v) .. ","
                    end
                end
            end
        end
        return ret
    end

    ret = ret .. "{"
    ret = ret .. _enter_level(root, 0, "")
    ret = ret .. "\n}\n"
    print(ret)
end

function GameConfig:init(config)
    for k, v in pairs(config.teams) do
        self.teams[k] = { }
        self.teams[k].id = tonumber(v.id)
        self.teams[k].name = tostring(v.name)
        self.teams[k].money = tonumber(v.money)
        self.teams[k].initPos = checkPos(unpack(v.initPos))
        self.teams[k].waitPos = checkPos(unpack(v.waitPos))
        self.teams[k].area = { }
        self.teams[k].area.min = checkPos(unpack(v.area[1]))
        self.teams[k].area.max = checkPos(unpack(v.area[2]))

        self.teams[k].guradSnowman = { }
        self.teams[k].guradSnowman.initPos = checkPos(unpack(v.guradSnowman.initPos))
        self.teams[k].guradSnowman.actor = v.guradSnowman.actor
        self.teams[k].guradSnowman.name = v.guradSnowman.name
        self.teams[k].guradSnowman.hp = tonumber(v.guradSnowman.hp)
        self.teams[k].guradSnowman.yaw = tonumber(v.guradSnowman.yaw)

        self.teams[k].flag = { }
        for flagK, flagV in pairs(v.flag) do
            self.teams[k].flag[flagK] = { }
            self.teams[k].flag[flagK].initPos = checkPos(unpack(flagV.initPos));
            self.teams[k].flag[flagK].actor = flagV.actor;
            self.teams[k].flag[flagK].name = flagV.name;
            self.teams[k].flag[flagK].hp = tonumber(flagV.hp);
            self.teams[k].flag[flagK].id = tonumber(flagV.id);
            self.teams[k].flag[flagK].num = tonumber(flagV.num);
            self.teams[k].flag[flagK].rewradHp = tonumber(flagV.rewradHp);
            self.teams[k].flag[flagK].yaw = tonumber(flagV.yaw);
        end
    end

    SnowBallConfig:init(FileUtil.getConfigFromCsv("SownBallPromote.csv"))

    self.ObstacleCom = FileUtil.getConfigFromCsv("ObstaclePromote.csv")

    self.potion = { }
    self.potion.pos = { }
    for posK, posV in pairs(config.potion.pos) do
        self.potion.pos[posK] = checkPos(unpack(posV))
    end
    self.potion.id = tonumber(config.potion.id);
    self.potion.life = tonumber(config.potion.life);
    self.potion.interval = tonumber(config.potion.interval);
    self.potion.count = tonumber(config.potion.count);
    self.potion.startTime = tonumber(config.potion.startTime);



    self.initMoney = config.initMoney;

    for k, v in pairs(config.snowballSpeed) do
        self.snowballSpeed[k] = checkPos(unpack(v));
    end
    self.snowballProportion = tonumber(config.snowballProportion)

    self.enableBreak = config.enableBreak
    self.waitingTime = tonumber(config.waitingTime)
    self.readyTime = tonumber(config.readyTime)
    self.fightTime = tonumber(config.fightTime)
    self.finalTime = tonumber(config.finalTime)
    self.gameHintTime = tonumber(config.gameHintTime)


    self.minPlayers = tonumber(config.minPlayers)
    self.maxPlayers = tonumber(config.maxPlayers)

    self.heightLimit = tonumber(config.heightLimit)


    self.attackRewradMoney = tonumber(config.attackRewradMoney)
    self.killRewradMoney = tonumber(config.killRewradMoney)

    self.moneyGrowth.growth = checkPos(unpack(config.moneyGrowth.growth))
    self.moneyGrowth.startTime = tonumber(config.moneyGrowth.startTime)
    self.moneyGrowth.stopTime = tonumber(config.moneyGrowth.stopTime)

    for k, v in pairs(config.obstacle) do
        self.obstacle[k] = { }
        self.obstacle[k].id = tonumber(v.id)
        self.obstacle[k].hp = tonumber(v.hp)
    end

    self.initObstacle = { }
    self.initObstacle.id = tonumber(config.initObstacle.id)
    self.initObstacle.count = tonumber(config.initObstacle.count)







    -- TNT
    self.TNT.damage = checkPos(unpack(config.TNT.damage))
    self.TNT.produce = checkPos(unpack(config.TNT.produce))
    self.TNT.stratTime = tonumber(config.TNT.stratTime)
    self.TNT.stopTime = tonumber(config.TNT.stopTime)

    self.pickupDuration = tonumber(config.pickupDuration)






    self.respawn = { }
    self.respawn.wait = tonumber(config.respawn.wait)
    self.respawn.count = tonumber(config.respawn.count)

    -- shop
    do
        self.shops = { }
        self.need_save_items = { }
        for _, shop_data in ipairs(config.shops) do
            local shop = {
                type = shop_data.type,
                icon = shop_data.icon,
                name = shop_data.name,
                goods = { }
            }
            for _, goods_data in ipairs(shop_data.goods) do
                table.insert(shop.goods, {
                    itemId = goods_data.price[1],
                    itemMeta = goods_data.price[2],
                    itemNum = goods_data.price[3],
                    coinId = goods_data.price[4],
                    blockymodsPrice = goods_data.price[5],
                    blockmanPrice = goods_data.price[6],
                    limit = goods_data.price[7],

                    desc = goods_data.desc
                } )
                self.need_save_items[tonumber(goods_data.price[1])] = true
            end

            table.insert(self.shops, shop)
        end
    end

    -- rank
    do
        self.ranks = { }
        self.ranks.name = config.ranks.name
        self.ranks.title = config.ranks.title
        self.ranks.poss = { }
        for k, pos in ipairs(config.ranks.poss) do
            self.ranks.poss[k] = pos
        end
    end

    self.airBlockArea={}
    self.airBlockArea.min= config.airBlockArea[1]
    self.airBlockArea.max= config.airBlockArea[2]

    -- TipNpc
    self.tipNpc = { }
    for k, v in pairs(config.tipNpc) do
        self.tipNpc[k] = v
    end

        self.block_attrs = {}
    for i, b in pairs(config.blockAttr) do
        local item = {}
        item.id = tonumber(b.id)
        item.hardness = tonumber(b.hardness)
        self.block_attrs[i] = item
    end
    self:prepareBlock()

    self.flooregg={}
    self.flooregg.id=tonumber(config.flooregg.id)
    self.flooregg.size=tonumber(config.flooregg.size)

    

end

function GameConfig:prepareBlock()
    for i, v in pairs(self.block_attrs) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

return GameConfig