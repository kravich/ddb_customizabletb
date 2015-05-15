#include "icon_selection_dialog.h"

#include <gtk/gtk.h>
#include <assert.h>

#include "interface.h"
#include "support.h"
#include "utils.h"
#include "trace.h"

enum
{
    CATEGORIES_COL_NAME,
    CATEGORIES_COL_ICONVIEW_MODEL,
    CATEGORIES_COLS_NUM
};

enum
{
    ICONS_COL_PIXBUF,
    ICONS_COL_NAME,
    ICONS_COLS_NUM
};

const char* volume_icons[] =
{
    "audio-volume-high",
    "audio-volume-low",
    "audio-volume-medium",
    "audio-volume-muted",
    "microphone-sensitivity-high",
    "microphone-sensitivity-low",
    "microphone-sensitivity-medium",
    "microphone-sensitivity-muted",
    NULL
};

const char* multimedia_icons[] =
{
    "media-playlist-repeat",
    "media-playlist-repeat-song",
    "media-playlist-shuffle",
    "media-playlist-consecutive",
    "media-skip-backward",
    "media-seek-backward",
    "media-playback-start",
    "media-seek-forward",
    "media-skip-forward",
    "media-playback-stop",
    "media-playback-pause",
    "media-eject",
    "media-record",
    "media-view-subtitles",
    NULL
};

const char* network_icons[] =
{
    "network-transmit-receive",
    "network-transmit",
    "network-receive",
    "network-idle",
    "network-error",
    "network-offline",
};

const char* weather_icons[] =
{
    "weather-clear",
    "weather-clear-night",
    "weather-few-clouds",
    "weather-few-clouds-night",
    "weather-fog",
    "weather-overcast",
    "weather-severe-alert",
    "weather-showers",
    "weather-showers-scattered",
    "weather-snow",
    "weather-storm",
    NULL
};

const char* navigation_icons[] =
{
    "go-first",
    "go-previous",
    "go-next",
    "go-last",
    "go-bottom",
    "go-down",
    "go-up",
    "go-top",
    "go-home",
    "go-jump",
    NULL
};

const char* editing_icons[] =
{
    "format-indent-less",
    "format-indent-more",
    "format-justify-center",
    "format-justify-fill",
    "format-justify-left",
    "format-justify-right",
    "format-text-direction-ltr",
    "format-text-direction-rtl",
    "format-text-bold",
    "format-text-italic",
    "format-text-underline",
    "format-text-strikethrough",
    "edit-clear",
    "edit-clear-all",
    "edit-copy",
    "edit-cut",
    "edit-delete",
    "edit-find-replace",
    "edit-paste",
    "edit-redo",
    "edit-select-all",
    "edit-select",
    "edit-undo",
    "document-properties",
    "document-new",
    "document-open",
    "document-open-recent",
    "document-save",
    "document-save-as",
    "document-send",
    "document-page-setup",
    "changes-allow",
    "changes-prevent",
    "object-flip-horizontal",
    "object-flip-vertical",
    "object-rotate-left",
    "object-rotate-right",
    "insert-image",
    "insert-link",
    "insert-object",
    "insert-text",
    "accessories-text-editor",
    NULL
};

const char* view_constols_icons[] =
{
    "view-list",
    "view-grid",
    "view-fullscreen",
    "view-restore",
    "zoom-fit-best",
    "zoom-in",
    "zoom-out",
    "zoom-original",
    "view-continuous",
    "view-paged",
    "view-dual",
    "view-wrapped",
    NULL
};

const char* calendar_icons[] =
{
    "task-due",
    "task-past-due",
    "appointment-soon",
    "appointment-missed",
    "alarm",
    NULL
};

const char* communication_icons[] =
{
    "mail-unread",
    "mail-read",
    "mail-replied",
    "mail-attachment",
    "mail-mark-important",
    "mail-send",
    "mail-send-receive",
    "call-start",
    "call-stop",
    "call-missed",
    "user-available",
    "user-offline",
    "user-idle",
    "user-invisible",
    "user-busy",
    "user-away",
    "user-status-pending",
    NULL
};

