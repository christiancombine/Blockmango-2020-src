---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 12:26
---

CsvUtil = {}

function CsvUtil.getRowContent(file, filePath)
    if file == nil then
        assert(false, filePath .. " -----------------> is not exist.")
    end
    local content;
    local check = false
    local count = 0
    while true do
        local t = file:read()
        if not t then
            if count == 0 then
                check = true
            end
            break
        end

        if not content then
            content = t
        else
            content = content .. t
        end

        local i = 1
        while true do
            local index = string.find(t, "\n", i)
            if not index then
                break
            end
            i = index + 1
            count = count + 1
        end

        if count % 2 == 0 then
            check = true
            break
        end
    end

    if not check then
        assert(1 ~= 1)
    end

    if content then
        return string.gsub(content, "\r", "")
    end

    return content
end

function CsvUtil.loadCsvFile(filePath, tRow)
    local alls = {}
    local file = io.open(filePath, "r")
    while true do
        local line = CsvUtil.getRowContent(file, filePath)
        if not line then
            break
        end
        table.insert(alls, line)
    end
    io.close(file)
    local titleRow = tRow or 2
    local titles = StringUtil.split(alls[titleRow], "\t", true)
    local ID = 1
    local arrs = {}
    for row = titleRow + 1, #alls, 1 do
        local content = StringUtil.split(alls[row], "\t", true)
        ID = tostring(content[1])
        if ID and #ID == 0 then
            break
        end
        arrs[ID] = {}
        for col = 1, #titles, 1 do
            if #titles[col] > 0 then
                arrs[ID][titles[col]] = content[col]
                if col <= #titles and content[col] and #content[col] == 0 then
                    assert(false, filePath .. " -----------------> " .. titles[col] .. " is nil value, row = " .. row .. " col = " .. col)
                end
            end
        end
    end
    return arrs
end

return CsvUtil