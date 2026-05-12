#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>
#include "app.h"

// Timer and UI updates
gboolean timer_tick(gpointer data);
gboolean on_focus_out(GtkWidget *widget, GdkEvent *event, gpointer data);
void update_if_active(GtkWidget *widget, TimerApp *app, int total_seconds);

// UI Signal Handlers
void on_entry_changed(GtkWidget *widget, gpointer data);
void on_adjust_clicked(GtkWidget *btn, gpointer data);
void on_start_stop_clicked(GtkWidget *btn, gpointer data);
void on_reset_clicked(GtkWidget *btn, gpointer data);

#endif