#ifndef UTILS_H
#define UTILS_H

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1

#include <deadbeef/deadbeef.h>

DB_plugin_action_t* find_action(char *action_name);

#endif
