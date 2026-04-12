require "data.MulticastEvent"
TaskHelper = {}

TaskType = {
    HaveLevelBirdEvent = 6,  --拥有小鸟事件  (小鸟等级，小鸟总数量)
    FeedBirdEvent = 7,  --给小鸟喂食事件  （食物id，数量）
    CollectFieldFruitEvent = 2, --采集水果  （田地ID, 数量）
    CollectFruitByNum = 21,           --采集水果（num）
    CollectFruitByColor = 22,        --采集水果 （num, color）
    BeaMonsterTaskEvent = 1,    -- 打败怪物事件（id, num）
    CollectPropEvent = 8,       --收集道具事件 （id, num）
    HaveChestEvent = 9,         --得到宝箱事件  （指定数量）
    NpcDialogueEvent = 3,       --与NPC 对话事件 （id,num）
    GetItemByOpenChestEvent = 10, -- 玩家通过打开箱子获得物品(id, num)
    CompleteNpcTaskNumEvent = 4, --完成指定npc的（npcId,num)
    HaveBirdShellsEvent = 5, -- 拥有多少小鸟的携带位(数量)
    DrawLuckyEvent = 23,      --抽蛋事件
}

TaskHelper = {}

for _, val in pairs(TaskType) do
    TaskHelper[val] = MulticastEvent.new()
end

-- rakssid  id  num  color
function TaskHelper.dispatch(taskType, ...)
    local action = TaskHelper[taskType]
    if type(action) == "table" then
        action:Invoke(...)
    end
end

function TaskHelper.remove(key)
    for _, action in pairs(TaskHelper) do
        if type(action) == 'table' then
            action:Remove(key)
        end
    end
end