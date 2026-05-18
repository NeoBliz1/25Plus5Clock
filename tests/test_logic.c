#include <glib.h>
#include "../include/app.h"
#include "../include/callbacks.h"

// Mock function to satisfy callbacks.c linkage requirements
void update_timer_display(const TimerApp *app) { (void)app; }

// TEST 1: Check if the application struct initializes correctly
static void test_app_initialization(void) {
    TimerApp *app = app_new();

    g_assert_nonnull(app);
    g_assert_cmpint(app->minutes, ==, 25);
    g_assert_cmpint(app->seconds, ==, 0);
    g_assert_true(app->is_session);
    g_assert_false(app->is_running);

    app_free(app);
}

// TEST 2: Check if standard timer ticks reduce seconds correctly
static void test_timer_countdown_tick(void) {
    TimerApp *app = app_new();
    app->is_running = TRUE;
    app->minutes = 25;
    app->seconds = 0;

    // Simulate 1 second passing
    const gboolean keep_running = timer_tick(app);

    g_assert_true(keep_running);
    g_assert_cmpint(app->minutes, ==, 24);
    g_assert_cmpint(app->seconds, ==, 59);

    app_free(app);
}

// TEST 3: Verify that hitting 00:00 shifts the environment from Session to Break
static void test_session_to_break_transition(void) {
    TimerApp *app = app_new();

    // 1. Create a mock top-level window so gtk_widget_get_window() doesn't fail
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_object_ref_sink(app->window);

    // 2. Create a mock timer label so the sound engine has a widget target
    app->timer_label = gtk_label_new(NULL);
    g_object_ref_sink(app->timer_label);

    app->is_running = TRUE;
    app->is_session = TRUE;
    app->minutes = 0;
    app->seconds = 0; // Next tick triggers transition

    timer_tick(app);

    // It should swap to Break mode and load default break parameters (5 mins)
    g_assert_false(app->is_session);
    g_assert_cmpint(app->minutes, ==, 5);
    g_assert_cmpint(app->seconds, ==, 0);

    // Clean up all initialized mock assets cleanly
    g_object_unref(app->timer_label);
    g_object_unref(app->window); // Release the mock window
    app_free(app);
}

int main(int argc, char **argv) {
    // Initialize mock environment properties for GTK
    g_setenv("G_SETTINGS_BACKEND", "memory", TRUE);

    // Initialize GTK in headless mode so layout functions don't panic
    if (!gtk_init_check(&argc, &argv)) {
        g_printerr("Warning: Could not initialize headless GTK context\n");
    }

    g_test_init(&argc, &argv, NULL);

    // Register your test targets
    g_test_add_func("/logic/initialization", test_app_initialization);
    g_test_add_func("/logic/countdown_tick", test_timer_countdown_tick);
    g_test_add_func("/logic/session_transition", test_session_to_break_transition);

    return g_test_run();
}
