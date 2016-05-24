/*
 * Copyright 2010 2011 2012 2014 2015 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>

#include <jive/regalloc/reroute.h>
#include <jive/regalloc/shaped-graph.h>
#include <jive/regalloc/shaped-node.h>
#include <jive/regalloc/shaped-region.h>
#include <jive/regalloc/shaped-variable.h>
#include <jive/types/bitstring.h>
#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/anchortype.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/theta.h>

#include "testnodes.h"

static jive_shaped_node *
shape(jive_shaped_graph * shaped_graph, jive_node * node)
{
	for (size_t n = 0; n < node->noutputs; n++) {
		jive_ssavar * ssavar = jive_output_auto_merge_variable(node->outputs[n]);
		shaped_graph->map_ssavar(ssavar)->lower_boundary_region_depth(node->region->depth());
	}
	for (size_t n = 0; n < node->ninputs; n++) {
		jive_ssavar * ssavar = jive_input_auto_merge_variable(node->inputs[n]);
		shaped_graph->map_ssavar(ssavar)->lower_boundary_region_depth(node->region->depth());
	}
	jive_shaped_region * shaped_region = shaped_graph->map_region(node->region);
	jive_cut * cut = shaped_region->create_top_cut();
	return cut->append(node);
}

static int test_main(void)
{
	setlocale(LC_ALL, "");
	jive_graph * graph = jive_graph_create();
	
	jive_test_value_type type;
	jive::achr::type anchor_type;
	jive_node * dummy = jive_test_node_create(graph->root_region, {}, {}, {&type});

	jive_region * r1 = jive_region_create_subregion(graph->root_region);

	jive_node * top = jive_test_node_create(r1, {&type}, {dummy->outputs[0]}, {&type, &type});

	jive_theta theta = jive_theta_begin(r1);
	jive_region * loop_region = theta.region;

	jive_theta_loopvar loopvar1 = jive_theta_loopvar_enter(theta, top->outputs[0]);

	jive_node * theta_op = jive_test_node_create(loop_region,
		{&type, &type}, {loopvar1.value, top->outputs[1]}, {&jive::ctl::boolean, &type});

	jive_theta_loopvar_leave(theta, loopvar1.gate, theta_op->outputs[1]);
	jive_node * theta_node = jive_theta_end(theta, theta_op->outputs[0],
		1, &loopvar1);
	jive_node * theta_head = loop_region->top;
	jive_node * theta_tail = loop_region->bottom;

	jive_node * bottom = jive_test_node_create(r1,
		{&type, &type}, {theta_node->outputs[0], top->outputs[1]}, {&anchor_type});

	jive_node * subroutine_anchor = jive_test_node_create(graph->root_region,
		{&anchor_type}, {bottom->outputs[0]}, {});

	jive_shaped_graph * shaped_graph = jive_shaped_graph_create(graph);
	
	shape(shaped_graph, subroutine_anchor);
	shape(shaped_graph, bottom);
	shape(shaped_graph, theta_node);
	jive_shaped_node * p = shape(shaped_graph, theta_tail);
	shape(shaped_graph, theta_op);
	shape(shaped_graph, theta_head);
	shape(shaped_graph, top);
	shape(shaped_graph, dummy);
	
	jive_view(graph, stdout);
	
	jive_ssavar * orig_ssavar = bottom->inputs[1]->ssavar;
	jive_variable * var = orig_ssavar->variable;
	
	jive_ssavar * ssavar_p = jive_regalloc_reroute_at_point(bottom->inputs[1]->ssavar, p);
	assert(ssavar_p != orig_ssavar);
	assert(orig_ssavar->variable == var);
	assert(ssavar_p->variable == var);
	
	assert(theta_head->ninputs == 2 && theta_head->noutputs == 3);
	assert(theta_tail->ninputs == 4);
	assert(theta_node->noutputs == 2);
	
	jive::gate * gate = theta_head->inputs[1]->gate;
	assert(gate);
	assert(theta_head->inputs[1]->gate == gate && theta_head->outputs[2]->gate == gate);
	assert(theta_tail->inputs[3]->gate == gate);
	assert(theta_node->outputs[1]->gate == gate);
	
	assert(theta_head->inputs[1]->origin() == top->outputs[1]);
	assert(theta_tail->inputs[3]->origin() == theta_head->outputs[2]);
	assert(bottom->inputs[1]->origin() == theta_node->outputs[1]);
	
	assert(top->outputs[1]->ssavar == orig_ssavar);
	assert(theta_head->inputs[1]->ssavar == orig_ssavar);
	assert(theta_head->outputs[2]->ssavar == ssavar_p);
	assert(theta_tail->inputs[3]->ssavar == ssavar_p);
	assert(theta_op->inputs[1]->origin() == theta_head->outputs[2]);
	assert(theta_op->inputs[1]->ssavar == ssavar_p);
	
	jive_ssavar * ssavar_below = bottom->inputs[1]->ssavar;
	assert(theta_node->outputs[1]->ssavar == ssavar_below);
	assert(bottom->inputs[1]->ssavar == ssavar_below);
	
	jive_view(graph, stdout);
	
	jive_shaped_graph_destroy(shaped_graph);
	
	jive_graph_destroy(graph);
	return 0;
}

JIVE_UNIT_TEST_REGISTER("shape/test-reroute-theta1", test_main);
