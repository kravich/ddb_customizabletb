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

#include "w_toolbar.h"
#include "trace.h"

#include "dbapi.h"

DB_functions_t *g_deadbeef = NULL;
ddb_gtkui_t *g_gtkui = NULL;

static int toolbar_connect(void)
{
    g_gtkui = (ddb_gtkui_t*)g_deadbeef->plug_get_for_id(DDB_GTKUI_PLUGIN_ID);

    if (g_gtkui == NULL)
    {
#if GTK_CHECK_VERSION(3,0,0)
        trace("Failed to connect to GTKUI(GTK3), deactivating\n");
#else
        trace("Failed to connect to GTKUI(GTK2), deactivating\n");
#endif
        return -1;
    }

    // register widget
    g_gtkui->w_reg_widget("Customizable toolbar", 0, w_toolbar_create, "customizable_toolbar", NULL);

    return 0;
}

static DB_misc_t toolbar_plugin =
{
    DDB_REQUIRE_API_VERSION(1, TARGET_API_LEVEL)

    .plugin.type = DB_PLUGIN_MISC,

    .plugin.version_major = 0,
    .plugin.version_minor = 2,

#if GTK_CHECK_VERSION(3,0,0)
    .plugin.id = "ddb_customizabletb_gtk3",
    .plugin.name = "Customizable toolbar for GTKUI(GTK3)",
#else
    .plugin.id = "ddb_customizabletb_gtk2",
    .plugin.name = "Customizable toolbar for GTKUI(GTK2)",
#endif

    .plugin.connect = toolbar_connect,

    .plugin.descr =
        "This plugin implements customizable toolbar widget that could be used\n"
        "instead of deadbeef's default fixed toolbar.\n"
        "Use deadbeef's layout design mode (View->Design Mode) to add toolbar\n"
        "widget to layout.\n"
        "To customize toolbar, while being in design mode, click with right mouse\n"
        "button on a toolbar widget and select \"Customize\"\n",

    .plugin.copyright =
        "ddb_customizabletb - Customizable toolbar plugin for DeaDBeeF Music player\n"
        "Copyright (C) 2015 Evgeny Kravchenko <cravchik@yandex.ru>\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 3 of the License, or\n"
        "(at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n",

    .plugin.website = "https://github.com/kravich/ddb_customizabletb"
};

#if GTK_CHECK_VERSION(3,0,0)
DB_plugin_t* ddb_customizabletb_gtk3_load(DB_functions_t *db_api)
#else
DB_plugin_t* ddb_customizabletb_gtk2_load(DB_functions_t *db_api)
#endif
{
    g_deadbeef = db_api;
    return (DB_plugin_t*)&toolbar_plugin;
}
