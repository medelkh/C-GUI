#include "ei_draw_util.h"


/**
 * \brief Inserts an ei_rect_t in a linked list
*/
void ei_insert_edge(int ymax, int x, int E, int dx, int dy, ei_polygon_edge_t** linked_list){
	ei_polygon_edge_t* tmp = *linked_list;
	*linked_list = (ei_polygon_edge_t*) malloc(sizeof(ei_polygon_edge_t));
	(*linked_list)->ymax = ymax;
	(*linked_list)->x = x;
	(*linked_list)->E = E;
	(*linked_list)->dx = dx;
	(*linked_list)->dy = dy;
	(*linked_list)->dx_abs = (dx>=0 ? dx : -dx);
	(*linked_list)->next = tmp;
}

/**
 * \brief Extends an ei_rect_t linked_list by another
*/
void ei_extend_edge_ll(ei_polygon_edge_t** dest, ei_polygon_edge_t* to_add){
    ei_polygon_edge_t* cur = *dest;
    if(cur==NULL){
        *dest = to_add;
        return;
    }
    while(cur->next!=NULL){
        cur = cur->next;
    }
    cur->next = to_add;
}

/**
 * \brief Removes all edges from a linked list with ymax equal to y
*/
void ei_remove_edges_y(int y, ei_polygon_edge_t** linked_list){
	while(true){
        if(*linked_list == NULL) return;
        if((*linked_list)->ymax != y) break;
        ei_polygon_edge_t* tmp = (*linked_list)->next;
        free(*linked_list);
        *linked_list = tmp;
    }
    ei_polygon_edge_t* cur = *linked_list;
    while(true){
        if(cur->next == NULL) return;
        if((cur->next)->ymax != y){
            cur = cur->next;
            continue;
        }
        ei_polygon_edge_t* tmp = (cur->next)->next;
        free(cur->next);
        cur->next = tmp;
    }
}

void merge(ei_polygon_edge_t** linked_list, ei_polygon_edge_t* first, ei_polygon_edge_t* second){
    while(first!=NULL || second!=NULL){
        if(first!=NULL && second!=NULL){
            if((first->x) <= (second->x)){
                *linked_list = first;
                first = first->next;
            }
            else{
                *linked_list = second;
                second = second->next;
            }
        }
        else if(first!=NULL){
            *linked_list = first;
            first = first->next;
        }
        else{
            *linked_list = second;
            second = second->next;
        }
        linked_list = &((*linked_list)->next);
    }
}

