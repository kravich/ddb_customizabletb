#include "icon_selection_dialog.h"

#include <gtk/gtk.h>
#include <assert.h>

#include "interface.h"
#include "support.h"
#include "utils.h"

enum
{
    ICONS_COL_PIXBUF,
    ICONS_COL_NAME,
    ICONS_COLS_NUM
};

const char* icons_list[] =
{
    "audio-volume-high",
    "audio-volume-low",
    "audio-volume-medium",
    "audio-volume-muted",
    "microphone-sensitivity-high",
    "microphone-sensitivity-low",
    "microphone-sensitivity-medium",
    "microphone-sensitivity-muted",
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
    "network-transmit-receive",
    "network-transmit",
    "network-receive",
    "network-idle",
    "network-error",
    "network-offline",
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
    "task-due",
    "task-past-due",
    "appointment-soon",
    "appointment-missed",
    "alarm",
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
    "application-certificate",
    "application-rss+xml",
    "application-x-appliance",
    "audio-x-generic",
    "folder",
    "text-x-generic",
    "video-x-generic",
    "x-office-calendar",
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
    "trophy-gold"
};

const int icons_list_size = sizeof(icons_list) / sizeof(icons_list[0]);

GtkTreeModel* create_icons_list()
{
    GtkListStore *list = gtk_list_store_new(ICONS_COLS_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);

    for(int i = 0; i < icons_list_size; i++)
    {
        const char *icon_name = icons_list[i];

        GdkPixbuf *icon = create_pixbuf_by_icon_name(icon_name, 24);
        if(icon != NULL)
        {
            GtkTreeIter iter;
            gtk_list_store_append(list, &iter);
            gtk_list_store_set(list, &iter, ICONS_COL_PIXBUF, icon, ICONS_COL_NAME, icon_name, -1);

            g_object_unref(icon);
        }
        else
        {
            printf("failed to create icon %s, skipping\n", icon_name);
        }
    }

    return GTK_TREE_MODEL(list);
}

GtkTreePath* icons_list_find(GtkTreeModel *icons_list, const char *icon_name_to_find)
{
    GtkTreePath *path = NULL;
    GtkTreeIter row_iter;

    gboolean res = gtk_tree_model_get_iter_first(icons_list, &row_iter);
    while(res == TRUE)
    {
        char *curr_icon_name = NULL;

        gtk_tree_model_get(icons_list, &row_iter, ICONS_COL_NAME, &curr_icon_name, -1);

        if(g_str_equal(curr_icon_name, icon_name_to_find))
        {
            path = gtk_tree_model_get_path(icons_list, &row_iter);
            g_free(curr_icon_name);
            break;
        }

        g_free(curr_icon_name);

        res = gtk_tree_model_iter_next(icons_list, &row_iter);
    }

    return path;
}

void select_icon(GtkIconView *iconview, const char *icon_name)
{
    GtkTreeModel *icons_list = gtk_icon_view_get_model(iconview);
    assert(icons_list != NULL);

    GtkTreePath *path = icons_list_find(icons_list, icon_name);

    if(path != NULL)
    {
        gtk_icon_view_select_path(iconview, path);
        gtk_tree_path_free(path);
    }
}

char* get_selected_icon(GtkIconView *iconview)
{
    GtkTreeModel *icons_list = gtk_icon_view_get_model(iconview);
    assert(icons_list != NULL);

    GList *selected_items = gtk_icon_view_get_selected_items(iconview);

    if(selected_items == NULL)
        return NULL;

    GtkTreeIter curr_iter;

    GtkTreePath *first_selected_path = (GtkTreePath*)(selected_items->data);
    gtk_tree_model_get_iter(icons_list, &curr_iter, first_selected_path);

    g_list_free_full(selected_items, (GDestroyNotify)gtk_tree_path_free);

    char *selected_icon_name = NULL;

    gtk_tree_model_get(icons_list, &curr_iter, ICONS_COL_NAME, &selected_icon_name, -1);

    return selected_icon_name;
}

char* run_icon_selection_dialog(const char *current_icon_name)
{
    assert(current_icon_name != NULL);

    GtkWidget *d = create_icon_selection_dialog();

    GtkWidget *iconview = lookup_widget(d, "iconview");
    assert(iconview != NULL);

    GtkTreeModel *icons_list = create_icons_list();
    gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), icons_list);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), ICONS_COL_PIXBUF);
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), ICONS_COL_NAME);

    select_icon(GTK_ICON_VIEW(iconview), current_icon_name);

    char *new_icon_name = NULL;

    gint res = gtk_dialog_run(GTK_DIALOG(d));
    if(res == GTK_RESPONSE_OK)
    {
        new_icon_name = get_selected_icon(GTK_ICON_VIEW(iconview));
    }

    gtk_widget_destroy(d);

    return new_icon_name;
}
