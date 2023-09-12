#include "ei_application.h"
#include "ei_impl_application.h"

ei_surface_t main_window;
ei_widget_t root_widget;
ei_surface_t pick_surface;

ei_widget_vec* widget_list;
size_t default_capacity = 2048;

ei_linked_rect_t* invalidated_rects = NULL;

bool quit_request = false;