#include "ei_draw.h"
#include "ei_draw_util.h"


//ei_draw_polyline
void	ei_draw_polyline	(ei_surface_t		surface,
				            ei_point_t*		point_array,
				            size_t			point_array_size,
				            ei_color_t		color,
				            const ei_rect_t*	clipper){

    //Get size nxm of the surface
    ei_size_t surface_size = hw_surface_get_size(surface);

    uint32_t converted_color = convert_color(surface, &color);

    size_t i = 0;

    for(; i+1 < point_array_size; i++){
        int start_x = point_array[i].x;
        int end_x = point_array[i+1].x;
        int start_y = point_array[i].y;
        int end_y = point_array[i+1].y;

        int dx_abs = end_x - start_x;
        int dy_abs = end_y - start_y;
        
        int8_t dx_sign = (dx_abs > 0 ? 1 : (dx_abs < 0 ? -1 : 0));
        int8_t dy_sign = (dy_abs > 0 ? 1 : (dy_abs < 0 ? -1 : 0));

        dx_abs *= dx_sign;
        dy_abs *= dy_sign;

        int error = 0;

        if(clipper==NULL || 
            ((start_x >= (clipper->top_left).x) && (start_x <= (clipper->top_left).x + (clipper->size).width) &&
            (start_y >= (clipper->top_left).y) && (start_y <= (clipper->top_left).y + (clipper->size).height))){
                *((uint32_t*)hw_surface_get_buffer(surface) + (start_y*(surface_size.width) + start_x)) = converted_color;
        }

        if(dx_abs >= dy_abs){
            while(true){
                start_x += dx_sign;
                error += dy_abs;
                if(2*error > dx_abs){
                    start_y += dy_sign;
                    error -= dx_abs;
                }
                if(clipper==NULL || 
                    ((start_x >= (clipper->top_left).x) && (start_x <= (clipper->top_left).x + (clipper->size).width) &&
                    (start_y >= (clipper->top_left).y) && (start_y <= (clipper->top_left).y + (clipper->size).height))){
                        *((uint32_t*)hw_surface_get_buffer(surface) + (start_y*(surface_size.width) + start_x)) = converted_color;
                }
                if(start_x == end_x) break;
            }
        }
        else{
            while(true){
                start_y += dy_sign;
                error += dx_abs;
                if(2*error > dy_abs){
                    start_x += dx_sign;
                    error -= dy_abs;
                }
                if(clipper==NULL || 
                    ((start_x >= (clipper->top_left).x) && (start_x <= (clipper->top_left).x + (clipper->size).width) &&
                    (start_y >= (clipper->top_left).y) && (start_y <= (clipper->top_left).y + (clipper->size).height))){
                        *((uint32_t*)hw_surface_get_buffer(surface) + (start_y*(surface_size.width) + start_x)) = converted_color;
                }
                if(start_y == end_y) break;
            }
        }

    }
}

//makes a single step (processing a scanline) for TCA
void TCA_step(ei_polygon_edge_t* edge){
    while(edge != NULL){
        edge->E += edge->dx_abs;
        int step = (2 * (edge->E)) / (edge->dy);
        edge->x += ( edge->dx >= 0 ? step : -step );
        edge->E -= step * (edge->dy);
        edge = edge->next;
    }
}

//ei_draw_polygon
void	ei_draw_polygon		(ei_surface_t		surface,
				 ei_point_t*		point_array,
				 size_t			point_array_size,
				 ei_color_t		color,
				 const ei_rect_t*	clipper){

    //Get size nxm of the surface
    ei_size_t surface_size = hw_surface_get_size(surface);


    //convert color to uint32_t
    uint32_t converted_color = convert_color(surface, &color);

    //find the y-range of the polygon
    int ytop = surface_size.height;
    int polygon_height = 0;
    for(size_t i = 0; i < point_array_size; i++){
        ytop = MIN(ytop, point_array[i].y);
        polygon_height = MAX(polygon_height, point_array[i].y); 
    }
    polygon_height = polygon_height-ytop+1;

    //initialisation du tableau des cotes
    ei_polygon_edge_t** TC = (ei_polygon_edge_t**) calloc(polygon_height, sizeof(ei_polygon_edge_t*));
    for(size_t i = 0; i < point_array_size; i++){
        if(point_array[(i+1)%point_array_size].y == point_array[i].y) continue;
        if(point_array[(i+1)%point_array_size].y > point_array[i].y){
            ei_insert_edge(point_array[(i+1)%point_array_size].y, 
                        point_array[i].x, 
                        0, 
                        point_array[(i+1)%point_array_size].x-point_array[i].x, 
                        point_array[(i+1)%point_array_size].y-point_array[i].y, 
                        &TC[point_array[i].y - ytop]);
        }
        else{
            ei_insert_edge(point_array[i].y, 
                        point_array[(i+1)%point_array_size].x, 
                        0, 
                        point_array[i].x-point_array[(i+1)%point_array_size].x, 
                        point_array[i].y-point_array[(i+1)%point_array_size].y, 
                        &TC[point_array[(i+1)%point_array_size].y - ytop]);
        }
    }
    //running the scanline algorithm
    ei_polygon_edge_t* TCA = NULL;

    uint32_t* origin = (uint32_t*)hw_surface_get_buffer(surface);
    for(int i = 0; i < polygon_height; i++){
        ei_remove_edges_y(ytop+i, &TCA);
        TCA_step(TCA);
        ei_extend_edge_ll(&TCA, TC[i]);
        merge_sort(&TCA);
        ei_polygon_edge_t* cur = TCA;
        int y = ytop+i;
        while(cur!=NULL && cur->next!=NULL){
            uint32_t* ptr = origin+((ytop+i) * surface_size.width +(cur->x));
            int x = cur->x;
            if(clipper==NULL || 
                ((x >= (clipper->top_left).x) && (x <= (clipper->top_left).x + (clipper->size).width) &&
                (y >= (clipper->top_left).y) && (y <= (clipper->top_left).y + (clipper->size).height))){
                    *(ptr) = converted_color;
            }
            for(int j = 1; j < (cur->next->x) - (cur->x); j++){
                ptr++;
                x++;
                if(clipper==NULL || 
                    ((x >= (clipper->top_left).x) && (x <= (clipper->top_left).x + (clipper->size).width) &&
                    (y >= (clipper->top_left).y) && (y <= (clipper->top_left).y + (clipper->size).height))){
                        *(ptr) = converted_color;
                }
            }
            cur = cur->next->next;
        }
    }

    //free the allocated memory
    free(TC);
}


