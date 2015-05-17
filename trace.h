/*
    ddb_customizabletb - Customizable toolbar plugin for DeaDBeeF Music player
    Copyright (C) 2015 Evgeny Kravchenko <cravchik@yandex.ru>

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

#ifndef TRACE_H
#define TRACE_H

#include <gtk/gtk.h>
#include <stdio.h>

#if GTK_CHECK_VERSION(3,0,0)
#define TRACE_HEADER "[ddb_customizabletb_gtk3] "
#else
#define TRACE_HEADER "[ddb_customizabletb_gtk2] "
#endif

#define trace(fmt,...) fprintf(stderr, TRACE_HEADER fmt, ##__VA_ARGS__)

#endif