#ifndef EI_WIDGET_ALL_H
#define EI_WIDGET_ALL_H


#include "ei_types.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_widget_attributes.h"
#include "ei_widget_configure.h"

/**
 * \brief	Fields common to all types of widget. Every widget classes specializes this base
 *		class by adding its own fields. 
 */
typedef struct ei_impl_widget_t {
	ei_widgetclass_t*	wclass;		///< The class of widget of this widget. Avoids the field name "class" which is a keyword in C++.
	uint32_t		pick_id;	///< Id of this widget in the picking offscreen.
	ei_color_t*		pick_color;	///< pick_id encoded as a color.
	void*			user_data;	///< Pointer provided by the programmer for private use. May be NULL.
	ei_widget_destructor_t	destructor;	///< Pointer to the programmer's function to call before destroying this widget. May be NULL.

	/* Widget Hierachy Management */
	ei_widget_t		parent;		///< Pointer to the parent of this widget.
	ei_widget_t		children_head;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
	ei_widget_t		children_tail;	///< Pointer to the last child of this widget.
	ei_widget_t		next_sibling;	///< Pointer to the next child of this widget's parent widget.
    ei_widget_t		previous_sibling;	///< Pointer to the previous child of this widget's parent widget.

	/* Geometry Management */
	struct ei_impl_placer_params_t* placer_params;	///< Pointer to the placer parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
	ei_size_t		requested_size;	///< Size requested by the widget (big enough for its label, for example), or by the programmer. This can be different than its screen size defined by the placer.
	ei_rect_t		screen_location;///< Position and size of the widget expressed in the root window reference.
	ei_rect_t*		content_rect;	///< Where to place children, when this widget is used as a container. By defaults, points to the screen_location.

    ei_color_t color;
    int border_width;
    ei_size_ptr_t min_size;

} ei_impl_widget_t;



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
					ei_rect_t*		clipper);

/**
 * Frees the memory allocated by a widget
 * @param widget widget to be freed
 */
void ei_widget_release(ei_impl_widget_t* widget);

#include "widgets/ei_widget_button.h"
#include "widgets/ei_widget_toplevel.h"
#include "widgets/ei_widget_frame.h"

#endif