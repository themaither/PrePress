#ifndef PREPRESS_TYPES_H
#define PREPRESS_TYPES_H
#include "adwaita.h"

///
/// unused
///
#define UNUSED __attribute__((unused))

///
/// resource storage prefix (SHOULD BE CHANGED TO /usr/share/whatever/ in PROD!!!!)
///
#define CRAP_SITE ""

///
/// temporary file storage
///
#define TEMP_SHIT "/tmp/prepress/"

typedef enum {
    LAY_T_GRID_2,
    LAY_T_GRID_3,
    LAY_T_STACK,
} layout_type_t;

typedef struct {
    AdwApplication *app;
    AdwWindow *window;
    GtkBuilder *builder;
    GSimpleActionGroup *actions;
    GtkFileDialog *file_dialog;
    GtkStack *stack;

    GtkWidget *select_images_view;
    GtkWidget *select_layout_view;
    GtkWidget *export_view;
    GtkGrid *layout_buttons;

    GListModel *files;
    size_t button_count;
    layout_type_t itype;
    GFile *target_dir;
} globals_t;
#endif
