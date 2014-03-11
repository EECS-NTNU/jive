/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include "test-registry.h"

#include <locale.h>

#include <jive/types/function.h>
#include <jive/view.h>
#include <jive/vsdg.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/phi.h>

static void
test_simple_lambda(struct jive_graph * graph)
{
	JIVE_DECLARE_VALUE_TYPE(vtype);
	jive_node * top = jive_node_create(graph->root_region, 0, NULL, NULL, 3,
		(const jive_type *[]){vtype, vtype, vtype});

	jive_lambda * lambda = jive_lambda_begin(graph,
		3, (const jive_type *[]){vtype, vtype, vtype}, (const char *[]){"x", "y", "z"});

	jive_node * node = jive_node_create(lambda->region, 1, &vtype, &lambda->arguments[0], 1, &vtype);

	jive_output * fct = jive_lambda_end(lambda, 2, (const jive_type *[]){vtype, vtype},
		(jive_output *[]){node->outputs[0], lambda->arguments[1]});

	jive_output * results[2];
	jive_apply_create(fct, 3, top->outputs, results);

	jive_node * bottom = jive_node_create(graph->root_region,
		2, (const jive_type *[]){vtype, vtype}, results,
		0, NULL);
	jive_node_reserve(bottom);

	jive_view(graph, stderr);

	jive_lambda_node_remove_dead_parameters(jive_lambda_node_cast(fct->node));
	jive_graph_prune(graph);

	jive_view(graph, stderr);

	assert(bottom->inputs[1]->origin->node == top);

	jive_node * apply = bottom->inputs[0]->origin->node;
	assert(apply->ninputs == 2);

}

static void
test_recursive_lambda(struct jive_graph * graph)
{
	JIVE_DECLARE_VALUE_TYPE(vtype);
	jive_node * top = jive_node_create(graph->root_region, 0, NULL, NULL, 3,
		(const jive_type *[]){vtype, vtype, vtype});

	jive_function_type * fcttype = jive_function_type_create(graph->context,
		3, (const jive_type *[]){vtype, vtype, vtype}, 2, (const jive_type *[]){vtype, vtype});

	jive_phi phi = jive_phi_begin(graph);
	jive_phi_fixvar fv = jive_phi_fixvar_enter(phi, &fcttype->base.base);

	jive_lambda * lambda = jive_lambda_begin(graph,
		3, (const jive_type *[]){vtype, vtype, vtype}, (const char *[]){"x", "y", "z"});

	jive_node * node = jive_node_create(lambda->region, 1, &vtype, &lambda->arguments[0], 1, &vtype);

	jive_output * results[2];
	jive_apply_create(fv.value, 3,
		(jive_output *[]){node->outputs[0], node->outputs[0], node->outputs[0]}, results);

	jive_output * fct = jive_lambda_end(lambda, 2,
		(const jive_type *[]){vtype, vtype}, (jive_output *[]){results[0], lambda->arguments[1]});

	jive_phi_fixvar_leave(phi, fv.gate, fct);
	jive_phi_end(phi, 1, &fv);

	jive_apply_create(fv.value, 3, top->outputs, results);

	jive_node * bottom = jive_node_create(graph->root_region,
		2, (const jive_type *[]){vtype, vtype}, results,
		0, NULL);
	jive_node_reserve(bottom);

	jive_view(graph, stderr);

	jive_lambda_node_remove_dead_parameters(jive_lambda_node_cast(fct->node));
	jive_graph_prune(graph);

	jive_view(graph, stderr);

	jive_function_type_destroy(fcttype);
}

static int
test_main(void)
{
	setlocale(LC_ALL, "");

	jive_context * context = jive_context_create();

	jive_graph * graph = jive_graph_create(context);
	test_simple_lambda(graph);
	jive_graph_destroy(graph);

	graph = jive_graph_create(context);
	test_recursive_lambda(graph);
	jive_graph_destroy(graph);

	jive_context_assert_clean(context);
	jive_context_destroy(context);

	return 0;
}

JIVE_UNIT_TEST_REGISTER("types/function/test-lambda_dead_parameter_removal", test_main);
