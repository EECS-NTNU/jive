/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/bitstring/type.h>
#include <jive/util/buffer.h>

#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>

#include <stdio.h>
#include <string.h>

namespace jive {
namespace bits {

/* type */

type::~type() noexcept {}

void
type::label(jive_buffer & buffer) const
{
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "bits%zd", nbits());
	jive_buffer_putstr(&buffer, tmp);
}

std::string
type::debug_string() const
{
	char tmp[16];
	snprintf(tmp, sizeof(tmp), "bits%zd", nbits());
	return std::string(tmp);
}

bool
type::operator==(const jive::base::type & _other) const noexcept
{
	const jive::bits::type * other = dynamic_cast<const jive::bits::type*>(&_other);
	return other != nullptr && this->nbits() == other->nbits();
}

jive::bits::type *
type::copy() const
{
	return new jive::bits::type(this->nbits());
}

jive::input *
type::create_input(jive_node * node, size_t index, jive::output * origin) const
{
	return new jive::bits::input(nbits_, node, index, origin);
}

jive::output *
type::create_output(jive_node * node, size_t index) const
{
	return new jive::bits::output(nbits(), node, index);
}

jive::gate *
type::create_gate(jive_graph * graph, const char * name) const
{
	return new jive::bits::gate(nbits(), graph, name);
}

/* input */

input::input(size_t nbits, struct jive_node * node, size_t index, jive::output * origin)
	: jive::value::input(node, index, origin)
	, type_(nbits)
{}

input::~input() noexcept {}

/* output */

output::output(size_t nbits, struct jive_node * node, size_t index)
	: jive::value::output(node, index)
	, type_(nbits)
{}

output::~output() noexcept {}

/* gate */

gate::gate(size_t nbits, jive_graph * graph, const char name[])
	: jive::value::gate(graph, name)
	, type_(nbits)
{}

gate::~gate() noexcept {}

}
}