/**
 * \brief Splits a linked list to its two halves, assigns the first half to first and the other to second
*/
void split_linked_list(ei_polygon_edge_t* linked_list, ei_polygon_edge_t** first, ei_polygon_edge_t** second){
    ei_polygon_edge_t* slow = linked_list;
    ei_polygon_edge_t* fast = linked_list->next;

    while(fast!=NULL){
        fast = fast->next;
        if(fast!=NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }

    *first = linked_list;
    *second = slow->next;
    slow->next = NULL; 
}

/**
 * \brief Sorts a linked list of ei_polygon_edge_t according to x
*/
void merge_sort(ei_polygon_edge_t** linked_list){
    ei_polygon_edge_t* first;
    ei_polygon_edge_t* second;

    if(*linked_list == NULL || (*linked_list)->next == NULL) return;

    split_linked_list(*linked_list, &first, &second);

    merge_sort(&first);
    merge_sort(&second);

    merge(linked_list, first, second);
}

/**
 * \brief Frees a linked list of ei_polygon_edge_t
*/
void ei_free_edge_ll(ei_polygon_edge_t* linked_list){
    if(linked_list == NULL) return;
    ei_free_edge_ll(linked_list->next);
    free(linked_list);
}

/**
 * \brief Returns the intersection of two rectangles
*/
ei_rect_t ei_get_rect_intersection(const ei_rect_t* rect1, const ei_rect_t* rect2)
{
    if(rect1==NULL) return *rect2;
    else if(rect2==NULL) return *rect1;
    ei_rect_t inter_rect;
    inter_rect.top_left.x = MAX(rect1->top_left.x, rect2->top_left.x);
    inter_rect.top_left.y = MAX(rect1->top_left.y, rect2->top_left.y);
    inter_rect.size.width = MIN(rect1->top_left.x + rect1->size.width, rect2->top_left.x + rect2->size.width) - inter_rect.top_left.x;
    inter_rect.size.height = MIN(rect1->top_left.y + rect1->size.height, rect2->top_left.y + rect2->size.height) - inter_rect.top_left.y;
    if (inter_rect.size.width <= 0 || inter_rect.size.height <= 0) {
        inter_rect.size.width = -1;
        inter_rect.size.height = -1;
    }
    return inter_rect;
}


ei_rect_t hw_surface_get_rect_clip(const ei_rect_t* rect, const ei_surface_t surface)
{
    ei_rect_t clip_rect;
    clip_rect = hw_surface_get_rect(surface);
    clip_rect = ei_get_rect_intersection(rect, &clip_rect);
    return clip_rect;
}

/**
 * \brief current is a pointer to a pixel, the function returns a pointer to the next pixel that is in the rectangle,
 * ptr_init is a pointer to the first pixel of surface
*/
uint32_t* next_in_rect(ei_surface_t surface, ei_size_t surface_size, uint32_t* ptr_init, const ei_rect_t* rect, uint32_t* current) {
    int32_t j = current - ptr_init;
    int x = j % surface_size.width;
    int y = j / surface_size.width;
    while ((!(pt_in_rect(x, y, rect)))) {
        current++;
        j++;
        x = j % surface_size.width;
        y = j / surface_size.width;
    }
    return current;
}

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
                    size_t precision){
    if(radius == 0){
        ei_point_t* ret = calloc(precision, sizeof(ei_point_t));
        for(size_t i = 0; i < precision; i++) ret[i] = center;
        return ret;
    }
    //the angle step corresponding to the division of the arc into $precision$ subarcs, as well as its sin and cos
    double angle_step = (end_angle - start_angle) / (double)(precision - 1);
    double sin_angle_step = -sin(angle_step);
    double cos_angle_step = cos(angle_step);

    //initialize the array of points and starting point
    ei_point_t* arc = calloc(precision, sizeof(ei_point_t));
    arc[0] = ei_point(radius*cos(start_angle), radius*sin(-start_angle));
    double prev_x = (double)arc[0].x;
    double prev_y = (double)arc[0].y;
    arc[0] = ei_point_add(arc[0], center);

    //filling the rest of the points (each one deduced from the previous one and the cos(a+e) and sin(a+e) formulas)
    double cur_x;
    double cur_y;
    for(size_t i = 1; i < precision-1; i++){
        cur_x = prev_x*cos_angle_step - prev_y*sin_angle_step;
        cur_y = prev_y*cos_angle_step + prev_x*sin_angle_step;
        prev_x = cur_x;
        prev_y = cur_y;
        arc[i] = ei_point((int)cur_x + center.x, (int)cur_y + center.y);
    }
    arc[precision-1] = ei_point(center.x + radius*cos(end_angle), center.y + radius*sin(-end_angle));

    return arc; 
}

