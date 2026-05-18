#include "../include/callbacks.h"
#include "../include/ui.h"
#include <canberra-gtk.h>

gboolean timer_tick(gpointer data) {
    TimerApp *app = data;

    // If the user paused, tell Linux to stop calling this function (return FALSE)
    if (!app->is_running) return FALSE;

    if (app->seconds == 0) {
        if (app->minutes == 0) {
            // PLAY SOUND: Uses the Linux system sound "complete"
            ca_gtk_play_for_widget(GTK_WIDGET(app->timer_label), 0,
                                   CA_PROP_EVENT_ID, "complete",
                                   CA_PROP_EVENT_DESCRIPTION, "Timer finished",
                                   CA_PROP_CANBERRA_ENABLE, "1",
                                   NULL);

            app->is_session = !app->is_session;
            GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(app->window));
            if (!app->is_session) {
                // STARTING BREAK
                if (gdk_window) {
                    const GdkWindowState state = gdk_window_get_state(gdk_window);
                    app->was_minimized = (state & GDK_WINDOW_STATE_ICONIFIED) != 0;
                    app->was_top_plane = (state & GDK_WINDOW_STATE_FOCUSED) != 0;

                    gdk_window_show(gdk_window);
                    gdk_window_raise(gdk_window);
                    gdk_window_focus(gdk_window, GDK_CURRENT_TIME);
                }
                gtk_window_fullscreen(GTK_WINDOW(app->window));
                gtk_window_set_keep_above(GTK_WINDOW(app->window), TRUE);
            } else {
                // RETURNING TO SESSION
                gtk_window_unfullscreen(GTK_WINDOW(app->window));
                gtk_window_set_keep_above(GTK_WINDOW(app->window), FALSE);

                if (app->was_minimized || !app->was_top_plane) {
                    gtk_window_iconify(GTK_WINDOW(app->window));
                }
            }

            // Pull the total seconds and split them
            const int next_total = app->is_session ? app->session_total_seconds : app->break_total_seconds;
            app->minutes = next_total / 60;
            app->seconds = next_total % 60;
        } else {
            app->minutes--;
            app->seconds = 59;
        }
    } else {
        app->seconds--;
    }

    update_timer_display(app);
    return TRUE;
}

// MANUAL INPUT: Handles when you type a number and press Enter.
void on_entry_changed(GtkWidget *widget, gpointer data) {
    TimerApp *app = data;
    if (app->is_running) return;

    const char *text = gtk_entry_get_text(GTK_ENTRY(widget));

    // 1. Convert commas to dots so the math works regardless of input style
    char *normalized = g_strdup(text);
    for (int i = 0; normalized[i]; i++) {
        if (normalized[i] == ',') normalized[i] = '.';
    }

    char *endptr;
    double input_minutes = g_ascii_strtod(normalized, &endptr);
    g_free(normalized);

    // 2. VALIDATION RULES
    // endptr == text means they typed letters, not numbers
    if (endptr == text || input_minutes <= 0) {
        // Reset to a safe default if invalid
        gtk_entry_set_text(GTK_ENTRY(widget), "1");
        input_minutes = 1.0;
    }

    // Max limit: 24 hours (1440 minutes)
    if (input_minutes > 1440) {
        gtk_entry_set_text(GTK_ENTRY(widget), "1440");
        input_minutes = 1440.0;
    }

    // 3. CONVERT TO SECONDS
    int total_seconds = (int)(input_minutes * 60 + 0.5);

    // 4. UPDATE STATE
    if (widget == app->session_entry) {
        app->session_total_seconds = total_seconds;
    } else {
        app->break_total_seconds = total_seconds;
    }

    // 5. UPDATE UI (Only if editing the active mode)
    update_if_active(widget, app, total_seconds);
}

