#include "Table.hh"
#include "Node.hh"
#include <cstring>
#include <unistd.h>

Table::Table(std::string filename)
{
  this->pager         = new Pager(filename);
  this->root_page_num = 0;

  if (this->pager->num_pages == 0)
  {
    void *root_node = this->pager->get_page(0);
    std::memset(root_node, 0, PAGE_SIZE);
    Node::initialize_leaf_node(root_node);
    Node::set_node_root(root_node, true);
  }
}

void Table::close_proc()
{
  for (uint32_t i = 0; i < pager->num_pages; i++)
  {
    if (pager->pages[i] == NULL)
    {
      continue;
    }

    pager->flush(i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
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
