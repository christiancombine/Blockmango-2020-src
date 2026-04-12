CheckInConfig = {}
CheckInConfig.data = {}

function CheckInConfig:init(config)
	local reward1 = {}
	local reward2 = {}
	local reward3 = {}
	local reward4 = {}
	local reward5 = {}
	for _, v in pairs(config) do
		local day = tonumber(v.day)

		local r1 = {}
		r1.day = day
		r1.rewardId = tonumber(v.rewardId1)
		r1.type = tonumber(v.type1)
		r1.num = tonumber(v.num1)
		r1.icon = v.icon1
		r1.name = v.name1
		r1.level = tonumber(v.level1)

		local r2 = {}
		r2.day = day
		r2.rewardId = tonumber(v.rewardId2)
		r2.type = tonumber(v.type2)
		r2.num = tonumber(v.num2)
		r2.icon = v.icon2
		r2.name = v.name2
		r2.level = tonumber(v.level2)

		local r3 = {}
		r3.day = day
		r3.rewardId = tonumber(v.rewardId3)
		r3.type = tonumber(v.type3)
		r3.num = tonumber(v.num3)
		r3.icon = v.icon3
		r3.name = v.name3
		r3.level = tonumber(v.level3)

		local r4 = {}
		r4.day = day
		r4.rewardId = tonumber(v.rewardId4)
		r4.type = tonumber(v.type4)
		r4.num = tonumber(v.num4)
		r4.icon = v.icon4
		r4.name = v.name4
		r4.level = tonumber(v.level4)

		local r5 = {}
		r5.day = day
		r5.rewardId = tonumber(v.rewardId5)
		r5.type = tonumber(v.type5)
		r5.num = tonumber(v.num5)
		r5.icon = v.icon5
		r5.name = v.name5
		r5.level = tonumber(v.level5)

		reward1[day] = r1
		reward2[day] = r2
		reward3[day] = r3
		reward4[day] = r4
		reward5[day] = r5
	end
	self.data = {
		reward1,
		reward2,
		reward3,
		reward4,
		reward5
	}
end

function CheckInConfig:getRewardDataByFulfil(fulfil)
	local rewardItems = {}
	if not next(self.data) then
		return nil
	end
	local rewards = {}
	if fulfil == 0 then
		rewards = self.data[1]
	elseif fulfil == 1 then
		rewards = self.data[2]
	elseif fulfil == 2 then
		rewards = self.data[3]
	elseif fulfil == 3 then
		rewards = self.data[4]
	elseif fulfil == 4 then
		rewards = self.data[5]
	end
	--  about item.state :
	--      1 -> can not checkIn yet
	--      2 -> can checkIn now
	--      3 -> already checkIn
	for _, v in pairs(rewards) do
		local item = {}
		item.id = v.day
		item.name = v.name
		item.icon = v.icon
		item.itemId = v.rewardId
		item.num = v.num
		item.type = v.type
		item.level = v.level
		item.state = 1
		rewardItems[item.id] = item
	end
	return rewardItems
end

return CheckInConfig