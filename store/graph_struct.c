#include <stdio.h>
#include "graph_struct.h"
#include "../collections/node_list.h"

FILE *file;
NodeList *nodes; 

NodeList *init_node_list()
{
    NodeList *list = (NodeList*) malloc(sizeof(NodeList));
    list->header = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

int restore_db()
{
    //printf("status %d\n", file == NULL);
    int obj_type = -1;

    while (!feof(file))
    {
        // printf("pos code: %lu\n", ftell(file));
        if(fread(&obj_type, OFFSET_TYPE, 1, file) < 1)
        {
            if(feof(file))
                break;
            
            int error = ferror(file);
            printf("Error num: %d\n", error);

        }
// printf("pos code after: %lu\n", ftell(file));
        if(obj_type == NODE_LABELS_CODE)
            fseek(file, OFFSET_LABEL, SEEK_CUR);
            
        if(obj_type == NODE_PROPS_CODE)
            fseek(file, OFFSET_PROP, SEEK_CUR);
            
        
        if(obj_type == NODE_CODE)
        {
            NodeInfo *node_info = init_node_info();
            node_info->pos = ftell(file);
            node_info->node_item = init_node_item();
            // printf("pos node: %lu\n", ftell(file));
            fread(node_info->node_item, OFFSET_NODE, 1, file);
// printf("pos node after: %lu\n", ftell(file));
            add_node(nodes, node_info);
        }
    }

    if(restore_labels() == -1)
    {
        printf("Error while loading labels!\n");
        return -1;
    }
    
    if(restore_props() == -1)
    {
        printf("Error while loading props!\n");
        return -1;
    }

    return 1;
}   

NodeList* init_store()
{
    //printf("status %d\n", file == NULL);
    nodes = init_node_list();

    if((file=fopen(FILE_NAME, "rb"))==NULL) {
        printf("File not found!\nCreating new\n");
        
        file = fopen(FILE_NAME, "wb");
        fflush(file);
        fclose(file);
    } else {
        int status = restore_db();
        if (status > 0)
            printf("Database has been restored.\n");
        else
            printf("Failed to restored database, occurred error!\n");

        fflush(file);
        fclose(file);
    }

    display_nodes(nodes);

    return nodes;
}  

int restore_labels()
{
    NodeInfo *node = nodes->header;

    while (node != NULL)
    {
        long label_pos = node->node_item->labels_pos;

        while (label_pos != -1)
        {

            NodeLabelInfo *label_info = init_label_info();
            label_info->label = init_label();
            label_info->pos = label_pos;

            fseek(file, label_pos, SEEK_SET);
            // printf("pos label: %lu\n", ftell(file));
            if(fread(label_info->label, OFFSET_LABEL, 1, file) < 1)
                return -1;
// printf("pos label after: %lu\n", ftell(file));
            //printf("restored label: %s %ld\n", label->value, label->next_pos); 
            add_label(node, label_info);
            label_pos = label_info->label->next_pos;
        }
        
        node = node->next;
    }
    
    printf("Labels loaded\n");
    return 1;
}

int restore_props()
{
    NodeInfo *node = nodes->header;

    while (node != NULL)
    {
        long prop_pos = node->node_item->props_pos;

        while (prop_pos != -1)
        {
            NodePropsInfo *prop_info = init_props_info();
            prop_info->prop = init_prop();
            prop_info->pos = prop_pos;
            
            fseek(file, prop_pos, SEEK_SET);
            // printf("pos prop: %lu\n", ftell(file));
                if(fread(prop_info->prop, OFFSET_PROP, 1, file) < 1)
                    return -1;
// printf("pos prop after: %lu\n", ftell(file));
            //printf("restored label: %s %ld\n", label->value, label->next_pos); 
            add_prop(node, prop_info);
            prop_pos = prop_info->prop->next_pos;
        }
        
        node = node->next;
    }

    printf("Props loaded\n");
    return 1;
}

NodeLabelInfo *init_node_labels()
{
    NodeLabelInfo *labels = (NodeLabelInfo*) malloc(sizeof(NodeLabelInfo));
    labels->next = NULL;
    labels->prev = NULL;
    labels->label = NULL;
    return labels;
}

NodePropsInfo *init_node_props()
{
    NodePropsInfo *props = (NodePropsInfo*) malloc(sizeof(NodePropsInfo));
    props->next = NULL;
    props->prev = NULL;
    props->prop = NULL;
    return props;
}

NodeInfo* init_node_info()
{
    NodeInfo *node_info = (NodeInfo*) malloc(sizeof(NodeInfo));
    node_info->node_item = NULL;
    node_info->next = NULL;
    node_info->prev = NULL;
    
    node_info->pos = -1;
    
    node_info->header_label = NULL;
    node_info->tail_label = NULL;
    node_info->labels_len = 0;

    node_info->header_prop = NULL;
    node_info->tail_prop = NULL;
    node_info->props_len = 0;

    return node_info;
}

NodeItem* init_node_item()
{
    NodeItem *node_item = (NodeItem*) malloc(sizeof(NodeItem));

    node_item->labels_pos = -1;
    node_item->props_pos = -1;

    return node_item;
}

NodeLabelInfo* init_label_info()
{
    NodeLabelInfo *node_label_info = (NodeLabelInfo*) malloc(sizeof(NodeLabelInfo));

    node_label_info->label = NULL;
    node_label_info->next = NULL;
    node_label_info->pos = -1;
    node_label_info->prev = NULL;

    return node_label_info;
}

NodeLabel* init_label()
{
    NodeLabel *node_label = (NodeLabel*) malloc(sizeof(NodeLabel));

        
    node_label->next_pos = -1;
    bzero(node_label->value, sizeof(node_label->value));

    return node_label;
}

NodePropsInfo* init_props_info()
{
    NodePropsInfo *node_props_info = (NodePropsInfo*) malloc(sizeof(NodePropsInfo));

    node_props_info->next = NULL;
    node_props_info->prev = NULL;
    node_props_info->prop = NULL;
    node_props_info->pos = -1;

    return node_props_info;
}

NodeProp* init_prop()
{
    NodeProp *node_prop = (NodeProp*) malloc(sizeof(NodeProp));

    node_prop->next_pos = -1;
    bzero(node_prop->key, sizeof(node_prop->key));
    bzero(node_prop->value, sizeof(node_prop->value));

    return node_prop;
}

void display_nodes(NodeList *list)
{
    NodeInfo *node_info = list->header;

    printf("\n========================BEGIN========================\n");

    while (node_info != NULL)
    {
        printf(">>Node:\n");
        NodeLabelInfo *label_info = node_info->header_label;

        while (label_info != NULL)
        {
            printf(">>>>Label: (%s)\n", label_info->label->value);
            label_info = label_info->next;
        }

        NodePropsInfo *props_info = node_info->header_prop;

        while (props_info != NULL)
        {
            printf(">>>>Prop: {%s: %s}\n", props_info->prop->key, props_info->prop->value);
            props_info = props_info->next;
        }
        
        node_info = node_info->next;
        printf("\n");
    }
    
    printf("=========================END=========================\n");
    
}
