#include "../include/app.h"

double default_session_minutes = 25;
double default_break_minutes = 5;

TimerApp *app_new() {
    TimerApp *app = g_new0(TimerApp, 1);
    app->session_total_seconds = 25 * 60;
    app->break_total_seconds = 5 * 60;
    app->minutes = 25;
    app->seconds = 0;
    app->is_running = FALSE;
    app->is_session = TRUE;
    return app;
}

void app_free(TimerApp *app) {
    g_free(app);
}
