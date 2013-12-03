/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/real/comparison/rlgreater.h>
#include <jive/types/real/rltype.h>
#include <jive/vsdg/controltype.h>
#include <jive/vsdg/node-private.h>

static jive_node *
jive_rlgreater_node_create_(struct jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, struct jive_output * const operands[]);

const jive_rlcomparison_operation_class JIVE_RLGREATER_NODE_ = {
	.base = { /* jive_binary_operation_class */
		.base = { /* jive_node_class */
			.parent = &JIVE_RLCOMPARISON_NODE,
			.name = "RLGREATER",
			.fini = jive_node_fini_, /* inherit */
			.get_default_normal_form = jive_binary_operation_get_default_normal_form_, /* inherit */
			.get_label = jive_node_get_label_, /* inherit */
			.get_attrs = jive_node_get_attrs_, /* inherit */
			.match_attrs = jive_node_match_attrs_, /* inherit */
			.create = jive_rlgreater_node_create_, /* override */
			.get_aux_rescls = jive_node_get_aux_rescls_ /* inherit */
		},

		.flags = jive_binary_operation_none,
		.single_apply_under = NULL,
		.multi_apply_under = NULL,
		.distributive_over = NULL,
		.distributive_under = NULL,

		.can_reduce_operand_pair = jive_binary_operation_can_reduce_operand_pair_, /* inherit */
		.reduce_operand_pair = jive_binary_operation_reduce_operand_pair_ /* inherit */
	},

	.type = jive_rlcmp_code_greater
};

static void
jive_rlgreater_node_init_(struct jive_rlgreater_node * self, struct jive_region * region,
	struct jive_output * op1, struct jive_output * op2)
{
	JIVE_DECLARE_CONTROL_TYPE(ctype);
	JIVE_DECLARE_REAL_TYPE(rltype);
	jive_node_init_(&self->base, region,
		2, (const jive_type *[]){rltype, rltype}, (jive_output *[]){op1, op2},
		1, &ctype);
}

static jive_node *
jive_rlgreater_node_create_(struct jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, struct jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 2);

	jive_rlgreater_node * node = jive_context_malloc(region->graph->context, sizeof(*node));
	node->base.class_ = &JIVE_RLGREATER_NODE;
	jive_rlgreater_node_init_(node, region, operands[0], operands[1]);

	return &node->base;
}

struct jive_output *
jive_rlgreater(jive_output * op1, jive_output * op2)
{
	jive_output * operands[] = {op1, op2};
	jive_region * region = jive_region_innermost(2, operands);
	const jive_binary_operation_normal_form * nf = (const jive_binary_operation_normal_form *)
		jive_graph_get_nodeclass_form(region->graph, &JIVE_RLGREATER_NODE);

	return jive_binary_operation_normalized_create_new(nf, region, NULL, 2, operands);
}