///Add
#ifdef ENABLE_SORT_INVEN
void CHARACTER::SortInven(BYTE option)
{
	if (IsDead()) 
		return;
	if (GetLastSortTime() > get_global_time()) {
		ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", GetLastSortTime() - get_global_time());
		return;
	}

	std::vector<LPITEM> all;
	LPITEM myitems;
	const auto size = static_cast<WORD>(INVENTORY_MAX_NUM);

	for (WORD i = 0; i < size; ++i) {
		if (myitems = GetInventoryItem(i)) {
			all.emplace_back(myitems);
			myitems->RemoveFromCharacter();
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), 255);
		}
	}
	if (all.empty())
		return;
	std::sort(all.begin(), all.end(), [option](const LPITEM i1, const LPITEM i2) {
		switch (option) {
		case 2:
			return i1->CustomSort() == i2->CustomSort() ? i1->GetSubType() < i2->GetSubType() : i1->CustomSort() < i2->CustomSort();
		case 3:
			return i1->GetLevelLimit() == i2->GetLevelLimit() ? i1->GetSubType() < i2->GetSubType() : i1->GetLevelLimit() > i2->GetLevelLimit();
		default:
			return std::strcmp(i1->GetName(), i2->GetName()) < 0;
		}
	});
	for (const auto& getitem : all) {
		const auto table = ITEM_MANAGER::instance().GetTable(getitem->GetVnum());
		if (!table)
			continue;
		static const std::initializer_list<BYTE> out = { ITEM_BLEND, ITEM_USE }; // edit
		if (table->dwFlags & ITEM_FLAG_STACKABLE && std::find(out.begin(), out.end(), table->bType) == out.end())
			AutoGiveItem(getitem->GetVnum(), getitem->GetCount(), -1, false);
		else
			AutoGiveItem(getitem);
	};
	SetLastSortTime(get_global_time() + 15); // 15 sec
}
#endif