const char* devices_and_media_icons[] =
{
    "audio-input-microphone",
    "camera-web",
    "camera-photo",
    "input-keyboard",
    "printer",
    "video-display",
    "computer",
    "media-optical",
    "phone",
    "input-dialpad",
    "input-touchpad",
    "scanner",
    "audio-card",
    "input-gaming",
    "input-mouse",
    "multimedia-player",
    "audio-headphones",
    "audio-headset",
    "display-projector",
    "media-removable",
    "printer-network",
    "audio-speakers",
    "camera-video",
    "drive-optical",
    "drive-removable-media",
    "input-tablet",
    "network-wireless",
    "network-wired",
    "media-floppy",
    "media-flash",
    NULL
};

const char* content_types_icons[] =
{
    "application-certificate",
    "application-rss+xml",
    "application-x-appliance",
    "audio-x-generic",
    "folder",
    "text-x-generic",
    "video-x-generic",
    "x-office-calendar",
    NULL
};

const char* emotes_icons[] =
{
    "face-angel",
    "face-angry",
    "face-cool",
    "face-crying",
    "face-devilish",
    "face-embarrassed",
    "face-kiss",
    "face-laugh",
    "face-monkey",
    "face-plain",
    "face-raspberry",
    "face-sad",
    "face-shutmouth",
    "face-sick",
    "face-smile",
    "face-smile-big",
    "face-smirk",
    "face-surprise",
    "face-tired",
    "face-uncertain",
    "face-wink",
    "face-worried",
    "face-yawn",
    NULL
};

const char* general_icons[] =
{
    "edit-find",
    "content-loading",
    "open-menu",
    "view-more",
    "tab-new",
    "bookmark-new",
    "mark-location",
    "find-location",
    "send-to",
    "object-select",
    "window-close",
    "view-refresh",
    "process-stop",
    "action-unavailable",
    "document-print",
    "printer-printing",
    "printer-warning",
    "printer-error",
    "dialog-information",
    "dialog-question",
    "dialog-warning",
    "dialog-password",
    "dialog-error",
    "list-add",
    "list-remove",
    "non-starred",
    "semi-starred",
    "starred",
    "star-new",
    "security-low",
    "security-medium",
    "security-high",
    "user-trash",
    "user-trash-full",
    "emblem-system",
    "avatar-default",
    "emblem-synchronizing",
    "emblem-shared",
    "folder-download",
    "help-browser",
    NULL
};

