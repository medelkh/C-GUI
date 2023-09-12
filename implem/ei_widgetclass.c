#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_all.h"

ei_widgetclass_t frame_class = (ei_widgetclass_t){
    .name = "frame",
    .allocfunc = &ei_frame_allocfunc,
    .releasefunc = &ei_frame_releasefunc,
    .drawfunc = &ei_frame_drawfunc,
    .geomnotifyfunc = &ei_frame_geomnotifyfunc,
    .setdefaultsfunc = &ei_frame_setdefaultsfunc,
    .handlefunc = &ei_frame_handlefunc,
    .next = NULL
};

ei_widgetclass_t button_class = (ei_widgetclass_t){
    .name = "button",
    .allocfunc = &ei_button_allocfunc,
    .releasefunc = &ei_button_releasefunc,
    .drawfunc = &ei_button_drawfunc,
    .geomnotifyfunc = &ei_button_geomnotifyfunc,
    .setdefaultsfunc = &ei_button_setdefaultsfunc,
    .handlefunc = &ei_button_handlefunc,
    .next = NULL
};

ei_widgetclass_t toplevel_class = (ei_widgetclass_t){
    .name = "toplevel",
    .allocfunc = &ei_toplevel_allocfunc,
    .releasefunc = &ei_toplevel_releasefunc,
    .drawfunc = &ei_toplevel_drawfunc,
    .geomnotifyfunc = &ei_toplevel_geomnotifyfunc,
    .setdefaultsfunc = &ei_toplevel_setdefaultsfunc,
    .handlefunc = &ei_toplevel_handlefunc,
    .next = NULL
};

/**
 * @brief	Returns the structure describing a class, from its name.
 *
 * @param	name		The name of the class of widget.
 *
 * @return			The structure describing the class.
 */
ei_widgetclass_t*	ei_widgetclass_from_name	(ei_const_string_t name){
    if(strcmp(name, "frame") == 0){
        return &frame_class;
    }
    else if(strcmp(name, "button") == 0){
        return &button_class;
    }
    else if(strcmp(name, "toplevel") == 0){
        return &toplevel_class;
    }
    else{
        printf("Error: Unknown widget class name\n");
        exit(1);
        return NULL;
    }
}