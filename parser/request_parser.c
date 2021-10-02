#include <cypher-parser.h>
#include <glib-object.h>
#include "request_parser.h"
#include "../generator/gen-c_glib/generator_types.h"

/*
Command code

RETURN 53
SET 38
REMOVE 45
DELETE 44
MATCH 28
CREATE 37

SET label 43
SET prop 40
REMOVE label 47
REMOVE prop 48
*/

QueryInfo *init_query_info() 
{
    QueryInfo *info = malloc(sizeof(QueryInfo));
    info->labels = NULL;
    info->props = NULL;
    info->rel_node_labels = init_list();
    info->rel_node_props = init_list();
    info->changed_labels = NULL;
    info->changed_props = NULL;
    info->has_relation = false;
    info->type_return = NODE;
    return info;
}

void set_changed_labels_and_props(const cypher_astnode_t *clause, QueryInfo *info) 
{

    if (cypher_astnode_type(clause) == CYPHER_AST_SET) {
        const cypher_astnode_t *item = cypher_ast_set_get_item(clause, 0);
        if (cypher_astnode_type(item) == CYPHER_AST_SET_LABELS) {
            info->changed_labels = init_list();
            //printf("%u\n", cypher_ast_set_labels_nlabels(item));
            for (int i = 0; i < cypher_ast_set_labels_nlabels(item); ++i) {
                const cypher_astnode_t *set_label = cypher_ast_set_labels_get_label(item, i);
                add_last(info->changed_labels, (void *) cypher_ast_label_get_name(set_label));
                //printf("Label: %s\n", (char *) info->labels->first->value);
            }
        }

        if (cypher_astnode_type(item) == CYPHER_AST_SET_PROPERTY) {
            info->changed_props = init_list();
            const cypher_astnode_t *set_prop = cypher_ast_set_property_get_property(item);
            const cypher_astnode_t *prop_name = cypher_ast_property_operator_get_prop_name(set_prop);
            const cypher_astnode_t *expression = cypher_ast_set_property_get_expression(item);

            Property *prop = malloc(sizeof(Property));
            if (
                    strlen(cypher_ast_prop_name_get_value(prop_name)) >
                    20 ||
                    cypher_astnode_type(expression) != CYPHER_AST_STRING ||
                    strlen(cypher_ast_string_get_value(expression)) > 20
                    ) {
                printf("Invalid Property. Only string properties allowed.\n");
            } else {
                strcpy(prop->key, cypher_ast_prop_name_get_value(prop_name));
                strcpy(prop->value, cypher_ast_string_get_value(expression));
                add_last(info->changed_props, prop);
                // printf("Property key: %s\n", prop->key);
                // printf("Property value: %s\n", prop->value);

            }
        }
    }
    
    if (cypher_astnode_type(clause) == CYPHER_AST_REMOVE) {
        const cypher_astnode_t *item = cypher_ast_remove_get_item(clause, 0);
        if (cypher_astnode_type(item) == CYPHER_AST_REMOVE_LABELS) {
            info->changed_labels = init_list();
            for (int i = 0; i < cypher_ast_remove_labels_nlabels(item); ++i) {
                const cypher_astnode_t *set_label = cypher_ast_remove_labels_get_label(item, i);
                add_last(info->changed_labels, (void *) cypher_ast_label_get_name(set_label));
            }
        }
        if (cypher_astnode_type(item) == CYPHER_AST_REMOVE_PROPERTY) {
             info->changed_props = init_list();
            const cypher_astnode_t *remove_prop = cypher_ast_remove_property_get_property(item);
            const cypher_astnode_t *prop_name = cypher_ast_property_operator_get_prop_name(remove_prop);
            Property *prop = malloc(sizeof(Property));
            if (
                    strlen(cypher_ast_prop_name_get_value(prop_name)) >
                    20) {
                printf("Invalid Property. Max key size = %d\n",
                       20);
            } else {
                strcpy(prop->key, cypher_ast_prop_name_get_value(prop_name));
                strcpy(prop->value, "");
                add_last(info->changed_props, prop);
            }
        }
    }
}

