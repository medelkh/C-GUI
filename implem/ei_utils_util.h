/**
 * @file	ei_utils.h
 *
 * @brief	General purpose utility functions: insertion in a linked list... (extension of ei_utils.h)
 */

#ifndef EI_UTILS_UTIL_H
#define EI_UTILS_UTIL_H

#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_utils.h"

#define MAX(a,b) ( a>=b ? a : b )
#define MIN(a,b) ( a<=b ? a : b )


/**
 * @brief Inserts an ei_rect_t in a linked list
*/
void ei_insert_rect(ei_rect_t item, ei_linked_rect_t** linked_list);

/**
 * @brief Frees a linked list of ei_rect_t
*/
void ei_free_rect_ll(ei_linked_rect_t* linked_list);


/**
 * @brief Converts an ei_color_t to an uint32_t while reordering to the surface's channel order
*/
uint32_t convert_color(ei_surface_t surface, const ei_color_t* color);

/**
 * @brief Converts an uint32_t to an ei_color_t while reordering to the reverse order of surface's channel
*/
ei_color_t* inv_convert_to_color(ei_surface_t surface, uint32_t color);


/**
 * @brief Returns whether the point at coordinates (x,y) is in the rectangle
*/
bool pt_in_rect(int x, int y, const ei_rect_t* rect);

/**
 * @brief Returns whether two sizes are equal
*/
bool sizes_are_equal(ei_size_t size1, ei_size_t size2);

/**
 * @brief Returns the biggest size in the first one that contains the second one
 */
 ei_size_t fit_to_label_size(ei_size_t size, ei_size_t label_rect_size);

#endif