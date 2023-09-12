/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */

#ifndef EI_IMPLEMENTATION_H
#define EI_IMPLEMENTATION_H

#include "hw_interface.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_widget_all.h"


/**
 * \brief	A structure storing the placement parameters of a widget.
 *		You have to define this structure: no suggestion provided.
 */
typedef struct ei_impl_placer_params_t {
    ei_anchor_t anchor;         // How to anchor the widget
    int x;                      // Absolute x position of the widget
    int y;                      // Absolute y position of the widget
    int width;                  // Absolute width of the widget
    int height;                 // Absolute height of the widget
    float rel_x;                // Relative x position of the widget
    float rel_y;                // Relative y position of the widget
    float rel_width;            // Relative width of the widget
    float rel_height;           // Relative height of the widget
} ei_impl_placer_params_t;





/**
 * \brief	Tells the placer to recompute the geometry of a widget.
 *		The widget must have been previsouly placed by a call to \ref ei_place.
 *		Geometry re-computation is necessary for example when the text label of
 *		a widget has changed, and thus the widget "natural" size has changed.
 *
 * @param	widget		The widget which geometry must be re-computed.
 */
void ei_impl_placer_run(ei_widget_t widget);


#endif