void set_labels_and_props(const cypher_astnode_t *node, QueryInfo *info) //LinkidList *label, 
{
    unsigned int labels_count = cypher_ast_node_pattern_nlabels(node);
    //printf("labels count: %u\n", labels_count);
    printf("label count %ud\n", labels_count);
    if (labels_count > 0){
        info->labels = init_list();
        //printf("null status %d\n", info->labels == NULL);
    }
    
    for (int i = 0; i < labels_count; ++i) {
        const cypher_astnode_t *label = cypher_ast_node_pattern_get_label(node, i);
        
        //printf("lable: %s\n", cypher_ast_label_get_name(label));

        add_last(info->labels, (void *) cypher_ast_label_get_name(label));
    }

    const cypher_astnode_t *props = cypher_ast_node_pattern_get_properties(node);
    if (props != NULL) {
        
        info->props = init_list();
        for (int i = 0; i < cypher_ast_map_nentries(props); ++i) {
            const cypher_astnode_t *key = cypher_ast_map_get_key(props, i);
            const cypher_astnode_t *value = cypher_ast_map_get_value(props, i);
            Property *prop = malloc(sizeof(Property));
            memset(prop, 0, 20 + 20);
            if (
                    strlen(cypher_ast_prop_name_get_value(key)) > 20 ||
                    cypher_astnode_type(value) != CYPHER_AST_STRING ||
                    strlen(cypher_ast_string_get_value(value)) > 20
                    ) {
                printf("Invalid Property. Max key size = %d. Max value size = %d. Only string properties allowed.\n",
                       20, 20);
            } else {
                strcpy(prop->key, cypher_ast_prop_name_get_value(key));
                strcpy(prop->value, cypher_ast_string_get_value(value));
                add_last(info->props, prop);
            }
        }
    }
}

QueryInfo *get_query_info(cypher_parse_result_t *result) 
{
    QueryInfo *query_info = init_query_info();
    const cypher_astnode_t *ast = cypher_parse_result_get_directive(result, 0);
    const cypher_astnode_t *query = cypher_ast_statement_get_body(ast);
    const cypher_astnode_t *clause = cypher_ast_query_get_clause(query, 0); //First command
    // const cypher_astnode_t *clause_return = cypher_ast_query_get_clause(query, 1); // TODO: return type ?
    // const cypher_astnode_t *limit = cypher_ast_return_get_limit(clause_return);

    // if (limit != NULL) {
    //     query_info->limit = (int) strtol(cypher_ast_integer_get_valuestr(limit), NULL, 10);
    // } else {
    //     query_info->limit = -1;
    // }

    cypher_astnode_type_t command = cypher_astnode_type(clause);
    
    //printf("command1 %u\n", (uint8_t)command);

    if (command == CYPHER_AST_MATCH) {
        const cypher_astnode_t *clause1 = cypher_ast_query_get_clause(query, 1);
        if (clause1 == NULL) {
            printf("Unknown command\n");
            return NULL;
        }
        command = cypher_astnode_type(clause1);

        if (command == CYPHER_AST_SET) {
            query_info->command_type = SET;
        }
        if (command == CYPHER_AST_REMOVE) {
            query_info->command_type = REMOVE;
        }
        if (command == CYPHER_AST_DELETE) {
            query_info->command_type = DELETE;
        }
 //printf("command2 %u\n", (uint8_t)command); 
         if (command == CYPHER_AST_RETURN) {
            query_info->command_type = MATCH;
        } else{
            set_changed_labels_and_props(clause1, query_info);
        }

       // printf("command2 %u\n", (uint8_t)command); 
    }

    if (command == CYPHER_AST_CREATE) {
        query_info->command_type = CREATE;
    }
    cypher_astnode_t *pattern;
    if (cypher_astnode_type(clause) == CYPHER_AST_MATCH) {
        pattern = cypher_ast_match_get_pattern(clause);
    } else {
        pattern = cypher_ast_create_get_pattern(clause);
    }
    const cypher_astnode_t *path = cypher_ast_pattern_get_path(pattern, 0);
    const cypher_astnode_t *node = cypher_ast_pattern_path_get_element(path, 0);
    //printf("berofe %d\n", query_info->labels == NULL);
    set_labels_and_props(node, query_info); // query_info->labels
    //printf("after %d\n", query_info->labels == NULL);
    // if (cypher_ast_pattern_path_nelements(path) == 3) {
    //     query_info->has_relation = true;
    //     const cypher_astnode_t *relation = cypher_ast_pattern_path_get_element(path, 1);
    //     const cypher_astnode_t *map = cypher_ast_rel_pattern_get_varlength(relation);
    //     if (map != NULL) {
    //         char *res = cypher_ast_integer_get_valuestr(cypher_ast_range_get_start(map));
    //         query_info->path_length = (int) strtol(res, NULL, 10);
    //     } else {
    //         query_info->path_length = 0;
    //     }

    //     const cypher_astnode_t *identifier_relation = cypher_ast_rel_pattern_get_identifier(relation);
    //     if (identifier_relation != NULL) query_info->type_return = RELATION;
    //     const cypher_astnode_t *rel_type = cypher_ast_rel_pattern_get_reltype(relation, 0);
    //     if (cypher_ast_reltype_get_name(rel_type) != NULL) {
    //         memcpy(query_info->rel_name, cypher_ast_reltype_get_name(rel_type), 20);
    //     }
    //     const cypher_astnode_t *rel_node = cypher_ast_pattern_path_get_element(path, 2);
    //     const cypher_astnode_t *identifier_connected_node = cypher_ast_node_pattern_get_identifier(rel_node);
    //     if (identifier_connected_node != NULL) query_info->type_return = CONNECTED_NODE;
    //     set_labels_and_props(rel_node, query_info->rel_node_labels, query_info->rel_node_props);
    // }
    return query_info;
}

