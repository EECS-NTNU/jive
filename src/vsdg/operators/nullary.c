/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/vsdg/operators/nullary.h>

#include <jive/vsdg/graph.h>
#include <jive/vsdg/operators/simple-normal-form.h>
#include <jive/vsdg/region.h>

namespace jive {
namespace base {

class nullary_normal_form final : public simple_normal_form {
public:
	virtual
	~nullary_normal_form() noexcept
	{
	}

	nullary_normal_form(
		const std::type_info & operator_class,
		jive::node_normal_form * parent,
		jive::graph * graph)
		: simple_normal_form(operator_class, parent, graph)
	{
	}
};

nullary_op::~nullary_op() noexcept {}

size_t
nullary_op::narguments() const noexcept
{
	return 0;
}

size_t
nullary_op::nresults() const noexcept
{
	return 1;
}

const type &
nullary_op::argument_type(size_t index) const noexcept
{
	throw std::logic_error("No arguments to nullary operation");
}

}
}

/* node class */

/* node class inheritable methods */

jive::node_normal_form *
jive_nullary_operation_get_default_normal_form_(
	const std::type_info & operator_class,
	jive::node_normal_form * parent,
	jive::graph * graph)
{
	jive::node_normal_form * nf = new jive::base::nullary_normal_form(
		operator_class, parent, graph);
	
	return nf;
}

static void  __attribute__((constructor))
register_node_normal_form(void)
{
	jive::node_normal_form::register_factory(
		typeid(jive::base::nullary_op), jive_nullary_operation_get_default_normal_form_);
}
