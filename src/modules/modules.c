#include "module.h"

#include <string.h>

static const tinyos_module *modules[] = {
    &tinyos_module_home,
    &tinyos_module_games,
    &tinyos_module_music,
    &tinyos_module_settings,
    &tinyos_module_system,
    &tinyos_module_about,
    &tinyos_module_appearance,
    &tinyos_module_themes,
    &tinyos_module_wallpapers,
    &tinyos_module_typeface,
};

const tinyos_module *tinyos_module_find(const char *id)
{
    if (!id)
        return NULL;
    for (unsigned int i = 0; i < sizeof(modules) / sizeof(modules[0]); i++) {
        if (strcmp(modules[i]->id, id) == 0)
            return modules[i];
    }
    return NULL;
}
