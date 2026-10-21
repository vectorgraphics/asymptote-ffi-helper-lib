
// Driver for libstruct. Note the bare `import struct_recfile;` (a plain Asy
// module, not the quoted+aliased .so import) -- it defines the struct the
// plugin returns.
import struct_recfile;
import libstruct;
size(4cm);
StructReturnData d = libstruct.createRandomData();
if (d.x != 100 || d.z != "hello world!" || abs(d.y - 3.14159) > 1e-6)
    abort("struct example: unexpected field values: x=" + string(d.x)
          + " y=" + string(d.y) + " z=" + d.z);
label("x=" + string(d.x) + "  y=" + string(d.y) + "  z=" + d.z);

