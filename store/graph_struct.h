#ifndef _GRAPH_STRUCT_H_
#define _GRAPH_STRUCT_H_

#include "../collections/linked_list.h"
#include <cypher-parser.h>
#include "../generator/gen-c_glib/generator_types.h"

#define FILE_NAME "../disk.txt"

#define NODE_PROPS_CODE 1
#define NODE_RELATION_CODE 2
#define NODE_LABELS_CODE 3
#define NODE_CODE 4

#define OFFSET_TYPE sizeof(int)


//#define SIZE_LIST_LABELS_NODE 4
#define SIZE_LABEL_STRING 20

//sizeof(_NodeLabel) = 28;
typedef struct _NodeLabel
{
    long next_pos;
    char value[SIZE_LABEL_STRING];
} NodeLabel;


typedef struct _NodeLabelInfo {
    long pos;
    NodeLabel *label;
    struct _NodeLabelInfo *next;
    struct _NodeLabelInfo *prev;
} NodeLabelInfo;


#define PROPERTY_FIELD_SIZE 20

//sizeof(_NodeProp) = 48;
typedef struct _NodeProp
{
    long next_pos;
    char key[PROPERTY_FIELD_SIZE];
    char value[PROPERTY_FIELD_SIZE];
} NodeProp;


typedef struct _NodePropsInfo {
    long pos;
    struct _NodePropsInfo *next;
    struct _NodePropsInfo *prev;
    NodeProp *prop;
} NodePropsInfo;

//_NodeItem = 16
typedef struct _NodeItem {
    long labels_pos;
    long props_pos;
} NodeItem;

typedef struct _NodeInfo {
    long pos;
    NodeItem *node_item;
    struct _NodeInfo *next;
    struct _NodeInfo *prev;
    NodeLabelInfo *header_label;
    NodeLabelInfo *tail_label;
    int labels_len;
    NodePropsInfo *header_prop;
    NodePropsInfo *tail_prop;
    int props_len;
    //long id_relationship;
} NodeInfo;

typedef struct _NodeList{
    NodeInfo *header;
    NodeInfo *tail;
    uint size;
} NodeList;
//#define RELATION_NAME_SIZE 24

// Структура для связей.
// id_next_node_sibling  - id_node следующего блока
// sizeof(Node_Relation) = 56
// typedef struct _NodeRelation {
//     long id;
//     char name[RELATION_NAME_SIZE];
//     long id_node_from;
//     long id_node_to;
//     long id_next_node_sibling;
// } _NodeRelation;
#define OFFSET_LABEL sizeof(NodeLabel)
#define OFFSET_PROP sizeof(NodeProp)
#define OFFSET_NODE sizeof(NodeItem)

NodeList* init_store();
NodeInfo* init_node_info();
NodeItem* init_node_item();
NodeLabelInfo* init_label_info();
NodeLabel* init_label();
NodePropsInfo* init_props_info();
NodeProp* init_prop();

void display_nodes(NodeList *list);

int restore_db();
int restore_labels();
int restore_props();
#endif
