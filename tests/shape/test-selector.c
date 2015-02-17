/*
 * Copyright 2010 2011 2012 2013 2014 2015 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"
#include "testtypes.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>

#include <jive/arch/registers.h>
#include <jive/regalloc/selector.h>
#include <jive/regalloc/shaped-graph.h>
#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/resource-private.h>

#include "testnodes.h"

extern const jive_register_class gpr;

const jive_register_name
	r0 = {base : {name : "r0", resource_class : &gpr.base}, code : 0},
	r1 = {base : {name : "r1", resource_class : &gpr.base}, code : 0},
	r2 = {base : {name : "r2", resource_class : &gpr.base}, code : 0},
	r3 = {base : {name : "r3", resource_class : &gpr.base}, code : 0};

static const jive_resource_name * res_names [] = {&r0.base, &r1.base, &r2.base, &r3.base};

const jive_register_class gpr = {
		base : {
			class_ : &JIVE_REGISTER_RESOURCE,
			name : "gpr",
			limit : 4,
			names : res_names,
			parent : &jive_root_resource_class,
			depth : 1,
			priority : jive_resource_class_priority_reg_low,
		},
	}
;

static void
shape(jive_shaped_graph * shaped_graph, jive_node * node)
{
	size_t n;
	for(n = 0; n < node->ninputs; n++) {
		jive::input * input = node->inputs[n];
		jive_ssavar * ssavar = jive_input_auto_merge_variable(input);
		jive_shaped_ssavar * shaped_ssavar = jive_shaped_graph_map_ssavar(shaped_graph, ssavar);
		jive_shaped_ssavar_lower_boundary_region_depth(shaped_ssavar, node->region->depth);
	}
	
	for(n = 0; n < node->noutputs; n++) {
		jive::output * output = node->outputs[n];
		jive_output_auto_merge_variable(output);
	}
	
	jive_shaped_region * shaped_region = jive_shaped_graph_map_region(shaped_graph, node->region);
	jive_cut * cut = jive_shaped_region_create_cut(shaped_region);
	jive_cut_append(cut, node);
}

static jive_node *
create_computation_node(jive_graph * graph,
	size_t noperands, jive::output ** operands,
	size_t noutputs)
{
	jive_test_value_type type;
	const jive::base::type * input_types[noperands];
	const jive::base::type * output_types[noperands];
	
	size_t n;
	for (n = 0; n < noperands; n++)
		input_types[n] = &type;
	for (n = 0; n < noutputs; n++)
		output_types[n] = &type;
	
	
	jive_node * node = jive_test_node_create(graph->root_region,
		noperands, input_types, operands,
		noutputs, output_types);
	
	for (n = 0; n < noperands; n++)
		node->inputs[n]->required_rescls = &gpr.base;
	for (n = 0; n < noutputs; n++)
		node->outputs[n]->required_rescls = &gpr.base;
	
	return node;
}

static jive_node *
create_spill_node(jive_graph * graph,
	jive::output * operand)
{
	jive_test_value_type type;
	const jive::base::type * type_ptr = &type;
	jive_node * node = jive_test_node_create(graph->root_region,
		1, &type_ptr, &operand,
		1, &type_ptr);
	
	node->inputs[0]->required_rescls = &gpr.base;
	node->outputs[0]->required_rescls = &jive_root_resource_class;
	
	return node;
}

static jive_node *
create_restore_node(jive_graph * graph,
	jive::output * operand)
{
	jive_test_value_type type;
	const jive::base::type * type_ptr = &type;
	jive_node * node = jive_test_node_create(graph->root_region,
		1, &type_ptr, &operand,
		1, &type_ptr);
	
	node->inputs[0]->required_rescls = &jive_root_resource_class;
	node->outputs[0]->required_rescls = &gpr.base;
	
	return node;
}

static int test_main(void)
{
	setlocale(LC_ALL, "");
	jive_graph * graph = jive_graph_create();
	
	jive_node * n1 = create_computation_node(graph, 0, NULL, 2);
	jive::output * tmparray0[] = {n1->outputs[0]};
	jive_node * l1 = create_computation_node(graph, 1, tmparray0, 1);
	jive::output * tmparray1[] = {n1->outputs[1]};
	jive_node * l2 = create_computation_node(graph, 1, tmparray1, 1);
	jive::output * tmparray2[] = {l1->outputs[0]};
	jive_node * a = create_computation_node(graph, 1, tmparray2, 1);
	jive::output * tmparray3[] = {l1->outputs[0]};
	jive_node * b = create_computation_node(graph, 1, tmparray3, 1);
	jive::output * tmparray4[] = {l1->outputs[0]};
	jive_node * c = create_computation_node(graph, 1, tmparray4, 1);
	jive::output * tmparray5[] = {l1->outputs[0]};
	jive_node * d = create_computation_node(graph, 1, tmparray5, 1);
	jive::output * tmparray6[] = {l1->outputs[0]};
	jive_node * e = create_computation_node(graph, 1, tmparray6, 1);
	jive::output * tmparray7[] = {l2->outputs[0]};
	jive_node * f = create_computation_node(graph, 1, tmparray7, 1);
	jive::output * tmparray8[] = {a->outputs[0], b->outputs[0]};
	
	jive_node * s1 = create_computation_node(
		graph, 2, tmparray8, 1);
	jive::output * tmparray9[] = {c->outputs[0], s1->outputs[0]};
	jive_node * s2 = create_computation_node(
		graph, 2, tmparray9, 1);
	jive::output * tmparray10[] = {s2->outputs[0], d->outputs[0]};
	jive_node * s3 = create_computation_node(
		graph, 2, tmparray10, 1);
	jive::output * tmparray11[] = {e->outputs[0], s3->outputs[0]};
	jive_node * s4 = create_computation_node(
		graph, 2, tmparray11, 1);
	jive::output * tmparray12[] = {s4->outputs[0], f->outputs[0]};
	jive_node * s5 = create_computation_node(
		graph, 2, tmparray12, 1);
	
	jive_node_add_input(graph->root_region->bottom, &s5->outputs[0]->type(), s5->outputs[0]);
	jive_node * bottom = graph->root_region->bottom;
	
	//jive_view(graph, stdout);
	
	jive_shaped_graph * shaped_graph = jive_shaped_graph_create(graph);
	
	{
		jive_master_shaper_selector master_selector(shaped_graph);
		
		jive_region_shaper_selector * region_selector;
		region_selector = master_selector.map_region(graph->root_region);
		
		{
			jive_node_cost * n1_cost = master_selector.map_node(n1);
			assert(n1_cost->prio_array.count[0] == jive_resource_class_priority_lowest);
			assert(n1_cost->force_tree_root == true);
			
			/* test invalidation -- artifically modify stored count to make
			sure count after recomputation differs */
			jive_resource_class_count_add(&n1_cost->rescls_cost, &jive_root_resource_class);
			n1_cost->prio_array.count[0] = 0;
			master_selector.invalidate_node(n1);
			n1_cost = master_selector.map_node(n1);
			assert(n1_cost->prio_array.count[0] == jive_resource_class_priority_lowest);
		}
		
		{
			assert(master_selector.map_node(l1)->force_tree_root == true);
			assert(master_selector.map_node(l2)->force_tree_root == false);
			
			assert(master_selector.map_node(l1)->blocked_rescls_priority
				== jive_root_resource_class.priority);
			assert(master_selector.map_node(bottom)->blocked_rescls_priority
				== jive_root_resource_class.priority);
			
			assert(master_selector.map_node(bottom)->state
				== jive_node_cost_state_queue);
			assert(master_selector.map_node(bottom)->index
				== 0);
		}

		jive_node * node;
		
		assert(region_selector->prio_heap.nitems == 1);
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == bottom);
		assert(master_selector.map_node(bottom)->state
			== jive_node_cost_state_stack);
		shape(shaped_graph, node);
		
		assert(master_selector.map_node(s5)->prio_array.count[0]
			== gpr.base.priority);
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == s5);
		shape(shaped_graph, node);
		
		assert(master_selector.map_node(f)->prio_array.count[0]
			== gpr.base.priority);
		assert(region_selector->prio_heap.nitems == 2);
		assert(region_selector->prio_heap.items[0]->node() == f);
		assert(region_selector->prio_heap.items[1]->node() == s4);
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == f);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == l2);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == s4);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == e);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == s3);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == d);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == s2);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == c);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == s1);
		shape(shaped_graph, node);
		
		jive_node * tmp = jive_region_shaper_selector_select_node(region_selector);
		assert(tmp == a || tmp == b);
		assert(master_selector.map_node(tmp)->state
			== jive_node_cost_state_stack);
		jive_node * spill = create_spill_node(graph, tmp->outputs[0]);
		assert(master_selector.map_node(spill)->state
			== jive_node_cost_state_stack);
		jive_node * restore = create_restore_node(graph, spill->outputs[0]);
		assert(master_selector.map_node(restore)->state
			== jive_node_cost_state_stack);
		jive_ssavar * ssavar = tmp->outputs[0]->users.first->ssavar;
		jive_ssavar_divert_origin(ssavar, restore->outputs[0]);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == restore);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == spill);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == tmp);
		shape(shaped_graph, node);
		
		node = jive_region_shaper_selector_select_node(region_selector);
		assert(node == a || node == b);
		shape(shaped_graph, node);
	}

	jive_shaped_graph_destroy(shaped_graph);
	jive_graph_destroy(graph);

	return 0;
}
JIVE_UNIT_TEST_REGISTER("shape/test-selector", test_main);
