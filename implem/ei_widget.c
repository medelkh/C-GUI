#include "ei_widget.h"
#include "ei_widget_all.h"
#include "ei_utils.h"
#include "ei_application.h"
#include "ei_impl_application.h"
#include "ei_utils_util.h"
#include "ei_widget_toplevel.h"


ei_widget_t		ei_widget_create		(ei_const_string_t	class_name,
							 ei_widget_t		parent,
							 ei_user_param_t	user_data,
							 ei_widget_destructor_t destructor){
	// Find the widget class
    ei_widgetclass_t* widget_class = ei_widgetclass_from_name(class_name);
    if (widget_class == NULL) {
        return NULL;
    }

    // Allocate memory for the widget
    ei_widget_t widget = widget_class->allocfunc();
    if (widget == NULL) {
        return NULL;
    }

    // Set the widget class and user data
    widget->wclass = widget_class;
    widget->user_data = user_data;
    widget->destructor = destructor;

    // Initialize the widget hierarchy
    widget->parent = parent;
    widget->children_head = NULL;
    widget->children_tail = NULL;
    widget->next_sibling = NULL;
    widget->previous_sibling = NULL;
    if(parent) {
        if (widget->parent->children_tail) {
            widget->previous_sibling = widget->parent->children_tail;
            widget->parent->children_tail->next_sibling = widget;
            widget->parent->children_tail = widget;
        } else {
            widget->parent->children_head = widget;
            widget->parent->children_tail = widget;
        }
    }


    // Initialize the geometry management
    widget->placer_params = NULL;
    widget->requested_size = ei_size_zero();
    widget->screen_location = ei_rect_zero();
    if(strcmp(class_name,"toplevel")==0){
        widget->content_rect = calloc(1, sizeof(ei_rect_t));
    }
    else widget->content_rect = &(widget->screen_location);

    // Call the setdefaultsfunc to set the default values
    widget_class->setdefaultsfunc(widget);

    //Registers the widget by adding it to the widget list
    widget->pick_id = ei_widget_list_size();
    widget->pick_color = inv_convert_to_color(ei_app_pick_surface(), widget->pick_id);
    ei_append_widget(widget);

    return widget;

}


/**
 * @brief	Returns the widget that is at a given location on screen.
 *
 * @param	where		The location on screen, expressed in the root window coordinates.
 *
 * @return			The top-most widget at this location, or NULL if there is no widget
 *				at this location (except for the root widget).
 */
ei_widget_t		ei_widget_pick			(ei_point_t*		where){
    ei_surface_t pick_surf = ei_app_pick_surface();
    ei_size_t pick_surf_sz = hw_surface_get_size(pick_surf);
    hw_surface_lock(pick_surf);
    if(where->y < pick_surf_sz.height && where->y >=0 && where->x < pick_surf_sz.width && where->x >=0){
        uint32_t id = *((uint32_t*)hw_surface_get_buffer(pick_surf) + (where->y * (pick_surf_sz.width) + where->x));
        hw_surface_unlock(pick_surf);
        return ei_widget_at(id);
    }
    else{
        return ei_app_root_widget();
    }
}

void ei_widget_destroy(ei_widget_t widget)
{
    ei_widget_t parent = widget->parent;
    if (parent != NULL) {
        if (parent->children_head == widget) {
            parent->children_head = widget->next_sibling;
        }
        if (parent->children_tail == widget) {
            parent->children_tail = widget->previous_sibling;
        }
    }
    if(widget->previous_sibling != NULL){
        widget->previous_sibling->next_sibling = widget->next_sibling;
    }
    if(widget->next_sibling != NULL){
        widget->next_sibling->previous_sibling = widget->previous_sibling;
    }

    ei_widget_t child = widget->children_head;
    while (child != NULL) {
        ei_widget_t next_sibling = child->next_sibling;
        ei_widget_destroy(child);
        child = next_sibling;
    }

    if (widget->destructor != NULL) {
        widget->destructor(widget);
    }

    widget->wclass->releasefunc(widget);
}


bool ei_widget_is_displayed(ei_widget_t widget)
{
    return widget->placer_params != NULL;
}