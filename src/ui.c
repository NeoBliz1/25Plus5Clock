#include "../include/ui.h"
#include "../include/callbacks.h"

void update_timer_display(const TimerApp *app) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", app->minutes, app->seconds);
    gtk_label_set_text(GTK_LABEL(app->timer_label), buf);
    gtk_label_set_text(GTK_LABEL(app->state_label), app->is_session ? "Session" : "Break");
}

static void on_window_destroyed(GtkWidget *widget, gpointer data) {
    TimerApp *app = data;
    if (app->timer_id > 0) {
        g_source_remove(app->timer_id);
        app->timer_id = 0;
    }
}

void create_ui(TimerApp *app) {
    // --- STYLE LOADER ---
    GtkCssProvider *provider = gtk_css_provider_new();

    // Load from the internal resource path instead of a file
    gtk_css_provider_load_from_resource(provider, APP_STYLE_RESOURCE);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);

    // --- LOAD PERSISTED STATE FROM GSETTINGS (TRACKING RAW SECONDS) ---
    const int session_seconds = g_settings_get_int(app->settings, "work-duration");
    const int break_seconds = g_settings_get_int(app->settings, "break-duration");

    // --- STATE INITIALIZATION ---
    app->session_total_seconds = session_seconds;
    app->break_total_seconds = break_seconds;
    app->minutes = app->session_total_seconds / 60;
    app->seconds = app->session_total_seconds % 60;
    app->is_running = FALSE;
    app->is_session = TRUE;

    // --- WINDOW SETUP ---
    GtkWidget *win = app->window;
    gtk_window_set_title(GTK_WINDOW(win), "25+5 Clock");
    gtk_window_set_default_size(GTK_WINDOW(win), 300, 400);
    gtk_container_set_border_width(GTK_CONTAINER(win), 20);
    GError *error = NULL;
    GdkPixbuf *icon = gdk_pixbuf_new_from_resource(APP_ICON_RESOURCE, &error);
    if (icon) {
        gtk_window_set_icon(GTK_WINDOW(app->window), icon);
        g_object_unref(icon);
    } else {
        g_warning("Could not load bundled icon: %s", error->message);
        g_error_free(error);
    }
    gtk_window_set_role(GTK_WINDOW(app->window), "timer");
    g_signal_connect(app->window, "destroy", G_CALLBACK(on_window_destroyed), app);

    // --- LAYOUT ENGINE ---
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    app->state_label = gtk_label_new("Session");
    gtk_box_pack_start(GTK_BOX(vbox), app->state_label, FALSE, FALSE, 0);

    app->timer_label = gtk_label_new(NULL);
    gtk_widget_set_name(app->timer_label, "main-timer");
    gtk_label_set_markup(GTK_LABEL(app->timer_label), "<span font_desc='36.0' weight='bold'>25:00</span>");
    gtk_box_pack_start(GTK_BOX(vbox), app->timer_label, TRUE, TRUE, 0);

    app->controls_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), app->controls_container, FALSE, FALSE, 0);

    // --- CONTROLS LOOP ---
    for (int i = 0; i < 2; i++) {
        const char *labels[] = {"Session", "Break"};
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(labels[i]), TRUE, TRUE, 0);

        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(entry), 3);

        const int display_minutes = ((i == 0 ? session_seconds : break_seconds) + 30) / 60;
        char *default_text = g_strdup_printf("%d", display_minutes);
        gtk_entry_set_text(GTK_ENTRY(entry), default_text);
        g_free(default_text);

        g_signal_connect(entry, "activate", G_CALLBACK(on_entry_changed), app);
        g_signal_connect(entry, "focus-out-event", G_CALLBACK(on_focus_out), app);

        gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
        if (i == 0) app->session_entry = entry;
        else app->break_entry = entry;

        GtkWidget *btn_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *p = gtk_button_new_with_label("+");
        GtkWidget *m = gtk_button_new_with_label("-");
        gtk_widget_set_name(p, i == 0 ? "s" : "b");
        gtk_widget_set_name(m, i == 0 ? "s" : "b");

        g_signal_connect(p, "clicked", G_CALLBACK(on_adjust_clicked), app);
        g_signal_connect(m, "clicked", G_CALLBACK(on_adjust_clicked), app);

        gtk_box_pack_start(GTK_BOX(btn_vbox), p, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(btn_vbox), m, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), btn_vbox, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(app->controls_container), hbox, FALSE, FALSE, 0);
    }

    // --- ACTION BUTTONS ---
    GtkWidget *actions_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    app->start_button = gtk_button_new_with_label("Start");
    gtk_style_context_add_class(gtk_widget_get_style_context(app->start_button), "green-btn");
    g_signal_connect(app->start_button, "clicked", G_CALLBACK(on_start_stop_clicked), app);

    GtkWidget *reset_btn = gtk_button_new_with_label("Reset");
    gtk_style_context_add_class(gtk_widget_get_style_context(reset_btn), "red-btn");
    g_signal_connect(reset_btn, "clicked", G_CALLBACK(on_reset_clicked), app);

    gtk_box_pack_start(GTK_BOX(actions_hbox), app->start_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(actions_hbox), reset_btn, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), actions_hbox, FALSE, FALSE, 0);

    update_timer_display(app);
}
