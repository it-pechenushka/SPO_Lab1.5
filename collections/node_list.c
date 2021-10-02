#include <stdio.h>
#include "node_list.h"
#include "../store/graph_struct.h"
#include <malloc.h>

int add_node(NodeList *list, NodeInfo *node)
{
    if (list->header == NULL)
    {
        list->header = node;
        list->tail = node;
        node->next = NULL;
        node->prev = NULL;
    } else {
            list->tail->next = node;
            NodeInfo *prev = list->tail; 
            list->tail = node;
            list->tail->prev = prev;
            list->tail->next = NULL;
    }

    list->size++;

    return list->size;
}

int add_label(NodeInfo *node_info, NodeLabelInfo *label_info)
{
    if(node_info->header_label == NULL)
    {
        node_info->header_label = label_info;
        node_info->tail_label = label_info;
        label_info->next = NULL;
        label_info->prev = NULL;
    } else {
        node_info->tail_label->next = label_info;
        NodeLabelInfo *prev = node_info->tail_label;
        node_info->tail_label = label_info;
        node_info->tail_label->prev = prev;
        node_info->tail_label->next = NULL;
    }    

    node_info->labels_len++;

    return node_info->labels_len;
}


int add_prop(NodeInfo *node_info, NodePropsInfo *prop_info)
{
    if(node_info->header_prop == NULL)
    {
        node_info->header_prop = prop_info;
        node_info->tail_prop = prop_info;
        prop_info->next = NULL;
        prop_info->prev = NULL;
    } else {
            node_info->tail_prop->next = prop_info;
            NodePropsInfo *prev = node_info->tail_prop;
            node_info->tail_prop = prop_info;
            node_info->tail_prop->prev = prev;
            node_info->tail_prop->next = NULL;
    }

    node_info->props_len++;

    return node_info->props_len;
}