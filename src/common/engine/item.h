#pragma once

#include <cstdint>
#include <memory>

namespace spacel {
namespace engine {

enum ItemType {
	ITEMTYPE_USELESS,
	ITEMTYPE_RESOURCE,
	ITEMTYPE_CURRENCY,
	ITEMTYPE_TOOL,
};

struct ItemDef
{
	ItemDef() {}
	~ItemDef() {}
	uint32_t id = 0;
	ItemType type = ITEMTYPE_USELESS;
	std::string name;
	std::string description = "";
	std::string icon = "";
	uint32_t stack_max = 99;
};

typedef std::shared_ptr<ItemDef> ItemDefPtr;

class ItemStack
{
public:
	ItemStack(uint32_t item_id, uint16_t count = 0):
			m_item_id(item_id), m_item_count(count) {}

	uint16_t AddItems(const uint16_t count);
	bool RemoveItems(const uint16_t count);
	void SetItemCount(const uint16_t count);

	uint32_t GetItemID() const { return m_item_id; }
	uint16_t GetItemCount() const { return m_item_count; }
	bool IsEmpty() const { return m_item_count == 0; }
private:
	uint32_t m_item_id = 0;
	uint16_t m_item_count = 0;
};

typedef std::shared_ptr<ItemStack> ItemStackPtr;
}
}
