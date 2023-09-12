#include "ei_impl_application.h"
#include "ei_app_global.h"

/**
 * @brief Initializes the global widget list
*/
void ei_init_widget_vec(void){
    widget_list = calloc(1, sizeof(ei_widget_vec));
    widget_list->capacity = default_capacity;
    widget_list->size = 0;
    widget_list->widgets = calloc(default_capacity, sizeof(ei_widget_t));
}

/**
 * @brief Inserts a widget to the back of the widget list
 * @param widget The widget to be inserted
 */
void ei_append_widget(ei_widget_t widget){
    if(widget_list->size < widget_list->capacity){
        widget_list->widgets[widget_list->size] = widget;
        widget_list->size++;
    }
    else{
        ei_widget_t* tmp = calloc(widget_list->capacity * 2, sizeof(ei_widget_t));
        for(size_t i = 0; i < widget_list->capacity; i++) tmp[i] = widget_list->widgets[i];
        widget_list->capacity *= 2;
        free(widget_list->widgets);
        widget_list->widgets = tmp;
        widget_list->widgets[widget_list->size] = widget;
        widget_list->size++;
    }
}

/**
 * @brief Get the i-th widget in the widget list
 * @param position position of the widget in the widget list
 * @return A pointer to the widget
 */
ei_widget_t ei_widget_at(size_t position){
    return widget_list->widgets[position];
}


/**
 * @brief Returns the size of the widget list
 */
size_t ei_widget_list_size(void){
    return widget_list->size;
}

/**
 * @brief Frees the space allocated for the widget list
 */
void ei_widget_vec_free(void){
    free(widget_list->widgets);
    free(widget_list);
}