const char* other_icons[] =
{
    "view-sort-ascending",
    "view-sort-descending",
    "document-revert",
    "address-book-new",
    "application-exit",
    "appointment-new",
    "contact-new",
    "document-print-preview",
    "folder-new",
    "help-about",
    "help-contents",
    "help-faq",
    "list-remove-all",
    "mail-forward",
    "mail-mark-junk",
    "mail-mark-notjunk",
    "mail-mark-read",
    "mail-mark-unread",
    "mail-message-new",
    "mail-reply-all",
    "mail-reply-sender",
    "pan-down",
    "pan-end",
    "pan-start",
    "pan-up",
    "system-lock-screen",
    "system-log-out",
    "system-run",
    "system-search",
    "system-reboot",
    "system-shutdown",
    "tools-check-spelling",
    "window-maximize",
    "window-minimize",
    "window-restore",
    "window-new",
    "accessories-calculator",
    "accessories-character-map",
    "accessories-dictionary",
    "multimedia-volume-control",
    "preferences-desktop-accessibility",
    "preferences-desktop-display",
    "preferences-desktop-font",
    "preferences-desktop-keyboard",
    "preferences-desktop-keyboard-shortcuts",
    "preferences-desktop-locale",
    "preferences-desktop-remote-desktop",
    "preferences-desktop-multimedia",
    "preferences-desktop-screensaver",
    "preferences-desktop-theme",
    "preferences-desktop-wallpaper",
    "preferences-system-privacy",
    "preferences-system-windows",
    "system-file-manager",
    "system-software-install",
    "system-software-update",
    "system-users",
    "user-info",
    "utilities-system-monitor",
    "utilities-terminal",
    "application-x-addon",
    "application-x-executable",
    "font-x-generic",
    "image-x-generic",
    "package-x-generic",
    "text-html",
    "text-x-generic-template",
    "text-x-preview",
    "text-x-script",
    "x-office-address-book",
    "x-office-document",
    "x-office-document-template",
    "x-office-presentation",
    "x-office-presentation-template",
    "x-office-spreadsheet",
    "x-office-spreadsheet-template",
    "x-package-repository",
    "applications-accessories",
    "applications-development",
    "applications-engineering",
    "applications-games",
    "applications-graphics",
    "applications-internet",
    "applications-multimedia",
    "applications-office",
    "applications-other",
    "applications-science",
    "applications-system",
    "applications-utilities",
    "preferences-desktop",
    "preferences-desktop-peripherals",
    "preferences-desktop-personal",
    "preferences-other",
    "preferences-system",
    "preferences-system-network",
    "system-help",
    "battery",
    "computer-apple-ipad",
    "colorimeter-colorhug",
    "drive-harddisk",
    "drive-harddisk-ieee1394",
    "drive-harddisk-system",
    "drive-multidisk",
    "media-optical-bd",
    "media-optical-cd-audio",
    "media-optical-dvd",
    "media-tape",
    "media-zip",
    "modem",
    "multimedia-player-apple-ipod-touch",
    "network-vpn",
    "pda",
    "phone-apple-iphone",
    "uninterruptible-power-supply",
    "emblem-default",
    "emblem-documents",
    "emblem-downloads",
    "emblem-favorite",
    "emblem-generic",
    "emblem-important",
    "emblem-mail",
    "emblem-new",
    "emblem-ok",
    "emblem-package",
    "emblem-photos",
    "emblem-readonly",
    "emblem-symbolic-link",
    "emblem-synchronized",
    "emblem-unreadable",
    "emblem-urgent",
    "emblem-videos",
    "emblem-web",
    "folder-documents",
    "folder-download",
    "folder-music",
    "folder-pictures",
    "folder-documents",
    "folder-publicshare",
    "folder-remote",
    "folder-saved-search",
    "folder-templates",
    "folder-videos",
    "network-server",
    "network-workgroup",
    "start-here",
    "user-bookmarks",
    "user-desktop",
    "user-home",
    "airplane-mode",
    "battery-caution-charging",
    "battery-caution",
    "battery-empty-charging",
    "battery-empty",
    "battery-full-charged",
    "battery-full-charging",
    "battery-full",
    "battery-good-charging",
    "battery-good",
    "battery-low-charging",
    "battery-low",
    "battery-missing",
    "bluetooth-active",
    "bluetooth-disabled",
    "channel-insecure",
    "channel-secure",
    "computer-fail",
    "display-brightness",
    "keyboard-brightness",
    "folder-drag-accept",
    "folder-open",
    "folder-visiting",
    "image-loading",
    "image-missing",
    "mail-signed",
    "mail-signed-verified",
    "network-cellular-3g",
    "network-cellular-4g",
    "network-cellular-edge",
    "network-cellular-gprs",
    "network-cellular-umts",
    "network-cellular-acquiring",
    "network-cellular-connected",
    "network-cellular-no-route",
    "network-cellular-offline",
    "network-cellular-signal-excellent",
    "network-cellular-signal-good",
    "network-cellular-signal-ok",
    "network-cellular-signal-weak",
    "network-cellular-signal-none",
    "network-vpn-acquiring",
    "network-vpn",
    "network-wired-acquiring",
    "network-wired-disconnected",
    "network-wired-no-route",
    "network-wired-offline",
    "network-wireless-acquiring",
    "network-wireless-connected",
    "network-wireless-encrypted",
    "network-wireless-hotspot",
    "network-wireless-no-route",
    "network-wireless-offline",
    "network-wireless-signal-excellent",
    "network-wireless-signal-good",
    "network-wireless-signal-ok",
    "network-wireless-signal-weak",
    "network-wireless-signal-none",
    "rotation-allowed",
    "rotation-locked",
    "software-update-available",
    "software-update-urgent",
    "sync-error",
    "sync-synchronizing",
    "touchpad-disabled",
    "trophy-bronze",
    "trophy-silver",
    "trophy-gold",
    NULL
};

