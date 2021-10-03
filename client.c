#include <stdio.h>
#include <glib-object.h>
#include <cypher-parser.h>
#include <malloc.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>

#include "generator/gen-c_glib/test.h"
#include "generator/gen-c_glib/generator_types.h"
#include "parser/request_parser.h"

void input_command(char *, size_t *len);
void print_result();
void print_props (gpointer key, gpointer value, gpointer user_data);
void print_labels (gpointer value, gpointer user_data);
void print_nodes (gpointer value, gpointer user_data);

int main (void)
{
  ThriftSocket *socket;
  ThriftTransport *transport;
  ThriftProtocol *protocol;
  TestIf *client;

    char *log;
    GError *error = NULL;
  int exit_status = 0;

#if (!GLIB_CHECK_VERSION (2, 36, 0))
  g_type_init ();
#endif

  socket = g_object_new (THRIFT_TYPE_SOCKET,
                            "hostname",  "localhost",
                            "port",      9090,
                            NULL);
  transport = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT,
                            "transport", socket,
                            NULL);
  protocol  = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL,
                            "transport", transport,
                            NULL);

  thrift_transport_open (transport, &error);


  /* In the C (GLib) implementation of Thrift, service methods on the
     server are accessed via a generated client class that implements
     the service interface. In this tutorial, we access a Calculator
     service through an instance of CalculatorClient, which implements
     CalculatorIf. */
  client = g_object_new (TYPE_TEST_CLIENT,
                         "input_protocol",  protocol,
                         "output_protocol", protocol,
                          NULL);

  //g_ptr_array_add(node->labels, "test");
  //g_hash_table_insert(node->props, "key1", node_prop);

  char *input;
  size_t size;

  while (1)
  {
    Response *response = g_object_new(TYPE_RESPONSE, NULL);
    Request *request = g_object_new(TYPE_REQUEST, NULL);
    input_command(input, &size);
    
    int status = parse_request(request, input);
    if (status)
    {
      if (test_if_ping (client, &response, request, &error)) 
          print_result(response); 
    } else {
      printf("Bad Request Build");
    }

    g_object_unref(response);
  }


  thrift_transport_close (transport, NULL);

  g_object_unref (client);
  g_object_unref (protocol);
  g_object_unref (transport);
  g_object_unref (socket);

  return exit_status;
}

void input_command(char *input, size_t *len)
{
    printf("[input]: ");
    getline(&input, len, stdin);
}

void print_result(Response *response)
{
    printf("==================BEGIN==================\n");
    printf("Response: %s\n", response->text);
    printf("Result: \n");
    g_ptr_array_foreach(response->nodes, print_nodes, NULL);
    printf("===================END===================\n");
}

void print_nodes(gpointer value, gpointer user_data)
{
    Node *node = (Node*) value;
    printf(">>Node: \n");
    printf(">>>>Labels: \n");
    g_ptr_array_foreach(node->labels, print_labels, NULL);
    printf("\n");
    printf(">>>>Props: \n");
    g_hash_table_foreach(node->props, print_props, NULL);
    printf("\n");
}

void print_props (gpointer key, gpointer value, gpointer user_data)
{
    printf("{%s: %s}\n", (char*) key, (char*) value);
}

void print_labels (gpointer value, gpointer user_data)
{
    printf("(%s) ", (char*) value);
}