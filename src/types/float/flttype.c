/*
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/flttype-private.h>
#include <jive/vsdg/valuetype-private.h>
#include <jive/vsdg/basetype-private.h>

#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>

#include <string.h>

const jive_type_class JIVE_FLOAT_TYPE = {
	parent : &JIVE_VALUE_TYPE,
	name : "flt",
	fini : jive_value_type_fini_, /* inherit */
	get_label : jive_type_get_label_, /* inherit */
	create_input : jive_float_type_create_input_, /* override */
	.create_output  = jive_float_type_create_output_, /* override */
	create_gate : jive_float_type_create_gate_, /* override */
	equals : jive_type_equals_, /* override */
	copy : jive_float_type_copy_, /* override */ 
};

const jive_output_class JIVE_FLOAT_OUTPUT = {
	parent : &JIVE_VALUE_OUTPUT,
	fini : jive_output_fini_, /* inherit */
	get_label : jive_output_get_label_, /* inherit */
	get_type : jive_float_output_get_type_, /* override */
};

const jive_gate_class JIVE_FLOAT_GATE = {
	parent : &JIVE_VALUE_GATE,
	fini : jive_gate_fini_, /* inherit */
	get_label : jive_gate_get_label_, /* inherit */
	get_type : jive_float_gate_get_type_, /* override */
};

/* float_type inheritable members */

jive_float_type::~jive_float_type() noexcept {}

jive_float_type::jive_float_type() noexcept
	: jive_value_type(&JIVE_FLOAT_TYPE)
{}

jive_input *
jive_float_type_create_input_(const jive_type * self_, struct jive_node * node, size_t index,
	jive_output * initial_operand)
{
	jive_float_input * input = new jive_float_input(node, index, initial_operand);
	return input;
}

jive_output *
jive_float_type_create_output_(const jive_type * self_, struct jive_node * node, size_t index)
{
	return new jive_float_output(node, index);
}

jive_gate *
jive_float_type_create_gate_(const jive_type * self_, struct jive_graph * graph, const char * name)
{
	return new jive_float_gate(graph, name);
}

jive_type *
jive_float_type_copy_(const jive_type * self_)
{
	const jive_float_type * self = (const jive_float_type *) self_;
	jive_float_type * type = new jive_float_type;
	*type = *self;
	return type;
}

static inline void
jive_float_type_init_(jive_float_type * self)
{
	self->class_ = &JIVE_FLOAT_TYPE;
}

/* float_input inheritable members */

jive_float_input::~jive_float_input() noexcept {}

jive_float_input::jive_float_input(struct jive_node * node, size_t index,
	jive_output * origin)
	: jive_value_input(node, index, origin)
{}

/* float_output inheritable members */

jive_float_output::~jive_float_output() noexcept {}

jive_float_output::jive_float_output(struct jive_node * node, size_t index)
	: jive_value_output(&JIVE_FLOAT_OUTPUT, node, index)
{}

const jive_type *
jive_float_output_get_type_(const jive_output * self_)
{
	const jive_float_output * self = (const jive_float_output *) self_;
	return &self->type();
}

/* bitstring_gate inheritable members */

jive_float_gate::~jive_float_gate() noexcept {}

jive_float_gate::jive_float_gate(jive_graph * graph, const char name[])
	: jive_value_gate(&JIVE_FLOAT_GATE, graph, name)
{}

const jive_type *
jive_float_gate_get_type_(const jive_gate * self_)
{
	const jive_float_gate * self = (const jive_float_gate *) self_;
	return &self->type();
}
