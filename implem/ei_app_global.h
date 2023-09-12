#ifndef PROJETC_IG_EI_APP_GLOBAL_H
#define PROJETC_IG_EI_APP_GLOBAL_H

#include "ei_application.h"
#include "ei_impl_application.h"

extern ei_surface_t main_window;
extern ei_widget_t root_widget;
extern ei_surface_t pick_surface;

extern ei_widget_vec* widget_list;
extern size_t default_capacity;

extern ei_linked_rect_t* invalidated_rects;

extern bool quit_request;

#endif
