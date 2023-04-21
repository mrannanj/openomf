#ifndef MECHLAB_H
#define MECHLAB_H

#include "game/protos/scene.h"

typedef enum
{
    DASHBOARD_NONE,
    DASHBOARD_STATS,
    DASHBOARD_NEW,
    DASHBOARD_SELECT_NEW_PIC,
    DASHBOARD_SELECT_DIFFICULTY,
    DASHBOARD_SELECT_TOURNAMENT,
} dashboard_type;

int mechlab_create(scene *scene);
void mechlab_update(scene *scene);

void mechlab_select_dashboard(scene *scene, dashboard_type type);

#endif // MECHLAB_H
