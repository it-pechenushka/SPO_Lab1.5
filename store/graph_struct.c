#include <stdio.h>
#include "graph_struct.h"
#include "../collections/node_list.h"

FILE *file;
NodeList *nodes; 
long cursor_position;

NodeList *init_node_list()
{
    NodeList *list = (NodeList*) malloc(sizeof(NodeList));
    list->header = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
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

int restore_db()
{
    //printf("status %d\n", file == NULL);
    int obj_type = -1;

    while (obj_type != NODE_CODE)
    {
        if(feof(file))
        {
            printf("Nodes not found.\n");
            return 2;
        }

        if(fread(&obj_type, OFFSET_TYPE, 1, file) < 1)
        {
             if(feof(file))
            {
                printf("Nodes not found.\n");
                return 2;
            }
            
            int error = ferror(file);
            printf("Error num: %d\n", error);

        }

        switch (obj_type)
        {
            case NODE_LABELS_CODE:
                fseek(file, OFFSET_LABEL,SEEK_CUR);
                break;
            case NODE_PROPS_CODE:
                fseek(file, OFFSET_PROP,SEEK_CUR);
            default:
                break;
        }

        cursor_position = ftell(file);
    }

    NodeItem *node_item = (NodeItem*) malloc(sizeof(NodeItem));
    
    while (fread(node_item, sizeof(NodeItem), 1, file) > 0)
    {
        NodeInfo *node_info = init_node();   
        node_info->pos = cursor_position;
        node_info->node_item = node_item;
        add_node(nodes, node_info);

        if (node_item->next_pos == -1)
            break;

        fseek(file, node_item->next_pos, SEEK_SET);
        cursor_position = ftell(file);
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
        printf("Cannot open file!\n");

    } else {
        int status = restore_db();
        if (status > 0)
            printf("Database has been restored.\n");
        else
            printf("Failed to restored database, occurred error!\n");

        fclose(file);
    }

    return nodes;
}  

int restore_labels()
{
    NodeInfo *node = nodes->header;

    do
    {
        long label_pos = node->node_item->labels_pos;
        if (label_pos != -1)
        {
            NodeLabelInfo *label_info = (NodeLabelInfo*) malloc(sizeof(NodeLabelInfo));

            while (label_pos != -1)
            {
                NodeLabel *label = (NodeLabel*) malloc(sizeof(NodeLabel));
                fseek(file, label_pos, SEEK_SET);
                if(fread(label, sizeof(NodeLabel), 1, file) < 1)
                    return -1;

                label_info->label = label;
                //printf("restored label: %s %ld\n", label->value, label->next_pos); 
                add_label(node, label_info);
                label_pos = label->next_pos;
            }
        }
        
        node = node->next;
    } while (node != NULL);
    
    printf("Labels loaded\n");
    return 1;
}

int restore_props()
{
    NodeInfo *node = nodes->header;

    do
    {
        long prop_pos = node->node_item->props_pos;
        if (prop_pos != -1)
        {
            NodePropsInfo *prop_info = (NodePropsInfo*) malloc(sizeof(NodePropsInfo));

            while (prop_pos != -1)
            {
                NodeProp *prop = (NodeProp*) malloc(sizeof(NodeProp));
                fseek(file, prop_pos, SEEK_SET);
                if(fread(prop, sizeof(NodeProp), 1, file))
                    return -1;

                prop_info->prop = prop; 
                //printf("restored prop: {%s, %s} %ld\n", prop->key, prop->value, prop->next_pos); 
                add_prop(node, prop_info);
                prop_pos = prop->next_pos;
            }
        }

        node = node->next;
        
    } while (node != NULL);

    printf("Props loaded\n");
    return 1;
}

NodeInfo* init_node()
{
    NodeInfo *node_info = (NodeInfo*) malloc(sizeof(NodeInfo));
    node_info->node_item = NULL;
    node_info->next = NULL;
    node_info->prev = NULL;

    node_info->header_label = NULL;
    node_info->tail_label = NULL;
    node_info->labels_len = 0;

    node_info->header_prop = NULL;
    node_info->tail_prop = NULL;
    node_info->props_len = 0;

    return node_info;
}