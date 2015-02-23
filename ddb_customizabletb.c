#include <stdio.h>

#define DDB_API_LEVEL 5
#define DDB_WARN_DEPRECATED 1
#include <deadbeef/deadbeef.h>

#include "w_toolbar.h"

DB_functions_t *deadbeef = NULL;
ddb_gtkui_t *gtkui = NULL;

static DB_misc_t toolbar_plugin;

#if GTK_CHECK_VERSION(3,0,0)
DB_plugin_t* ddb_customizabletb_gtk3_load(DB_functions_t *db_api)
#else
DB_plugin_t* ddb_customizabletb_gtk2_load(DB_functions_t *db_api)
#endif
{
    deadbeef = db_api;
    return (DB_plugin_t*)&toolbar_plugin;
}

static int toolbar_connect(void)
{
    gtkui = (ddb_gtkui_t*)deadbeef->plug_get_for_id(DDB_GTKUI_PLUGIN_ID);

    if(gtkui == NULL)
    {
#if GTK_CHECK_VERSION(3,0,0)
        printf("Failed to connect to GTKUI(GTK3), deactivating\n");
#else
        printf("Failed to connect to GTKUI(GTK2), deactivating\n");
#endif
        return -1;
    }

    // register widget
    gtkui->w_reg_widget("Customizable toolbar", 0, w_toolbar_create, "customizable_toolbar", NULL);

    return 0;
}

static DB_misc_t toolbar_plugin =
{
    DDB_REQUIRE_API_VERSION(1, 0)

    .plugin.type = DB_PLUGIN_MISC,

    .plugin.version_major = 0,
    .plugin.version_minor = 1,

    .plugin.flags = 0,
    .plugin.reserved1 = 0,
    .plugin.reserved2 = 0,
    .plugin.reserved3 = 0,

#if GTK_CHECK_VERSION(3,0,0)
    .plugin.id = "ddb_customizabletb_gtk3",
    .plugin.name = "Customizable toolbar for GTKUI(GTK3)",
#else
    .plugin.id = "ddb_customizabletb_gtk2",
    .plugin.name = "Customizable toolbar for GTKUI(GTK2)",
#endif

    .plugin.descr = "",
    .plugin.copyright = "kravich",
    .plugin.website = "",

    .plugin.command = NULL,

    .plugin.start = NULL,
    .plugin.stop = NULL,

    .plugin.connect = toolbar_connect,
    .plugin.disconnect = NULL,

    .plugin.exec_cmdline = NULL,

    .plugin.get_actions = NULL,

    .plugin.message = NULL,

    .plugin.configdialog = NULL
};
