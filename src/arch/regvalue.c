/*
 * Copyright 2010 2011 2012 2013 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/arch/regvalue.h>

#include <string.h>

#include <jive/types/bitstring/type.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/region.h>
#include <jive/vsdg/simple_node.h>

namespace jive {

regvalue_op::~regvalue_op() noexcept
{
}

bool
regvalue_op::operator==(const operation & other) const noexcept
{
	const regvalue_op * op =
		dynamic_cast<const regvalue_op *>(&other);
	return op && op->regcls() == regcls();
}

size_t
regvalue_op::narguments() const noexcept
{
	return 1;
}

const jive::base::type &
regvalue_op::argument_type(size_t index) const noexcept
{
	return *regcls()->type;
}

size_t
regvalue_op::nresults() const noexcept
{
	return 1;
}

const jive::base::type &
regvalue_op::result_type(size_t index) const noexcept
{
	return *regcls()->type;
}
std::string
regvalue_op::debug_string() const
{
	return regcls()->name();
}

std::unique_ptr<jive::operation>
regvalue_op::copy() const
{
	return std::unique_ptr<jive::operation>(new regvalue_op(*this));
}

}

jive::output *
jive_regvalue(jive::output * value, const jive_register_class * regcls)
{
	return jive::create_normalized(value->region(), jive::regvalue_op(regcls), {value})[0];
}
