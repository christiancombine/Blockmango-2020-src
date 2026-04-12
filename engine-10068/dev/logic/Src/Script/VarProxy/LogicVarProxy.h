struct ExplosionAttr
{
	bool needSync{};
	bool isCanHurt{};
	bool isBreakBlock{};
	bool isHurtEntityItem{};
	float custom_explosionSize{};
	float custom_explosionDamageFactor{};
	float custom_explosionRepelDistanceFactor{};
	int sourceItemId{};
};