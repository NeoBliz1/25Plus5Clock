#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>

extern double default_session_minutes;
extern double default_break_minutes;
#define APP_RESOURCE_PREFIX "/com/github/neobliz1/25Plus5Clock"
#define APP_STYLE_RESOURCE  APP_RESOURCE_PREFIX "/style.css"
#define APP_ICON_RESOURCE   APP_RESOURCE_PREFIX "/icon.svg"

typedef struct {
    GtkWidget *window;
    GtkWidget *controls_container;
    GtkWidget *timer_label;
    GtkWidget *state_label;
    GtkWidget *session_entry;
    GtkWidget *break_entry;
    GtkWidget *start_button;
    int session_total_seconds;
    int break_total_seconds;
    int minutes;
    int seconds;
    gboolean is_running;
    gboolean was_minimized;
    gboolean was_top_plane;
    gboolean is_session;
    guint timer_id;
} TimerApp;

TimerApp *app_new();
void app_free(TimerApp *app);

#endif
