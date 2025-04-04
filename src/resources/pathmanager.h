#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include "resources/ids.h"

enum
{
    RESOURCE_PATH,
    LOG_PATH,
    CONFIG_PATH,
    SCORE_PATH,
    SAVE_PATH,
    SHADER_PATH,
    NUMBER_OF_LOCAL_PATHS
};

typedef enum music_file_type
{
    MUSIC_FILE_TYPE_PSM,
    MUSIC_FILE_TYPE_OGG,
} music_file_type;

extern char const pm_path_sep;

int pm_init(void);
void pm_free(void);
void pm_log(void);
const char *pm_get_errormsg(void);
int pm_validate_resources(void);
int pm_in_portable_mode(void);
int pm_in_release_mode(void);
int pm_in_debug_mode(void);
char *pm_get_local_base_dir(void);
const char *pm_get_local_path_type_name(unsigned int path_id);
const char *pm_get_local_path(unsigned int resource_id);
const char *pm_get_resource_path(unsigned int local_id);
int pm_create_dir(const char *dirname);
void pm_get_music_path(char *dst, size_t size, music_file_type *type, unsigned int resource_id);

#endif // PATHMANAGER_H
