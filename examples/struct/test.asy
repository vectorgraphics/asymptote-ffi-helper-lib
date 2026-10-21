import struct_recfile;
import libstruct;
size(4cm);
StructReturnData d = libstruct.createRandomData();
if (d.x != 100 || d.z != "hello world!" || abs(d.y - 1.0/7.0) > 10*realEpsilon)
    abort("struct example: unexpected field values: x=" + string(d.x)
          + " y=" + string(d.y) + " z=" + d.z);

// Struct as a *function argument*: doubleX reads d.x through the AsyFfi::record
// proxy and returns 2*x.
int doubled = libstruct.doubleX(d);
if (doubled != 200)
    abort("struct example: doubleX failed, expected 200, got " + string(doubled));

// Struct argument *and* return: copyStruct reads d via the record proxy, returns
// an independent copy with x bumped by 1, and writes it out through
// AsyFfi::toItem<AsyFfi::record> (the write path). The original d is left
// untouched, proving the returned struct is a separate, writable value.
StructReturnData c = libstruct.copyStruct(d);
if (c.x != d.x + 1)
    abort("struct example: copyStruct x not incremented, got " + string(c.x)
          + ", want " + string(d.x + 1));
if (abs(c.y - d.y) > 10*realEpsilon || c.z != d.z)
    abort("struct example: copyStruct y/z mismatch: y=" + string(c.y) + " z=" + c.z);
if (d.x != 100)
    abort("struct example: original d.x was modified, got " + string(d.x));

label("x=" + string(d.x) + "  y=" + string(d.y) + "  z=" + d.z,(0,10));
label("doubleX=" + string(doubled) + "  copyStruct x=" + string(c.x),(0,0),S);
