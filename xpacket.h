/*
 * XPacket
 * Copyright (C) 2017-18 Matteo Parolari <mparolari.dev@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file xpacket.h
 * \brief Main header of XPacket.
 * \author Matteo Parolari <mparolari.dev@gmail.com>
 * \copyright GNU Lesser General Public License version 3.
 * \version 0.3
 * \date 02/2018
 * \warning Macro conflicts are possible.
 * \warning If more arguments are used than needed, behavior is undefined.
 * \todo Add an automatic delimiter (like '\0') to the arrays
 * \todo Check number of arguments in overloading macro.
 * \todo Signed type marshalling.
 * \todo Graceful error handling.
 * \todo Enable/disable inline attribute.
 *
 *    XPacket is an utility that generates a C struct and two functions
 *    for serialize/deserialize it into/from a given payload.
 *
 *    When xpacket.h is included, preprocessor uses XPACKET_NAME and
 *    XPACKET_STRUCT macro for generate the structure; if these two mandatory
 *    macro are not defined when the header is included, a compile-time
 *    error is fired.
 *    Instead the two functions are generated only if the macro XPACKET_C is
 *    defined (and the structure is valid).
 *
 *    XPACKET_NAME is simply the name of the structure, while
 *    XPACKET_STRUCT is the list of fields.
 *    Different types of field are supported:
 *    * FIELD(type, name, [dim]): a simple variable,
 *      or an array if "dim" argument is given.
 *    * FIELD_PTR(type, name, [dim]): a pointer to a variable;
 *      or a pointer to an array if "dim" argument is given.
 *
 *    Only unsigned types are currently supported: uint8_t, uint16_t, uint32_t
 *    from stdint.h header, that are supposed to have fixed size (operator
 *    sizeof is used at compile time).
 *
 *    For example:
 *    \code{.c}
 *    #define XPACKET_NAME msg
 *    #define XPACKET_STRUCT \
 *      FIELD(uint16_t, a) \
 *      FIELD(uint8_t, b, 32) \
 *      FIELD_PTR(uint32_t, c)
 *    #include <xpacket.h>
 *    \endcode
 *    generates:
 *    \code{.c}
 *    struct msg {
 *      uint16_t a;
 *      uint8_t b[32];
 *      uint32_t* c;
 *    };
 *    uint16_t serialize_msg(uint8_t*, const struct msg*);
 *    uint16_t deserialize_msg(const uint8_t*, struct msg*);
 *    \endcode
 *
 *    A decent compiler is necessary for optimize (roll/unroll) the loops.
 *    Attributes (such as __attribute__((__packed__))) can be assigned to
 *    the structure by simply adding them before include xpacket.h.
 *
 *    The two functions are defined only if macro XPACKET_C is defined;
 *    in this way struct and functions declaration can be easily separeted
 *    in an header, while the definitions are placed in a C file.
 *
 *    If the XPACKET_OVERLOADING macro is defined, the functions will be
 *    simply called "serialize" and "deserialize"; while this may generate
 *    name conflicts in the C language, in C++ overloading can solve the
 *    possible ambiguity.
 *
 *    The macros can be safely undefined after the header inclusion;
 *    it's a common practice redefine their values for include the xpacket
 *    header again, in order to generate another different structure
 *    and their corresponding functions.
 *
 *    In alternative, the preprocessor can usually run as standalone
 *    (option -E with gcc) and generate the C code only once.
 */

/*---------------------------------------------------------------------------*/
/* check if XPACKET_NAME and XPACKET_STRUCT are defined */
#if !defined(XPACKET_NAME)
#error "XPacket - XPACKET_NAME not defined"
#elif !defined(XPACKET_STRUCT)
#error "XPacket - XPACKET_STRUCT not defined"
#else /* endif is at the end of the file */
/*---------------------------------------------------------------------------*/
/* define overloading for macros (valid until the end of the file) */
/* TODO check/stop if there are more arguments than needed */
#define OVERLOAD_FIELD(_1, _2, _3, name, ...) name
#define FIELD(...) \
  OVERLOAD_FIELD(__VA_ARGS__, FIELD_ARRAY, FIELD_VAR, FIELD_ERROR)(__VA_ARGS__)
#define OVERLOAD_FIELD_PTR(_1, _2, _3, name, ...) name
#define FIELD_PTR(...) \
  OVERLOAD_FIELD_PTR(__VA_ARGS__, \
    FIELD_PTR_ARRAY, FIELD_PTR_VAR, FIELD_ERROR)(__VA_ARGS__)