typedef struct
{
    const char* context_name;
    const char** icon_names_arr;
} ContextRecord;

ContextRecord contexes[] =
{
    { "Volume", volume_icons },
    { "Multimedia", multimedia_icons },
    { "Network", network_icons },
    { "Weather", weather_icons },
    { "Navigation", navigation_icons },
    { "Editing", editing_icons },
    { "View Controls", view_constols_icons },
    { "Calendar", calendar_icons },
    { "Communication", communication_icons },
    { "Devices and Media", devices_and_media_icons },
    { "Content Types", content_types_icons },
    { "Emotes", emotes_icons },
    { "General", general_icons },
    { "Other", other_icons },
    { NULL, NULL }
};

GtkTreeModel* create_categories_list_store()
{
    GtkListStore *categories_list = gtk_list_store_new(CATEGORIES_COLS_NUM, G_TYPE_STRING, G_TYPE_OBJECT);

    GtkIconTheme *default_icon_theme = gtk_icon_theme_get_default();

    ContextRecord *current_context = contexes;
    while(current_context->context_name != NULL)
    {
        GtkListStore *icons_list = gtk_list_store_new(ICONS_COLS_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);

        const char **current_icon_name_ptr = current_context->icon_names_arr;
        while(*current_icon_name_ptr != NULL)
        {
            const char *icon_name = *current_icon_name_ptr;

            if(gtk_icon_theme_has_icon(default_icon_theme, icon_name) == FALSE)
            {
                current_icon_name_ptr++;
                continue;
            }

            GError *error = NULL;
            GdkPixbuf *icon = gtk_icon_theme_load_icon(default_icon_theme,
                                                       icon_name,
                                                       24,
                                                       GTK_ICON_LOOKUP_FORCE_SIZE,
                                                       &error);

            if(error != NULL)
            {
                trace("Failed to create icon %s: %s\n", icon_name, error->message);
                g_error_free(error);
                current_icon_name_ptr++;
                continue;
            }

            gtk_list_store_insert_with_values(icons_list,
                                              NULL,
                                              -1,
                                              ICONS_COL_PIXBUF, icon,
                                              ICONS_COL_NAME, icon_name,
                                              -1);

            g_object_unref(icon);

            current_icon_name_ptr++;
        }

        gtk_list_store_insert_with_values(categories_list,
                                          NULL,
                                          -1,
                                          CATEGORIES_COL_NAME, current_context->context_name,
                                          CATEGORIES_COL_ICONVIEW_MODEL, icons_list,
                                          -1);

        g_object_unref(icons_list);

        current_context++;
    }

    return GTK_TREE_MODEL(categories_list);
}

void on_categories_selection_changed(GtkTreeSelection *categories_selection, gpointer user_data)
{
    GtkIconView *iconview = GTK_ICON_VIEW(user_data);

    if(gtk_tree_selection_count_selected_rows(categories_selection) != 1)
    {
        gtk_icon_view_set_model(iconview, NULL);
        return;
    }

    GtkTreeModel *categories_list = NULL;
    GtkTreeIter iter;
    gtk_tree_selection_get_selected(categories_selection, &categories_list, &iter);

    GtkTreeModel *icons_list = NULL;

    gtk_tree_model_get(categories_list, &iter, CATEGORIES_COL_ICONVIEW_MODEL, &icons_list, -1);

    assert(icons_list != NULL);

    gtk_icon_view_set_model(iconview, icons_list);

    g_object_unref(icons_list);
}

