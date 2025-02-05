#include "export.h"
#include "gio/gio.h"
#include "glib.h"
#include "types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char buf[2048]; // THREADS??? wtf

#define VAL_OR_DEF(arg, default) arg ? arg : default 

void export_image_grid_2(globals_t *data) {
    size_t n = g_list_model_get_n_items(data->files);
    char *target = g_file_get_path(data->target_dir);
    char *files_pocket[4];
    for (size_t i = 0; i < n; i += 4) {
        memset(files_pocket, 0, sizeof(files_pocket));
        for (size_t j = 0; j < 4; j++) {
            GFile *file = g_list_model_get_item(data->files, i + j);
            files_pocket[j] = g_file_get_path(file);
        }
        snprintf(
            buf, 2047, 
            "/usr/bin/magick montage -geometry 300x424+16+16 '%s' '%s' '%s' '%s' '%s/out%ld.png'",
            VAL_OR_DEF(files_pocket[0], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[1], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[2], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[3], CRAP_SITE "null.png"),
            target, i / 4);
        printf("Command: %s\n", buf);
        system(buf);
    }
}

void export_image_grid_3(globals_t *data) {
    size_t n = g_list_model_get_n_items(data->files);
    char *target = g_file_get_path(data->target_dir);
    char *files_pocket[9];
    for (size_t i = 0; i < n; i += 9) {
        memset(files_pocket, 0, sizeof(files_pocket));
        for (size_t j = 0; j < 9; j++) {
            GFile *file = g_list_model_get_item(data->files, i + j);
            files_pocket[j] = g_file_get_path(file);
        }
        snprintf(
            buf, 2047, 
            "/usr/bin/magick montage -geometry 300x424+16+16 '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s/out%ld.png'",
            VAL_OR_DEF(files_pocket[0], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[1], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[2], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[3], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[4], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[5], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[6], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[7], CRAP_SITE "null.png"),
            VAL_OR_DEF(files_pocket[8], CRAP_SITE "null.png"),
            target, i / 9);
        printf("Command: %s\n", buf);
        system(buf);
    }
}

void export_image(globals_t *data) {
    switch (data->itype) {
    case LAY_T_GRID_2:
        export_image_grid_2(data);
        break;
    case LAY_T_GRID_3:
        export_image_grid_3(data);
        break;
    case LAY_T_STACK:
        break;
    }
}