/*---------------------------------------------------------------------------*/
/* check if fields are well formed */
/* supported types must be set to 1 */
#define XPACKET_TYPE_uint8_t  1
#define XPACKET_TYPE_uint16_t 1
#define XPACKET_TYPE_uint32_t 1
/* auxiliary macro for checking if a given macro argument is null or not */
#define TRUE 1
/* names must be not null, types are converted into the corresponding macro, */
/* and array size are checked (overload functions manage if they are null) */
#define FIELD_ERROR(...) 0 &&
#define FIELD_VAR(type, name) !(TRUE##name) && XPACKET_TYPE_##type &&
#define FIELD_ARRAY(type, name, dim) \
  !(TRUE##name) && XPACKET_TYPE_##type && (dim > 0) &&
#define FIELD_PTR_VAR(type, name) !(TRUE##name) && XPACKET_TYPE_##type &&
#define FIELD_PTR_ARRAY(type, name, dim) \
  !(TRUE##name) && XPACKET_TYPE_##type && (dim > 0) &&
#define FIELD_CUSTOM(type, name, ser, de) \
  !(TRUE##type) && !(TRUE##name) && !(TRUE##ser) &&!(TRUE##de) &&
/* substitution and evaluation (an undefined macro is considered 0) */
#if !(XPACKET_STRUCT 1)
/* define a macro (see later) and report the error */
#define XPACKET_BAD_FORMAT
#error "XPacket - Bad format"
#endif
/* undefine various macros */
#undef FIELD_ERROR
#undef FIELD_VAR
#undef FIELD_ARRAY
#undef FIELD_PTR_VAR
#undef FIELD_PTR_ARRAY
#undef FIELD_CUSTOM
#undef TRUE
#undef XPACKET_TYPE_uint8_t
#undef XPACKET_TYPE_uint16_t
#undef XPACKET_TYPE_uint32_t
/* if BAD_FORMAT is defined, do not proceed; endif is at the end of the file */
#ifndef XPACKET_BAD_FORMAT
/*---------------------------------------------------------------------------*/
/* packet structure definition */
struct XPACKET_NAME {
  #define FIELD_VAR(type, name)             type name;
  #define FIELD_ARRAY(type, name, dim)      type name[dim];
  #define FIELD_PTR_VAR(type, name)         type* name;
  #define FIELD_PTR_ARRAY(type, name, dim)  type* name;
  #define FIELD_CUSTOM(type, name, ser, de) type name;
  XPACKET_STRUCT
  #undef FIELD_VAR
  #undef FIELD_ARRAY
  #undef FIELD_PTR_VAR
  #undef FIELD_PTR_ARRAY
  #undef FIELD_CUSTOM
};
/* function naming */
#ifndef XPACKET_OVERLOADING
#define METHOD(prefix, name) METHOD_AUX(prefix, name)
#define METHOD_AUX(prefix, name) prefix##_##name
#else
#define METHOD(prefix, name) prefix
#endif
/* function declaration */
uint16_t METHOD(serialize, XPACKET_NAME)(uint8_t*, const struct XPACKET_NAME*);
uint16_t METHOD(deserialize, XPACKET_NAME)(const uint8_t*, struct XPACKET_NAME*);
/* function definition enabled only by the apposite macro */
#ifdef XPACKET_C
/*---------------------------------------------------------------------------*/
/**
 * \brief        Serialize the data in the given payload.
 * \param _pl    Payload memory address.
 * \param _data  Pointer to the structure that will be serialized.
 * \return       Number of bytes serialized.
 */
uint16_t METHOD(serialize, XPACKET_NAME)
    (uint8_t* _pl, const struct XPACKET_NAME* _data) {
  uint16_t idx = 0; /* index */
  /* other variables declaration (if needed) */
  #define FIELD_VAR(type, name)             DECL_OFFSET ||
  #define FIELD_ARRAY(type, name, dim)      DECL_OFFSET || DECL_IT ||
  #define FIELD_PTR_VAR(type, name)         DECL_OFFSET ||
  #define FIELD_PTR_ARRAY(type, name, dim)  DECL_OFFSET || DECL_IT ||
  #define FIELD_CUSTOM(type, name, ser, de)
  /* offset variable */
  #define DECL_OFFSET 1
  #if (XPACKET_STRUCT 0)
  int8_t offset = 0; /* offset for bit shifting */
  #endif
  #undef DECL_OFFSET
  /* iterator variable */
  #define DECL_IT 1
  #if (XPACKET_STRUCT 0)
  uint16_t it = 0; /* index for array iteration */
  #endif
  #undef DECL_IT
  /* undefine various macros */
  #undef FIELD_VAR
  #undef FIELD_ARRAY
  #undef FIELD_PTR_VAR
  #undef FIELD_PTR_ARRAY
  #undef FIELD_CUSTOM
  /* FIELD_VAR serialization definition */
  #define FIELD_VAR(type, name) \
    for (offset = (int8_t)sizeof(type) * 8 - 8; offset >= 0; offset -= 8) \
      _pl[idx++] = _data->name >> offset;
  /* "convert" FIELD_ARRAY in a FIELD_VAR macro, iterating over the array */
  #define FIELD_ARRAY(type, name, dim) \
    for (it = 0; it < dim; it++) { FIELD_VAR(type, name[it]) }
  /* FIELD_PTR_VAR serialization definition */
  #define FIELD_PTR_VAR(type, name) \
    for (offset = (int8_t)sizeof(type) * 8 - 8; offset >= 0; offset -= 8) \
      _pl[idx++] = *(_data->name) >> offset;
  /* FIELD_PTR_ARRAY into FIELD_PTR_VAR macro, iterating over the array */
  #define FIELD_PTR_ARRAY(type, name, dim) \
    for (it = 0; it < dim; it++) { FIELD_PTR_VAR(type, name + it) }
  /* for FIELD_CUSTOM call the external function */
  #define FIELD_CUSTOM(type, name, ser, de) ser(_pl + idx, &_data->name, &idx);
  /* substitution */
  XPACKET_STRUCT
  /* undefine temporary macros */
  #undef FIELD_VAR
  #undef FIELD_ARRAY
  #undef FIELD_PTR_VAR
  #undef FIELD_PTR_ARRAY
  #undef FIELD_CUSTOM
  /* return the number of bytes serialized */
  return idx;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief        Deserialize the payload in the structure.
 * \param _pl    Payload memory address.
 * \param _data  Pointer to the structure where values will be saved.
 * \return       Number of bytes deserialized.
 */
uint16_t METHOD(deserialize, XPACKET_NAME)
    (const uint8_t* _pl, struct XPACKET_NAME* _data) {
  uint16_t idx = 0; /* index */
  /* other variables declaration (if needed) */
  #define FIELD_VAR(type, name)             DECL_OFFSET ||
  #define FIELD_ARRAY(type, name, dim)      DECL_OFFSET || DECL_IT ||
  #define FIELD_PTR_VAR(type, name)         DECL_OFFSET ||
  #define FIELD_PTR_ARRAY(type, name, dim)  DECL_OFFSET || DECL_IT ||
  #define FIELD_CUSTOM(type, name, ser, de)
  /* offset variable */
  #define DECL_OFFSET 1
  #if (XPACKET_STRUCT 0)
  int8_t offset = 0; /* offset for bit shifting */
  #endif
  #undef DECL_OFFSET
  /* iterator variable */
  #define DECL_IT 1
  #if (XPACKET_STRUCT 0)
  uint16_t it = 0; /* index for array iteration */
  #endif
  #undef DECL_IT
  /* undefine various macros */
  #undef FIELD_VAR
  #undef FIELD_ARRAY
  #undef FIELD_PTR_VAR
  #undef FIELD_PTR_ARRAY
  #undef FIELD_CUSTOM
  /* FIELD_VAR deserialization definition */
  #define FIELD_VAR(type, name) \
    _data->name = 0; /* set value to zero for next bitwise OR operations */ \
    for (offset = (int8_t)sizeof(type) * 8 - 8; offset >= 0; offset -= 8) \
      _data->name |= (type)_pl[idx++] << offset;
  /* "convert" FIELD_ARRAY in a FIELD_VAR macro, iterating over the array */
  #define FIELD_ARRAY(type, name, dim) \
    for (it = 0; it < dim; it++) { FIELD_VAR(type, name[it]) }
  /* FIELD_PTR_VAR deserialization definition */
  #define FIELD_PTR_VAR(type, name) \
    *(_data->name) = 0; /* set value to zero for next bitwise OR operations */ \
    for (offset = (int8_t)sizeof(type) * 8 - 8; offset >= 0; offset -= 8) \
      *(_data->name) |= (type)_pl[idx++] << offset;
  /* "convert" FIELD_ARRAY in a FIELD_VAR macro, iterating over the array */
  #define FIELD_PTR_ARRAY(type, name, dim) \
    for (it = 0; it < dim; it++) { FIELD_PTR_VAR(type, name + it) }
  /* for FIELD_CUSTOM call the external function */
  #define FIELD_CUSTOM(type, name, ser, de) de(_pl + idx, &_data->name, &idx);
  /* substitution */
  XPACKET_STRUCT
  /* undefine temporary macros */
  #undef FIELD_VAR
  #undef FIELD_ARRAY
  #undef FIELD_PTR_VAR
  #undef FIELD_PTR_ARRAY
  #undef FIELD_CUSTOM
  /* return the number of bytes serialized */
  return idx;
}
/*---------------------------------------------------------------------------*/
/* end of file */
#endif /* XPACKET_C */
#undef METHOD
#undef METHOD_AUX
#endif /* XPACKET_BAD_FORMAT (struct format check) */
/* undefine overloading macros */
#undef FIELD
#undef FIELD_PTR
#undef OVERLOAD_FIELD
#undef OVERLOAD_FIELD_PTR
#endif /* XPACKET_NAME and XPACKET_STRUCT definition check */
