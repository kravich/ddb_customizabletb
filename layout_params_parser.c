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

#include "layout_params_parser.h"

#include <glib.h>

#include <assert.h>

const char* parse_params(const char *s, param_cb_t cb, void *user_data)
{
    assert(cb);

    enum
    {
        STATE_WS_BEFORE_KEY, // also initial state
        STATE_KEY,
        STATE_WS_AFTER_KEY,
        STATE_EQUAL,
        STATE_WS_BEFORE_VALUE,
        STATE_UNQUOTED_VALUE,
        STATE_OPENING_QUOTE,
        STATE_QUOTED_VALUE,
        STATE_ESCAPE_CHAR,
        STATE_CLOSING_QUOTE,
        STATE_CURLY_BRACE,  // final state
        STATE_ERROR,
    };

    int state = STATE_WS_BEFORE_KEY;

    const char *key = NULL;
    size_t key_len = 0;

    const char *value = NULL;
    size_t value_len = 0;

    size_t i = 0;
    for (; s[i] && state != STATE_CURLY_BRACE; i++)
    {
        char c = s[i];

        switch (state)
        {
            case STATE_WS_BEFORE_KEY:
                if (g_ascii_isalpha(c))
                {
                    assert(!key);
                    assert(!key_len);

                    key = s + i;
                    key_len++;
                    state = STATE_KEY;
                }
                else if (c == ' ')
                {
                    // stay in the same state
                }
                else if (c == '{')
                {
                    state = STATE_CURLY_BRACE;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_KEY:
                if (g_ascii_isalpha(c))
                {
                    key_len++;
                }
                else if (c == ' ')
                {
                    state = STATE_WS_AFTER_KEY;
                }
                else if (c == '=')
                {
                    state = STATE_EQUAL;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_WS_AFTER_KEY:
                if (c == ' ')
                {
                    // stay in the same state
                }
                else if (c == '=')
                {
                    state = STATE_EQUAL;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_EQUAL:
            case STATE_WS_BEFORE_VALUE:
                if (c == ' ')
                {
                    state = STATE_WS_BEFORE_VALUE;
                }
                else if (c == '"')
                {
                    state = STATE_OPENING_QUOTE;
                }
                else
                {
                    assert(!value);
                    assert(!value_len);

                    value = s + i;
                    value_len++;
                    state = STATE_UNQUOTED_VALUE;
                }
                break;

            case STATE_UNQUOTED_VALUE:
                if (c == '"')
                {
                    state = STATE_ERROR;
                }
                else if (c == ' ')
                {
                    cb(key, key_len, value, value_len, user_data);

                    key = NULL;
                    key_len = 0;

                    value = NULL;
                    value_len = 0;

                    state = STATE_WS_BEFORE_KEY;
                }
                else
                {
                    value_len++;
                }
                break;

            case STATE_OPENING_QUOTE:
                if (c == '"')
                {
                    assert(!value);
                    assert(!value_len);

                    // special case - empty value
                    value = "";
                    value_len = 0;
                    cb(key, key_len, value, value_len, user_data);

                    key = NULL;
                    key_len = 0;

                    value = NULL;
                    value_len = 0;

                    state = STATE_CLOSING_QUOTE;
                }
                else if (c == '\\')
                {
                    assert(!value);
                    assert(!value_len);

                    value = s + i;
                    value_len++;

                    state = STATE_ESCAPE_CHAR;
                }
                else
                {
                    assert(!value);
                    assert(!value_len);

                    value = s + i;
                    value_len++;

                    state = STATE_QUOTED_VALUE;
                }
                break;

            case STATE_QUOTED_VALUE:
                if (c == '"')
                {
                    cb(key, key_len, value, value_len, user_data);

                    key = NULL;
                    key_len = 0;

                    value = NULL;
                    value_len = 0;

                    state = STATE_CLOSING_QUOTE;
                }
                else if (c == '\\')
                {
                    value_len++;
                    state = STATE_ESCAPE_CHAR;
                }
                else
                {
                    // stay in the same state
                    value_len++;
                }
                break;

            case STATE_ESCAPE_CHAR:
                if (c == '"' || c == '\\')
                {
                    value_len++;
                    state = STATE_QUOTED_VALUE;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_CLOSING_QUOTE:
                if (c == ' ')
                {
                    state = STATE_WS_BEFORE_KEY;
                }
                else if (c == '{')
                {
                    state = STATE_CURLY_BRACE;
                }
                else
                {
                    state = STATE_ERROR;
                }
                break;

            case STATE_ERROR:
                if (c == '{')
                {
                    state = STATE_CURLY_BRACE;
                }
                break;
        }
    }

    // do not propagate errors outside for now

    return &s[i];
}