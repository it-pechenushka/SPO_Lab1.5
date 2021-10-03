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

//DEBUGGING
void print_props (gpointer key, gpointer value, gpointer user_data);
void print_labels (gpointer value, gpointer user_data);
void print_nodes (gpointer value, gpointer user_data);


void build_data(NodeList *nodes, const Request *req,  Response *resp)
{
    node_list = nodes;

    if((store=fopen(FILE_NAME, "r+b"))==NULL) {
        printf("Error with openning file\n");
        return;
    }
    fseek(store, 0, SEEK_END);

    int command = req->command_type;
    node_info = init_node_info();

    switch (command)
    {
        case CREATE:
            if(create_command(req, resp) == -1)
                g_object_set(resp, "text", "Error while occurred create node\n", NULL);
            else
                g_object_set(resp, "text", "Node succesfully created\n", NULL);
        break;
        case MATCH:
            if(match_command(req, resp) == -1)
                g_object_set(resp, "text", "No match nodes found\n", NULL);
            else
                g_object_set(resp, "text", "Match nodes found\n", NULL);
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

    //display_nodes(node_list);
    fflush(store);
    fclose(store);
}

int create_command(const Request *req, Response *resp)
{
    int error_status = -1;
    int code;
    
    g_ptr_array_foreach(req->node->labels, list_foreach_action, NULL);
    g_hash_table_foreach(req->node->props, hash_table_foreach_action, NULL);
    
    node_info->node_item = init_node_item();
    //fill labels
    NodeLabelInfo *label_info = node_info->tail_label;
    
    if (label_info == NULL)
    {
        node_info->node_item->labels_pos = -1;
    } else {
        //printf("label pos code: %lu\n", ftell(store));
        code = NODE_LABELS_CODE;
        if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
            return error_status;
        //printf("label pos code after: %lu\n", ftell(store));
        cursor_position = ftell(store);
        label_info->pos = cursor_position;
        label_info->label->next_pos = -1;
        //printf("label pos: %lu\n", ftell(store));
        if(fwrite(label_info->label, OFFSET_LABEL, 1, store) < 1)
            return error_status;
//printf("label pos after: %lu\n", ftell(store));
        label_info = label_info->prev;
        while (label_info != NULL)
        {
           // printf("label pos code: %lu\n", ftell(store));
            if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
                return error_status;
            //printf("label pos code after: %lu\n", ftell(store));
            cursor_position = ftell(store);
            label_info->pos = cursor_position;
            label_info->label->next_pos = label_info->next->pos;
//printf("label pos: %lu\n", ftell(store));
            if(fwrite(label_info->label, OFFSET_LABEL, 1, store) < 1)
                return error_status;
//printf("label pos after: %lu\n", ftell(store));
            label_info = label_info->prev;
        }
    
        node_info->node_item->labels_pos = cursor_position;
    }

    //fill props
    NodePropsInfo *prop_info = node_info->tail_prop;

    if (prop_info == NULL)
    {
        node_info->node_item->props_pos = -1;
    } else {
        code = NODE_PROPS_CODE;
        //printf("prop pos code: %lu\n", ftell(store));
        if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
            return error_status;
 //printf("prop pos code after: %lu\n", ftell(store));
        cursor_position = ftell(store);

 //printf("pos: %d\n", ftell(store));
        prop_info->pos = cursor_position;
        prop_info->prop->next_pos = -1;
        // printf("prop pos: %lu\n", ftell(store));
        if(fwrite(prop_info->prop, OFFSET_PROP, 1, store) < 1)
            return error_status;
 //printf("prop pos after: %lu\n", ftell(store));
   //     printf("pos after: %d\n", ftell(store));
        prop_info = prop_info->prev;

        while (prop_info != NULL)
        {
             //printf("prop pos code: %lu\n", ftell(store));
            if(fwrite(&code, OFFSET_TYPE, 1, store) < 1)
                return error_status;
 //printf("prop pos code after: %lu\n", ftell(store));
            cursor_position = ftell(store);
            prop_info->pos = cursor_position;
            prop_info->prop->next_pos = prop_info->next->pos;
 //printf("pos: %d\n", ftell(store));
  //printf("prop pos: %lu\n", ftell(store));
            if(fwrite(prop_info->prop, OFFSET_PROP, 1, store) < 1)
                return error_status;
                 //printf("prop pos after: %lu\n", ftell(store));
 //printf("pos after: %d\n", ftell(store));
            prop_info = prop_info->prev;
        }
        
        node_info->node_item->props_pos = cursor_position;
    }

    //fill node
    code = NODE_CODE;
    //printf("node pos code: %lu\n", ftell(store));
    if( fwrite(&code, OFFSET_TYPE, 1, store) < 1)
        return error_status;
//printf("node pos code after: %lu\n", ftell(store));
    node_info->pos = ftell(store);
       // printf("node pos: %lu\n", ftell(store));
    if(fwrite(node_info->node_item, OFFSET_NODE, 1, store) < 1)
        return error_status;
//printf("node pos after: %lu\n", ftell(store));
    add_node(node_list, node_info);
    

    //fill result
    GPtrArray* result_array = g_ptr_array_sized_new(1);
    Node *resp_node = g_object_new(TYPE_NODE, NULL);
    NodeLabelInfo *node_label_to_add = node_list->tail->header_label;

    while (node_label_to_add != NULL)
    {
        g_ptr_array_add(resp_node->labels, node_label_to_add->label->value);
        node_label_to_add = node_label_to_add->next;
    }
    
    NodePropsInfo *node_prop_to_add = node_list->tail->header_prop;

    while (node_prop_to_add != NULL)
    {
        g_hash_table_insert(resp_node->props, node_prop_to_add->prop->key, node_prop_to_add->prop->value);
        node_prop_to_add = node_prop_to_add->next;
    }
    
    g_ptr_array_add(result_array, resp_node);
    g_object_set(resp, "nodes", result_array, NULL);

    return 1;
}

int match_command(const Request *req, Response *resp)
{
    int match_nodes_cnt = -1;
    
    g_ptr_array_foreach(req->node->labels, list_foreach_action, NULL);
    g_hash_table_foreach(req->node->props, hash_table_foreach_action, NULL);

    GPtrArray* result_array = g_ptr_array_sized_new(3);
    NodeInfo *node = node_list->header;
    
    while (node != NULL)
    {

        int match_label_cnt = 0;
        NodeLabelInfo *node_label_in_store = node->header_label;

        //chack label match
        while (node_label_in_store != NULL)
        {
            NodeLabelInfo *label_to_match = node_info->header_label;

            while (label_to_match != NULL)
            {
                if(strcmp(node_label_in_store->label->value, label_to_match->label->value) == 0)
                {
                    match_label_cnt++;
                    break;
                }

                label_to_match = label_to_match->next;
            }
            
            node_label_in_store = node_label_in_store->next;
        }
        
        if (match_label_cnt >= node_info->labels_len)
        {
            int match_prop_cnt = 0;
            NodePropsInfo *node_prop_in_store = node->header_prop;

            while (node_prop_in_store != NULL)
            {
                NodePropsInfo *prop_to_match = node_info->header_prop;

                while (prop_to_match != NULL)
                {
                    if(strcmp(node_prop_in_store->prop->value, prop_to_match->prop->value) == 0 &&
                    strcmp(node_prop_in_store->prop->key, prop_to_match->prop->key) == 0)
                    {
                        match_prop_cnt++;
                        break;
                    }

                    prop_to_match = prop_to_match->next;
                }
                
                node_prop_in_store = node_prop_in_store->next;
            }

            if (match_prop_cnt >= node_info->props_len)
            {
                Node *resp_node = g_object_new(TYPE_NODE, NULL);
                NodeLabelInfo *node_label_to_add = node->header_label;

                while (node_label_to_add != NULL)
                {
                    g_ptr_array_add(resp_node->labels, node_label_to_add->label->value);
                    node_label_to_add = node_label_to_add->next;
                }
                
                NodePropsInfo *node_prop_to_add = node->header_prop;

                while (node_prop_to_add != NULL)
                {
                    g_hash_table_insert(resp_node->props, node_prop_to_add->prop->key, node_prop_to_add->prop->value);
                    node_prop_to_add = node_prop_to_add->next;
                }

                match_nodes_cnt++;
                g_ptr_array_add(result_array, resp_node);
            }
            
        }

        node = node->next;
    }
    
    g_object_set(resp, "nodes", result_array, NULL);
   // g_ptr_array_foreach(resp->nodes, print_nodes, NULL);
    //printf("rofl\n");
    return match_nodes_cnt;
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
    NodePropsInfo *prop_info = init_props_info();
    prop_info->prop = init_prop();
    
    strcpy(prop_info->prop->key, (char*)key);
    strcpy(prop_info->prop->value, (char*)value);
    add_prop(node_info, prop_info);
}

void list_foreach_action (gpointer value, gpointer user_data)
{
    NodeLabelInfo *label_info = init_label_info();
    label_info->label = init_label();
    label_info->pos = -1;

    strcpy(label_info->label->value, (char*)value);
    add_label(node_info, label_info);
    //printf("label: %s %ld\n", node_info->tail_label->label->value, node_info->tail_label->pos);
}



//DEGUBBING
void print_props (gpointer key, gpointer value, gpointer user_data)
{
    printf("{%s: %s}\n", (char *) key, (char *) value);
}

void print_labels (gpointer value, gpointer user_data)
{
    printf("(%s)\n", (char *) value);
}

void print_nodes (gpointer value, gpointer user_data)
{
    Node *node = (Node*) value;

    printf(">>Labels:\n");
     g_ptr_array_foreach(node->labels, print_labels, NULL);
     printf(">>Props:\n");
    g_hash_table_foreach(node->props, print_props, NULL);
}
