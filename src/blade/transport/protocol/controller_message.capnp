@0xc4bde6cc21ed7497;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("blade::protocol");

using import "ping_message.capnp".PingMessage;
using import "query_message.capnp".QueryMessage;
using import "error_message.capnp".ErrorMessage;

struct ControllerMessage {
    host @0 :Text;
    controller @1 :Text;

    message :union {
        ping @2 :PingMessage;
        query @3 :QueryMessage;
        error @4 :ErrorMessage;
    }
}

