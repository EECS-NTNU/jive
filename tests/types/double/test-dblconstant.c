/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <locale.h>

#include <jive/types/double/dblconstant.h>
#include <jive/view.h>
#include <jive/vsdg.h>

static int
test_main(void)
{
	setlocale(LC_ALL, "");

	jive_context * context = jive_context_create();
	jive_graph * graph = jive_graph_create(context);

	jive_dblconstant_double(graph, -1.0);
	jive_dblconstant_double(graph, 0.0);
	jive_dblconstant_double(graph, 1.0);
	jive_dblconstant_double(graph, 0.0 / 0.0);
	jive_dblconstant_double(graph, 1.0 / 0.0);

	jive_view(graph, stdout);
	jive_graph_destroy(graph);
	jive_context_assert_clean(context);
	jive_context_destroy(context);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("types/double/test-dblconstant", test_main);