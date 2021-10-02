#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_


void build_data(NodeList *node_list, const Request *req, Response *resp);
int create_command(const Request *req, Response *resp);
int match_command(const Request *req, Response *resp);
int remove_command(const Request *req, Response *resp);
int delete_command(const Request *req, Response *resp);
int set_command(const Request *req, Response *resp);

#endif
