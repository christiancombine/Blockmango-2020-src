---
--- Created by Jimmy.
--- DateTime: 2017/10/20 0020 14:49
---
TeamColor = { "▢FFFF1F1F", "▢FF0AC3F7", "▢FF3AE34E", "▢FFE3BF0E",
              "▢FF0DE5ED", "▢FFFFFFFF", "▢FFFB99E0", "▢FFC1C1B6" }

if isClient then
    TeamGUIInfo = {
        ["1"] = { name = "gui_team_red", name_color = { 241 / 255, 31 / 255, 31 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_red",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_red", bed = "set:bed_war_team_info.json image:bed_red",
                  item_color = "set:bed_war_battle_info.json image:bg_item_red", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_red",
                  tab_icon = "set:bed_war_battle_info.json image:tab_red", tab_icon_normal = "set:bed_war_battle_info.json image:tab_red_normal",
                  icon = "set:bedwar_toolbar.json image:team_red", header_border2 = "set:bed_war_team_info.json image:head_red",
                  iconSingle = "set:bedwar_toolbar.json image:single_red", iconSelf = "set:bedwar_toolbar.json image:self_red" },
        ["2"] = { name = "gui_team_blue", name_color = { 10 / 255, 195 / 255, 247 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_blue",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_blue", bed = "set:bed_war_team_info.json image:bed_blue",
                  item_color = "set:bed_war_battle_info.json image:bg_item_blue", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_blue",
                  tab_icon = "set:bed_war_battle_info.json image:tab_blue", tab_icon_normal = "set:bed_war_battle_info.json image:tab_blue_normal",
                  icon = "set:bedwar_toolbar.json image:team_blue", header_border2 = "set:bed_war_team_info.json image:head_blue",
                  iconSingle = "set:bedwar_toolbar.json image:single_blue", iconSelf = "set:bedwar_toolbar.json image:self_blue" },
        ["3"] = { name = "gui_team_green", name_color = { 58 / 255, 227 / 255, 78 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_green",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_green", bed = "set:bed_war_team_info.json image:bed_green",
                  item_color = "set:bed_war_battle_info.json image:bg_item_green", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_green",
                  tab_icon = "set:bed_war_battle_info.json image:tab_green", tab_icon_normal = "set:bed_war_battle_info.json image:tab_green_normal",
                  icon = "set:bedwar_toolbar.json image:team_green", header_border2 = "set:bed_war_team_info.json image:head_green",
                  iconSingle = "set:bedwar_toolbar.json image:single_green", iconSelf = "set:bedwar_toolbar.json image:self_green" },
        ["4"] = { name = "gui_team_yellow", name_color = { 227 / 255, 191 / 255, 14 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_yellow",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_yellow", bed = "set:bed_war_team_info.json image:bed_yellow",
                  item_color = "set:bed_war_battle_info.json image:bg_item_yellow", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_yellow",
                  tab_icon = "set:bed_war_battle_info.json image:tab_yellow", tab_icon_normal = "set:bed_war_battle_info.json image:tab_yellow_normal",
                  icon = "set:bedwar_toolbar.json image:team_yellow", header_border2 = "set:bed_war_team_info.json image:head_yellow",
                  iconSingle = "set:bedwar_toolbar.json image:single_yellow", iconSelf = "set:bedwar_toolbar.json image:self_yellow" },
        ["5"] = { name = "gui_team_cyan", name_color = { 13 / 255, 229 / 255, 237 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_red",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_cyan", bed = "set:bed_war_team_info.json image:bed_cyan",
                  item_color = "set:bed_war_battle_info.json image:bg_item_cyan", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_cyan",
                  tab_icon = "set:bed_war_battle_info.json image:tab_cyan", tab_icon_normal = "set:bed_war_battle_info.json image:tab_cyan_normal",
                  icon = "set:bedwar_toolbar.json image:team_cyan", header_border2 = "set:bed_war_team_info.json image:head_cyan",
                  iconSingle = "set:bedwar_toolbar.json image:single_cyan", iconSelf = "set:bedwar_toolbar.json image:self_cyan" },
        ["6"] = { name = "gui_team_white", name_color = { 255 / 255, 255 / 255, 255 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_blue",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_white", bed = "set:bed_war_team_info.json image:bed_white",
                  item_color = "set:bed_war_battle_info.json image:bg_item_white", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_white",
                  tab_icon = "set:bed_war_battle_info.json image:tab_white", tab_icon_normal = "set:bed_war_battle_info.json image:tab_white_normal",
                  icon = "set:bedwar_toolbar.json image:team_white", header_border2 = "set:bed_war_team_info.json image:head_white",
                  iconSingle = "set:bedwar_toolbar.json image:single_white", iconSelf = "set:bedwar_toolbar.json image:self_white" },
        ["7"] = { name = "gui_team_pink", name_color = { 251 / 255, 153 / 255, 224 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_pink",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_pink", bed = "set:bed_war_team_info.json image:bed_pink",
                  item_color = "set:bed_war_battle_info.json image:bg_item_pink", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_pink",
                  tab_icon = "set:bed_war_battle_info.json image:tab_pink", tab_icon_normal = "set:bed_war_battle_info.json image:tab_pink_normal",
                  icon = "set:bedwar_toolbar.json image:team_pink", header_border2 = "set:bed_war_team_info.json image:head_pink",
                  iconSingle = "set:bedwar_toolbar.json image:single_pink", iconSelf = "set:bedwar_toolbar.json image:self_pink" },
        ["8"] = { name = "gui_team_gray", name_color = { 193 / 255, 193 / 255, 182 / 255, 1 }, header_border = "set:bed_war_battle_info.json image:bg_head_gray",
                  header = "default_headshot.png", color = "set:bed_war_battle_info.json image:bg_gray", bed = "set:bed_war_team_info.json image:bed_gray",
                  item_color = "set:bed_war_battle_info.json image:bg_item_gray", me_item_color = "set:bed_war_battle_info.json image:bg_me_item_gray",
                  tab_icon = "set:bed_war_battle_info.json image:tab_gray", tab_icon_normal = "set:bed_war_battle_info.json image:tab_gray_normal",
                  icon = "set:bedwar_toolbar.json image:team_gray", header_border2 = "set:bed_war_team_info.json image:head_gray",
                  iconSingle = "set:bedwar_toolbar.json image:single_gray", iconSelf = "set:bedwar_toolbar.json image:self_gray" },
    }
end

local Teams = {}
local TeamAreas = {}

TeamConfig = {}

local function initTeams(teams)
    for _, item in pairs(teams) do
        if tonumber(GameConfig.mapId) == tonumber(item.mapId) then
            local id = tonumber(item.id)
            Teams[id] = {}
            Teams[id].id = id
            Teams[id].name = tostring(item.name)

            local initPos = StringUtil.split(tostring(item.initPos), ",")
            Teams[id].initPos = VectorUtil.newVector3i(initPos[1], initPos[2], initPos[3])
            Teams[id].resourcePointId = item.resourcePointId

            Teams[id].bedPos = {}
            if item.bedPos then
                local bedPos = StringUtil.split(tostring(item.bedPos), "#")
                local pos1 = StringUtil.split(tostring(bedPos[1]), ",")
                local pos2 = StringUtil.split(tostring(bedPos[2]), ",")
                Teams[id].bedPos[1] = VectorUtil.newVector3(pos1[1], pos1[2], pos1[3])
                Teams[id].bedPos[2] = VectorUtil.newVector3(pos2[1], pos2[2], pos2[3])
            else
                Teams[id].bedPos[1] = VectorUtil.newVector3i(item.bedStartPosX, item.bedStartPosY, item.bedStartPosZ)
                Teams[id].bedPos[2] = VectorUtil.newVector3i(item.bedEndPosX, item.bedEndPosY, item.bedEndPosZ)
            end

            if isClient then
                local bedBlock = cBlockManager.cGetBlockById(BlockID.BED)
                if bedBlock then
                    bedBlock:setPositionValue(Teams[id].bedPos[1], id - 1)
                    bedBlock:setPositionValue(Teams[id].bedPos[2], id - 1)
                end
            end

            Teams[id].bedYaw = tonumber(item.bedYaw or 0)
            Teams[id].schemeticGroupId = tonumber(item.schemeticGroupId or 0)
            Teams[id].clothBlockId = tonumber(item.clothBlockId or 35)
            local cannotPlaceBlockArea = {}
            if item.cannotPlaceBlock then
                local cannotPlacePos = StringUtil.split(tostring(item.cannotPlaceBlock), "#")
                cannotPlaceBlockArea[1] = StringUtil.split(tostring(cannotPlacePos[1]), ",")
                cannotPlaceBlockArea[2] = StringUtil.split(tostring(cannotPlacePos[2]), ",")
            else
                cannotPlaceBlockArea[1] = {}
                cannotPlaceBlockArea[1][1] = tonumber(item.cannotPlaceBlockStartPosX)
                cannotPlaceBlockArea[1][2] = tonumber(item.cannotPlaceBlockStartPosY)
                cannotPlaceBlockArea[1][3] = tonumber(item.cannotPlaceBlockStartPosZ)
                cannotPlaceBlockArea[2] = {}
                cannotPlaceBlockArea[2][1] = tonumber(item.cannotPlaceBlockEndPosX)
                cannotPlaceBlockArea[2][2] = tonumber(item.cannotPlaceBlockEndPosY)
                cannotPlaceBlockArea[2][3] = tonumber(item.cannotPlaceBlockEndPosZ)
            end

            TeamAreas[id] = Area.new(cannotPlaceBlockArea)
            Teams[id].playerInArea = {}
            Teams[id].initItems = {}

            local initItems = StringUtil.split(tostring(item.initItems), "#")
            if initItems then
                for _, itemId in ipairs(initItems) do
                    table.insert(Teams[id].initItems, tonumber(itemId))
                end
            end

            local homePos = StringUtil.split(tostring(item.homeAreaPos), "#")
            if #homePos < 2 then
                homePos = {
                    "0,0,0",
                    "0,0,0",
                }
            end
            local pos1 = StringUtil.split(tostring(homePos[1]), ",")
            local pos2 = StringUtil.split(tostring(homePos[2]), ",")
            Teams[id].homeArea = IArea.new()
            Teams[id].homeArea:init(pos1[1], pos2[1], pos1[2], pos2[2], pos1[3], pos2[3])
        end
    end
end

function TeamConfig:getTeams()
    return Teams
end

function TeamConfig:getTeamCount()
    return #Teams
end

function TeamConfig:getTeam(id)
    local team = Teams[id]
    if team == nil then
        team = Teams[1]
    end
    return team
end

function TeamConfig:getTeamColor(id)
    return TeamColor[tonumber(id)] or TeamColor[1]
end

function TeamConfig:isInTeamArea(vec3)
    for _, area in pairs(TeamAreas) do
        local inArea = area:BlockPlaceArea(vec3)
        if inArea == true then
            return true
        end
    end
    return false
end

function TeamConfig:onPlayerMove(player, x, y, z)
    if not GameMatch.allowPvp then
        return
    end
    for teamId, team in pairs(Teams) do
        ---检查玩家是否在家.
        if tostring(teamId) == tostring(player.teamId) then
            if team.homeArea:inArea(VectorUtil.newVector3(x, y, z)) then
                player.info:setValue("inHome", "1")
            else
                player.info:setValue("inHome", "0")
            end
        end
    end
end

function TeamConfig:isBeChecked(id, playerInArea)
    for _, userId in pairs(playerInArea) do
        if id == userId then
            return true
        end
    end
    return false
end

function TeamConfig:getTeamMaxMinDiff()
    local max = 0
    local min = 9999
    if Teams then
        for _, team in pairs(Teams) do
            local teamInfo = GameMatch.Teams[team.id]
            if teamInfo then
                if teamInfo.curPlayers and teamInfo.curPlayers > max then
                    max = teamInfo.curPlayers
                end
                if teamInfo.curPlayers and teamInfo.curPlayers < min then
                    min = teamInfo.curPlayers
                end
            else
                min = 0
            end
        end
        if max - min >= 0 then
            return max - min
        else
            return -1
        end
    end
    return -1
end

initTeams(FileUtil.getConfigFromCsv("setting/TeamConfig.csv"))
