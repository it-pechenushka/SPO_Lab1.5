#include <stdio.h>
#include <glib-object.h>
#include <cypher-parser.h>
#include <malloc.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>

#include "generator/gen-c_glib/test.h"
#include "generator/gen-c_glib/generator_types.h"

void input_command(char *, size_t *len);
void input_command(char *, size_t *len);

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

  Node *node = g_object_new (TYPE_NODE, NULL);
  Response *response = g_object_new(TYPE_RESPONSE, NULL);
  
  //g_ptr_array_add(node->labels, "test");
  //g_hash_table_insert(node->props, "key1", node_prop);

  char *input;
  size_t size;

  while (1)
  {
    // input_command(input, size);

    // parse_comma
    // if (test_if_ping (client, node, response, &error)) {
    //   printf("Success");
    // }    
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
    printf("Input command:\n");
    getline(&input, len, stdin);
}