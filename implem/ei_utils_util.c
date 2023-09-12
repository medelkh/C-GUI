#include "ei_utils_util.h"


/**
 * @brief Inserts an ei_rect_t in a linked list
*/
void ei_insert_rect(ei_rect_t item, ei_linked_rect_t** linked_list){
	ei_linked_rect_t* tmp = *linked_list;
	*linked_list = (ei_linked_rect_t*) malloc(sizeof(ei_linked_rect_t));
	(*linked_list)->rect = item;
	(*linked_list)->next = tmp;
}

/**
 * @brief Frees a linked list of ei_rect_t
*/
void ei_free_rect_ll(ei_linked_rect_t* linked_list){
    if(linked_list == NULL) return;
    ei_free_rect_ll(linked_list->next);
    free(linked_list);
}


/**
 * @brief Converts an ei_color_t to an uint32_t while reordering to the surface's channel order
*/
uint32_t convert_color(ei_surface_t surface, const ei_color_t* color){
    int ir;
    int ig;
    int ib;
    int ia;
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
    char conv_color[4];
    conv_color[ir] = color->red;
    conv_color[ig] = color->green;
    conv_color[ib] = color->blue;
    conv_color[ia] = color->alpha;
	return *((uint32_t*) conv_color);
}

/**
 * @brief Converts an uint32_t to an ei_color_t while reordering to the reverse order of surface's channel
*/
ei_color_t* inv_convert_to_color(ei_surface_t surface, uint32_t color){
    int permutation[4] = {0,1,2,3};
    hw_surface_get_channel_indices(surface, permutation, permutation+1, permutation+2, permutation+3);
    int inv_permutation[4];
    for(size_t i=0;i<4;i++) inv_permutation[permutation[i]] = i;
    ei_color_t* inv_color = calloc(1, sizeof(ei_color_t));
    int eight_ones = (1<<8)-1;
    inv_color->red = color & (eight_ones<<(8*inv_permutation[0]));
    inv_color->green = color & (eight_ones<<(8*inv_permutation[1]));
    inv_color->blue = color & (eight_ones<<(8*inv_permutation[2]));
    inv_color->alpha = color & (eight_ones<<(8*inv_permutation[3]));
    return inv_color;
}

/**
 * @brief Returns whether the point at coordinates (x,y) is in the rectangle
*/
bool pt_in_rect(int x, int y, const ei_rect_t* rect) {
    return (x < rect->top_left.x + rect->size.width && x >= rect->top_left.x && y >= rect->top_left.y && y < rect->top_left.y + rect->size.height);
}


/**
 * @brief Returns whether two sizes are equal
*/
bool sizes_are_equal(ei_size_t size1, ei_size_t size2) {
    return (size1.width == size2.width && size1.height == size2.height);
}

/**
 * @brief Returns the biggest size in the first one that contains the second one
 */
ei_size_t fit_to_label_size(ei_size_t size, ei_size_t label_rect_size){
    return ei_size(MAX(size.width, label_rect_size.width), MAX(size.height, label_rect_size.height));
}