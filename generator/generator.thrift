/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  i8 (byte)   Signed 8-bit integer
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */

struct Node { 
  1: map<string, string> props,
  2: list<string> labels
}

struct Response {
  1: string text,
  2: list<Node> nodes;
}

struct Request {
  1: i32 command_type,
  2: Node node,
  3: Node node_updates
}

service Test{
  Response ping(1: Request request)
}