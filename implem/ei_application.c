#include "ei_application.h"
#include "ei_widget_all.h"
#include "ei_utils.h"
#include "ei_event.h"
#include "ei_impl_application.h"
#include "ei_app_global.h"
#include "ei_utils_util.h"


void ei_app_create(ei_size_t main_window_size, bool fullscreen) {
    hw_init();
    //ei_widgetclass_register(qlsdfkjmqlkfj)
    main_window = hw_create_window(main_window_size, fullscreen);
    main_window_size = hw_surface_get_size(main_window);
    pick_surface = hw_surface_create(main_window, main_window_size, false);
    //initialisation de la liste des widgets
    ei_init_widget_vec();
    //création root widget
    root_widget = ei_widget_create("frame", NULL, NULL, NULL);
    root_widget->screen_location = ei_rect(ei_point_zero(), main_window_size);
    root_widget->content_rect = &(root_widget->screen_location);
    //set default handle func

}



ei_widget_t ei_app_root_widget(void){
    return root_widget;
}

ei_surface_t ei_app_root_surface(void){
    return main_window;
}

/**
 * @return Returns the pick surface
 */
ei_surface_t ei_app_pick_surface(void){
    return pick_surface;
}



void ei_app_run(void) {
    bool handled = false;
    ei_widget_t act_widg = NULL;
    ei_widget_t pick_widg = NULL;
    ei_default_handle_func_t default_handle_f = NULL;
    ei_event_t event;

    ei_surface_t root_surface = ei_app_root_surface();
    ei_surface_t picking_surface = ei_app_pick_surface();
    ei_widget_t widget_root = ei_app_root_widget();


    hw_surface_lock(root_surface);
    hw_surface_lock(picking_surface);
    widget_root->wclass->drawfunc(root_widget, root_surface, picking_surface, root_widget->content_rect);
    hw_surface_unlock(root_surface);
    hw_surface_unlock(picking_surface);
    hw_surface_update_rects(root_surface, NULL);
    while (!quit_request) {
        hw_event_wait_next(&event);
        //traitement de l'événement:
        //appel des traitants, qui appelleront potentiellement ei_invalidate_rect, attention faudra calculer les intersections ?
        invalidated_rects = NULL;
        handled = false;
        act_widg = ei_event_get_active_widget();
        if (act_widg != NULL) {
            handled = act_widg->wclass->handlefunc(act_widg, &event);
        }
        if (!handled) {
            if (event.type != ei_ev_mouse_buttondown &&
                                        event.type != ei_ev_mouse_buttonup) {
                default_handle_f = ei_event_get_default_handle_func();
                default_handle_f(&event);
            } else {
                //identifier le widget pointé
                pick_widg = ei_widget_pick(&event.param.mouse.where);
                pick_widg->wclass->handlefunc(pick_widg, &event);
            }
        }
        //appel de la draw_func du widget root, avec les rectangles des appels à ei_invalidate_rect en clipping
        hw_surface_lock(root_surface);
        hw_surface_lock(picking_surface);
        //while (blablalba) {         //parcours de la liste des rectangles
            root_widget->wclass->drawfunc(widget_root, root_surface, picking_surface, root_widget->content_rect);
        //}
        hw_surface_unlock(root_surface);
        hw_surface_unlock(picking_surface);
        hw_surface_update_rects(root_surface, NULL);
        ei_free_rect_ll(invalidated_rects);
    }
}

void ei_app_invalidate_rect(const ei_rect_t* rect) {
    ei_insert_rect(*rect, &invalidated_rects);
}


void ei_app_quit_request(void) {
    quit_request = true;
}

void ei_app_free(void) {
    ei_widget_destroy(root_widget);
    hw_surface_free(pick_surface);
    hw_surface_free(main_window);
}