/**
 * \brief Creates a rectangle with rounded corners, either whole if type is 0, the upper part if 1, or the lower part if 2
 * 
 * @param top_left the position of the top left of the rectangle
 * @param size width and height of the rectangle
 * @param radius of the rounded corners
 * @param precision precision of the rounded corners
 * @param type whole rectangle if 0, upper part if 1, lower part if 2, whole rect with upper rounded corners only if 3
 * 
 * @return an array of points representing the rounded rectangle, size if return array is depending on the type:
 * <ul>
 *  <li>4*precision if type = 0</li>
 *  <li>2*precision+2 otherwise</li>
 * </ul>
*/
ei_point_t* ei_rounded_rect(ei_point_t top_left,
                            ei_size_t size,
                            int radius,
                            size_t precision,
                            uint8_t type){
    ei_point_t* rounded_rect;

    int h = MIN(size.width, size.height)/2.;
    radius = MIN(radius, h);

    //whole rounded rectangle
    if(type==0){
        //the returned array of points
        rounded_rect = calloc(4*precision, sizeof(ei_point_t));

        //each corner's set of points
        ei_point_t* top_left_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + radius), 
                                            radius,
                                            PI/2. , PI,
                                            precision);
        ei_point_t* bottom_left_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + size.height - radius), 
                                            radius,
                                            PI , 3.*PI/2.,
                                            precision);
        ei_point_t* bottom_right_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + size.height - radius), 
                                            radius,
                                            3.*PI/2. , 2.*PI,
                                            precision);
        ei_point_t* top_right_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + radius), 
                                            radius,
                                            0. , PI/2.,
                                            precision);
        
        //filling the whole rectangle with each point from the corners
        for(size_t i = 0; i < precision; i++) rounded_rect[i] = top_left_curve[i];
        for(size_t i = 0; i < precision; i++) rounded_rect[precision + i] = bottom_left_curve[i];
        for(size_t i = 0; i < precision; i++) rounded_rect[2*precision + i] = bottom_right_curve[i];
        for(size_t i = 0; i < precision; i++) rounded_rect[3*precision + i] = top_right_curve[i];

        //freeing the corner point arrays
        free(top_left_curve);
        free(bottom_left_curve);
        free(bottom_right_curve);
        free(top_right_curve);
    }

    //upper rounded rectangle
    else if(type==1){
        //the returned array of points
        rounded_rect = calloc(2*precision+2, sizeof(ei_point_t));

        //each corner's set of points
        ei_point_t* top_right_half_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + radius), 
                                            radius,
                                            PI/4. , PI/2.,
                                            (size_t) (precision/2));
        ei_point_t* top_left_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + radius), 
                                            radius,
                                            PI/2. , PI,
                                            precision);
        ei_point_t* bottom_left_half_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + size.height - radius), 
                                            radius,
                                            PI , 5.*PI/4.,
                                            (size_t)((precision+1)/2));
        
        //filling the rectangle array with each point from the corners
        for(size_t i = 0; i < (size_t) (precision/2); i++) rounded_rect[i] = top_right_half_curve[i];
        for(size_t i = 0; i < precision; i++) rounded_rect[(size_t) (precision/2) + i] = top_left_curve[i];
        for(size_t i = 0; i < (size_t)((precision+1)/2); i++) rounded_rect[(size_t) (precision/2) + precision + i] = bottom_left_half_curve[i];
        
        //completing the half rectangle with the middle horizontal line
        if(size.width>=size.height){
            rounded_rect[2*precision] = ei_point(top_left.x + h, top_left.y + h);
            rounded_rect[2*precision+1] = ei_point(top_left.x + size.width - h, top_left.y + h);
        }
        else{
            rounded_rect[2*precision] = ei_point(top_left.x + h, top_left.y + size.height - h);
            rounded_rect[2*precision+1] = ei_point(top_left.x + h, top_left.y + h);
        }

        //freeing the corner point arrays
        free(top_left_curve);
        free(top_right_half_curve);
        free(bottom_left_half_curve);
    }

    //lower rounded rectangle
    else if(type==2){
        //the returned array of points
        rounded_rect = calloc(2*precision+2, sizeof(ei_point_t));

        //each corner's set of points
        ei_point_t* bottom_left_half_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + size.height - radius), 
                                            radius,
                                            5.*PI/4., 3.*PI/2.,
                                            (size_t)(precision/2));
        ei_point_t* bottom_right_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + size.height - radius), 
                                            radius,
                                            3.*PI/2. , 2.*PI,
                                            precision);
        ei_point_t* top_right_half_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + radius), 
                                            radius,
                                            0. , PI/4.,
                                            (size_t)((precision+1)/2));
        
        //filling the rectangle array with each point from the corners
        for(size_t i = 0; i < (size_t) (precision/2); i++) rounded_rect[i] = bottom_left_half_curve[i];
        for(size_t i = 0; i < precision; i++) rounded_rect[(size_t) (precision/2) + i] = bottom_right_curve[i];
        for(size_t i = 0; i < (size_t)((precision+1)/2); i++) rounded_rect[(size_t) (precision/2) + precision + i] = top_right_half_curve[i];
        
        //completing the half rectangle with the middle horizontal line
        if(size.width>=size.height){
            rounded_rect[2*precision+1] = ei_point(top_left.x + h, top_left.y + h);
            rounded_rect[2*precision] = ei_point(top_left.x + size.width - h, top_left.y + h);
        }
        else{
            rounded_rect[2*precision+1] = ei_point(top_left.x + h, top_left.y + size.height - h);
            rounded_rect[2*precision] = ei_point(top_left.x + h, top_left.y + h);
        }

        //freeing the corner point arrays
        free(bottom_left_half_curve);
        free(top_right_half_curve);
        free(bottom_right_curve);
    }
    else if(type==3){
        //the returned array of points
        rounded_rect = calloc(2*precision+2, sizeof(ei_point_t));

        //each corner's set of points
        ei_point_t* top_left_curve = ei_arc(ei_point(top_left.x + radius, top_left.y + radius),
                                            radius,
                                            PI/2. , PI,
                                            precision);
        ei_point_t* top_right_curve = ei_arc(ei_point(top_left.x + size.width - radius, top_left.y + radius),
                                             radius,
                                             0. , PI/2.,
                                             precision);

        //filling the whole rectangle with each point from the corners
        for(size_t i = 0; i < precision; i++) rounded_rect[i] = top_left_curve[i];
        rounded_rect[precision] = ei_point(top_left.x, top_left.y + size.height);
        rounded_rect[precision+1] = ei_point(top_left.x + size.width, top_left.y + size.height);
        for(size_t i = 0; i < precision; i++) rounded_rect[precision + 2 + i] = top_right_curve[i];

        //freeing the corner point arrays
        free(top_left_curve);
        free(top_right_curve);
    }
    return rounded_rect;
}

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
                    ei_rect_t* clipper){
    //getting the darker versions of the color
    ei_color_t darker_color = (ei_color_t){(int)(color.red*0.6), (int)(color.green*0.6), (int)(color.blue*0.6), color.alpha};
    ei_color_t mid_darker_color = (ei_color_t){(int)(color.red*0.8), (int)(color.green*0.8), (int)(color.blue*0.8), color.alpha};

    //number of points used to draw each arc
    int precision = radius*2+2;

    ei_point_t* lower_background;
    ei_point_t* higher_background;
    ei_point_t* main_button_rect;
    ei_rect_t button_clip;

    if(relief != ei_relief_none){
        ei_point_t top_left_border = ei_point(top_left.x - border_width, top_left.y - border_width);
        ei_size_t size_border = ei_size(size.width + 2 * border_width, size.height + 2 * border_width);
        lower_background = ei_rounded_rect(top_left_border, size_border, radius, precision, 2);
        higher_background = ei_rounded_rect(top_left_border, size_border, radius, precision, 1);
        button_clip = ei_rect(ei_point(top_left.x, top_left.y),
                            ei_size(size.width, size.height));
        main_button_rect = ei_rounded_rect(button_clip.top_left, button_clip.size, radius, precision, 0);


        ei_draw_polygon(surface, lower_background, 2*precision+2, (relief == ei_relief_raised ? darker_color : color), clipper);
        ei_draw_polygon(surface, higher_background, 2*precision+2,(relief == ei_relief_raised ? color : darker_color), clipper);
        ei_draw_polygon(surface, main_button_rect, 4*precision, mid_darker_color, clipper);

        free(lower_background);
        free(higher_background);
        free(main_button_rect);
    }
    else{
        button_clip = ei_rect(ei_point(top_left.x, top_left.y), ei_size(size.width, size.height));
        main_button_rect = ei_rounded_rect(button_clip.top_left, button_clip.size, radius, precision, 0);

        ei_draw_polygon(surface, main_button_rect, 4*precision, color, clipper);
        free(main_button_rect);
    }


    if(text){
        if(text[0] == '\0') return;
        ei_point_t text_position = ei_point(top_left.x+radius, top_left.y+radius);
        ei_size_t size_available = ei_size(size.width -  2*radius, size.height - 2*radius);
        if(text_anchor == ei_anc_center){
            text_position = ei_point(text_position.x + size_available.width/2 - text_rect_sz->width/2, text_position.y + size_available.height/2 - text_rect_sz->height/2);
        }
        else if(text_anchor == ei_anc_east){
            text_position = ei_point(text_position.x + size_available.width - text_rect_sz->width, text_position.y + size_available.height/2 - text_rect_sz->height/2);
        }
        else if(text_anchor == ei_anc_north){
            text_position = ei_point(text_position.x + size_available.width/2 - text_rect_sz->width/2, text_position.y);
        }
        else if(text_anchor == ei_anc_south){
            text_position = ei_point(text_position.x + size_available.width/2 - text_rect_sz->width/2, text_position.y + size_available.height - text_rect_sz->height);
        }
        else if(text_anchor == ei_anc_northeast){
            text_position = ei_point(text_position.x + size_available.width - text_rect_sz->width, text_position.y);
        }
        else if(text_anchor == ei_anc_northwest){
            //unchanged
        }
        else if(text_anchor == ei_anc_southeast){
            text_position = ei_point(text_position.x + size_available.width - text_rect_sz->width, text_position.y + size_available.height - text_rect_sz->height);
        }
        else if(text_anchor == ei_anc_southwest){
            text_position = ei_point(text_position.x, text_position.y + size_available.height - text_rect_sz->height);
        }
        else if(text_anchor == ei_anc_west){
            text_position = ei_point(text_position.x, text_position.y + size_available.height/2 - text_rect_sz->height/2);
        }
        //move the text down a bit when button is pressed
        if(relief == ei_relief_sunken){
            text_position = ei_point(text_position.x + 2, text_position.y + 3);
        }
        ei_rect_t text_clip = ei_get_rect_intersection(clipper, &button_clip);
        ei_draw_text(surface, &text_position, text, text_font, *text_color, &text_clip);
    }
    else if(img){
        ei_point_t img_position = ei_point(top_left.x+radius, top_left.y+radius);
        ei_size_t size_available = ei_size(size.width -  2*radius, size.height - 2*radius);
        if(img_anchor == ei_anc_center){
            img_position = ei_point(img_position.x + size_available.width/2 - img_rect->size.width/2, img_position.y + size_available.height/2 - img_rect->size.height/2);
        }
        else if(img_anchor == ei_anc_east){
            img_position = ei_point(img_position.x + size_available.width - img_rect->size.width, img_position.y + size_available.height/2 - img_rect->size.height/2);
        }
        else if(img_anchor == ei_anc_north){
            img_position = ei_point(img_position.x + size_available.width/2 - img_rect->size.width/2, img_position.y);
        }
        else if(img_anchor == ei_anc_south){
            img_position = ei_point(img_position.x + size_available.width/2 - img_rect->size.width/2, img_position.y + size_available.height - img_rect->size.height);
        }
        else if(img_anchor == ei_anc_northeast){
            img_position = ei_point(img_position.x + size_available.width - img_rect->size.width, img_position.y);
        }
        else if(img_anchor == ei_anc_northwest){
            //unchanged
        }
        else if(img_anchor == ei_anc_southeast){
            img_position = ei_point(img_position.x + size_available.width - img_rect->size.width, img_position.y + size_available.height - img_rect->size.height);
        }
        else if(img_anchor == ei_anc_southwest){
            img_position = ei_point(img_position.x, img_position.y + size_available.height - img_rect->size.height);
        }
        else if(img_anchor == ei_anc_west){
            img_position = ei_point(img_position.x, img_position.y + size_available.height/2 - img_rect->size.height/2);
        }
        //move the text down a bit when button is pressed
        if(relief == ei_relief_sunken){
            img_position = ei_point(img_position.x + 2, img_position.y + 3);
        }
        ei_rect_t img_pos = ei_rect(img_position, img_rect->size);
        ei_rect_t img_clip = ei_get_rect_intersection(clipper, &img_pos);
        ei_copy_surface(surface, &img_clip, img, img_rect, false);
    }

}

