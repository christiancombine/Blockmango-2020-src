-- Test

-- local start_pos = VectorUtil.newVector3i(1724, 92, -526)
-- local file_str = "D:\\trunk\\blockmango-client\\res\\client\\map\\gameName-debug\\H3.json"

-- createHouse(file_str, start_pos)

function createHouse(file_path, start_pos)
    local file = io.open(file_path, "r")
    if file == nil then
        return
    end
    local fileStream = file.read(file, "*a")
    local jsonTable = json.decode(fileStream)

    if jsonTable == nil then
        return
    end

    for k, v in pairs(jsonTable.elements) do
        local pos = VectorUtil.newVector3i(tonumber(v.from[1]) + start_pos.x, tonumber(v.from[2]) + start_pos.y, tonumber(v.from[3]) + start_pos.z)

        local target_str = v.faces.North.texture
        local start_index1, end_index1 = string.find(target_str, "#")
        local start_index2, end_index2 = string.find(target_str, "_")
        local end_pos = string.len(target_str)

        local block_id = string.sub(target_str, end_index1 + 1, start_index2 - 1)
        local mate = string.sub(target_str, end_index2 + 1, end_pos)

        GameMatch.world:setBlock(pos, block_id, mate, 3)
    end

    file:close()
end

