/*
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/bitstring/type.h>
#include <jive/util/buffer.h>
#include <jive/vsdg/basetype-private.h>
#include <jive/vsdg/valuetype-private.h>

#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>

#include <stdio.h>
#include <string.h>

static void
jive_bitstring_type_get_label_(const jive_type * self, struct jive_buffer * buffer);

static jive_input *
jive_bitstring_type_create_input_(const jive_type * self, struct jive_node * node, size_t index,
	jive_output * initial_operand);

static jive_output *
jive_bitstring_type_create_output_(const jive_type * self, struct jive_node * node, size_t index);

static jive_gate *
jive_bitstring_type_create_gate_(const jive_type * self, struct jive_graph * graph,
	const char * name);

static jive_type *
jive_bitstring_type_copy_(const jive_type * self);

static bool
jive_bitstring_type_equals_(const jive_type * self, const jive_type * other);

static void
jive_bitstring_output_init_(jive_bitstring_output * self, size_t nbits, struct jive_node * node,
	size_t index);

static void
jive_bitstring_output_fini_(jive_output * self);

static const jive_type *
jive_bitstring_output_get_type_(const jive_output * self);

static void
jive_bitstring_gate_init_(jive_bitstring_gate * self, size_t nbits, struct jive_graph * graph,
	const char name[]);

static void
jive_bitstring_gate_fini_(jive_gate * self);

static const jive_type *
jive_bitstring_gate_get_type_(const jive_gate * self);

const jive_type_class JIVE_BITSTRING_TYPE = {
	parent : &JIVE_VALUE_TYPE,
	name : "bit",
	fini : jive_value_type_fini_, /* inherit */
	get_label : jive_bitstring_type_get_label_, /* override */
	create_input : jive_bitstring_type_create_input_, /* override */
	create_output : jive_bitstring_type_create_output_, /* override */
	create_gate : jive_bitstring_type_create_gate_, /* override */
	equals : jive_bitstring_type_equals_, /* override */
	copy : jive_bitstring_type_copy_, /* override */
};

const jive_output_class JIVE_BITSTRING_OUTPUT = {
	parent : &JIVE_VALUE_OUTPUT,
	fini : jive_bitstring_output_fini_, /* override */
	get_label : jive_output_get_label_, /* inherit */
	get_type : jive_bitstring_output_get_type_, /* override */
};

const jive_gate_class JIVE_BITSTRING_GATE = {
	parent : &JIVE_VALUE_GATE,
	fini : jive_bitstring_gate_fini_, /* override */
	get_label : jive_gate_get_label_, /* inherit */
	get_type : jive_bitstring_gate_get_type_, /* override */
};

/* bitstring_type inheritable members */

jive_bitstring_type::~jive_bitstring_type() noexcept {}

jive_bitstring_type::jive_bitstring_type(size_t nbits) noexcept
	: jive_value_type(&JIVE_BITSTRING_TYPE)
	, nbits_(nbits)
{}

static void
jive_bitstring_type_get_label_(const jive_type * self_, struct jive_buffer * buffer)
{
	const jive_bitstring_type * self = (const jive_bitstring_type *) self_;
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "bits%zd", self->nbits());
	jive_buffer_putstr(buffer, tmp);
}

static jive_input *
jive_bitstring_type_create_input_(const jive_type * self_, struct jive_node * node, size_t index,
	jive_output * initial_operand)
{
	const jive_bitstring_type * self = (const jive_bitstring_type *) self_;
	jive_bitstring_input * input = new jive_bitstring_input(self->nbits(), node, index,
		initial_operand);
	return input;
}

static jive_output *
jive_bitstring_type_create_output_(const jive_type * self_, struct jive_node * node, size_t index)
{
	const jive_bitstring_type * self = (const jive_bitstring_type *) self_;
	return new jive_bitstring_output(self->nbits(), node, index);
}

static jive_gate *
jive_bitstring_type_create_gate_(const jive_type * self_, struct jive_graph * graph,
	const char * name)
{
	const jive_bitstring_type * self = (const jive_bitstring_type *) self_;
	return new jive_bitstring_gate(self->nbits(), graph, name);
}

static jive_type *
jive_bitstring_type_copy_(const jive_type * self_)
{
	const jive_bitstring_type * self = (const jive_bitstring_type *) self_;
	
	jive_bitstring_type * type = new jive_bitstring_type(self->nbits());
	
	return type;
}

static bool
jive_bitstring_type_equals_(const jive_type * self_, const jive_type * other_)
{
	if (self_->class_ != other_->class_) return false;
	const jive_bitstring_type * self = (const jive_bitstring_type *)self_;
	const jive_bitstring_type * other = (const jive_bitstring_type *)other_;
	
	return self->nbits() == other->nbits();
}

/* bitstring_input inheritable members */

jive_bitstring_input::jive_bitstring_input(size_t nbits, struct jive_node * node, size_t index,
	jive_output * origin)
	: jive_value_input(node, index, origin)
	, type_(nbits)
{}

jive_bitstring_input::~jive_bitstring_input() noexcept {}

/* bitstring_output inheritable members */

jive_bitstring_output::jive_bitstring_output(size_t nbits, struct jive_node * node,
	size_t index)
	: jive_value_output(&JIVE_BITSTRING_OUTPUT, node, index)
	, type_(nbits)
{}

jive_bitstring_output::~jive_bitstring_output() noexcept {}

static void
jive_bitstring_output_fini_(jive_output * self_)
{
}

static const jive_type *
jive_bitstring_output_get_type_(const jive_output * self_)
{
	const jive_bitstring_output * self = (const jive_bitstring_output *) self_;
	return &self->type();
}

/* bitstring_gate inheritable members */

jive_bitstring_gate::jive_bitstring_gate(size_t nbits, jive_graph * graph,
	const char name[])
	: jive_value_gate(&JIVE_BITSTRING_GATE, graph, name)
	, type_(nbits)
{}

jive_bitstring_gate::~jive_bitstring_gate() noexcept {}

static void
jive_bitstring_gate_fini_(jive_gate * self_)
{
}

static const jive_type *
jive_bitstring_gate_get_type_(const jive_gate * self_)
{
	const jive_bitstring_gate * self = (const jive_bitstring_gate *) self_;
	return &self->type();
}
