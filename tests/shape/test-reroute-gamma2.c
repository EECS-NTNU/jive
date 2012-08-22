/* test rerouting when the value is used below the gamma
point and the two gamma regions have not yet been
finished */

#include <assert.h>
#include <stdio.h>
#include <locale.h>
#include <jive/vsdg.h>
#include <jive/vsdg/anchortype.h>
#include <jive/vsdg/control.h>
#include <jive/vsdg/node-private.h>
#include <jive/view.h>

#include <jive/regalloc/reroute.h>
#include <jive/regalloc/shaped-graph.h>
#include <jive/regalloc/shaped-region.h>
#include <jive/regalloc/shaped-node-private.h>
#include <jive/regalloc/shaped-variable-private.h>
#include <jive/types/bitstring.h>

static jive_shaped_node *
shape(jive_shaped_graph * shaped_graph, jive_node * node)
{
	size_t n;
	for (n = 0; n < node->noutputs; n++) {
		jive_ssavar * ssavar = jive_output_auto_merge_variable(node->outputs[n]);
		jive_shaped_ssavar * shaped_ssavar = jive_shaped_graph_map_ssavar(shaped_graph, ssavar);
		jive_shaped_ssavar_lower_boundary_region_depth(shaped_ssavar, node->region->depth);
	}
	for (n = 0; n < node->ninputs; n++) {
		jive_ssavar * ssavar = 0;
		jive_input * user;
		JIVE_LIST_ITERATE(node->inputs[n]->origin->users, user, output_users_list) {
			if (user->ssavar && jive_region_contains_node(user->node->region, node)) {
				ssavar = user->ssavar;
				break;
			}
		}
		if (!ssavar)
			ssavar = jive_input_auto_assign_variable(node->inputs[n]);
		else
			jive_ssavar_assign_input(ssavar, node->inputs[n]);
		jive_shaped_ssavar * shaped_ssavar = jive_shaped_graph_map_ssavar(shaped_graph, ssavar);
		jive_shaped_ssavar_lower_boundary_region_depth(shaped_ssavar, node->region->depth);
	}
	jive_shaped_region * shaped_region = jive_shaped_graph_map_region(shaped_graph, node->region);
	jive_cut * cut = jive_shaped_region_create_cut(shaped_region);
	return jive_cut_append(cut, node);
}

int main()
{
	setlocale(LC_ALL, "");
	jive_context * ctx = jive_context_create();
	jive_graph * graph = jive_graph_create(ctx);
	
	JIVE_DECLARE_TYPE(type);
	JIVE_DECLARE_CONTROL_TYPE(control_type);
	
	jive_node * top = jive_node_create(graph->root_region,
		0, NULL, NULL,
		1, (const jive_type *[]){type});
	
	jive_node * pred = jive_node_create(graph->root_region,
		1, (const jive_type *[]){type}, top->outputs,
		1, (const jive_type *[]){control_type});
	
	jive_node * l1 = jive_node_create(graph->root_region,
		1, (const jive_type *[]){type}, top->outputs,
		1, (const jive_type *[]){type});
	
	jive_node * l2 = jive_node_create(graph->root_region,
		1, (const jive_type *[]){type}, top->outputs,
		1, (const jive_type *[]){type});
	
	jive_node * r1 = jive_node_create(graph->root_region,
		1, (const jive_type *[]){type}, top->outputs,
		1, (const jive_type *[]){type});
	
	jive_node * r2 = jive_node_create(graph->root_region,
		1, (const jive_type *[]){type}, top->outputs,
		1, (const jive_type *[]){type});
	
	jive_output * gamma[2];
	jive_gamma(pred->outputs[0],
		2, (const jive_type *[]){type, type},
		(jive_output *[]){l1->outputs[0], l2->outputs[0]},
		(jive_output *[]){r1->outputs[0], r2->outputs[0]},
		gamma);
	jive_node * gamma_node = gamma[0]->node;
	
	jive_node * bottom = jive_node_create(graph->root_region,
		3, (const jive_type *[]){type, type, type}, (jive_output *[]) {gamma[0], gamma[1], top->outputs[0]},
		0, NULL);
	
	jive_graph_pull_inward(graph);
	
	jive_shaped_graph * shaped_graph = jive_shaped_graph_create(graph);
	
	shape(shaped_graph, bottom);
	shape(shaped_graph, gamma_node);
	shape(shaped_graph, gamma_node->inputs[0]->origin->node);
	shape(shaped_graph, l1);
	shape(shaped_graph, l2);
	shape(shaped_graph, gamma_node->inputs[1]->origin->node);
	shape(shaped_graph, r1);
	jive_shaped_node * p = shape(shaped_graph, r2);
	
	jive_ssavar * orig_ssavar = bottom->inputs[2]->ssavar;
	jive_variable * var = orig_ssavar->variable;
	assert(l1->inputs[0]->ssavar == orig_ssavar);
	assert(l2->inputs[0]->ssavar == orig_ssavar);
	assert(r1->inputs[0]->ssavar == orig_ssavar);
	assert(r2->inputs[0]->ssavar == orig_ssavar);
	
	jive_ssavar * ssavar_p = jive_regalloc_reroute_at_point(orig_ssavar, p);
	
	jive_output * new_orig = bottom->inputs[2]->origin;
	jive_gate * reroute_gate = new_orig->gate;
	assert(new_orig->node == gamma_node && reroute_gate && reroute_gate->variable == var);
	
	jive_ssavar * ssavar_below = new_orig->ssavar;
	assert(ssavar_below != orig_ssavar);
	assert(bottom->inputs[2]->ssavar == ssavar_below);
	assert(ssavar_below->variable == var);
	
	jive_input * in_l = jive_node_get_gate_input(gamma_node->inputs[0]->origin->node, reroute_gate);
	jive_ssavar * ssavar_l = in_l->ssavar;
	assert(ssavar_l->variable == var);
	jive_input * in_r = jive_node_get_gate_input(gamma_node->inputs[1]->origin->node, reroute_gate);
	jive_ssavar * ssavar_r = in_r->ssavar;
	assert(ssavar_r->variable == var);
	assert(ssavar_r == ssavar_p);
	
	assert(ssavar_l != ssavar_r);
	assert(l1->inputs[0]->ssavar == ssavar_l);
	assert(l2->inputs[0]->ssavar == ssavar_l);
	assert(r1->inputs[0]->ssavar == ssavar_r);
	assert(r2->inputs[0]->ssavar == ssavar_r);
	
	assert(orig_ssavar->assigned_output == 0 && orig_ssavar->assigned_inputs.first == 0);
	
	jive_shaped_graph_destroy(shaped_graph);
	
	jive_graph_destroy(graph);
	assert(jive_context_is_empty(ctx));
	jive_context_destroy(ctx);
	return 0;
}