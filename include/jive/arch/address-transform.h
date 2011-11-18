#ifndef JIVE_ARCH_ADDRESS_TRANSFORM_H
#define JIVE_ARCH_ADDRESS_TRANSFORM_H

#include <jive/common.h>
#include <jive/vsdg/operators.h>

struct jive_memlayout_mapper;

/* address_to_bitstring node */

extern const jive_unary_operation_class JIVE_ADDRESS_TO_BITSTRING_NODE_;
#define JIVE_ADDRESS_TO_BITSTRING_NODE (JIVE_ADDRESS_TO_BITSTRING_NODE_.base)

typedef struct jive_address_to_bitstring_node jive_address_to_bitstring_node;
typedef struct jive_address_to_bitstring_node_attrs jive_address_to_bitstring_node_attrs;

struct jive_address_to_bitstring_node_attrs {
	jive_node_attrs base;
	size_t nbits;
};

struct jive_address_to_bitstring_node {
	jive_node base;
	jive_address_to_bitstring_node_attrs attrs;
};

jive_node *
jive_address_to_bitstring_node_create(struct jive_region * region,
	jive_output * address, size_t nbits);

jive_output *
jive_address_to_bitstring_create(jive_output * address, size_t nbits);

JIVE_EXPORTED_INLINE jive_address_to_bitstring_node *
jive_address_to_bitstring_node_cast(jive_node * node)
{
	if(jive_node_isinstance(node, &JIVE_ADDRESS_TO_BITSTRING_NODE))
		return (jive_address_to_bitstring_node *) node;
	else
		return 0;
}

/* bitstring_to_address node */

extern const jive_unary_operation_class JIVE_BITSTRING_TO_ADDRESS_NODE_;
#define JIVE_BITSTRING_TO_ADDRESS_NODE (JIVE_BITSTRING_TO_ADDRESS_NODE_.base)

typedef struct jive_bitstring_to_address_node jive_bitstring_to_address_node;
typedef struct jive_bitstring_to_address_node_attrs jive_bitstring_to_address_node_attrs;

struct jive_bitstring_to_address_node_attrs {
	jive_node_attrs base;
	size_t nbits;
};

struct jive_bitstring_to_address_node {
	jive_node base;
	jive_bitstring_to_address_node_attrs attrs;
};

jive_node *
jive_bitstring_to_address_node_create(struct jive_region * region,
	jive_output * bitstring, size_t nbits);

jive_output *
jive_bitstring_to_address_create(jive_output * bitstring, size_t nbits);

JIVE_EXPORTED_INLINE jive_bitstring_to_address_node *
jive_bitstring_to_address_node_cast(jive_node * node)
{
	if(jive_node_isinstance(node, &JIVE_BITSTRING_TO_ADDRESS_NODE))
		return (jive_bitstring_to_address_node *) node;
	else
		return 0;
}

#endif