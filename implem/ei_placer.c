#include "ei_placer.h"
#include "ei_widget_all.h"
#include "ei_widgetclass.h"
#include "ei_implementation.h"
#include "ei_types.h"
#include "ei_utils_util.h"

void		ei_place	(ei_widget_t		widget,
				 ei_anchor_t*		anchor,
				 int*			x,
				 int*			y,
				 int*			width,
				 int*			height,
				 float*			rel_x,
				 float*			rel_y,
				 float*			rel_width,
				 float*			rel_height){
	// Get the placer parameters for the widget
    ei_impl_placer_params_t* placer_params = widget->placer_params;

    // Allocate a new placer_params structure if the widget is not managed by the placer 
    if (placer_params == NULL) {
        placer_params = calloc(1, sizeof(ei_impl_placer_params_t));
        widget->placer_params = placer_params;
    }

    // Update the placer parameters
    if (anchor != NULL)
        placer_params->anchor = *anchor;
    else
        placer_params->anchor = ei_anc_northwest;

    if (x != NULL)
        placer_params->x = *x;
    else
        placer_params->x = 0;

    if (y != NULL)
        placer_params->y = *y;
    else
        placer_params->y = 0;

    if (width != NULL)
        placer_params->width = *width;
    else if (rel_width == NULL)
        placer_params->width = widget->requested_size.width;
    else
        placer_params->width = 0;

    if (height != NULL)
        placer_params->height = *height;
    else if (rel_height == NULL)
        placer_params->height = widget->requested_size.height;
    else
        placer_params->height = 0;

    if (rel_x != NULL)
        placer_params->rel_x = *rel_x;
    else
        placer_params->rel_x = 0.0;

    if (rel_y != NULL)
        placer_params->rel_y = *rel_y;
    else
        placer_params->rel_y = 0.0;

    if (rel_width != NULL)
        placer_params->rel_width = *rel_width;
    else
        placer_params->rel_width = 0.0;

    if (rel_height != NULL)
        placer_params->rel_height = *rel_height;
    else
        placer_params->rel_height = 0.0;

    ei_impl_placer_run(widget);
}

void ei_placer_forget(ei_widget_t widget)
{
    free(widget->placer_params);
    widget->placer_params = NULL;
}

void ei_impl_placer_run(ei_widget_t widget)
{
    // Checks if the widget is managed by the placer
    ei_widget_t parent = ei_widget_get_parent(widget);
    if (parent == NULL) {
        return;
    }

    // Widget placement widget
    ei_impl_placer_params_t* params = (ei_impl_placer_params_t*)(widget->placer_params);

    ei_rect_t parent_content_rect = *(parent->content_rect);
    ei_rect_t widget_rect;

    // Priority feed for widget size
    if (params->width != 0)
        widget_rect.size.width = params->width;
    else if (params->rel_width != 0)
        widget_rect.size.width = (int)(params->rel_width * parent_content_rect.size.width);
    else
        widget_rect.size.width = widget->requested_size.width;

    if (params->height != 0)
        widget_rect.size.height = params->height;
    else if (params->rel_height != 0)
        widget_rect.size.height = (int)(params->rel_height * parent_content_rect.size.height);
    else
        widget_rect.size.height = widget->requested_size.height;

    widget_rect.size = fit_to_label_size(widget_rect.size, *widget->min_size);
    
    // Calculates absolute widget geometry based on placement parameters and anchor
	ei_point_t anchor_offset;
	ei_size_t widget_size = widget_rect.size;
	switch (params->anchor) {
		case ei_anc_none:
			anchor_offset.x = 0;
			anchor_offset.y = 0;
			break;
		case ei_anc_center:
			anchor_offset.x = -(widget_size.width / 2);
			anchor_offset.y = -(widget_size.height / 2);
			break;
		case ei_anc_north:
			anchor_offset.x = -(widget_size.width / 2);
			anchor_offset.y = 0;
			break;
		case ei_anc_northeast:
			anchor_offset.x = -widget_size.width;
			anchor_offset.y = 0;
			break;
		case ei_anc_east:
			anchor_offset.x = -widget_size.width;
			anchor_offset.y = -(widget_size.height / 2);
			break;
		case ei_anc_southeast:
			anchor_offset.x = -widget_size.width;
			anchor_offset.y = -widget_size.height;
			break;
		case ei_anc_south:
			anchor_offset.x = -(widget_size.width / 2);
			anchor_offset.y = -widget_size.height;
			break;
		case ei_anc_southwest:
			anchor_offset.x = 0;
			anchor_offset.y = -widget_size.height;
			break;
		case ei_anc_west:
			anchor_offset.x = 0;
			anchor_offset.y = -(widget_size.height / 2);
			break;
		case ei_anc_northwest:
			anchor_offset.x = 0;
			anchor_offset.y = 0;
			break;
		default:
			anchor_offset.x = 0;
			anchor_offset.y = 0;
			break;
	}


    widget_rect.top_left.x = parent->content_rect->top_left.x + (int)(params->rel_x * parent_content_rect.size.width) + anchor_offset.x + params->x;
	widget_rect.top_left.y = parent->content_rect->top_left.y + (int)(params->rel_y * parent_content_rect.size.height) + anchor_offset.y + params->y;
	
	 // Updates screen_location
    widget->screen_location = widget_rect;

    //Updates widget content rect
    if(widget->content_rect != &(widget->screen_location)){
        widget->content_rect->size = ei_size(widget_rect.size.width, widget_rect.size.height - toplevel_bar_height);
        widget->content_rect->top_left = ei_point(widget_rect.top_left.x, widget_rect.top_left.y + toplevel_bar_height);
    }

	// Updates children position
    ei_widget_t child = widget->children_head;
    while (child != NULL) {
        ei_impl_placer_run(child);
        child = child->next_sibling;
	}


}

