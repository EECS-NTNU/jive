/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_ARCH_SIZEOF_H
#define JIVE_ARCH_SIZEOF_H

#include <memory>

#include <jive/vsdg/node.h>
#include <jive/vsdg/valuetype.h>

struct jive_memlayout_mapper;
struct jive_value_type;

namespace jive {

class sizeof_operation final : public operation {
public:
	inline explicit
	sizeof_operation(const jive_value_type & type)
		: type_(type.copy())
	{
	}

	inline
	sizeof_operation(const sizeof_operation & other)
		: type_(other.type().copy())
	{
	}

	inline
	sizeof_operation(sizeof_operation && other) = default;

	inline const jive_value_type & type() const noexcept { return *type_; }

private:
	std::unique_ptr<jive_value_type> type_;
};

}

typedef jive::operation_node<jive::sizeof_operation> jive_sizeof_node;

extern const jive_node_class JIVE_SIZEOF_NODE;

struct jive_node *
jive_sizeof_node_create(struct jive_region * region,
	const struct jive_value_type * type);

struct jive_output *
jive_sizeof_create(struct jive_region * region,
	const struct jive_value_type * type);

JIVE_EXPORTED_INLINE jive_sizeof_node *
jive_sizeof_node_cast(jive_node * node)
{
	if (jive_node_isinstance(node, &JIVE_SIZEOF_NODE))
		return (jive_sizeof_node *) node;
	else
		return NULL;
}

void
jive_sizeof_node_reduce(const jive_sizeof_node * node, struct jive_memlayout_mapper * mapper);

#endif
