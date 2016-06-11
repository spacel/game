#pragma once

#include <cstdint>

namespace spacel {
namespace engine {

enum ItemType {
	ITEMTYPE_NONE,
	ITEMTYPE_RESOURCE,
	ITEMTYPE_CURRENCY,
	ITEMTYPE_TOOL,
};

struct ItemDef
{
	ItemDef() {}
	~ItemDef() {}
	uint32_t id = 0;
	ItemType type = ITEMTYPE_NONE;
	uint32_t stack_max = 99;
};

class ItemStack
{
public:
	ItemStack(uint32_t item_id, uint32_t count = 1):
			m_item_id(item_id), m_item_count(count) {}
	bool AddItems(uint32_t count);
	bool RemoveItems(uint32_t count);
	uint32_t GetItemID() const { return m_item_id; }
	uint32_t GetItemCount() const { return m_item_count; }
private:
	uint32_t m_item_id;
	uint32_t m_item_count;
};

typedef std::shared_ptr<ItemStack> ItemStackPtr;
}
}
