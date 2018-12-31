@0xac7345a6dc9feafb;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("blade::protocol");

struct ErrorMessage {
    code @0 :UInt16;
    message @1 :Text;
}

