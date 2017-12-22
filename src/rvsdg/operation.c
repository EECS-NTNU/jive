/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/rvsdg/graph.h>
#include <jive/rvsdg/operation.h>
#include <jive/rvsdg/region.h>
#include <jive/rvsdg/resource.h>
#include <jive/rvsdg/simple-normal-form.h>
#include <jive/rvsdg/structural-normal-form.h>

namespace jive {

/* port */

port::port(jive::gate * gate)
: gate_(gate)
, rescls_(gate->rescls())
, type_(gate->type().copy())
{}

port::port(const jive::type & type)
: port(std::move(type.copy()))
{}

port::port(std::unique_ptr<jive::type> type)
: gate_(nullptr)
, rescls_(&jive_root_resource_class)
, type_(std::move(type))
{}

port::port(const resource_class * rescls)
: gate_(nullptr)
, rescls_(rescls)
, type_(std::move(rescls->type().copy()))
{}

/* operation */

operation::~operation() noexcept
{}

jive::node_normal_form *
operation::normal_form(jive::graph * graph) noexcept
{
	return graph->node_normal_form(typeid(operation));
}

/* simple operation */

simple_op::~simple_op()
{}

jive::simple_normal_form *
simple_op::normal_form(jive::graph * graph) noexcept
{
	return static_cast<jive::simple_normal_form*>(graph->node_normal_form(typeid(simple_op)));
}

/* structural operation */

bool
structural_op::operator==(const operation & other) const noexcept
{
	return typeid(*this) == typeid(other);
}

size_t
structural_op::narguments() const noexcept
{
	return 0;
}

const jive::port &
structural_op::argument(size_t index) const noexcept
{
	JIVE_ASSERT(0 && "Structural operations have no arguments.");
}

size_t
structural_op::nresults() const noexcept
{
	return 0;
}

const jive::port &
structural_op::result(size_t index) const noexcept
{
	JIVE_ASSERT(0 && "Structural operations have no results.");
}

jive::structural_normal_form *
structural_op::normal_form(jive::graph * graph) noexcept
{
	return static_cast<structural_normal_form*>(graph->node_normal_form(typeid(structural_op)));
}

}
