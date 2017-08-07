/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <assert.h>

#include <jive/arch/memorytype.h>
#include <jive/vsdg.h>

#include "testnodes.h"

static int test_main(void)
{
	jive::graph graph;

	auto node0 = jive::test::simple_node_create(graph.root(), {}, {}, {});
	auto node1 = jive::test::simple_node_create(graph.root(), {}, {}, {});

	jive::mem::type memtype;
	auto arg_gate = graph.create_gate("arg", memtype);
	auto ret_gate = graph.create_gate("ret", memtype);

	jive::oport * arg = node0->add_output(arg_gate);
	auto ret = node1->add_input(ret_gate, arg);

	assert(dynamic_cast<const jive::state::type*>(&arg->type()));
	assert(dynamic_cast<const jive::state::type*>(&ret->type()));
	assert(dynamic_cast<const jive::state::type*>(&arg_gate->type()));
	assert(dynamic_cast<jive::state::type*>(&memtype) != nullptr);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("arch/test-memory-type", test_main);
