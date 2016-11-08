/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <locale.h>

#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/node-private.h>

#include "testnodes.h"

static int test_main(void)
{
	jive_graph * graph = jive_graph_create();
	
	jive::region * region = graph->root_region;
	jive_test_state_type type;
	jive_test_value_type value_type;

	jive_node * n1 = jive_test_node_create(region, {}, {}, {&type});

	bool error_handler_called = false;
	try {
		jive_test_node_create(region, {&value_type}, {n1->output(0)}, {});
	} catch (jive::type_error e) {
		error_handler_called = true;
	}
	
	assert(error_handler_called);

	jive_graph_destroy(graph);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("vsdg/test-typemismatch", test_main);
