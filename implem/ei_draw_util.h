/**
 * @file	ei_draw_util.h
 *
 * @brief	Custom useful drawing functions
 */

#ifndef EI_DRAW_UTIL_H
#define EI_DRAW_UTIL_H

#include <stdint.h>
#include "ei_types.h"
#include "ei_utils.h"
#include "hw_interface.h"
#include "ei_utils_util.h"

#define PI 3.14159265358979323846


typedef struct ei_polygon_edge_t {
    int ymax; // ordonnée maximale du côté
    int x; // abscisse de l'intersection avec la première scanline
    int E; // terme d'erreur de Bresenham
    int dx; // différence en x entre les deux extrémités du côté
    int dy; // différence en y entre les deux extrémités du côté
    int dx_abs; // valeur absolue de dx
    struct ei_polygon_edge_t* next;
} ei_polygon_edge_t;

/**
 * \brief Inserts an ei_rect_t in a linked list
*/
void ei_insert_edge(int ymax, int x, int E, int dx, int dy, ei_polygon_edge_t** linked_list);

/**
 * \brief Extends an ei_rect_t linked_list by another
*/
void ei_extend_edge_ll(ei_polygon_edge_t** dest, ei_polygon_edge_t* to_add);

/**
 * \brief Removes all edges from a linked list with ymax equal to y
*/
void ei_remove_edges_y(int y, ei_polygon_edge_t** linked_list);

void merge(ei_polygon_edge_t** linked_list, ei_polygon_edge_t* first, ei_polygon_edge_t* second);

/**
 * \brief Splits a linked list to its two halves, assigns the first half to first and the other to second
*/
void split_linked_list(ei_polygon_edge_t* linked_list, ei_polygon_edge_t** first, ei_polygon_edge_t** second);

/**
 * \brief Sorts a linked list of ei_polygon_edge_t according to x
*/
void merge_sort(ei_polygon_edge_t** linked_list);

/**
 * \brief Frees a linked list of ei_polygon_edge_t
*/
void ei_free_edge_ll(ei_polygon_edge_t* linked_list);

/**
 * \brief Returns the intersection of two rectangles
*/
ei_rect_t ei_get_rect_intersection(const ei_rect_t* rect1, const ei_rect_t* rect2);


ei_rect_t hw_surface_get_rect_clip(const ei_rect_t* rect, const ei_surface_t surface);



/**
 * \brief current is a pointer to a pixel, the function returns a pointer to the next pixel that is in the rectangle,
 * ptr_init is a pointer to the first pixel of surface
*/
uint32_t* next_in_rect(ei_surface_t surface, ei_size_t surface_size, uint32_t* ptr_init, const ei_rect_t* rect, uint32_t* current);



/**
 * \brief Creates an arc with a given center, radius, starting and ending angles
 * 
 * @param center center of the circle
 * @param radius radius of the circle
 * @param start_angle the angle of the beginning of the arc, in radians
 * @param end_angle the angle of the end of the arc, in radians
 * @param precision the number of points used to draw the arc
 * 
 * @return an array of points representing the arc
*/
ei_point_t*	ei_arc	(ei_point_t		center,
                    int			radius,
                    double start_angle,
                    double end_angle,
                    size_t precision);



/**
 * \brief Creates a rectangle with rounded corners, either whole if type is 0, the upper part if 1, or the lower part if 2
 * 
 * @param top_left the position of the top left of the rectangle
 * @param size width and height of the rectangle
 * @param radius of the rounded corners
 * @param precision precision of the rounded corners
 * @param type whole rectangle if 0, upper part if 1, lower part if 2
 * 
 * @return an array of points representing the rounded rectangle, size if return array is depending on the type:
 * <ul>
 *  <li>4*precision if type = 0<\li>
 *  <li>2*precision+2 otherwise<\li>
 * <\ul>
*/
ei_point_t* ei_rounded_rect(ei_point_t top_left,
                            ei_size_t size,
                            int radius,
                            size_t precision,
                            uint8_t type);



/**
 * \brief Draws a button
 * 
 * @param surface the surface to be drawn on
 * @param top_left the top left position of the button
 * @param size the width and height of the button
 * @param color the button's color
 * @param clipper the clipping surface as usual
*/
void ei_draw_button(ei_surface_t surface,
                    ei_point_t top_left,
                    ei_size_t size,
                    ei_color_t color,
                    int border_width,
                    int radius,
                    ei_relief_t relief,
                    ei_string_t text,
                    ei_font_t text_font,
                    ei_color_t* text_color,
                    ei_anchor_t text_anchor,
                    ei_size_t* text_rect_sz,
                    ei_surface_t* img,
                    ei_rect_ptr_t img_rect,
                    ei_anchor_t img_anchor,
                    ei_rect_t* clipper);



/**
 * \brief déréférence juste le pointeur, avec cast, utilisé pour éviter des if dans copy_surface
*/
void deref(uint8_t* ptr_src, uint8_t* ptr_dst, int* tab_channel_indices);



/**
 * \brief formule de la page 15
*/
char formule_couleur(int16_t s, int16_t p_coul, int16_t p_a);



/**
 * \brief pour copy_surface dans le cas où alpha est true
*/
void formule_couleurs(uint8_t* ptr_src, uint8_t* ptr_dst, int* tab_channel_indices);

#endif