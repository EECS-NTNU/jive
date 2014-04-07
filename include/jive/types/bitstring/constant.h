/*
 * Copyright 2013 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_BITSTRING_CONSTANT_H
#define JIVE_TYPES_BITSTRING_CONSTANT_H

#include <stdint.h>

#include <jive/types/bitstring/type.h>
#include <jive/util/bitstring.h>
#include <jive/vsdg/node.h>

extern const jive_node_class JIVE_BITCONSTANT_NODE;

typedef struct jive_bitconstant_node jive_bitconstant_node;
typedef struct jive_bitconstant_node_attrs jive_bitconstant_node_attrs;

struct jive_bitconstant_node_attrs : public jive_node_attrs {
	size_t nbits;
	char * bits;
};

struct jive_bitconstant_node {
	jive_node base;
	jive_bitconstant_node_attrs attrs;
};

/**
	\brief Create bitconstant
	\param graph Graph to create constant in
	\param nbits Number of bits
	\param bits Values of bits
	\returns Bitstring value representing constant
	
	Convenience function that either creates a new constant or
	returns the output handle of an existing constant.
*/
jive_output *
jive_bitconstant(struct jive_graph * graph, size_t nbits, const char bits[]);

jive_output *
jive_bitconstant_unsigned(struct jive_graph * graph, size_t nbits, uint64_t value);

jive_output *
jive_bitconstant_signed(struct jive_graph * graph, size_t nbits, int64_t value);

JIVE_EXPORTED_INLINE struct jive_output *
jive_bitconstant_undefined(struct jive_graph * graph, size_t nbits)
{
	size_t i;
	char bits[nbits];
	for (i = 0; i < nbits; i++)
		bits[i] = 'X';

	return jive_bitconstant(graph, nbits, bits);
}

JIVE_EXPORTED_INLINE struct jive_output *
jive_bitconstant_defined(struct jive_graph * graph, size_t nbits)
{
	size_t i;
	char bits[nbits];
	for (i = 0; i < nbits; i++)
		bits[i] = 'D';

	return jive_bitconstant(graph, nbits, bits);
}

JIVE_EXPORTED_INLINE jive_bitconstant_node *
jive_bitconstant_node_cast(jive_node * node)
{
	if (node->class_ == &JIVE_BITCONSTANT_NODE) return (jive_bitconstant_node *) node;
	else return 0;
}

JIVE_EXPORTED_INLINE bool
jive_bitconstant_is_zero(const jive_bitconstant_node * node)
{
	size_t n;
	for(n=0; n<node->attrs.nbits; n++) if (node->attrs.bits[n] != '0') return false;
	return true;
}

JIVE_EXPORTED_INLINE bool
jive_bitconstant_is_one(const jive_bitconstant_node * node)
{
	size_t n;
	for(n=1; n<node->attrs.nbits; n++) if (node->attrs.bits[n] != '0') return false;
	return node->attrs.bits[0] == '1';
}

JIVE_EXPORTED_INLINE bool
jive_bitconstant_is_minus_one(const jive_bitconstant_node * node)
{
	size_t n;
	for(n=0; n<node->attrs.nbits; n++) if (node->attrs.bits[n] != '1') return false;
	return true;
}

JIVE_EXPORTED_INLINE bool
jive_bitconstant_equals_signed(const jive_bitconstant_node * node, int64_t value)
{
	return jive_bitstring_equals_signed(node->attrs.bits, node->attrs.nbits, value);
}

JIVE_EXPORTED_INLINE bool
jive_bitconstant_equals_unsigned(const jive_bitconstant_node * node, uint64_t value)
{
	return jive_bitstring_equals_unsigned(node->attrs.bits, node->attrs.nbits, value);
}

JIVE_EXPORTED_INLINE uint64_t
jive_bitconstant_node_to_unsigned(const jive_bitconstant_node * node)
{
	return jive_bitstring_to_unsigned(node->attrs.bits, node->attrs.nbits);
}

JIVE_EXPORTED_INLINE int64_t
jive_bitconstant_node_to_signed(const jive_bitconstant_node * node)
{
	return jive_bitstring_to_signed(node->attrs.bits, node->attrs.nbits);
}

#endif
