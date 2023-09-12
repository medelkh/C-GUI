#ifndef EI_WIDGET_BUTTON_H
#define EI_WIDGET_BUTTON_H


#include "ei_widget_all.h"
#include "ei_widgetclass.h"

//button specific structure
typedef struct ei_impl_button_t {
    ei_impl_widget_t widget;
    int corner_radius;
    ei_relief_t relief;
    ei_string_t text;
    ei_font_t text_font;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_size_t* text_rect_sz;
    ei_surface_t img;
    ei_rect_ptr_t img_rect;
    ei_anchor_t img_anchor;
    ei_callback_t callback;
    ei_user_param_t user_param;
} ei_impl_button_t;


/** 
 * \brief Allocation function for a button
 * @return Returns a pointer to a button
*/
ei_widget_t ei_button_allocfunc(void);

/** 
 * \brief Releases the memory allocated for a button
*/
void ei_button_releasefunc(ei_widget_t widget);


/** 
 * \brief Drawing function for a button
*/
void ei_button_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);


/**
 * \brief 	A function that is called to notify the button that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		The new location can be retrieved by \ref ei_widget_get_screen_location.
*/
void ei_button_geomnotifyfunc(ei_widget_t widget);

/**
 * \brief	A function that sets the default values for a button.
 */
void ei_button_setdefaultsfunc(ei_widget_t widget);


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
						 	 struct ei_event_t*	event);

/**
 * the callback function for a close button
 * @param widget the close button
 * @param event the event
 * @param user_param no one cares
 */
void		close_toplevel		(ei_widget_t		widget,
                                             struct ei_event_t*	event,
                                             ei_user_param_t	user_param);

#endif