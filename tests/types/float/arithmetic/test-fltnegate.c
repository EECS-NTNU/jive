/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <locale.h>
#include <assert.h>

#include <jive/vsdg.h>
#include <jive/view.h>
#include <jive/vsdg/node-private.h>
#include <jive/types/float/flttype.h>
#include <jive/types/float/arithmetic/fltnegate.h>

static int
test_main(void)
{
	setlocale(LC_ALL, "");

	jive_context * context = jive_context_create();
	jive_graph * graph = jive_graph_create(context);

	JIVE_DECLARE_FLOAT_TYPE(flttype);
	jive_node * top = jive_node_create(graph->root_region,
		0, NULL, NULL,
		1, &flttype);

	jive_output * neg = jive_fltnegate(top->outputs[0]);

	jive_node * bottom = jive_node_create(graph->root_region,
		1, &flttype, &neg,
		0, NULL);
	jive_node_reserve(bottom);

	jive_graph_normalize(graph);
	jive_graph_prune(graph);
	jive_view(graph, stdout);

	jive_graph_destroy(graph);
	assert(jive_context_is_empty(context));
	jive_context_destroy(context);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("types/float/arithmetic/test-fltnegate", test_main)
