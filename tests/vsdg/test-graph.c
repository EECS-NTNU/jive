/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <stdio.h>

#include <jive/view.h>
#include <jive/vsdg.h>

#include "testnodes.h"

static int test_main(void)
{
	jive::graph graph;
	
	jive::region * region = graph.root();
	
	jive_test_value_type type;
	auto n1 = jive::test::simple_node_create(region, {}, {}, {&type});
	assert(n1);
	assert(n1->depth() == 0);

	auto n2 = jive::test::simple_node_create(region, {&type}, {n1->output(0)}, {});
	assert(n2);
	assert(n2->depth() == 1);
	
	return 0;
}

JIVE_UNIT_TEST_REGISTER("vsdg/test-graph", test_main);
