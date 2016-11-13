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
#include <jive/vsdg/substitution.h>

#include "testnodes.h"

static int test_main(void)
{
	setlocale(LC_ALL, "");
	jive_graph graph;
	
	jive::region * r1 = new jive::region(graph.root(), &graph);
	
	jive_test_value_type type;
	jive_node * top = jive_test_node_create(r1, {}, {}, {&type, &type, &jive::ctl::boolean});
	r1->set_top(top);
	
	auto tmp = jive_gamma(top->output(2), {&type}, {{top->output(0)}, {top->output(1)}})[0];
	jive_node * gamma = dynamic_cast<jive::output*>(tmp)->node();

	jive_node * bottom = jive_test_node_create(r1, {&type}, {gamma->output(0)}, {});
	r1->set_bottom(bottom);
	
	jive_view(&graph, stderr);
	
	jive::region * r2 = new jive::region(graph.root(), &graph);
	jive::substitution_map subst;
	r1->copy(r2, subst, true, true);

	jive_node * copied_top = r2->top();
	jive_node * copied_bottom = r2->bottom();
	assert(copied_top && copied_top->ninputs() == 0 && copied_top->noutputs() == 3);
	assert(copied_bottom && copied_bottom->ninputs() == 1 && copied_bottom->noutputs() == 0);
	jive_node * copied_gamma;
	copied_gamma = dynamic_cast<jive::output*>(copied_bottom->input(0)->origin())->node();
	assert(copied_gamma->operation() == gamma->operation());
	jive_node * alt1 = dynamic_cast<jive::output*>(copied_gamma->input(0)->origin())->node();
	jive_node * alt2 = dynamic_cast<jive::output*>(copied_gamma->input(1)->origin())->node();
	assert(alt1->region()->parent() == r2);
	assert(alt2->region()->parent() == r2);
	assert(dynamic_cast<const jive::gamma_tail_op *>(&alt1->operation()));
	assert(dynamic_cast<const jive::gamma_tail_op *>(&alt2->operation()));

	jive_view(&graph, stderr);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("transforms/test-region-copy", test_main);
