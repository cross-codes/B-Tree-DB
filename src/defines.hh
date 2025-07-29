#pragma once

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#define TABLE_MAX_PAGES 100
#define INVALID_PAGE_NUM 4294967295U;

#define mem_sz(Struct, member) sizeof(((Struct *)0)->member)
