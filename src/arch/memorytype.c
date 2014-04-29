/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/arch/memorytype.h>

#include <string.h>

#include <jive/context.h>
#include <jive/vsdg/basetype-private.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/statetype-private.h>

jive_memory_input::~jive_memory_input() noexcept {}

jive_memory_input::jive_memory_input(struct jive_node * node, size_t index,
	jive_output * origin)
	: jive_state_input(node, index, origin)
{}

jive_memory_output::~jive_memory_output() noexcept {}

jive_memory_output::jive_memory_output(jive_node * node, size_t index)
	: jive_state_output(&JIVE_MEMORY_OUTPUT, node, index)
{}

static const jive_type *
jive_memory_output_get_type_(const jive_output * self_)
{
	const jive_memory_output * self = (const jive_memory_output *) self_;
	return &self->type();
}

jive_memory_gate::~jive_memory_gate() noexcept {}

jive_memory_gate::jive_memory_gate(jive_graph * graph, const char name[])
	: jive_state_gate(&JIVE_MEMORY_GATE, graph, name)
{}

static const jive_type *
jive_memory_gate_get_type_(const jive_gate * self_)
{
	const jive_memory_gate * self = (const jive_memory_gate *) self_;
	return &self->type();
}

jive_memory_type::~jive_memory_type() noexcept {}

jive_memory_type::jive_memory_type() noexcept
	: jive_state_type(&JIVE_MEMORY_TYPE)
{}

static jive_type *
jive_memory_type_copy_(const jive_type * self_)
{
	const jive_memory_type * self = (const jive_memory_type *) self_;
	
	jive_memory_type * type = new jive_memory_type;
	type->class_ = &JIVE_MEMORY_TYPE;	
	
	return type;
}

static jive_input *
jive_memory_type_create_input_(const jive_type * self, struct jive_node * node, size_t index, jive_output * initial_operand)
{
	return new jive_memory_input(node, index, initial_operand);
}

static jive_output *
jive_memory_type_create_output_(const jive_type * self, struct jive_node * node, size_t index)
{
	return new jive_memory_output(node, index);
}

static jive_gate *
jive_memory_type_create_gate_(const jive_type * self, struct jive_graph * graph, const char * name)
{
	return new jive_memory_gate(graph, name);
}

const jive_type_class JIVE_MEMORY_TYPE = {
	parent : &JIVE_STATE_TYPE,
	name : "mem",
	fini : jive_state_type_fini_, /* inherit */
	get_label : jive_type_get_label_, /* inherit */
	create_input : jive_memory_type_create_input_, /* override */
	create_output : jive_memory_type_create_output_, /* override */
	create_gate : jive_memory_type_create_gate_, /* override */
	equals : jive_type_equals_, /* inherit */
	copy : jive_memory_type_copy_, /* override */
};

const jive_output_class JIVE_MEMORY_OUTPUT = {
	parent : &JIVE_STATE_OUTPUT,
	fini : jive_output_fini_, /* inherit */
	get_label : jive_output_get_label_, /* inherit */
	get_type : jive_memory_output_get_type_, /* override */
};

const jive_gate_class JIVE_MEMORY_GATE = {
	parent : &JIVE_STATE_GATE,
	fini : jive_gate_fini_, /* inherit */
	get_label : jive_gate_get_label_, /* inherit */
	get_type : jive_memory_gate_get_type_, /* override */
};