// ADJUST BUTTONS: Logic for the + and - buttons
void on_adjust_clicked(GtkWidget *btn, gpointer data) {
    TimerApp *app = data;
    if (app->is_running) return;

    const char *label = gtk_button_get_label(GTK_BUTTON(btn));
    // Determine which total we are editing
    int *total_sec = (g_strcmp0(gtk_widget_get_name(btn), "s") == 0) ?
                     &app->session_total_seconds : &app->break_total_seconds;
    GtkWidget *widget = (g_strcmp0(gtk_widget_get_name(btn), "s") == 0) ?
                       app->session_entry : app->break_entry;

    // 1. Get current value in full minutes
    int current_mins = *total_sec / 60;

    // 2. Increment/Decrement by 1 full minute
    if (g_strcmp0(label, "+") == 0) {
        if (current_mins < 1440) current_mins++;
    } else {
        if (current_mins > 1) current_mins--;
    }

    // 3. Save back as total seconds
    *total_sec = current_mins * 60;

    // 4. Update the Entry box (remove decimals for clean look)
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", current_mins);
    gtk_entry_set_text(GTK_ENTRY(widget), buf);

    update_if_active(widget, app, *total_sec);
}

// START/PAUSE: Toggles the 1-second interval
void on_start_stop_clicked(GtkWidget *btn, gpointer data) {
    TimerApp *app = data;
    app->is_running = !app->is_running;

    if (app->is_running) {
        gtk_button_set_label(GTK_BUTTON(btn), "Pause");
        // Start the heartbeat!
        gtk_widget_hide(app->controls_container);
        app->timer_id = g_timeout_add(1000, timer_tick, app);
    } else {
        gtk_button_set_label(GTK_BUTTON(btn), "Start");
        gtk_window_unfullscreen(GTK_WINDOW(app->window));
        // Remove the heartbeat to save CPU
        gtk_widget_show(app->controls_container);
        if (app->timer_id > 0) {
            g_source_remove(app->timer_id);
            app->timer_id = 0;
        }
    }
}

// RESET: Puts everything back to default
void on_reset_clicked(GtkWidget *btn, gpointer data) {
    TimerApp *app = data;

    // 1. Stop any active timer
    if (app->timer_id > 0) {
        g_source_remove(app->timer_id);
        app->timer_id = 0;
    }

    // 2. Reset Logic State using your variables
    app->is_running = FALSE;
    app->is_session = TRUE;

    // Convert minutes to total seconds (0.1 -> 6 seconds)
    app->session_total_seconds = g_settings_get_int(app->settings, "work-duration");
    app->break_total_seconds = g_settings_get_int(app->settings, "break-duration");

    const int session_minutes = app->session_total_seconds / 60;
    // Set the display state to match the session start
    app->minutes = session_minutes;
    app->seconds = app->session_total_seconds % 60;

    // 3. Update UI Buttons and Labels
    gtk_button_set_label(GTK_BUTTON(app->start_button), "Start");

    // 4. Update Input Boxes (Convert double to string)
    char session_buf[16], break_buf[16];
    g_ascii_formatd(session_buf, sizeof(session_buf), "%g", session_minutes);
    g_ascii_formatd(break_buf, sizeof(break_buf), "%g", app->break_total_seconds / 60);

    gtk_entry_set_text(GTK_ENTRY(app->session_entry), session_buf);
    gtk_entry_set_text(GTK_ENTRY(app->break_entry), break_buf);

    // 5. Final Display Refresh
    update_timer_display(app);
    gtk_widget_show(app->controls_container);
    gtk_window_unfullscreen(GTK_WINDOW(app->window));
    gtk_window_set_keep_above(GTK_WINDOW(app->window), FALSE);
}

// Focus events have a different signature than button clicks
gboolean on_focus_out(GtkWidget *widget, GdkEvent *event, gpointer data) {
    on_entry_changed(widget, data);
    return FALSE;
}

void update_if_active(GtkWidget *widget, TimerApp *app, int total_seconds) {
    const gboolean editing_session = (widget == app->session_entry);
    if (editing_session) {
        app->session_total_seconds = total_seconds;
        g_settings_set_int(app->settings, "work-duration", total_seconds);
    } else {
        app->break_total_seconds = total_seconds;
        g_settings_set_int(app->settings, "break-duration", total_seconds);
    }
    if ((editing_session && app->is_session) || (!editing_session && !app->is_session)) {
        app->minutes = total_seconds / 60;
        app->seconds = total_seconds % 60;
        update_timer_display(app);
    }
}