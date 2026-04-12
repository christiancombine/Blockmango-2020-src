local utils = require "utils"

local config = {}

local function checkInteger(value)
    return math.floor(tonumber(value))
end

local function checkPos(x, y, z)
    return {
        tonumber(x),
        tonumber(y),
        tonumber(z)
    }
end

local function checkRegion(x1, y1, z1, x2, y2, z2)
    x1 = tonumber(x1)
    y1 = tonumber(y1)
    z1 = tonumber(z1)
    x2 = tonumber(x2)
    y2 = tonumber(y2)
    z2 = tonumber(z2)

    return {
        math.min(x1, x2), math.min(y1, y2), math.min(z1, z2),
        math.max(x1, x2), math.max(y1, y2), math.max(z1, z2)
    }
end

function config:init(content)

    -- init Pos
    do
        config.initPos = checkPos(unpack(content.initPos))
    end

    -- speed level
    do
        config.speedLevel = checkInteger(content.speedLevel)
    end

    -- player count limit
    do
        config.playerMin = checkInteger(content.playerMin)
        --config.playerMax = checkInteger(content.playerMax)
        assert(config.playerMin > 0)
    end

    -- timer
    do
        config.waitTime = checkInteger(content.waitTime)
        config.readyTime = checkInteger(content.readyTime)
        config.gameTime = checkInteger(content.gameTime)
        config.reviveTime = checkInteger(content.reviveTime)
    end

    -- buff
    do
        config.buffs = {}
        for _, buff in ipairs(content.buffs) do
            local itemId = checkInteger(buff.item)
            config.buffs[itemId] = {
                effect = checkInteger(buff.effect),
                time = checkInteger(buff.time),
                level = checkInteger(buff.level)
            }
        end
    end

    -- potions
    do
        config.potions = {}
        for _, potion in ipairs(content.potions) do
            local potionId = checkInteger(potion.id)
            config.potions[potionId] = {
                item = checkInteger(potion.item),
                lifeTime = checkInteger(potion.lifeTime),
            }
            assert(config.buffs[checkInteger(potion.item)], string.format("miss buff: %s", potion.item))
        end
    end

    -- sites
    do
        config.sites = {}
        for _, site_data in ipairs(content.sites) do
            local site = {
                name = assert(site_data.name),
                initRegion = checkRegion(unpack(site_data.initRegion)),
                initPoint = checkPos(unpack(site_data.initPoint))
            }

            -- fatalBlock
            site.fatalBlock = {}
            for _, blockId in ipairs(site_data.fatalBlock) do
                blockId = checkInteger(blockId)
                site.fatalBlock[blockId] = true
            end

            -- tunnels
            site.tunnels = {}
            for _, tunnel_data in ipairs(site_data.tunnels) do
                local tunnel = {
                    entry = checkRegion(unpack(tunnel_data.entry)),
                    exit = checkPos(unpack(tunnel_data.exit))
                }
                table.insert(site.tunnels, tunnel)
            end
            -- todo check region conflict

            -- potions
            site.potions = {}
            for _, potion_data in ipairs(site_data.potions) do
                local potion = {
                    potion = checkInteger(potion_data.potion),
                    spawnTime = checkInteger(potion_data.spawnTime),
                    list = {}
                }
                for _, pos in ipairs(potion_data.list) do
                    table.insert(potion.list, checkPos(unpack(pos)))
                end

                table.insert(site.potions, potion)
            end

            -- stages
            site.stages = {}
            for _, stage_data in ipairs(site_data.stages) do
                local stage = {
                    region = checkRegion(unpack(stage_data.region)),
                    score = checkInteger(stage_data.score),
                    point = checkPos(unpack(stage_data.point))
                }
                table.insert(site.stages, stage)
            end
            -- todo check region conflict

            table.insert(config.sites, site)
        end
    end

    -- shop
    do
        config.shops = {}
        config.need_save_items = {}
        for _, shop_data in ipairs(content.shops) do
            local shop = {
                type = shop_data.type,
                icon = shop_data.icon,
                name = shop_data.name,
                goods = {}
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
                })
                config.need_save_items[checkInteger(goods_data.price[1])] = true
            end

            table.insert(config.shops, shop)
        end
    end

    -- rank
    do
        config.ranks = {}
        for _, rank_data in ipairs(content.ranks) do
            local rank = {
                site = rank_data.site,
                npc = {
                    pos = checkPos(unpack(rank_data.npc.pos)),
                    dir = tonumber(rank_data.npc.dir),
                    name = rank_data.npc.name
                }
            }
            table.insert(config.ranks, rank)
        end
    end

    --session npc
    if content and content.session_npc then
        do 
            config.session_npc = {}
            for _, session_npc in ipairs(content.session_npc) do
                local npc = {
                    pos = session_npc.initPos,
                    name = session_npc.name,
                    message = session_npc.message,
                    actor = session_npc.actor,
                    session_npc_type = session_npc.session_npc_type
                }
                table.insert(config.session_npc, npc)
            end
        end
    end
end

return config
