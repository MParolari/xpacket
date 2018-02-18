# XPacket
XPacket is an utility that generates a C struct and two functions
for serialize/deserialize it into/from a given payload.

Version 0.3, 02/2018.

Distributed under the GNU Lesser General Public License version 3.

### Bugs, warnings and todos
Warnings:
* Macro conflicts are possible.
* If more arguments are used than needed, behavior is undefined.

Todos and possible improvements (in random order):
* Add an automatic delimiter (like '\0') to the arrays
* Check number of arguments in overloading macro.
* Signed type marshalling.
* Graceful error handling.
* Enable/disable inline attribute.

### Description

XPacket is an utility that generates a C struct and two functions
for serialize/deserialize it into/from a given payload.

When xpacket.h is included, preprocessor uses XPACKET\_NAME and
XPACKET\_STRUCT macro for generate the structure; if these two mandatory
macro are not defined when the header is included, a compile-time
error is fired.
Instead the two functions are generated only if the macro XPACKET\_C is
defined (and the structure is valid).

XPACKET\_NAME is simply the name of the structure, while
XPACKET\_STRUCT is the list of fields.
Different types of field are supported:
* FIELD(type, name, [dim]): a simple variable,
or an array if "dim" argument is given.
* FIELD\_PTR(type, name, [dim]): a pointer to a variable;
or a pointer to an array if "dim" argument is given.

Only unsigned types are currently supported: uint8\_t, uint16\_t, uint32\_t
from stdint.h header, that are supposed to have fixed size (operator
sizeof is used at compile time).

For example:
```c
#define XPACKET_NAME msg
#define XPACKET_STRUCT \
  FIELD(uint16_t, a) \
  FIELD(uint8_t, b, 32) \
  FIELD_PTR(uint32_t, c)
#include <xpacket.h>
```
generates:
```c
struct msg {
  uint16_t a;
  uint8_t b[32];
  uint32_t* c;
};
uint16_t serialize_msg(uint8_t*, const struct msg*);
uint16_t deserialize_msg(const uint8_t*, struct msg*);
```

A decent compiler is necessary for optimize (roll/unroll) the loops.
Attributes (such as \_\_attribute\_\_((\_\_packed\_\_))) can be assigned to
the structure by simply adding them before include xpacket.h.

The two functions are defined only if macro XPACKET\_C is defined;
in this way struct and functions declaration can be easily separeted
in an header, while the definitions are placed in a C file.

If the XPACKET\_OVERLOADING macro is defined, the functions will be
simply called "serialize" and "deserialize"; while this may generate
name conflicts in the C language, in C++ overloading can solve the
possible ambiguity.

The macros can be safely undefined after the header inclusion;
it's a common practice redefine their values for include the xpacket
header again, in order to generate another different structure
and their corresponding functions.

In alternative, the preprocessor can usually run as standalone
(option -E with gcc) and generate the C code only once.
