#include "Pager.hh"
#include "Table.hh"
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

Pager::Pager(const std::string &filename)
{
  int fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  if (fd == -1)
    throw std::runtime_error("Could not open file.");

  this->file_descriptor = fd;
  this->file_length     = lseek(fd, 0, SEEK_END);
  this->num_pages       = (file_length / PAGE_SIZE);

  if (file_length % PAGE_SIZE != 0)
  {
    std::cerr << "Db file is not a whole number of pages. Corrupt file.\n";
    std::exit(1);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    this->pages[i] = nullptr;
  }
}

Pager::~Pager()
{
  close(this->file_descriptor);
}

auto Pager::get_page(int64_t page_num) -> void *
{
  if (page_num > TABLE_MAX_PAGES)
  {
    std::cerr << "Error tried to fetch page number out of bounds\n";
    exit(1);
  }

  if (pages[page_num] == NULL)
  {
    void *page = malloc(PAGE_SIZE);
    int64_t np = file_length / PAGE_SIZE;

    if (this->file_length % PAGE_SIZE)
      np += 1;

    if (page_num <= np)
    {
      lseek(this->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(this->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1)
      {
        std::cerr << "Error reading file: " << errno << "\n";
        exit(1);
      }
    }

    if (page_num >= num_pages)
      num_pages = page_num + 1;

    this->pages[page_num] = page;
  }

  return this->pages[page_num];
}

void Pager::flush(int64_t page_num)
{
  if (pages[page_num] == NULL)
  {
    std::cerr << "Attempted to flush a null page\n";
    exit(1);
  }

  off_t offset = lseek(file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  if (offset == -1)
  {
    std::cerr << "Error seeking: " << errno << "\n";
    exit(1);
  }

  ssize_t bytes_written = write(file_descriptor, pages[page_num], PAGE_SIZE);
  if (bytes_written == -1)
  {
    std::cerr << "Error writing: " << errno << "\n";
    exit(1);
  }
}

auto Pager::get_unused_page() -> uint32_t
{
  return static_cast<uint32_t>(num_pages);
}
