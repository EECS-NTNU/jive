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
		jive_ssavar * ssavar = jive_input_auto_merge_variable(node->inputs[n]);
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
	JIVE_DECLARE_CONTROL_TYPE(ctl);
	JIVE_DECLARE_ANCHOR_TYPE(anchor_type);
	
	jive_node * dummy = jive_node_create(graph->root_region,
		0, NULL, NULL,
		1, (const jive_type *[]){type});
	
	jive_region * r1 = jive_region_create_subregion(graph->root_region);
	
	jive_node * top = jive_node_create(r1,
		1, (const jive_type *[]){type}, dummy->outputs,
		2, (const jive_type *[]){type, type});
	
	jive_node * theta_node = jive_theta_create(r1,
		1, (const jive_type *[]){type}, (jive_output *[]){top->outputs[0]});
	
	
	jive_region * loop_region = theta_node->inputs[0]->origin->node->region;
	jive_node * theta_head = loop_region->top;
	jive_node * theta_tail = loop_region->bottom;
	jive_node * theta_op = jive_node_create(loop_region,
		2, (const jive_type *[]){type, type}, (jive_output *[]){theta_head->outputs[1], top->outputs[1]},
		2, (const jive_type *[]){ctl, type});
	jive_input_divert_origin(theta_tail->inputs[0], theta_op->outputs[0]);
	jive_input_divert_origin(theta_tail->inputs[1], theta_op->outputs[1]);
	
	jive_node * bottom = jive_node_create(r1,
		2, (const jive_type *[]){type, type}, (jive_output *[]){theta_node->outputs[0], top->outputs[1]},
		1, (const jive_type *[]){anchor_type});
	
	jive_node * subroutine_anchor = jive_node_create(graph->root_region,
		1, (const jive_type *[]){anchor_type}, bottom->outputs,
		0, NULL);
	
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
	assert(theta_tail->ninputs == 3);
	assert(theta_node->noutputs == 2);
	
	jive_gate * gate = theta_head->inputs[1]->gate;
	assert(gate);
	assert(theta_head->inputs[1]->gate == gate && theta_head->outputs[2]->gate == gate);
	assert(theta_tail->inputs[2]->gate == gate);
	assert(theta_node->outputs[1]->gate == gate);
	
	assert(theta_head->inputs[1]->origin == top->outputs[1]);
	assert(theta_tail->inputs[2]->origin == theta_head->outputs[2]);
	assert(bottom->inputs[1]->origin == theta_node->outputs[1]);
	
	assert(top->outputs[1]->ssavar == orig_ssavar);
	assert(theta_head->inputs[1]->ssavar == orig_ssavar);
	assert(theta_head->outputs[2]->ssavar == ssavar_p);
	assert(theta_tail->inputs[2]->ssavar == ssavar_p);
	assert(theta_op->inputs[1]->origin == theta_head->outputs[2]);
	assert(theta_op->inputs[1]->ssavar == ssavar_p);
	
	jive_ssavar * ssavar_below = bottom->inputs[1]->ssavar;
	assert(theta_node->outputs[1]->ssavar == ssavar_below);
	assert(bottom->inputs[1]->ssavar == ssavar_below);
	
	jive_view(graph, stdout);
	
	jive_shaped_graph_destroy(shaped_graph);
	
	jive_graph_destroy(graph);
	assert(jive_context_is_empty(ctx));
	jive_context_destroy(ctx);
	return 0;
}