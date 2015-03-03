/*
 * Copyright 2010 2011 2012 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>

#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/node-private.h>

#include "testnodes.h"

typedef struct graph_desc {
	jive_graph * graph;
	jive_node * a1, * a2;
	jive_node * b1, * b2;
} graph_desc;

static graph_desc
prepare_graph()
{
	graph_desc g;
	g.graph = jive_graph_create();
	
	jive_region * region = g.graph->root_region;
	jive_test_value_type type;
	g.a1 = jive_test_node_create(region, {}, {}, {&type});
	g.a2 = jive_test_node_create(region, {&type}, {g.a1->outputs[0]}, {&type});
	g.b1 = jive_test_node_create(region, {}, {}, {&type});
	g.b2 = jive_test_node_create(region, {&type}, {g.b1->outputs[0]}, {&type});
	
	return g;
}

static void
test_simple_upward_cone()
{
	graph_desc g = prepare_graph();
	
	{
		jive::upward_cone_traverser trav(g.a2);
		
		assert( trav.next() == g.a2 );
		assert( trav.next() == g.a1 );
		assert( trav.next() == NULL );
	}
	
	jive_graph_destroy(g.graph);
}

static void
test_mutable_upward_cone_1()
{
	graph_desc g = prepare_graph();
	
	{
		jive::upward_cone_traverser trav(g.a2);
	
		assert( trav.next() == g.a2 );
		jive_node_destroy(g.b2);
		assert( trav.next() == g.a1 );
		assert( trav.next() == nullptr );
	}
	
	jive_graph_destroy(g.graph);
}

static void
test_mutable_upward_cone_2()
{
	graph_desc g = prepare_graph();
	
	{
		jive::upward_cone_traverser trav(g.a2);

		jive_node_destroy(g.a2);
		assert( trav.next() == g.a1 );
		assert( trav.next() == nullptr );
	}

	jive_graph_destroy(g.graph);
}

static void
test_mutable_upward_cone_3()
{
	graph_desc g = prepare_graph();
	
	{
		jive::upward_cone_traverser trav(g.a2);
	
		g.a2->inputs[0]->divert_origin(g.b1->outputs[0]);
		assert( trav.next() == g.a2 );
		assert( trav.next() == g.b1 );
	}
	
	jive_graph_destroy(g.graph);
}

static int test_main(void)
{
	test_simple_upward_cone();
	test_mutable_upward_cone_1();
	test_mutable_upward_cone_2();
	test_mutable_upward_cone_3();
	
	return 0;
}

JIVE_UNIT_TEST_REGISTER("vsdg/test-cone-traverser", test_main);
