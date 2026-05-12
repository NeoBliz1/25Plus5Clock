#include <stddef.h>
#include <gtk/gtk.h>
#include "./include/app.h"
#include "./include/ui.h"

static void on_activate(GtkApplication *gtk_app, gpointer user_data) {
    TimerApp *app = user_data;

    // Create the specialized Application Window
    app->window = gtk_application_window_new(gtk_app);

    // Set unique ID for desktop integration
    gtk_window_set_application(GTK_WINDOW(app->window), gtk_app);

    create_ui(app);

    gtk_widget_show_all(app->window);
}

int main(int argc, char **argv) {
    TimerApp *app = app_new();

    GtkApplication *gtk_app = gtk_application_new("github.neobliz1.Clock25Plus5", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(gtk_app, "activate", G_CALLBACK(on_activate), app);
    const int status = g_application_run(G_APPLICATION(gtk_app), argc, argv);

    g_object_unref(gtk_app);
    app_free(app);

    return status;
}