void bind_info(Request *request, QueryInfo *info)
{
    printf("Input Info:\n");
    
    if (info->labels != NULL)
    {
        Node_L *node = info->labels->first;
        printf(">>>Target Label info:\n");

        do
        {
            
            char *label = (char *) node->value;
            g_ptr_array_add(request->node->labels, label);
            printf(">>>>>> (%s)\n", label);

            node = node->next;
        } while (node != NULL);
    }

    if (info->props != NULL)
    {
        Node_L *node = info->props->first;
        printf(">>>Target Props info:\n");

        do
        {
            Property *prop = (Property*) malloc(sizeof(Property));
            prop = node->value;
            g_hash_table_insert(request->node->props, prop->key, prop->value);
            printf(">>>>>> {key: %s, value: %s}\n", prop->key, prop->value);
            
            node = node->next;
        } while (node != NULL);
    }

    if (info->changed_labels != NULL)
    {
        Node_L *node = info->changed_labels->first;
        printf(">>>Labels to modify:\n");

        do
        {
            char * label = (char *) node->value;
            g_ptr_array_add(request->node_updates->labels, label);
            printf(">>>>>> (%s)\n", label);

            node = node->next;
        } while (node != NULL);
    }

    if (info->changed_props != NULL)
    {
        Node_L *node = info->changed_props->first;
        printf(">>>Props to modify:\n");

        do
        {
            Property *prop = (Property*) malloc(sizeof(Property));
            prop = node->value;
            g_hash_table_insert(request->node_updates->props, prop->key, prop->value);
            printf(">>>>>> {key: %s, value: %s}\n", prop->key, prop->value);

            node = node->next;
        } while (node != NULL);
    }

    request->command_type = info->command_type;
}

int parse_request(Request *request, char *input_command)
{
    printf("You entered: %s\n", input_command);
    cypher_parse_result_t *result = cypher_parse(input_command, NULL, NULL, CYPHER_PARSE_ONLY_STATEMENTS);

    if (result == NULL)
    {
        perror("cypher_parse");
        return EXIT_FAILURE;
    }
    
    
    printf("Parsed %d AST nodes\n", cypher_parse_result_nnodes(result));
    printf("Read %d statements\n", cypher_parse_result_ndirectives(result));
    printf("Encountered %d errors\n", cypher_parse_result_nerrors(result));

    if (cypher_parse_result_nerrors(result) > 0) {
        cypher_parse_result_free(result);
        return -EXIT_FAILURE;
    }
    
    QueryInfo *info = get_query_info(result);

    if (info == NULL)
        return -1;

    bind_info(request, info);
    
    cypher_parse_result_free(result);
    return 1;
}