#include "Table.hh"
#include <unistd.h>

Table::Table(std::string filename) : num_rows{}
{
  this->pager = new Pager(filename);
  num_rows    = pager->file_length / ROW_SIZE;
}

auto Table::row_slot(uint32_t row_num) -> void *
{
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  char *page        = reinterpret_cast<char *>(this->pager->get_page(page_num));

  uint32_t row_offset  = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;

  return page + byte_offset;
}

void Table::close_proc()
{
  int64_t num_full_pages = num_rows / ROWS_PER_PAGE;

  for (uint32_t i = 0; i < num_full_pages; i++)
  {
    if (pager->pages[i] == NULL)
    {
      continue;
    }

    pager->flush(i, PAGE_SIZE);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  int64_t num_additional_rows = num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0)
  {
    int64_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL)
    {
      pager->flush(page_num, num_additional_rows * ROW_SIZE);
      free(pager->pages[page_num]);
      pager->pages[page_num] = NULL;
    }
  }

  close(pager->file_descriptor);
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    void *page = pager->pages[i];
    if (page != nullptr)
    {
      free(page);
      pager->pages[i] = NULL;
    }
  }
}
