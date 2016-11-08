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
#include <jive/vsdg/statetype.h>

#include "testnodes.h"

static int test_main(void)
{
	setlocale(LC_ALL, "");
	
	jive_graph * graph = jive_graph_create();
	
	jive_test_state_type statetype;
	jive_node * top = jive_test_node_create(graph->root(), {}, {}, {&statetype, &statetype});

	std::vector<jive::output*> outputs;
	for (size_t n = 0; n < top->noutputs(); n++)
		outputs.push_back(top->output(n));

	jive::output * merged = jive_state_merge(&statetype, 2, &outputs[0]);
	std::vector<jive::output *> split = jive_state_split(&statetype, merged, 2);
	jive_test_node_create(graph->root(), {&statetype, &statetype}, {split[0], split[1]}, {});

	jive_view(graph, stdout);
	
	jive_graph * graph2 = jive_graph_copy(graph);
	
	jive_graph_destroy(graph);
	
	jive_view(graph2, stdout);
	
	jive_graph_destroy(graph2);
	return 0;
}

JIVE_UNIT_TEST_REGISTER("vsdg/test-statesplit", test_main);
