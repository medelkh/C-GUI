#ifndef EI_WIDGET_FRAME_H
#define EI_WIDGET_FRAME_H

#include "ei_widget_all.h"
#include "ei_widgetclass.h"

//frame specific structure
typedef struct ei_impl_frame_t {
    ei_impl_widget_t widget;
    ei_relief_t relief;
    ei_string_t text;           //text et img ne peuvent être tous les deux non NULL
    ei_font_t text_font;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_size_t* text_rect_sz;
    ei_surface_t img;
    ei_rect_ptr_t img_rect;
    ei_anchor_t img_anchor;
} ei_impl_frame_t;

ei_widget_t ei_frame_allocfunc(void);

void ei_frame_releasefunc(ei_widget_t widget);

void ei_frame_setdefaultsfunc(ei_widget_t widget);

bool ei_frame_handlefunc(ei_widget_t widget, struct ei_event_t* event);

void ei_frame_geomnotifyfunc(ei_widget_t widget);

void ei_frame_drawfunc(ei_widget_t widget,
                    ei_surface_t surface, ei_surface_t pick_surface,
                    ei_rect_t* clipper);

#endif 