/**
 * \brief déréférence juste le pointeur, avec cast, utilisé pour éviter des if dans copy_surface
*/
void deref(uint8_t* ptr_src, uint8_t* ptr_dst, int* tab_channel_indices) {
    *((uint32_t*) ptr_dst) = *((uint32_t*) ptr_src);
}

/**
 * \brief formule de la page 15
*/
char formule_couleur(int16_t s, int16_t p_coul, int16_t p_a) {
    return (p_a*p_coul + (255-p_a)*s)/255;
}

/**
 * \brief pour copy_surface dans le cas où alpha est true
*/
void formule_couleurs(uint8_t* ptr_src, uint8_t* ptr_dst, int* tab_channel_indices) {
    ptr_dst[tab_channel_indices[4]] = formule_couleur((int16_t) ptr_dst[tab_channel_indices[4]], (int16_t) ptr_src[tab_channel_indices[0]], (int16_t) ptr_src[tab_channel_indices[3]]);
    ptr_dst[tab_channel_indices[5]] = formule_couleur((int16_t) ptr_dst[tab_channel_indices[5]], (int16_t) ptr_src[tab_channel_indices[1]], (int16_t) ptr_src[tab_channel_indices[3]]);
    ptr_dst[tab_channel_indices[6]] = formule_couleur((int16_t) ptr_dst[tab_channel_indices[6]], (int16_t) ptr_src[tab_channel_indices[2]], (int16_t) ptr_src[tab_channel_indices[3]]);
}