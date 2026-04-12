---

CheckPointTotalRank = class("CheckPointTotalRank", BaseRank)

function CheckPointTotalRank:init()
    self.rankKey = Define.RankKey.CheckPointTotal
    self.pageSize = 4
    self.rowNum = 5
    ---每一列 偏移位置 和 列头
    self.rowKey = { BaseRank.LangKey.Rank, BaseRank.LangKey.Captain, BaseRank.LangKey.Teammate, BaseRank.LangKey.TopPass, BaseRank.LangKey.PassTime }
    self.titleOffset = { -3.5, 1.75 }
    self.countDownOffset = { -3.5, 1.5 }
    self.countDownTimeOffset = { 3.5, 1.48 }
    self.rowKeyOffsetX = { -3.5, -1.65, 0.95, 2.25, 3.35 }
    self.rowKeyOffsetY = { 1.08, 0.5, -0.26, -1.02, -1.78 }
end

function CheckPointTotalRank:printData(list, scale, alpha)
    scale = scale or 1
    alpha = alpha or 1
    local globalRender = Blockman.Instance().m_globalRender
    if not globalRender then
        return
    end
    for index, record in pairs(list) do
        local rowKeyOffsetY = self.rowKeyOffsetY[index + 1]

        globalRender:luaAddWallText(5, -1, record.rank,
                self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.rowKeyOffsetX[1], rowKeyOffsetY),
                self.config.scale * 1.2 * scale, self.config.yaw, 0, { 1, 1, 1, 1 * alpha })

        local playerNames = StringUtil.split(record.value, "#")
        local teammate = ""
        for i = 2, #playerNames do
            local values = StringUtil.split(playerNames[i], ",")
            teammate = teammate .. values[1] .. "\n"
        end

        local values = StringUtil.split(playerNames[1], ",")
        globalRender:luaAddWallText(5, -1, values[1],
                self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.rowKeyOffsetX[2], rowKeyOffsetY),
                self.config.scale * scale, self.config.yaw, 0, { 1, 1, 1, 1 * alpha })

        if #playerNames > 1 then
            local offset = { 0, -0.08, -0.165 }
            globalRender:luaAddWallText(5, -1, teammate,
                    self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.rowKeyOffsetX[3], rowKeyOffsetY + offset[#playerNames - 1]),
                    self.config.scale * 0.65 * scale, self.config.yaw, 0, { 1, 1, 1, 1 * alpha })
        end

        globalRender:luaAddWallText(5, -1, (Define.MapName[math.floor(record.score / 100000)] or 0),
                self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.rowKeyOffsetX[4], rowKeyOffsetY),
                self.config.scale * 1.2 * scale, self.config.yaw, 0, { 1, 1, 1, 1 * alpha })

        local totalSecond = 100000 - math.fmod(record.score, 100000)
        local minute = math.floor(totalSecond / 60)
        local second = math.fmod(totalSecond, 60)
        local showTime = minute .. "′ " .. second .. " ″"
        if second < 10 then
            showTime = minute .. "′ 0" .. second .. "″"
        end
        globalRender:luaAddWallText(5, -1, showTime,
                self:getNewPosition(self.config.effectPos, self.config.scale, self.config.yaw, self.rowKeyOffsetX[5], rowKeyOffsetY),
                self.config.scale * 1.2 * scale, self.config.yaw, 0, { 1, 1, 1, 1 * alpha })

    end

end

function CheckPointTotalRank:refreshActor(number)
    MaxRecordHelper:getRecord(self.rankKey, 1, 1, function(list)
        local userIds = {}
        for _, record in pairs(list) do
            local playerNames = StringUtil.split(record.value, "#")
            for index, data in pairs(playerNames) do
                if index <= number then
                    local values = StringUtil.split(data or "", ",")
                    table.insert(userIds, tonumber(values[1]))
                end
            end
        end
        self:changeActor(userIds)
    end)
end