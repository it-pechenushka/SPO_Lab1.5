#include <glib-object.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include <thrift/c_glib/thrift.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c_glib/protocol/thrift_protocol_factory.h>
#include <thrift/c_glib/server/thrift_server.h>
#include <thrift/c_glib/server/thrift_simple_server.h>
#include <thrift/c_glib/transport/thrift_buffered_transport_factory.h>
#include <thrift/c_glib/transport/thrift_server_socket.h>
#include <thrift/c_glib/transport/thrift_server_transport.h>

#include "generator/gen-c_glib/test.h"
#include "store/graph_struct.h"
#include "collections/node_list.h"
#include "handler/request_handler.h"

//M_TEST
#define TYPE_M_TEST_HANDLER \
  (m_test_handler_get_type ())

#define M_TEST_HANDLER(obj)                                \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                   \
                               TYPE_M_TEST_HANDLER,        \
                               MTestHandler))
#define M_TEST_HANDLER_CLASS(c)                    \
  (G_TYPE_CHECK_CLASS_CAST ((c),                                \
                            TYPE_M_TEST_HANDLER,   \
                            MTestHandlerClass))
#define IS_M_TEST_HANDLER(obj)                             \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                   \
                               TYPE_M_TEST_HANDLER))
#define IS_TUTORIAL_CALCULATOR_HANDLER_CLASS(c)                 \
  (G_TYPE_CHECK_CLASS_TYPE ((c),                                \
                            TYPE_M_TEST_HANDLER))
#define TUTORIAL_CALCULATOR_HANDLER_GET_CLASS(obj)              \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                            \
                              TYPE_M_TEST_HANDLER, \
                              MTestHandlerClass))

struct _MTestHandler {
  TestHandler parent_instance;

};
typedef struct _MTestHandler MTestHandler;

struct _MTestHandlerClass {
  TestHandlerClass parent_class;
};
typedef struct _MTestHandlerClass MTestHandlerClass;

GType m_test_handler_get_type (void);

G_DEFINE_TYPE (MTestHandler,
        m_test_handler,
        TYPE_TEST_HANDLER)

NodeList *node_list;
//function foreach


///////////////////////////////////////////////////////////
static gboolean
m_test_handler_ping (TestIf  *iface, Response ** response, const Request * request, GError **error)
{
  
  THRIFT_UNUSED_VAR (iface);
  THRIFT_UNUSED_VAR (error);
  build_data(node_list, request, *response);
  printf("%s", (*response)->text);
  
  //g_hash_table_foreach(request->node->props, hash_table_foreach_action, NULL);
  //g_ptr_array_foreach(request->node->labels, list_foreach_action, NULL);
  printf("recive\n");
  return TRUE;
}
////////////////////////////////////////////////////////////
static void
m_test_handler_init (MTestHandler *self)
{

}

static void
m_test_handler_class_init (MTestHandlerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  
  TestHandlerClass *test_handler_class =
    TEST_HANDLER_CLASS (klass);

  test_handler_class->ping = m_test_handler_ping;
}

ThriftServer *server = NULL;
gboolean sigint_received = FALSE;

static void
sigint_handler (int signal_number)
{
  THRIFT_UNUSED_VAR (signal_number);

  /* Take note we were called */
  sigint_received = TRUE;

  /* Shut down the server gracefully */
  if (server != NULL)
    thrift_server_stop (server);
}

int main()
{
    MTestHandler *handler;
    TestProcessor *processor;

    ThriftServerTransport *server_transport;
    ThriftTransportFactory *transport_factory;
    ThriftProtocolFactory *protocol_factory;

    struct sigaction sigint_action;

    GError *error = NULL;
    int exit_status = 0;

#if (!GLIB_CHECK_VERSION (2, 36, 0))
  g_type_init ();
#endif

  handler = g_object_new (TYPE_M_TEST_HANDLER,
                  NULL);

  processor = g_object_new (TYPE_TEST_PROCESSOR,
                  "handler", handler,
                  NULL);

  server_transport = g_object_new (THRIFT_TYPE_SERVER_SOCKET,
                  "port", 9090,
                  NULL);

  transport_factory = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT_FACTORY,
                  NULL);

  protocol_factory = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL_FACTORY,
                  NULL);

  server = g_object_new (THRIFT_TYPE_SIMPLE_SERVER,
                  "processor",                processor,
                  "server_transport",         server_transport,
                  "input_transport_factory",  transport_factory,
                  "output_transport_factory", transport_factory,
                  "input_protocol_factory",   protocol_factory,
                  "output_protocol_factory",  protocol_factory,
                  NULL);

  memset (&sigint_action, 0, sizeof (sigint_action));
  sigint_action.sa_handler = sigint_handler;
  sigint_action.sa_flags = SA_RESETHAND;
  sigaction (SIGINT, &sigint_action, NULL);

  node_list = init_store();

  puts ("Starting the server...");
  thrift_server_serve (server, &error);

  if (!sigint_received) {
    g_message ("thrift_server_serve: %s",
               error != NULL ? error->message : "(null)");
    g_clear_error (&error);
  }

  puts ("done.");

  g_object_unref(server);
  g_object_unref (transport_factory);
  g_object_unref (protocol_factory);
  g_object_unref (server_transport);

  g_object_unref (processor);
  g_object_unref (handler);

  return exit_status;
}


