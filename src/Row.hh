#pragma once
#include "defines.hh"

#include <cstdint>
#include <string>

class Row
{
public:
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];
  bool truncated{};

  Row(uint32_t id, const std::string &username, const std::string &email);

  void serialize_into_dest(void *destination);

  static void deserialize_into_row(void *source, Row *destination);

  void console_display();
};

const uint32_t ID_SIZE         = mem_sz(Row, id);
const uint32_t USERNAME_SIZE   = mem_sz(Row, username);
const uint32_t EMAIL_SIZE      = mem_sz(Row, email);
const uint32_t ID_OFFSET       = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET    = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE        = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
