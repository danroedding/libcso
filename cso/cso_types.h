#ifndef __CSO_TYPES_H
#define __CSO_TYPES_H


typedef enum {
  CSO_NONE		= 0,
  CSO_STRING		= 1,
  CSO_INT		= 2,
  CSO_OBJECT		= 3,
  CSO_BINARY		= 4
} cso_fieldtype;

/* ----- */

struct container; /* forward declaration */

typedef struct {
  int			arrsize_logical,arrsize_physical;
  int			*arrdata;
  int			arrdata_allocated;
  int			inline_val;
} cso_intval;

typedef struct {
  int			arrsize_logical,arrsize_physical;
  char			**arrdata;
  int			arrdata_allocated;
  char			*inline_val;
} cso_strval;

typedef struct {
  int			arrsize_logical,arrsize_physical;
  struct container	**arrdata;
  int			arrdata_allocated;
  struct container	*inline_val;
} cso_objval;

typedef struct {
  char			*data;
  int			data_allocated,data_len;
} cso_binval;

/* ----- */

typedef struct cso_field {
  struct cso_field	*next;

  char			*name;
  cso_fieldtype		type;
  int			arrayflag;
  union {
    cso_intval		intval;
    cso_strval		strval;
    cso_objval		objval;
    cso_binval		binval;
  }			value;
} cso_field;

typedef struct container {
  cso_field		*fields;
  char			*diag_id;
} container;

#endif

