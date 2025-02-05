#include "export.h"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtkactionable.h"
#include <gtk/gtk.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <adwaita.h>
#include <stdlib.h>
#include "types.h"

GtkButton *add_button_to_layout_thingy(globals_t *data, const char *text, const char *image_file, layout_type_t itype) {
    GtkButton *btn = GTK_BUTTON(gtk_button_new());
    gtk_widget_set_size_request(GTK_WIDGET(btn), 150, 150);
    GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    GtkWidget *image = gtk_image_new_from_file(image_file);
    gtk_image_set_pixel_size(GTK_IMAGE(image), 128);
    gtk_widget_set_vexpand(image, true);
    GtkWidget *label = gtk_label_new(text);
    gtk_box_append(box, image);
    gtk_box_append(box, label);
    gtk_actionable_set_action_name(GTK_ACTIONABLE(btn), "app.layout_change");
    gtk_actionable_set_action_target_value(GTK_ACTIONABLE(btn), g_variant_new_int64((int64_t)itype));
    gtk_button_set_child(btn, GTK_WIDGET(box));

    gtk_grid_attach(
        data->layout_buttons,
        GTK_WIDGET(btn),
        data->button_count % 4,
        data->button_count / 4,
        1, 1);

    data->button_count++;
    return btn;
}

void switch_to_layout_view(globals_t *data) {
    gtk_stack_set_visible_child_name(data->stack, "select_layout");
    GtkBox *view = GTK_BOX(gtk_builder_get_object(data->builder, "files_view"));
    size_t n = g_list_model_get_n_items(data->files);
    for (size_t i = 0; i < n; i++) {
        GFile *file = (GFile *)g_list_model_get_item(data->files, i);
        char *name = g_file_get_basename(file);
        GtkLabel *label = GTK_LABEL(gtk_label_new(name));
        gtk_box_append(view, GTK_WIDGET(label));
    }
    add_button_to_layout_thingy(data, "Grid 2x2", CRAP_SITE "ui/icons/grid-2.svg", LAY_T_GRID_2);
    add_button_to_layout_thingy(data, "Grid 3x3", CRAP_SITE "ui/icons/grid-3.svg", LAY_T_GRID_3);
    add_button_to_layout_thingy(data, "Half", CRAP_SITE "ui/icons/stack.svg", LAY_T_STACK);
}

void files_accepted(GObject *obj, GAsyncResult *result, void *data) {
    GError *error;
    ((globals_t *)data)->files = 
        gtk_file_dialog_open_multiple_finish(GTK_FILE_DIALOG(obj), result, &error);
    switch_to_layout_view((globals_t *)data);
}

void picked_layout(UNUSED GSimpleAction *action, GVariant *var, globals_t *data) {
    data->itype = (layout_type_t)g_variant_get_int64(var);
    gtk_stack_set_visible_child_name(data->stack, "export_layout");
}

void get_file(UNUSED GSimpleAction *action, UNUSED GVariant *var, globals_t *data) {
    GCancellable *cancellable = g_cancellable_new();
    gtk_file_dialog_open_multiple(
        data->file_dialog,
        GTK_WINDOW(data->window),
        cancellable,
        files_accepted,
        data);
}

void save_folder_selected(GObject *obj, GAsyncResult *result, void *data) {
    GError *error;
    ((globals_t *)data)->target_dir =
        gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(obj), result, &error);
    export_image((globals_t *)data);
}

void export(UNUSED GSimpleAction *action, UNUSED GVariant *var, globals_t *data) {
    GCancellable *cancellable = g_cancellable_new();
    gtk_file_dialog_select_folder(data->file_dialog, GTK_WINDOW(data->window), cancellable, save_folder_selected, data);
}

void activate(AdwApplication *data) {
    globals_t *globals = calloc(sizeof(globals_t), sizeof(char));
    globals->app = data;
    globals->builder = gtk_builder_new_from_file(CRAP_SITE "ui/main.ui");
    globals->actions = g_simple_action_group_new();
    GSimpleAction *file_request = g_simple_action_new("file_request", G_VARIANT_TYPE_UNIT);
    GSimpleAction *layout_change = g_simple_action_new("layout_change", G_VARIANT_TYPE_INT64);
    GSimpleAction *exportd = g_simple_action_new("export", G_VARIANT_TYPE_UNIT);

    g_action_map_add_action(G_ACTION_MAP(globals->actions), G_ACTION(file_request));
    g_action_map_add_action(G_ACTION_MAP(globals->actions), G_ACTION(layout_change));
    g_action_map_add_action(G_ACTION_MAP(globals->actions), G_ACTION(exportd));
    g_signal_connect(file_request, "activate", G_CALLBACK(get_file), globals);
    g_signal_connect(layout_change, "activate", G_CALLBACK(picked_layout), globals);
    g_signal_connect(exportd, "activate", G_CALLBACK(export), globals);

    globals->window = ADW_WINDOW(gtk_builder_get_object(globals->builder, "window"));
    globals->file_dialog = gtk_file_dialog_new();
    globals->select_images_view = GTK_WIDGET(gtk_builder_get_object(globals->builder, "select_images_view"));
    globals->select_layout_view = GTK_WIDGET(gtk_builder_get_object(globals->builder, "select_layout_view"));
    globals->export_view = GTK_WIDGET(gtk_builder_get_object(globals->builder, "export_view"));
    globals->layout_buttons = GTK_GRID(gtk_builder_get_object(globals->builder, "layout_buttons"));
    globals->stack = GTK_STACK(gtk_builder_get_object(globals->builder, "stack"));

    gtk_stack_add_named(globals->stack, globals->select_images_view, "select_images");
    gtk_stack_add_named(globals->stack, globals->select_layout_view, "select_layout");
    gtk_stack_add_named(globals->stack, globals->export_view, "export_layout");

    gtk_widget_insert_action_group(GTK_WIDGET(globals->window), "app", G_ACTION_GROUP(globals->actions));
    gtk_application_add_window(GTK_APPLICATION(globals->app), GTK_WINDOW(globals->window));

    gtk_stack_set_visible_child_name(globals->stack, "select_images");

    gtk_window_present(GTK_WINDOW(globals->window));
}

int main(int argc, char **argv) {
    puts("Prepress is initialized");
    adw_init();
    AdwApplication *app = adw_application_new(
        "me.themaither.prepress", G_APPLICATION_DEFAULT_FLAGS
    );
    g_signal_connect(app, "activate", G_CALLBACK(activate), app);
    g_application_run(G_APPLICATION(app), argc, argv);
}
