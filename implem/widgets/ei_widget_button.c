#include "ei_widget_all.h"
#include "ei_widget_button.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_draw_util.h"
#include "ei_event.h"
#include "ei_application.h"


ei_widget_t ei_button_allocfunc(void) {
    ei_impl_button_t* widget = calloc(1, sizeof(ei_impl_button_t));
    return &(widget->widget);
}

void ei_button_releasefunc(ei_widget_t widget) {
    ei_impl_button_t* widget_spec = (ei_impl_button_t*) widget;
    if(widget_spec->text) free(widget_spec->text);
    if(widget_spec->text_rect_sz) free(widget_spec->text_rect_sz);
    if(widget_spec->img_rect) free(widget_spec->img_rect);
    if(widget_spec->img) hw_surface_free(widget_spec->img);
    ei_widget_release(widget);
    free((ei_impl_button_t*)widget);
}

void ei_button_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
    // Cast the widget to a toplevel widget
    ei_impl_button_t* button = (ei_impl_button_t*)widget;

    //draw the button in the display
    ei_draw_button(surface,
                   button->widget.screen_location.top_left,
                   button->widget.screen_location.size,
                   button->widget.color,
                   button->widget.border_width,
                   button->corner_radius,
                   button->relief,
                   button->text,
                   button->text_font,
                   &(button->text_color),
                   button->text_anchor,
                   button->text_rect_sz,
                   button->img,
                   button->img_rect,
                   button->img_anchor,
                   clipper);

    //draw the button in the pick surface
    ei_draw_button(pick_surface,
                   button->widget.screen_location.top_left,
                   button->widget.screen_location.size,
                   *(button->widget.pick_color),
                   0,
                   0,
                   ei_relief_none,
                   NULL,
                   NULL,
                   NULL,
                   ei_anc_none,
                   NULL,
                   NULL,
                   NULL,
                   ei_anc_none,
                   clipper);

    ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
}


/**
 * \brief	A function that sets the default values for a button.
 */
void ei_button_setdefaultsfunc(ei_widget_t widget) {
    ei_impl_button_t* widget_spec = (ei_impl_button_t*) widget;
    widget_spec->widget.requested_size = (ei_size_t){2*k_default_button_border_width, 2*k_default_button_border_width};
    widget_spec->widget.color = ei_default_background_color;
    widget_spec->widget.border_width = k_default_button_border_width;
    widget_spec->corner_radius = k_default_button_corner_radius;
    widget_spec->relief = ei_relief_raised;
    widget_spec->text = NULL;
    widget_spec->text_font = ei_default_font;
    widget_spec->text_color = ei_font_default_color;
    widget_spec->text_anchor = ei_anc_center;
    widget_spec->img = NULL;
    widget_spec->img_rect = NULL;
    widget_spec->img_anchor = ei_anc_center;
    widget_spec->callback = NULL;
    widget_spec->user_param = NULL;
    widget_spec->widget.min_size = malloc(sizeof(ei_size_t));
    *(widget_spec->widget.min_size) = ei_size_zero();
}

/**
 * \brief 	A function that is called to notify the button that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
*/
void ei_button_geomnotifyfunc(ei_widget_t widget){

}



/**
 * @brief	A function that is called in response to an event. It implements the generic behavior of
 *		a button during an event.
 *
 * @param	widget		The widget for which the event was generated.
 * @param	event		The event containing all its parameters (type, etc.)
 *
 * @return			A boolean telling if the event was consumed by the callback or not.
 *				If true, the library does not try to call other callbacks for this
 *				event. If false, the library will call the next callback registered
 *				for this event, if any.
 *				Note: The callback may execute many operations and still return
 *				false, or return true without having done anything.
 */
bool	ei_button_handlefunc	(ei_widget_t		widget,
						 	 struct ei_event_t*	event) {
    ei_impl_button_t* widget_full = (ei_impl_button_t*) widget;
    ei_widget_t pick_widg = ei_widget_pick(&(((event->param).mouse).where));
    ei_widget_t act_widg = ei_event_get_active_widget();
    switch (event->type) {
        case ei_ev_mouse_buttondown:
            ei_event_set_active_widget(widget);
            widget_full->relief = ei_relief_sunken;
            ei_app_invalidate_rect(&(widget->screen_location));
            break;
        case ei_ev_mouse_buttonup:
            ei_event_set_active_widget(NULL);
            if (pick_widg == widget) {
                widget_full->relief = ei_relief_raised;
                bool close = (widget_full->callback == &close_toplevel);
                if(widget_full->callback != NULL) widget_full->callback(widget, event, widget_full->user_param);
                if(close) break;
                ei_app_invalidate_rect(&(widget->screen_location));
            }
            break;
        case ei_ev_mouse_move:
            if (act_widg == widget) {
                if (pick_widg == widget) {
                    if (widget_full->relief == ei_relief_raised) {
                        widget_full->relief = ei_relief_sunken;
                        ei_app_invalidate_rect(&(widget->screen_location));
                    }
                } else {
                    if (widget_full->relief == ei_relief_sunken) {
                        widget_full->relief = ei_relief_raised;
                        ei_app_invalidate_rect(&(widget->screen_location));
                    }
                }
            }
            break;
        default:
            //normalement ce cas ne peut pas arriver
            break;
    }
    return true;
}

/**
 * the callback function for a close button
 * @param widget the close button
 * @param event the event
 * @param user_param no one cares
 */
void close_toplevel	(ei_widget_t		widget,
                                   struct ei_event_t*	event,
                                   ei_user_param_t	user_param){
    ei_widget_destroy(widget->parent);
}