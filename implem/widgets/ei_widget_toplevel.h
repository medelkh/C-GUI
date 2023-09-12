#ifndef EI_WIDGET_TOPLEVEL_H
#define EI_WIDGET_TOPLEVEL_H


#include "ei_widget_all.h"
#include "ei_widgetclass.h"

//toplevel specific structure
typedef struct ei_impl_toplevel_t {
    ei_impl_widget_t widget;
    ei_string_t title;
    bool closable;
    ei_axis_set_t resizable;
} ei_impl_toplevel_t;

extern int close_button_radius;
extern int close_button_border_width;
extern int close_button_pos_x;
extern int close_button_pos_y;
extern int toplevel_bar_height;
extern int toplevel_border_width;
extern int toplevel_corner_radius;
extern int title_pos_x;
extern int title_pos_y;
extern int resize_button_size;

extern ei_color_t grey;
extern ei_color_t white;

/** 
 * \brief Allocation function for a toplevel
 * @return Returns a pointer to a toplevel
*/
ei_widget_t ei_toplevel_allocfunc(void);

/** 
 * \brief Releases the memory allocated for a toplevel
*/
void ei_toplevel_releasefunc(ei_widget_t widget);


/** 
 * \brief Drawing function for a toplevel
*/
void ei_toplevel_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);


/**
 * \brief 	A function that is called to notify the toplevel that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
*/
void ei_toplevel_geomnotifyfunc(ei_widget_t widget);

/**
 * \brief	A function that sets the default values for a toplevel.
 */
void ei_toplevel_setdefaultsfunc(ei_widget_t widget);


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
						 	 struct ei_event_t*	event);

#endif