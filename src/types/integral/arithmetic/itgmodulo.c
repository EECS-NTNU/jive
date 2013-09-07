/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/integral/arithmetic/itgmodulo.h>
#include <jive/types/integral/itgoperation-classes-private.h>
#include <jive/types/integral/itgtype.h>
#include <jive/vsdg/node-private.h>

static jive_node *
jive_itgmodulo_node_create_(struct jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, struct jive_output * const operands[]);

const jive_itgbinary_operation_class JIVE_ITGMODULO_NODE_ = {
	.base = { /* jive_binary_operation_class */
		.base = { /* jive_node_class */
			.parent = &JIVE_ITGBINARY_NODE,
			.name = "ITGMODULO",
			.fini = jive_node_fini_, /* inherit */
			.get_default_normal_form = jive_binary_operation_get_default_normal_form_, /* inherit */
			.get_label = jive_node_get_label_, /* inherit */
			.get_attrs = jive_node_get_attrs_, /* inherit */
			.match_attrs = jive_node_match_attrs_, /* inherit */
			.check_operands = jive_itgbinary_operation_check_operands_, /* inherit */
			.create = jive_itgmodulo_node_create_, /* override */
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

	.type = jive_itgop_code_modulo
};

static void
jive_itgmodulo_node_init_(jive_itgmodulo_node * self, jive_region * region,
	jive_output * operand1, jive_output * operand2)
{
	JIVE_DECLARE_INTEGRAL_TYPE(itgtype);
	jive_node_init_(&self->base, region,
		2, (const jive_type *[]){itgtype, itgtype}, (jive_output *[]){operand1, operand2},
		1, &itgtype);
}

static jive_node *
jive_itgmodulo_node_create_(jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 2);

	jive_itgmodulo_node * node = jive_context_malloc(region->graph->context, sizeof(*node));
	node->base.class_ = &JIVE_ITGMODULO_NODE;
	jive_itgmodulo_node_init_(node, region, operands[0], operands[1]);

	return &node->base;
}

struct jive_output *
jive_itgmodulo(struct jive_output * operand1, struct jive_output * operand2)
{
	jive_graph * graph = operand1->node->graph;
	return jive_binary_operation_create_normalized(&JIVE_ITGMODULO_NODE_.base, graph, NULL, 2,
		(jive_output *[]){operand1, operand2});
}