void on_iconview_selection_changed(GtkIconView *iconview, gpointer user_data)
{
    GtkWidget *dialog = GTK_WIDGET(user_data);
    GtkWidget *ok_button = lookup_widget(dialog, "ok_button");

    assert(ok_button != NULL);

    GList *selected_items = gtk_icon_view_get_selected_items(GTK_ICON_VIEW(iconview));

    gboolean single_item_selected = FALSE;
    if(selected_items != NULL && selected_items->next == NULL)
        single_item_selected = TRUE;

    g_list_free_full(selected_items, (GDestroyNotify)gtk_tree_path_free);

    if(single_item_selected)
        gtk_widget_set_sensitive(ok_button, TRUE);
    else
        gtk_widget_set_sensitive(ok_button, FALSE);
}

void setup_icon_selection_dialog(GtkWidget *dialog)
{
    GtkWidget *categories_treeview = lookup_widget(dialog, "categories_treeview");
    GtkWidget *iconview = lookup_widget(dialog, "iconview");
    GtkWidget *ok_button = lookup_widget(dialog, "ok_button");

    assert(categories_treeview != NULL);
    assert(iconview != NULL);

    // setup list of categories
    GtkTreeViewColumn *category_name_col = gtk_tree_view_column_new();
    GtkCellRenderer *category_name_renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(category_name_col, category_name_renderer, TRUE);
    gtk_tree_view_column_add_attribute(category_name_col, category_name_renderer, "text", CATEGORIES_COL_NAME);

    gtk_tree_view_append_column(GTK_TREE_VIEW(categories_treeview), category_name_col);

    // setup list of icons
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), ICONS_COL_PIXBUF);
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), ICONS_COL_NAME);

    // connect category change handler
    GtkTreeSelection *categories_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(categories_treeview));
    g_signal_connect(categories_selection, "changed", G_CALLBACK(on_categories_selection_changed), iconview);

    // connect icon selection handler
    g_signal_connect(iconview, "selection-changed", G_CALLBACK(on_iconview_selection_changed), dialog);

    // make ok button insensitive by default
    gtk_widget_set_sensitive(ok_button, FALSE);

    GtkTreeModel *categories_list = create_categories_list_store();
    gtk_tree_view_set_model(GTK_TREE_VIEW(categories_treeview), categories_list);
    g_object_unref(categories_list);
}

char* get_selected_icon(GtkWidget *dialog)
{
    GtkWidget *iconview = lookup_widget(dialog, "iconview");
    assert(iconview != NULL);

    GtkTreeModel *icons_list = gtk_icon_view_get_model(GTK_ICON_VIEW(iconview));

    GList *selected_list = gtk_icon_view_get_selected_items(GTK_ICON_VIEW(iconview));

    if(selected_list  == NULL)
        return NULL;

    GtkTreePath *selected_path = (GtkTreePath*)(selected_list->data);

    GtkTreeIter iter;
    gtk_tree_model_get_iter(icons_list, &iter, selected_path);

    g_list_free_full(selected_list, (GDestroyNotify)gtk_tree_path_free);

    char *icon_name = NULL;
    gtk_tree_model_get(icons_list, &iter, ICONS_COL_NAME, &icon_name, -1);

    assert(icon_name != NULL);

    return icon_name;
}

char* run_icon_selection_dialog(GtkWindow *parent, const char *current_icon_name)
{
    assert(current_icon_name != NULL);

    GtkWidget *d = create_icon_selection_dialog();

    gtk_window_set_transient_for(GTK_WINDOW(d), parent);

    setup_icon_selection_dialog(d);

    char *new_icon_name = NULL;

    gint res = gtk_dialog_run(GTK_DIALOG(d));
    if(res == GTK_RESPONSE_OK)
    {
        new_icon_name = get_selected_icon(d);
    }

    gtk_widget_destroy(d);

    return new_icon_name;
}
