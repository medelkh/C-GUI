#include "ei_widget_all.h"

/**
 * @brief	Draws the children of a widget.
 * 		The children are draw withing the limits of the clipper and
 * 		the widget's content_rect.
 *
 * @param	widget		The widget which children are drawn.
 * @param	surface		A locked surface where to draw the widget's children.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void ei_impl_widget_draw_children      (ei_widget_t		widget,
					ei_surface_t		surface,
					ei_surface_t		pick_surface,
					ei_rect_t*		clipper){
    ei_widget_t cur_child = ei_widget_get_first_child(widget);
    while(cur_child != NULL){
        cur_child->wclass->drawfunc(cur_child, surface, pick_surface, clipper);
        cur_child = ei_widget_get_next_sibling(cur_child);
    }
}

/**
 * Frees the memory allocated by a widget
 * @param widget widget to be freed
 */
void ei_widget_release(ei_impl_widget_t* widget){
    if(widget->min_size){
        free(widget->min_size);
    }
    if(widget->content_rect) if(widget->content_rect != &(widget->screen_location)) free(widget->content_rect);
    if(widget->pick_color) free(widget->pick_color);
    if(widget->placer_params) free(widget->placer_params);
    //if(widget->user_data) free(widget->user_data);
}