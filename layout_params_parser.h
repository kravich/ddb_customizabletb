/*
    ddb_customizabletb - Customizable toolbar plugin for DeaDBeeF Music player
    Copyright (C) 2021 Evgeny Kravchenko <cravchik@yandex.ru>

    This file is part of ddb_customizabletb.

    ddb_customizabletb is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ddb_customizabletb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ddb_customizabletb.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LAYOUT_PARAMS_PARSER_H
#define __LAYOUT_PARAMS_PARSER_H

#include <stddef.h>

typedef void (*param_cb_t)(const char *key, size_t key_len, const char *value, size_t value_len, void *user_data);

const char* parse_params(const char *s, param_cb_t cb, void *user_data);

#endif