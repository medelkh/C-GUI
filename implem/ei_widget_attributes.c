#include "ei_widget_all.h"
#include "ei_widget_attributes.h"


/**
 * @brief	Get the class of a widget.
 */
ei_widgetclass_t*	ei_widget_get_class		(ei_widget_t		widget){
    return widget->wclass;
}

/**
 * @brief	Get the identifier of the widget in the picking offscreen under the form
 * 		of a color.
 */
const ei_color_t*	ei_widget_get_pick_color	(ei_widget_t		widget){
    return widget->pick_color;
}

/**
 * @brief	Get the parent of a widget.
 */
ei_widget_t 		ei_widget_get_parent		(ei_widget_t		widget){
    return widget->parent;
}

/**
 * @brief	Get the first child of a widget.
 */
ei_widget_t 		ei_widget_get_first_child	(ei_widget_t		widget){
    return widget->children_head;
}

/**
 * @brief	Get the last child of a widget.
 */
ei_widget_t 		ei_widget_get_last_child	(ei_widget_t		widget){
    return widget->children_tail;
}

/**
 * @brief	Get the next sibling of a widget.
 */
ei_widget_t 		ei_widget_get_next_sibling	(ei_widget_t		widget){
    return widget->next_sibling;
}

/**
 * @brief	Get the user data associated to a widget.
 */
void*			ei_widget_get_user_data		(ei_widget_t		widget){
    return widget->user_data;
}

/**
 * @brief	Get the screen location of the widget.
 */
const ei_rect_t*	ei_widget_get_screen_location	(ei_widget_t		widget){
    return &(widget->screen_location);
}

/**
 * @brief	Get the content rect of a widget.
 */
const ei_rect_t*	ei_widget_get_content_rect	(ei_widget_t		widget){
    return widget->content_rect;
}

/**
 * @brief	Set the content rect of a widget.
 */
void	 		ei_widget_set_content_rect	(ei_widget_t		widget,
							 const ei_rect_t*	content_rect){
    widget->content_rect = malloc(sizeof(ei_rect_t));
    *(widget->content_rect) = *content_rect;
}

