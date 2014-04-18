/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_VALUETYPE_H
#define JIVE_VSDG_VALUETYPE_H

#include <jive/vsdg/basetype.h>
#include <jive/arch/registers.h>

typedef struct jive_value_type jive_value_type;
typedef struct jive_value_input jive_value_input;
typedef struct jive_value_output jive_value_output;
typedef struct jive_value_gate jive_value_gate;

extern const jive_type_class JIVE_VALUE_TYPE;
#define JIVE_DECLARE_VALUE_TYPE(name) \
	jive_value_type name##_struct; name##_struct.class_ = &JIVE_VALUE_TYPE; \
	const jive_type * name = &name##_struct

struct jive_value_type : public jive_type {
};

extern const jive_input_class JIVE_VALUE_INPUT;
struct jive_value_input : public jive_input {
};

extern const jive_output_class JIVE_VALUE_OUTPUT;
struct jive_value_output : public jive_output {
};

extern const jive_gate_class JIVE_VALUE_GATE;
struct jive_value_gate {
	jive_gate base;
};

JIVE_EXPORTED_INLINE const jive_value_type *
jive_value_type_cast(const jive_type * type)
{
	if (jive_type_isinstance(type, &JIVE_VALUE_TYPE))
		return (const jive_value_type *) type;
	else
		return NULL;
}

#endif