//ei_fill
void    ei_fill            (ei_surface_t        surface,
                 const ei_color_t*    color,
                 const ei_rect_t*    clipper){

    uint32_t converted_color = convert_color(surface, color);
    ei_size_t draw_size = hw_surface_get_size(surface);
    uint32_t* pixel_ptr = (uint32_t*) hw_surface_get_buffer(surface);
    int x;
    int y;

    if (clipper == NULL) {
        for (int i = 0; i < draw_size.height*draw_size.width; i++) {
            x = i % draw_size.width;
            y = i / draw_size.width;
            *pixel_ptr = converted_color;
            pixel_ptr++;
        }
    } 
    else {
        for (int i = 0; i < draw_size.height*draw_size.width; i++) {
            x = i % draw_size.width;
            y = i / draw_size.width;
            if (x < clipper->top_left.x + clipper->size.width && x >= clipper->top_left.x && y >= clipper->top_left.y && y < clipper->top_left.y + clipper->size.height) {
                *pixel_ptr = converted_color;
            }
            pixel_ptr++;
        }
    }
}


//copy surface

int	ei_copy_surface		(ei_surface_t		destination,
				 const ei_rect_t*	dst_rect,
				 ei_surface_t		source,
				 const ei_rect_t*	src_rect,
				 bool			alpha) {
    hw_surface_lock(source);
    uint32_t* ptr_dst = (uint32_t*) hw_surface_get_buffer(destination);
    uint32_t* ptr_src = (uint32_t*) hw_surface_get_buffer(source);
    ei_size_t src_size = hw_surface_get_size(source);
    ei_size_t dst_size = hw_surface_get_size(destination);
    ei_rect_t dst_as_rect = ei_rect(ei_point(0, 0), dst_size);
    ei_rect_t src_as_rect = ei_rect(ei_point(0, 0), src_size);
    ei_size_t surface_size;
    if (dst_rect == NULL) {
        surface_size = dst_size;
        dst_rect = &dst_as_rect;
    } else {
        surface_size = dst_rect->size;
    }
    if (src_rect == NULL) {
        src_rect = &src_as_rect;
    }
    if (!(sizes_are_equal(dst_rect->size, src_rect->size))) {
        hw_surface_unlock(source);
        return 1;
    }
    void (*f)(uint8_t*, uint8_t*, int*);
    int tab_channel_indices[8];
    if (alpha) {
        hw_surface_get_channel_indices(destination, tab_channel_indices+4, tab_channel_indices+5, tab_channel_indices+6, tab_channel_indices+7);
        hw_surface_get_channel_indices(source, tab_channel_indices, tab_channel_indices+1, tab_channel_indices+2, tab_channel_indices+3);
        f = &formule_couleurs;
    }
    else {
        f = &deref;
    }
    uint32_t* src_temp_ptr;
    uint32_t* dst_temp_ptr;
    for (int i = 0; i < surface_size.height; ++i) {
        src_temp_ptr = ptr_src + ((src_rect->top_left.y+i) * src_size.width + src_rect->top_left.x);
        dst_temp_ptr = ptr_dst + ((dst_rect->top_left.y+i) * dst_size.width + dst_rect->top_left.x);
        for(int j=0; j< surface_size.width;j++) {
            f((uint8_t*)src_temp_ptr, (uint8_t*)dst_temp_ptr, tab_channel_indices);
            src_temp_ptr++;
            dst_temp_ptr++;
        }
    }
    hw_surface_unlock(source);
    return 0;
}



void ei_draw_text(ei_surface_t surface, const ei_point_t* where, ei_const_string_t text, ei_font_t font, ei_color_t color, const ei_rect_t* clipper) {
    if(text[0] == '\0') return;
    if (font == NULL) {
        if(ei_default_font == NULL) ei_default_font = hw_text_font_create(ei_default_font_filename, ei_style_normal, ei_font_default_size);
        font = ei_default_font;
    }
    ei_surface_t text_surface = hw_text_create_surface(text, font, color);
    ei_size_t text_size = hw_surface_get_size(text_surface);
    ei_rect_t dst_rect = ei_rect(*where, text_size);
    if (clipper != NULL) {
        dst_rect = ei_get_rect_intersection(&dst_rect, clipper);
    }
    ei_rect_t src_rect = ei_rect(ei_point_zero(), text_size);
    ei_copy_surface(surface, &dst_rect, text_surface, &src_rect, true);
    hw_surface_free(text_surface);
}
