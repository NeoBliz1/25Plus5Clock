#include <glib.h>
#include <gtk/gtk.h>
#include "../include/app.h"

__attribute__((weak)) void update_timer_display(const TimerApp *app) {
    (void)app;
}
void on_reset_clicked(GtkWidget *btn, gpointer data);
void update_if_active(GtkWidget *widget, TimerApp *app, int total_seconds);

// TEST 1: Check if fallback schema defaults (1500s / 300s) match app_new load values
static void test_gsettings_initialization(void) {
    TimerApp *app = app_new();
    g_assert_nonnull(app);
    g_assert_nonnull(app->settings);

    // Verify fallback defaults from your schema are parsed correctly into seconds
    // 25 minutes = 1500 seconds, 5 minutes = 300 seconds
    g_assert_cmpint(app->session_total_seconds, ==, 1500);
    g_assert_cmpint(app->break_total_seconds, ==, 300);

    app_free(app);
}

// TEST 2: Verify that update_if_active commits modifications straight to the schema backend
static void test_gsettings_persistence_saving(void) {
    TimerApp *app = app_new();
    g_assert_nonnull(app);
    g_assert_nonnull(app->settings);

    // Create a mock tracking pointer address for entry element checks
    app->session_entry = (GtkWidget*)0xDEADBEEF;

    // Simulate updating a work session configuration to 30 minutes (1800 seconds)
    update_if_active(app->session_entry, app, 1800);

    // 1. Verify running runtime state variables updated instantly
    g_assert_cmpint(app->session_total_seconds, ==, 1800);
    g_assert_cmpint(app->minutes, ==, 30);
    g_assert_cmpint(app->seconds, ==, 0);

    // 2. Verify values were physically committed down into GSettings memory cache
    int persisted_seconds = g_settings_get_int(app->settings, "work-duration");
    g_assert_cmpint(persisted_seconds, ==, 1800);

    app_free(app);
}

// TEST 3: Verify that clicking reset restores states from GSettings and updates variables
static void test_gsettings_reset_function(void) {
    TimerApp *app = app_new();
    g_assert_nonnull(app);

    // 1. Initialize custom values into GSettings memory layer
    // Work = 30 mins (1800s), Break = 10 mins (600s)
    g_settings_set_int(app->settings, "work-duration", 1800);
    g_settings_set_int(app->settings, "break-duration", 600);

    // 2. Set dirty runtime configuration state parameters
    app->is_running = TRUE;
    app->is_session = FALSE;
    app->minutes = 2;
    app->seconds = 15;
    app->timer_id = 999; // Mock active interval handle

    // 3. Mandatory headless layout placeholders to prevent segmentation faults in production code [1]
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_object_ref_sink(app->window);

    app->start_button = gtk_button_new();
    g_object_ref_sink(app->start_button);

    app->session_entry = gtk_entry_new();
    g_object_ref_sink(app->session_entry);

    app->break_entry = gtk_entry_new();
    g_object_ref_sink(app->break_entry);

    app->controls_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(app->controls_container);

    // 4. Trigger the reset routine pass
    on_reset_clicked(NULL, app);

    // 5. VALIDATE ONLY GSETTINGS AND LOGICAL TRANSITIONS
    g_assert_false(app->is_running);
    g_assert_true(app->is_session);
    g_assert_cmpint(app->timer_id, ==, 0);

    // Confirm core time properties successfully synchronised with database values
    g_assert_cmpint(app->session_total_seconds, ==, 1800);
    g_assert_cmpint(app->break_total_seconds, ==, 600);
    g_assert_cmpint(app->minutes, ==, 30);
    g_assert_cmpint(app->seconds, ==, 0);

    // 6. Clean up mock resources
    g_object_unref(app->window);
    g_object_unref(app->start_button);
    g_object_unref(app->session_entry);
    g_object_unref(app->break_entry);
    g_object_unref(app->controls_container);
    app_free(app);
}

int main(int argc, char **argv) {
    // Force GLib to use an isolated in-memory backend for testing
    g_setenv("G_SETTINGS_BACKEND", "memory", TRUE);

    // Initialize GTK context in headless mode so validation checks don't crash
    if (!gtk_init_check(&argc, &argv)) {
        g_printerr("Warning: Could not initialize headless GTK context\n");
    }

    g_test_init(&argc, &argv, NULL);

    // Register both test targets under the gsettings index path
    g_test_add_func("/gsettings/initialization", test_gsettings_initialization);
    g_test_add_func("/gsettings/persistence_saving", test_gsettings_persistence_saving);

    return g_test_run();
}