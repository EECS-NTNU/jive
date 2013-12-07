/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_DOUBLE_COMPARISON_DBLLESS_H
#define JIVE_TYPES_DOUBLE_COMPARISON_DBLLESS_H

#include <jive/types/double/dbloperation-classes.h>

typedef struct jive_dblless_node jive_dblless_node;

extern const jive_dblcomparison_operation_class JIVE_DBLLESS_NODE_;
#define JIVE_DBLLESS_NODE (JIVE_DBLLESS_NODE_.base.base)

struct jive_dblless_node {
	jive_node base;
};

struct jive_output *
jive_dblless(struct jive_output * operand1, struct jive_output * operand2);

JIVE_EXPORTED_INLINE struct jive_dblless_node *
jive_dblless_node_cast(struct jive_node * node)
{
	if (jive_node_isinstance(node, &JIVE_DBLLESS_NODE))
		return (jive_dblless_node *) node;
	else
		return NULL;
}

JIVE_EXPORTED_INLINE const struct jive_dblless_node *
jive_dblless_node_const_cast(const struct jive_node * node)
{
	if (jive_node_isinstance(node, &JIVE_DBLLESS_NODE))
		return (const jive_dblless_node *) node;
	else
		return NULL;
}

#endif
