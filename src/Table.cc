#include "Table.hh"

Table::Table() : num_rows{}
{
  std::fill(std::begin(pages), std::end(pages), nullptr);
}

Table::~Table()
{
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i)
  {
    if (pages[i] != nullptr)
    {
      std::free(pages[i]);
      pages[i] = nullptr;
    }
  }
}

auto Table::row_slot(uint32_t row_num) -> void *
{
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  if (page_num >= TABLE_MAX_PAGES)
    return nullptr;

  char *page = reinterpret_cast<char *>(pages[page_num]);
  if (page == nullptr)
  {
    page = reinterpret_cast<char *>(std::malloc(PAGE_SIZE));
    if (page == nullptr)
      return nullptr;

    pages[page_num] = page;
  }

  uint32_t row_offset  = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;

  return page + byte_offset;
}
