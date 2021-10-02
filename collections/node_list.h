#ifndef _NODE_LIST_H_
#define _NODE_LIST_H_

#include "../collections/linked_list.h"
#include <cypher-parser.h>
#include "../generator/gen-c_glib/generator_types.h"
#include "../store/graph_struct.h"

#define FREE_NODE_PROPS_CODE 5
#define FREE_NODE_RELATION_CODE 6
#define FREE_NODE_LABELS_CODE 7
#define FREE_NODE_CODE 8

int add_node(NodeList *list, NodeInfo *node);
int add_label(NodeInfo *node_info, NodeLabelInfo *label);
int add_prop(NodeInfo *node_info, NodePropsInfo *prop);

#endif
