#include <stdio.h>
#include <glib-object.h>
#include "../generator/gen-c_glib/generator_types.h"
#include "../store/graph_struct.h"
#include "request_handler.h"
#include "../collections/node_list.h"
#include "../parser/request_parser.h"
/*
    NOTHING = 0,
    CREATE = 1,
    MATCH = 2,
    REMOVE = 3,
    DELETE = 4,
    SET = 5
*/

NodeList *node_list;
FILE *store;
long cursor_position;
NodeInfo *node_info;
NodeInfo *node_update_info;

void hash_table_foreach_action (gpointer key, gpointer value, gpointer user_data);
void list_foreach_action (gpointer value, gpointer user_data);

void build_data(NodeList *nodes, const Request *req,  Response *resp)
{
    node_list = nodes;

    if((store=fopen(FILE_NAME, "wb+"))==NULL) {
        printf("File does not exist.\n");
        return;
    }
    fseek(store, 0, SEEK_END);

    int command = req->command_type;
    printf("%d, command: \n", command);
    node_info = (NodeInfo*) malloc(sizeof(NodeInfo));

    switch (command)
    {
        case CREATE:
            if(create_command(req, resp) == -1)
                resp->text = "Error while occurred create node\n";
            else
                resp->text = "Node successfully created!\n";
        break;
        case MATCH:
            match_command(req, resp);
        break;
        case REMOVE:
        
        break;
        case DELETE:
        
        break;
        case SET:
        
        break;
        
        default:
            break;
    }

    fclose(store);
}

int create_command(const Request *req, Response *resp)
{
    int return_status = -1;
    int error_status = -1;
    int code;

    g_ptr_array_foreach(req->node->labels, list_foreach_action, NULL);
    g_hash_table_foreach(req->node->props, hash_table_foreach_action, NULL);
    
    node_info->node_item = (NodeItem*) malloc(sizeof(NodeItem));
    //fill labels
    NodeLabelInfo *label_info = node_info->tail_label;
    
    if (label_info == NULL)
    {
        node_info->node_item->labels_pos = -1;
    } else {
        code = NODE_LABELS_CODE;
        if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
            return return_status;

        cursor_position = ftell(store);
        label_info->pos = cursor_position;
        label_info->label->next_pos = -1;
        if(fwrite(label_info->label, OFFSET_LABEL, 1, store) < 1)
            return return_status;

        label_info = label_info->prev;
        while (label_info != NULL)
        {
            if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
                return return_status;

            cursor_position = ftell(store);
            label_info->pos = cursor_position;
            label_info->label->next_pos = label_info->next->pos;

            if(fwrite(label_info->label, OFFSET_LABEL, 1, store) < 1)
                return return_status;

            label_info = label_info->prev;
        }
    
        node_info->node_item->labels_pos = cursor_position;
    }

    //printf("after labels fill\n");

    //fill props
    NodePropsInfo *prop_info = node_info->tail_prop;

    if (prop_info == NULL)
    {
        node_info->node_item->props_pos = -1;
    } else {
        code = NODE_PROPS_CODE;
        if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
            return return_status;

        cursor_position = ftell(store);

        prop_info->pos = cursor_position;
        prop_info->prop->next_pos = -1;
        if(fwrite(prop_info->prop, OFFSET_PROP, 1, store) < 1)
            return return_status;

        prop_info = prop_info->prev;

        while (prop_info != NULL)
        {
            if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
                return return_status;

            cursor_position = ftell(store);
            prop_info->pos = cursor_position;
            prop_info->prop->next_pos = prop_info->next->pos;

            if(fwrite(prop_info->prop, OFFSET_PROP, 1, store) < 1)
                return return_status;

            prop_info = prop_info->prev;
        }
        
        node_info->node_item->props_pos = cursor_position;
    }
    //fill node
    code = NODE_CODE;
   
    if( fwrite(&code, OFFSET_TYPE, 1, store) < 1)
        return return_status;

    cursor_position = ftell(store);
    if (node_list->tail != NULL)
    {
         node_list->tail->node_item->next_pos = cursor_position;
    }
    
    node_info->node_item->next_pos = -1;
    if(fwrite(node_info->node_item, OFFSET_NODE, 1, store) < 1)
        return return_status;
    
    cursor_position = ftell(store);
    node_info->pos = cursor_position;
    if (node_list->header != NULL)
    {
        fseek(store, node_list->tail->pos, SEEK_SET);
        if(fwrite(node_list->tail->node_item, OFFSET_NODE, 1, store) < 1)
            return return_status;
    }

    add_node(node_list, node_info);
    
    return 1;
}

int match_command(const Request *req, Response *resp)
{
    req->node;
    return 1;
}

int remove_command(const Request *req, Response *resp)
{
    return 1;
}

int delete_command(const Request *req, Response *resp)
{
    return 1;
}

int set_command(const Request *req, Response *resp)
{
    return 1;
}

void hash_table_foreach_action (gpointer key, gpointer value, gpointer user_data)
{
    //printf("{key: %s, value: %s}\n", (char *) key, (char *) value);
    NodePropsInfo *prop_info = (NodePropsInfo*) malloc(sizeof(NodePropsInfo));
    prop_info->prop = (NodeProp*) malloc(sizeof(NodeProp));
    
    strcpy(prop_info->prop->key, (char*)key);
    strcpy(prop_info->prop->value, (char*)value);
    add_prop(node_info, prop_info);
}

void list_foreach_action (gpointer value, gpointer user_data)
{
    NodeLabelInfo *label_info = (NodeLabelInfo*) malloc(sizeof(NodeLabelInfo));
    label_info->label = (NodeLabel *) malloc(sizeof(NodeLabel));
    label_info->pos = -1;

    strcpy(label_info->label->value, (char*)value);
    add_label(node_info, label_info);
    printf("label: %s %ld\n", node_info->tail_label->label->value, node_info->tail_label->pos);
}