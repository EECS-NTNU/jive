/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/statetype.h>

#include <jive/common.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/region.h>

namespace jive {
namespace state {

mux_op::~mux_op() noexcept {}

bool
mux_op::operator==(const operation & other) const noexcept
{
	const mux_op * op = dynamic_cast<const mux_op *>(&other);
	return op &&
		op->narguments_ == narguments_ &&
		op->nresults_ == nresults_ &&
		*op->state_type_ == *state_type_;
}

size_t
mux_op::narguments() const noexcept
{
	return narguments_;
}

const jive::base::type &
mux_op::argument_type(size_t index) const noexcept
{
	return *state_type_;
}

size_t
mux_op::nresults() const noexcept
{
	return nresults_;
}

const jive::base::type &
mux_op::result_type(size_t index) const noexcept
{
	return *state_type_;
}
std::string
mux_op::debug_string() const
{
	return "STATEMUX";
}

std::unique_ptr<jive::operation>
mux_op::copy() const
{
	return std::unique_ptr<jive::operation>(new mux_op(*this));
}

}
}

jive::oport *
jive_state_merge(const jive::state::type * statetype, size_t nstates, jive::oport * const states[])
{
	jive::region * region = states[0]->region();

	jive::state::mux_op op(*statetype, nstates, 1);
	return jive_node_create_normalized(region, op,
		std::vector<jive::oport*>(states, states + nstates))[0];
}

std::vector<jive::oport*>
jive_state_split(const jive::state::type * statetype, jive::oport * state, size_t nstates)
{
	jive::state::mux_op op(*statetype, 1, nstates);
	return jive_node_create_normalized(state->region(), op, {state});
}
