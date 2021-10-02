#ifndef _REQUEST_PARSER_H_
#define _REQUEST_PARSER_H_

#include "../collections/linked_list.h"
#include <cypher-parser.h>
#include "../generator/gen-c_glib/generator_types.h"

typedef enum _CommandType
{
    NOTHING = 0,
    CREATE = 1,
    MATCH = 2,
    REMOVE = 3,
    DELETE = 4,
    SET = 5
} CommandType;

typedef enum _ReturnType {
    NODE,
    RELATION,
    CONNECTED_NODE
} ReturnType;


typedef struct _QueryInfo {
    CommandType command_type;
    LinkedList *labels;
    LinkedList *props;
    bool has_relation;
    char rel_name[20];
    int path_length;
    LinkedList *rel_node_labels;
    LinkedList *rel_node_props;
    LinkedList *changed_labels;
    LinkedList *changed_props;
    ReturnType type_return;
    int limit;
} QueryInfo;

    int parse_request(Request *, char *);
    QueryInfo *get_query_info(cypher_parse_result_t *);

#endif
