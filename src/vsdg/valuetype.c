/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/util/list.h>
#include <jive/vsdg/graph-private.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/valuetype.h>

namespace jive {
namespace value {

type::~type() noexcept {}

/* value inputs */

input::~input() noexcept {};

input::input(struct jive_node * node, size_t index, jive::output * initial_operand)
	: jive::input(node, index, initial_operand)
{}

/* value outputs */

output::~output() noexcept {}

output::output(struct jive_node * node, size_t index)
	: jive::output(node, index)
{}

/* value gates */

gate::~gate() noexcept {}

gate::gate(jive_graph * graph, const char name[])
	: jive::gate(graph, name)
{}

}
}
