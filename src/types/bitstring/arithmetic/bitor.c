/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/bitstring/arithmetic/bitor.h>
#include <jive/types/bitstring/bitoperation-classes-private.h>
#include <jive/types/bitstring/constant.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/region.h>

static jive_node *
jive_bitor_create_(struct jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, struct jive_output * const operands[]);

static jive_binop_reduction_path_t
jive_bitor_node_can_reduce_operand_pair_(const jive_node_class * cls,
	const jive_node_attrs * attrs, const jive_output * op1, const jive_output * op2);

static jive_output *
jive_bitor_node_reduce_operand_pair_(jive_binop_reduction_path_t path, const jive_node_class * cls,
	const jive_node_attrs * attrs, jive_output * op1, jive_output * op2);

const jive_bitbinary_operation_class JIVE_BITOR_NODE_ = {
	base : { /* jive_bitbinary_operation_class */
		base : { /* jive_node_class */
			parent : &JIVE_BITBINARY_NODE,
			name : "BITOR",
			fini : jive_node_fini_, /* inherit */
			get_default_normal_form : jive_binary_operation_get_default_normal_form_, /* inherit */
			get_label : jive_node_get_label_, /* inherit */
			match_attrs : jive_node_match_attrs_, /* inherit */
			check_operands : jive_bitbinary_operation_check_operands_, /* inherit */
			create : jive_bitor_create_, /* override */
		},

		flags : jive_binary_operation_associative | jive_binary_operation_commutative,
		single_apply_under : NULL,
		multi_apply_under : NULL,
		distributive_over : NULL,
		distributive_under : NULL,

		can_reduce_operand_pair : jive_bitor_node_can_reduce_operand_pair_, /* override */
		reduce_operand_pair : jive_bitor_node_reduce_operand_pair_ /* override */
	},
	type : jive_bitop_code_or
};

static void
jive_bitor_node_init_(jive_node * self, jive_region * region,
	size_t noperands, jive_output * const operands[])
{
	size_t nbits = jive_bitstring_output_nbits((jive_bitstring_output *)operands[0]);
	
	size_t n;
	const jive_type * operand_types[noperands];
	jive_bitstring_type output_type(nbits);
	for(n = 0; n < noperands; n++)
		operand_types[n] = &output_type;

	const jive_type * type_array[] = {&output_type};
	jive_node_init_(self, region,
		noperands, operand_types, operands,
		1, type_array);
}

static jive_node *
jive_bitor_create_(jive_region * region, const jive_node_attrs * attrs,
	size_t noperands, jive_output * const operands[])
{
	jive_node * node = jive::create_operation_node(jive::bitstring::or_operation());
	node->class_ = &JIVE_BITOR_NODE;
	jive_bitor_node_init_(node, region, noperands, operands);

	return node;
}

static jive_binop_reduction_path_t
jive_bitor_node_can_reduce_operand_pair_(const jive_node_class * cls,
	const jive_node_attrs * attrs, const jive_output * op1, const jive_output * op2)
{
	if (jive_node_isinstance(op1->node, &JIVE_BITCONSTANT_NODE) &&
		jive_node_isinstance(op2->node, &JIVE_BITCONSTANT_NODE))
		return jive_binop_reduction_constants;

	return jive_binop_reduction_none;
}

static jive_output *
jive_bitor_node_reduce_operand_pair_(jive_binop_reduction_path_t path, const jive_node_class * cls,
	const jive_node_attrs * attrs, jive_output * op1, jive_output * op2)
{
	jive_graph * graph = (op1)->node->graph;

	if (path == jive_binop_reduction_constants) {
		jive_bitconstant_node * n1 = dynamic_cast<jive_bitconstant_node *>((op1)->node);
		jive_bitconstant_node * n2 = dynamic_cast<jive_bitconstant_node *>((op2)->node);

		size_t nbits = n1->operation().bits.size();
		char bits[nbits];
		jive_bitstring_or(bits, &n1->operation().bits[0], &n2->operation().bits[0], nbits);

		return jive_bitconstant(graph, nbits, bits);
	}

	return NULL;
}

jive_output *
jive_bitor(size_t noperands, jive_output * const * operands)
{
	JIVE_DEBUG_ASSERT(noperands != 0);

	jive_graph * graph = operands[0]->node->graph;
	jive::bitstring::or_operation op;
	return jive_binary_operation_create_normalized(&JIVE_BITOR_NODE_.base, graph, &op, noperands,
		operands);
}
