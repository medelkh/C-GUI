#include "ei_widget_all.h"
#include "ei_widget_toplevel.h"
#include "ei_implementation.h"
#include "ei_utils.h"
#include "ei_draw_util.h"
#include "ei_application.h"
#include "ei_event.h"

int close_button_radius = 10;
int close_button_border_width = 2;
int close_button_pos_x = 12;
int close_button_pos_y = -26;
int toplevel_bar_height = 36;
int toplevel_border_width = 3;
int toplevel_corner_radius = 17;
int title_pos_x = 40;
int title_pos_y = 4;
int resize_button_size = 20;
int toplevel_move_click_x;
int toplevel_move_click_y;
int toplevel_move_prev_x;
int toplevel_move_prev_y;

ei_color_t grey = {0x60, 0x60, 0x60, 0xff};
ei_color_t white = {0xff, 0xff, 0xff, 0xff};

ei_widget_t ei_toplevel_allocfunc(void) {
    ei_impl_toplevel_t* widget = calloc(1, sizeof(ei_impl_toplevel_t));
    return &(widget->widget);
}

void ei_toplevel_releasefunc(ei_widget_t widget) {
    ei_impl_toplevel_t* widget_full = (ei_impl_toplevel_t*) widget;
    if(widget_full->title) free(widget_full->title);
    ei_widget_release(widget);
    free(widget_full);
}

void ei_toplevel_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
    // Cast the widget to a toplevel widget
    ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)widget;

    ei_point_t position = widget->screen_location.top_left;
    ei_size_t size = widget->screen_location.size;

    // Draw the toplevel widget header
    int precision = 2*toplevel_corner_radius+2;
    ei_point_t* toplevel_bar = ei_rounded_rect(position,
                                               ei_size(size.width, toplevel_bar_height),
                                                toplevel_corner_radius, precision, 3);
    ei_draw_polygon(surface, toplevel_bar, 2*precision+2, grey, clipper);
    free(toplevel_bar);


    //draw the side bars
    ei_point_t left_side_bar[4] = {ei_point(position.x, position.y + toplevel_bar_height),
                                    ei_point(position.x + toplevel_border_width, position.y + toplevel_bar_height),
                                   ei_point(position.x + toplevel_border_width, position.y + size.height),
                                   ei_point(position.x, position.y + size.height)};
    ei_point_t right_side_bar[4] = {ei_point(position.x + size.width - toplevel_border_width, position.y + toplevel_bar_height),
                                   ei_point(position.x  + size.width, position.y + toplevel_bar_height ),
                                   ei_point(position.x + size.width, position.y + size.height),
                                   ei_point(position.x  + size.width - toplevel_border_width, position.y + size.height)};
    ei_point_t bottom_side_bar[4] = {ei_point(position.x, position.y + size.height - toplevel_border_width),
                                    ei_point(position.x + size.width, position.y + size.height - toplevel_border_width),
                                    ei_point(position.x + size.width, position.y + size.height),
                                    ei_point(position.x, position.y + size.height)};
    ei_draw_polygon(surface,  left_side_bar, 4, grey, clipper);
    ei_draw_polygon(surface, right_side_bar, 4, grey, clipper);
    ei_draw_polygon(surface, bottom_side_bar, 4, grey, clipper);

    //Draw toplevel's inner frame
    ei_point_t inner_frame[4] = {ei_point(position.x + toplevel_border_width, position.y + toplevel_bar_height),
                                 ei_point(position.x + size.width - toplevel_border_width, position.y + toplevel_bar_height),
                                 ei_point(position.x + size.width - toplevel_border_width, position.y + size.height - toplevel_border_width),
                                 ei_point(position.x + toplevel_border_width, position.y + size.height - toplevel_border_width)};
    ei_draw_polygon(surface, inner_frame, 4, widget->color, clipper);

   
    // Draw the toplevel widget title
    ei_point_t text_pos = ei_point_add(toplevel->widget.screen_location.top_left, (ei_point_t){title_pos_x, title_pos_y});
    ei_draw_text(surface, &text_pos, toplevel->title, NULL, white, clipper);

    //Draw in the picksurface
    ei_point_t* toplevel_bar_pick = ei_rounded_rect(position,
                                               ei_size(size.width, size.height),
                                               toplevel_corner_radius, precision, 3);
    ei_draw_polygon(pick_surface, toplevel_bar_pick, 2*precision+2, *(widget->pick_color), clipper);
    free(toplevel_bar_pick);
    ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
}

static ei_size_t toplevel_default_min_size = (ei_size_t){160,120};
static ei_size_t toplevel_default_requested_size = (ei_size_t){320, 240};

void ei_toplevel_setdefaultsfunc(ei_widget_t widget)
{
    // Vérification du widget
    if (widget == NULL)
        return;

    // Définition des valeurs par défaut
    ei_impl_toplevel_t* widget_spec = (ei_impl_toplevel_t*) widget;
    widget_spec->widget.requested_size = toplevel_default_requested_size;
    widget_spec->widget.color = ei_default_background_color;  // Couleur par défaut (Black)
    widget_spec->widget.border_width = 4;
    widget_spec->title = "toplevel";
    widget_spec->closable = true;
    widget_spec->resizable = ei_axis_both;
    widget_spec->widget.min_size = malloc(sizeof(ei_size_t));
    *(widget_spec->widget.min_size) = toplevel_default_min_size;
}

/**
 * \brief 	A function that is called to notify the toplevel that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
*/
void ei_toplevel_geomnotifyfunc(ei_widget_t widget){
    
}

/**
 * @brief	A function that is called in response to an event. It implements the generic behavior of
 *		a toplevel during an event.
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
bool	ei_toplevel_handlefunc	(ei_widget_t		widget,
						 	 struct ei_event_t*	event){
    //ei_impl_toplevel_t* widget_full = (ei_impl_toplevel_t*) widget;
    //ei_widget_t pick_widg = ei_widget_pick(&(((event->param).mouse).where));
    ei_widget_t act_widg = ei_event_get_active_widget();
    ei_rect_t header_rectangle = ei_rect(widget->screen_location.top_left, ei_size(widget->screen_location.size.width, toplevel_bar_height));
    switch (event->type) {
        case ei_ev_mouse_buttondown:
            if (pt_in_rect(event->param.mouse.where.x, event->param.mouse.where.y, &header_rectangle)) {
                ei_event_set_active_widget(widget);
                toplevel_move_click_x = event->param.mouse.where.x;
                toplevel_move_click_y = event->param.mouse.where.y;
                toplevel_move_prev_x = widget->screen_location.top_left.x;
                toplevel_move_prev_y = widget->screen_location.top_left.y;
            }
            break;
        case ei_ev_mouse_buttonup:
            ei_event_set_active_widget(NULL);
            break;
        case ei_ev_mouse_move:
            if (act_widg == widget) {
                ei_place_xy(widget, toplevel_move_prev_x + (event->param.mouse.where.x - toplevel_move_click_x),
                                toplevel_move_prev_y + (event->param.mouse.where.y - toplevel_move_click_y));
                toplevel_move_click_x = event->param.mouse.where.x;
                toplevel_move_click_y = event->param.mouse.where.y;
                toplevel_move_prev_x = widget->screen_location.top_left.x;
                toplevel_move_prev_y = widget->screen_location.top_left.y;
                ei_app_invalidate_rect(&(widget->screen_location));
            }
            break;
        default:
            //normalement ce cas ne peut pas arriver
            break;
    }
    return true;
}