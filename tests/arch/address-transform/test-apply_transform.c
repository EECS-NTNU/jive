/*
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <locale.h>
#include <assert.h>

#include <jive/arch/address-transform.h>
#include <jive/arch/addresstype.h>
#include <jive/types/function/fctapply.h>
#include <jive/types/function/fcttype.h>
#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/node-private.h>

#include "testnodes.h"

static int
test_main(void)
{
	setlocale(LC_ALL, "");

	jive_context * context = jive_context_create();
	jive_graph * graph = jive_graph_create(context);

	jive::addr::type addrtype;
	const jive_type * addrptr = &addrtype;
	jive_function_type fcttype(1, &addrptr, 1, &addrptr);
	const jive_type * tmparray0[] = {&fcttype, &addrtype};

	jive_node * top = jive_test_node_create(graph->root_region,
		0, NULL, NULL, 2, tmparray0);

	jive_output * results[1];
	jive_apply_create(top->outputs[0], 1, &top->outputs[1], results);

	jive_node * bottom = jive_test_node_create(graph->root_region,
		1, &addrptr, results, 0, NULL);
	(void)bottom;

	jive_view(graph, stdout);

	jive_apply_node_address_transform((const jive_apply_node *)results[0]->node(), 32);

	jive_view(graph, stdout);

	assert(jive_node_isinstance(bottom->producer(0), &JIVE_BITSTRING_TO_ADDRESS_NODE));

	jive_graph_destroy(graph);
	jive_context_assert_clean(context);
	jive_context_destroy(context);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("arch/address-transform/test-apply_transform", test_main);
