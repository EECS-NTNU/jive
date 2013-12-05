/*
 * Copyright 2011 2012 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_BITSTRING_COMPARISON_BITNOTEQUAL_H
#define JIVE_TYPES_BITSTRING_COMPARISON_BITNOTEQUAL_H

#include <jive/types/bitstring/bitoperation-classes.h>

extern const jive_bitcomparison_operation_class JIVE_BITNOTEQUAL_NODE_;
#define JIVE_BITNOTEQUAL_NODE (JIVE_BITNOTEQUAL_NODE_.base.base)

jive_output *
jive_bitnotequal(struct jive_output * operand1, struct jive_output * operand2);

JIVE_EXPORTED_INLINE jive_node *
jive_bitnotequal_node_cast(jive_node * node)
{
	if (jive_node_isinstance(node, &JIVE_BITNOTEQUAL_NODE))
		return node;
	else
		return NULL;
}

#endif
