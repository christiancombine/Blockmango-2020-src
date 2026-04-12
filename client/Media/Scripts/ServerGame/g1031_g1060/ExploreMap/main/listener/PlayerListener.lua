--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)

    CreateExploreMapEvent.registerCallBack(self.onCreateExploreMap)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    GameMatch:getDBDataRequire(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 43200
end

function PlayerListener.onCreateExploreMap(rakssid)
    --HostApi.log("onCreateExploreMap")
    local spaceStart = VectorUtil.newVector3i(GameConfig.spaceStart[1], GameConfig.spaceStart[2], GameConfig.spaceStart[3])
    local roomCount = GameConfig.roomCount
    local spaceSize = GameConfig.spaceSize
    local roomInfo = GameConfig.roomInfo
    local radioInfo = GameConfig.typeRadio
    local fillBlockId = GameConfig.fillBlockId
    local needFillBlock = GameConfig.needFillBlock
    local spaceEnd = VectorUtil.newVector3i(spaceStart.x + spaceSize[1] - 1, spaceStart.y + spaceSize[2] - 1, spaceStart.z + spaceSize[3] - 1)

    local allRoomInfo = {}
    local maxRoomCountX = 0
    local maxRoomCountY = 0
    local maxRoomCountZ = 0

    local bottomSpace = VectorUtil.newVector3i(spaceEnd.x, spaceStart.y, spaceEnd.z)
    EngineWorld:getWorld():fillAreaByBlockIdAndMate(spaceStart, bottomSpace, fillBlockId)

    if needFillBlock and needFillBlock == 1 then
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(spaceStart, spaceEnd, fillBlockId)
    end

    math.randomseed(tostring(os.time()):reverse():sub(1, 6))

    for i = 1, #roomInfo do
        local num = math.floor(roomCount * radioInfo[roomInfo[i].type] / 100)
        for j = 1, num do
            local index = math.random(1, #roomInfo[i].info)
            local item = {}
            local size = VectorUtil.newVector3i(roomInfo[i].info[index].size[1], roomInfo[i].info[index].size[2], roomInfo[i].info[index].size[3])
            item.size = size
            item.file = roomInfo[i].info[index].file
            item.type = roomInfo[i].type
            table.insert(allRoomInfo, item)

            --HostApi.log("roomInfo " .. i .. " " .. num)
            --HostApi.log("roomInfo[i].size " .. roomInfo[i].info[index].size[1] .. " " .. roomInfo[i].info[index].size[2] .. " " .. roomInfo[i].info[index].size[3])

            if tonumber(roomInfo[i].info[index].size[1]) > maxRoomCountX then
                maxRoomCountX = tonumber(roomInfo[i].info[index].size[1])
            end
            if tonumber(roomInfo[i].info[index].size[2]) > maxRoomCountY then
                maxRoomCountY = tonumber(roomInfo[i].info[index].size[2])
            end
            if tonumber(roomInfo[i].info[index].size[3]) > maxRoomCountZ then
                maxRoomCountZ = tonumber(roomInfo[i].info[index].size[3])
            end
        end
    end

    --HostApi.log("maxRoomCountX " .. maxRoomCountX) 
    --HostApi.log("maxRoomCountY " .. maxRoomCountY) 
    --HostApi.log("maxRoomCountZ " .. maxRoomCountZ)

    local spaceInterval = 1

    local xCount = math.floor(spaceSize[1] / (maxRoomCountX + spaceInterval))
    local yCount = math.floor(spaceSize[2] / (maxRoomCountY + spaceInterval))
    local zCount = math.floor(spaceSize[3] / (maxRoomCountZ + spaceInterval))

    --HostApi.log("xCount " .. xCount) 
    --HostApi.log("yCount " .. yCount) 
    --HostApi.log("zCount " .. zCount) 

    local posOrderCount = xCount * yCount * zCount

    local path = FileUtil:getMapPath()
    local file = io.open(path .. "exploreroom.yml", "w")

    file:write("spaceStart: [" .. spaceStart.x .. ", " .. spaceStart.y .. ", " .. spaceStart.z .. "]\n")
    file:write("spaceEnd: [" .. spaceEnd.x .. ", " .. spaceEnd.y .. ", " .. spaceEnd.z .. "]\n")

    file:write("roomInfo: \n")

    local orderVec = {}
    local room_id = 1
    local findblockIdlist = {}
    for k, v in pairs(allRoomInfo) do
        local randomSign = false
        local posOrder = -1

        while not randomSign do
            posOrder = math.random(1, posOrderCount)

            local repeatSign = false
            for _, num in pairs(orderVec) do
                if num == posOrder then
                    repeatSign = true
                    break
                end
            end

            if not repeatSign then
                randomSign = true
                table.insert(orderVec, posOrder)
            end
        end
        --HostApi.log("posOrder " .. posOrder) 
        --HostApi.log("spaceStart " .. spaceStart.x .. " " .. spaceStart.y .. " " .. spaceStart.z)

        local y = math.floor(posOrder / (xCount * zCount))
        local z = math.floor((posOrder - y * (xCount * zCount)) / xCount)
        local x = math.floor((posOrder - y * (xCount * zCount)) % xCount)

        --HostApi.log("v.size.y " .. v.size.y)
        --HostApi.log("v.size.z " .. v.size.z)
        --HostApi.log("v.size.x " .. v.size.x)

        local startPosY = spaceStart.y + (yCount - y) * maxRoomCountY + math.random(0, maxRoomCountY - v.size.y)
        local startPosZ = spaceStart.z + z * maxRoomCountZ + math.random(0, maxRoomCountZ - v.size.z)
        local startPosX = spaceStart.x + x * maxRoomCountX + math.random(0, maxRoomCountX - v.size.x)

        --HostApi.log("Y " .. y)
        --HostApi.log("Z " .. z)
        --HostApi.log("X " .. x)
        
        local startPos = VectorUtil.newVector3i(startPosX, startPosY, startPosZ)
        local endPos = VectorUtil.newVector3i(startPosX + v.size.x - 1, startPosY + v.size.y - 1, startPosZ + v.size.z - 1)
        --HostApi.log("startPos " .. startPos.x .. " " .. startPos.y .. " " .. startPos.z)
        --HostApi.log("endPos " .. endPos.x .. " " .. endPos.y .. " " .. endPos.z)

        EngineWorld:getWorld():fillAreaByBlockIdAndMate(startPos, endPos, 0)
        EngineWorld:createSchematic(v.file, startPos)

        --HostApi.log("path: " .. path)
        --HostApi.log("file: " .. v.file)
        
        file:write("    ")
        file:write("-\n")
        file:write("        id: " .. room_id .. "\n")
        file:write("        startPos: [" .. startPos.x .. ", " .. startPos.y .. ", " .. startPos.z .. "]\n")
        file:write("        size: [" .. v.size.x .. ", " .. v.size.y .. ", " .. v.size.z .. "]\n")
        file:write("        type: " .. v.type .. " \n")
        file:write("        file: " .. v.file .. " \n")

        local roomBlockConfig = GameConfig:getRoomBlockConfig(v.file)
        if roomBlockConfig ~= nil then
            if roomBlockConfig.jumpblock ~= nil then
                file:write("        jumpblock: [" .. roomBlockConfig.jumpblock[1] .. ", " .. roomBlockConfig.jumpblock[2] .. ", " .. roomBlockConfig.jumpblock[3] .. "]\n")
            end

            if roomBlockConfig.pressure ~= nil then
                file:write("        pressure: \n")

                for x = startPos.x, endPos.x do
                    for y = startPos.y, endPos.y do
                        for z = startPos.z, endPos.z do
                            local blockPos = VectorUtil.newVector3i(x, y, z)
                            local blockId = EngineWorld:getBlockId(blockPos)
                            -- --HostApi.log("pressure:" .. x .. " " .. y .. " " .. z .. " " .. blockId)
                            if blockId == 217 then
                                file:write("            -\n")
                                file:write("                pos: [" .. x .. ", " .. y .. ", " .. z .. "]" .. "\n")
                            end
                        end
                    end
                end
            end

            if roomBlockConfig.chest ~= nil then
                file:write("        chest: \n")

                for x = startPos.x, endPos.x do
                    for y = startPos.y, endPos.y do
                        for z = startPos.z, endPos.z do
                            local blockPos = VectorUtil.newVector3i(x, y, z)
                            local blockId = EngineWorld:getBlockId(blockPos)
                            -- --HostApi.log("chest:" .. x .. " " .. y .. " " .. z .. " " .. blockId)
                            if blockId == 54 then
                                file:write("            -\n")
                                file:write("                pos: [" .. x .. ", " .. y .. ", " .. z .. "]" .. "\n")
                            end
                        end
                    end
                end

                if roomBlockConfig.range and roomBlockConfig.items then
                    file:write("        range: [" .. roomBlockConfig.range[1]
                        .. ", " .. roomBlockConfig.range[2] .. "]" .. "\n")
                    file:write("        items: \n")

                    for k, v in pairs(roomBlockConfig.items) do
                        file:write("            -\n")
                        file:write("                id: " .. v.id .. "\n")
                        file:write("                randomRange: [" .. v.randomRange[1] .. ", " .. v.randomRange[2] .. "]" .. "\n")
                        file:write("                probability: " .. v.probability .. "\n")
                    end
                end
            end

            if roomBlockConfig.findblockId ~= nil then
                file:write("        findblockId: " ..  tonumber(roomBlockConfig.findblockId) .. "\n")
                local repeat_sign = false
                for k, v in pairs(findblockIdlist) do
                    if v == tonumber(roomBlockConfig.findblockId) then
                        repeat_sign = true
                    end
                end
                if repeat_sign == false then
                    table.insert(findblockIdlist, tonumber(roomBlockConfig.findblockId))
                end
            end
        end

        file:write("\n")

        room_id = room_id + 1
        --HostApi.log("room_id:" .. room_id)

        if room_id > posOrderCount then
            break
        end
    end

    if #findblockIdlist > 0 then
        file:write("findblockIdlist: [")
        for i = 1, #findblockIdlist do
            file:write(findblockIdlist[i])
            if i < #findblockIdlist then
                file:write(", ")
            end
        end
        file:write("]\n")
    end

    local fillBlockCount = 0
    local boxCount = 0
    for x = spaceStart.x, spaceEnd.x do
        for y = spaceStart.y, spaceEnd.y do
            for z = spaceStart.z, spaceEnd.z do
                local blockPos = VectorUtil.newVector3i(x, y, z)
                local blockId = EngineWorld:getBlockId(blockPos)
                if blockId == fillBlockId then
                    fillBlockCount = fillBlockCount + 1
                end
                if blockId == BlockID.CHEST then
                    boxCount = boxCount + 1
                end
            end
        end
    end

    file:write("fillBlockCount: " .. fillBlockCount .. "\n")
    file:write("boxCount: " .. boxCount .. "\n")

    file:close()
end


return PlayerListener
--endregion
