/*
 * Copyright 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TESTS_TESTNODES_H
#define JIVE_TESTS_TESTNODES_H

#include <jive/vsdg/node.h>

/* test node */

extern const jive_node_class JIVE_TEST_NODE;

typedef struct jive_test_node jive_test_node;
typedef struct jive_test_node_attrs jive_test_node_attrs;

struct jive_test_node_attrs {
	jive_node_attrs base;
	size_t noperands;
	size_t nresults;
	const jive_type ** operand_types;
	const jive_type ** result_types;
};

struct jive_test_node {
	jive_node base;
	jive_test_node_attrs attrs;
};

jive_node *
jive_test_node_create(struct jive_region * region,
	size_t noperands, const struct jive_type * const operand_types[],
	struct jive_output * const operands[], size_t nresults,
	const struct jive_type * const result_types[]);

void
jive_test_node_create_normalized(jive_graph * graph, size_t noperands,
	const struct jive_type * const operand_types[], struct jive_output * const operands[],
	size_t nresults, const struct jive_type * const result_types[], jive_output * results[]);

JIVE_EXPORTED_INLINE const jive_test_node *
jive_test_node_const_cast(const jive_node * self)
{
	if (jive_node_isinstance(self, &JIVE_TEST_NODE))
		return (const jive_test_node *)self;
	else
		return NULL;
}

JIVE_EXPORTED_INLINE jive_test_node *
jive_test_node_cast(jive_node * self)
{
	if (jive_node_isinstance(self, &JIVE_TEST_NODE))
		return (jive_test_node *)self;
	else
		return NULL;
}

#endif
