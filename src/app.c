#include "../include/app.h"

TimerApp *app_new() {
    TimerApp *app = g_new0(TimerApp, 1);
    app->session_total_seconds = 25 * 60;
    app->break_total_seconds = 5 * 60;
    app->minutes = 25;
    app->seconds = 0;
    app->is_running = FALSE;
    app->is_session = TRUE;
    app->settings = g_settings_new("com.github.neobliz1.25Plus5Clock");
    if (!app->settings) {
        g_warning("Failed to initialize GSettings wrapper object.");
    }
    return app;
}

void app_free(TimerApp *app) {
    g_free(app);
}
