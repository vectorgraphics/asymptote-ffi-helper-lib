
# Examples

Self-contained Asymptote FFI plugin examples. Each folder is a standalone
CMake project that builds a loadable plugin (`.so`) right next to a
ready-to-run `test.asy`.

| example   | what it shows |
|-----------|---------------|
| `minimal` | "hello world": one function (`doubleIt`) registered with the type-inferred `AsyFfi::registerFunction` (no hand-written metadata, no manual unpacking). |
| `picture` | draw a path into a picture with a red pen, using the injected `AsyFfi::Context`. |
| `clip`    | clip a random cloud to a path. |
| `thread`  | create random pens/points in parallel threads. |
| `struct`  | return a user-defined Asymptote struct (escape-hatch overload for a runtime record return type). |

## Build

From inside each example folder:

```sh
cmake -S . -B build -DASYFFI_HEADER_DOWNLOAD_URL=/path/to/asymptote/asyffi.h
cmake --build build
```

This writes the plugin (`lib<name>.so`, or `lib<name>.dll` on Windows) into
the example folder itself, next to `test.asy`.

`-DASYFFI_HEADER_DOWNLOAD_URL` is optional; the default downloads
`asyffi.h` from the Asymptote repository's GitHub Pages. Override it to use a
local copy instead.

## Run

```sh
asy -V test
```

Each `test.asy` imports the plugin, calls the functions, and `abort()`s on a
wrong value -- so exit 0 means the plugin is correct.

### Special case: `struct`

`struct_recfile.asy` defines the Asymptote struct the plugin returns and is
loaded by the plugin at import time, so it must sit next to the `.so` (it
already does in the folder).

## Use a plugin anywhere

`test.asy` and the built `.so` are self-contained. Copy both into any folder
and run `asy -V test` there. Or call the function from any `.asy` file:

```asy
import libminimal;
int x = libminimal.doubleIt(21);     // 42
```

