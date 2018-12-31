@0xc4cd6e2837b18273;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("blade::protocol");

struct QueryMessage {
    id @0 :UInt64;
    text @1 :Text;
}

