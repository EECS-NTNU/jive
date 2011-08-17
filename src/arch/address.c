#include <jive/arch/address.h>

#include <stdio.h>
#include <string.h>

#include <jive/context.h>
#include <jive/bitstring/arithmetic.h>
#include <jive/bitstring/constant.h>
#include <jive/bitstring/type.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node-private.h>
#include <jive/vsdg/region.h>

/* memberof */

static char *
jive_memberof_node_get_label_(const jive_node * self_);

static const jive_node_attrs *
jive_memberof_node_get_attrs_(const jive_node * self_);

static bool
jive_memberof_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_);

static jive_node *
jive_memberof_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[]);

static bool
jive_memberof_can_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand);

static bool
jive_memberof_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand_);

const jive_unary_operation_class JIVE_MEMBEROF_NODE_ = {
	.base = {
		.parent = &JIVE_UNARY_OPERATION,
		.name = "MEMBEROF",
		.fini = _jive_node_fini, /* inherit */
		.get_label = jive_memberof_node_get_label_, /* override */
		.get_attrs = jive_memberof_node_get_attrs_, /* override */
		.match_attrs = jive_memberof_node_match_attrs_, /* override */
		.create = jive_memberof_node_create_, /* override */
		.get_aux_rescls = _jive_node_get_aux_rescls /* inherit */
	},
	
	.single_apply_over = NULL,
	.multi_apply_over = NULL,
	
	.can_reduce_operand = jive_memberof_can_reduce_operand_, /* override */
	.reduce_operand = jive_memberof_reduce_operand_ /* override */
};

static char *
jive_memberof_node_get_label_(const jive_node * self_)
{
	const jive_memberof_node * self = (const jive_memberof_node *) self_;
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "MEMBEROF %p:%zd", self->attrs.record_layout, self->attrs.index);
	return strdup(tmp);
}

static const jive_node_attrs *
jive_memberof_node_get_attrs_(const jive_node * self_)
{
	const jive_memberof_node * self = (const jive_memberof_node *) self_;
	return &self->attrs.base;
}

static bool
jive_memberof_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_)
{
	const jive_memberof_node * self = (const jive_memberof_node *) self_;
	const jive_memberof_node_attrs * attrs = (const jive_memberof_node_attrs *) attrs_;
	return (self->attrs.record_layout == attrs->record_layout) && (self->attrs.index == attrs->index);
}

static jive_node *
jive_memberof_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 1);
	const jive_memberof_node_attrs * attrs = (const jive_memberof_node_attrs *) attrs_;
	return jive_memberof_node_create(region, operands[0], attrs->record_layout, attrs->index);
}

static bool
jive_memberof_can_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand)
{
	const jive_memberof_node_attrs * attrs = (const jive_memberof_node_attrs *) attrs_;
	
	const jive_containerof_node * node = jive_containerof_node_cast(operand->node);
	if (!node)
		return false;
	
	if (node->attrs.record_layout == attrs->record_layout && node->attrs.index == attrs->index)
		return true;
	
	return false;
}

static bool
jive_memberof_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand)
{
	const jive_memberof_node_attrs * attrs = (const jive_memberof_node_attrs *) attrs_;
	
	const jive_containerof_node * node = jive_containerof_node_cast((*operand)->node);
	if (!node)
		return false;
	
	if (node->attrs.record_layout == attrs->record_layout && node->attrs.index == attrs->index) {
		*operand = node->base.inputs[0]->origin;
		return true;
	}
	
	return false;
}

jive_node *
jive_memberof_node_create(jive_region * region,
	jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_context * context = region->graph->context;
	jive_memberof_node * node = jive_context_malloc(context, sizeof(*node));
	
	node->base.class_ = &JIVE_MEMBEROF_NODE;
	
	JIVE_DECLARE_ADDRESS_TYPE(address_type);
	
	_jive_node_init(&node->base, region,
		1, &address_type, &address,
		1, &address_type);
	node->attrs.record_layout = record_layout;
	node->attrs.index = index;
	
	return &node->base;
}

jive_node *
jive_memberof_create(jive_region * region,
	jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_memberof_node_attrs attrs;
	attrs.record_layout = record_layout;
	attrs.index = index;
	
	return jive_unary_operation_normalized_create(&JIVE_MEMBEROF_NODE, region, &attrs.base, address)->node;
}

jive_output *
jive_memberof(jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_memberof_node_attrs attrs;
	attrs.record_layout = record_layout;
	attrs.index = index;
	
	return jive_unary_operation_normalized_create(&JIVE_MEMBEROF_NODE, address->node->region, &attrs.base, address);
}

/* containerof */

static char *
jive_containerof_node_get_label_(const jive_node * self_);

static const jive_node_attrs *
jive_containerof_node_get_attrs_(const jive_node * self_);

static bool
jive_containerof_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_);

static jive_node *
jive_containerof_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[]);

static bool
jive_containerof_can_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand);

static bool
jive_containerof_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand_);

const jive_unary_operation_class JIVE_CONTAINEROF_NODE_ = {
	.base = {
		.parent = &JIVE_UNARY_OPERATION,
		.name = "CONTAINEROF",
		.fini = _jive_node_fini, /* inherit */
		.get_label = jive_containerof_node_get_label_, /* override */
		.get_attrs = jive_containerof_node_get_attrs_, /* override */
		.match_attrs = jive_containerof_node_match_attrs_, /* override */
		.create = jive_containerof_node_create_, /* override */
		.get_aux_rescls = _jive_node_get_aux_rescls /* inherit */
	},
	
	.single_apply_over = NULL,
	.multi_apply_over = NULL,
	
	.can_reduce_operand = jive_containerof_can_reduce_operand_, /* override */
	.reduce_operand = jive_containerof_reduce_operand_ /* override */
};

static char *
jive_containerof_node_get_label_(const jive_node * self_)
{
	const jive_containerof_node * self = (const jive_containerof_node *) self_;
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "CONTAINEROF %p:%zd", self->attrs.record_layout, self->attrs.index);
	return strdup(tmp);
}

static const jive_node_attrs *
jive_containerof_node_get_attrs_(const jive_node * self_)
{
	const jive_containerof_node * self = (const jive_containerof_node *) self_;
	return &self->attrs.base;
}

static bool
jive_containerof_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_)
{
	const jive_containerof_node * self = (const jive_containerof_node *) self_;
	const jive_containerof_node_attrs * attrs = (const jive_containerof_node_attrs *) attrs_;
	return (self->attrs.record_layout == attrs->record_layout) && (self->attrs.index == attrs->index);
}

static jive_node *
jive_containerof_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 1);
	const jive_containerof_node_attrs * attrs = (const jive_containerof_node_attrs *) attrs_;
	return jive_containerof_node_create(region, operands[0], attrs->record_layout, attrs->index);
}

static bool
jive_containerof_can_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand)
{
	const jive_containerof_node_attrs * attrs = (const jive_containerof_node_attrs *) attrs_;
	
	const jive_memberof_node * node = jive_memberof_node_cast(operand->node);
	if (!node)
		return false;
	
	if (node->attrs.record_layout == attrs->record_layout && node->attrs.index == attrs->index)
		return true;
	
	return false;
}

static bool
jive_containerof_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand)
{
	const jive_containerof_node_attrs * attrs = (const jive_containerof_node_attrs *) attrs_;
	
	const jive_memberof_node * node = jive_memberof_node_cast((*operand)->node);
	if (!node)
		return false;
	
	if (node->attrs.record_layout == attrs->record_layout && node->attrs.index == attrs->index) {
		*operand = node->base.inputs[0]->origin;
		return true;
	}
	
	return false;
}

jive_node *
jive_containerof_node_create(jive_region * region,
	jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_context * context = region->graph->context;
	jive_containerof_node * node = jive_context_malloc(context, sizeof(*node));
	
	node->base.class_ = &JIVE_CONTAINEROF_NODE;
	
	JIVE_DECLARE_ADDRESS_TYPE(address_type);
	
	_jive_node_init(&node->base, region,
		1, &address_type, &address,
		1, &address_type);
	node->attrs.record_layout = record_layout;
	node->attrs.index = index;
	
	return &node->base;
}

jive_node *
jive_containerof_create(jive_region * region,
	jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_containerof_node_attrs attrs;
	attrs.record_layout = record_layout;
	attrs.index = index;
	
	return jive_unary_operation_normalized_create(&JIVE_CONTAINEROF_NODE, region, &attrs.base, address)->node;
}

jive_output *
jive_containerof(jive_output * address,
	const jive_record_layout * record_layout, size_t index)
{
	jive_containerof_node_attrs attrs;
	attrs.record_layout = record_layout;
	attrs.index = index;
	
	return jive_unary_operation_normalized_create(&JIVE_CONTAINEROF_NODE, address->node->region, &attrs.base, address);
}

/* arraysubscript */

static void
jive_arraysubscript_node_fini_(jive_node * self_);

static jive_node *
jive_arraysubscript_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[]);

static const jive_node_attrs *
jive_arraysubscript_node_get_attrs_(const jive_node * self_);

static bool
jive_arraysubscript_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_);

static bool
jive_arraysubscript_can_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand1, jive_output * operand2);

static bool
jive_arraysubscript_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand1, jive_output ** operand2);

const jive_binary_operation_class JIVE_ARRAYSUBSCRIPT_NODE_ = {
	.base = {
		.parent = &JIVE_BINARY_OPERATION,
		.name = "ARRAYSUBSCRIPT",
		.fini = jive_arraysubscript_node_fini_, /* override */
		.get_label = _jive_node_get_label, /* inherit */
		.get_attrs = jive_arraysubscript_node_get_attrs_, /* override */
		.match_attrs = jive_arraysubscript_node_match_attrs_, /* override */
		.create = jive_arraysubscript_node_create_, /* override */
		.get_aux_rescls = _jive_node_get_aux_rescls /* inherit */
	},
	
	.flags = jive_binary_operation_none,
	.single_apply_under = NULL,
	.multi_apply_under = NULL,
	.distributive_over = NULL,
	.distributive_under = NULL,
	
	.can_reduce_operand_pair = jive_arraysubscript_can_reduce_operand_pair_, /* override */
	.reduce_operand_pair = jive_arraysubscript_reduce_operand_pair_ /* override */
};

static void
jive_arraysubscript_node_fini_(jive_node * self_)
{
	jive_arraysubscript_node * self = (jive_arraysubscript_node *) self_;
	jive_type_fini(self->attrs.element_type);
	jive_context_free(self->base.graph->context, self->attrs.element_type);
	_jive_node_fini(&self->base);
}

static const jive_node_attrs *
jive_arraysubscript_node_get_attrs_(const jive_node * self_)
{
	const jive_arraysubscript_node * self = (const jive_arraysubscript_node *) self_;
	return &self->attrs.base;
}

static bool
jive_arraysubscript_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_)
{
	const jive_arraysubscript_node * self = (const jive_arraysubscript_node *) self_;
	const jive_arraysubscript_node_attrs * attrs = (const jive_arraysubscript_node_attrs *) attrs_;
	
	return jive_type_equals(self->attrs.element_type, attrs->element_type);
}

static jive_node *
jive_arraysubscript_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 2);
	const jive_arraysubscript_node_attrs * attrs = (const jive_arraysubscript_node_attrs *) attrs_;
	return jive_arraysubscript_node_create(region, operands[0], attrs->element_type, operands[1]);
}

static bool
jive_arraysubscript_can_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand1, jive_output * operand2)
{
	jive_bitconstant_node * offset_constant = jive_bitconstant_node_cast(operand2->node);
	if (offset_constant && jive_bitconstant_is_zero(offset_constant))
		return true;
	
	const jive_arraysubscript_node * node = jive_arraysubscript_node_cast(operand1->node);
	if (node && jive_type_equals(jive_output_get_type(operand2), jive_input_get_type(node->base.inputs[1])))
		return true;
	
	return false;
}

static bool
jive_arraysubscript_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand1, jive_output ** operand2)
{
	jive_bitconstant_node * offset_constant = jive_bitconstant_node_cast((*operand2)->node);
	if (offset_constant && jive_bitconstant_is_zero(offset_constant)) {
		return true;
	}
	
	const jive_arraysubscript_node * node = jive_arraysubscript_node_cast((*operand1)->node);
	if (node && jive_type_equals(jive_output_get_type(*operand2), jive_input_get_type(node->base.inputs[1]))) {
		jive_output * operands[2] = {node->base.inputs[1]->origin, *operand2};
		jive_output * sum = jive_bitsum(2, operands);
		
		operands[0] = node->base.inputs[0]->origin;
		operands[1] = sum;
		jive_region * region = operands[0]->node->region;
		if (operands[1]->node->region->depth > region->depth)
			region = operands[1]->node->region;
		*operand1 = jive_binary_operation_normalized_create(&JIVE_ARRAYSUBSCRIPT_NODE, region, NULL, 2, operands);
		return true;
	}
	
	return false;
}

jive_node *
jive_arraysubscript_node_create(jive_region * region,
	jive_output * address, const jive_type * element_type, jive_output * index)
{
	jive_context * context = region->graph->context;
	jive_arraysubscript_node * node = jive_context_malloc(context, sizeof(*node));
	
	node->base.class_ = &JIVE_ARRAYSUBSCRIPT_NODE;
	
	JIVE_DECLARE_ADDRESS_TYPE(address_type);
	const jive_type * index_type = jive_output_get_type(index);
	JIVE_DEBUG_ASSERT(index_type->class_ == &JIVE_BITSTRING_TYPE);
	
	const jive_type * operand_types[2] = {address_type, index_type};
	jive_output * operands[2] = {address, index};
	
	_jive_node_init(&node->base, region,
		2, operand_types, operands,
		1, &address_type);
	node->attrs.element_type = jive_type_copy(element_type, context);
	
	return &node->base;
}

jive_node *
jive_arraysubscript_create(jive_region * region,
	jive_output * address, const jive_type * element_type,jive_output * index)
{
	return jive_arraysubscript_node_create(region, address, element_type, index);
}

jive_output *
jive_arraysubscript(jive_output * address, const jive_type * element_type, jive_output * index)
{
	jive_region * region = address->node->region;
	if (index->node->region->depth > region->depth)
		region = index->node->region;
	
	jive_arraysubscript_node_attrs attrs;
	attrs.element_type = (jive_type *) element_type;
	
	jive_output * operands[2] = {address, index};
	return jive_binary_operation_normalized_create(&JIVE_ARRAYSUBSCRIPT_NODE, region, &attrs.base, 2, operands);
}

/* arrayindex */

static void
jive_arrayindex_node_fini_(jive_node * self_);

static jive_node *
jive_arrayindex_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[]);

static const jive_node_attrs *
jive_arrayindex_node_get_attrs_(const jive_node * self_);

static bool
jive_arrayindex_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_);

static bool
jive_arrayindex_can_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand1, jive_output * operand2);

static bool
jive_arrayindex_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand1, jive_output ** operand2);

const jive_binary_operation_class JIVE_ARRAYINDEX_NODE_ = {
	.base = {
		.parent = &JIVE_BINARY_OPERATION,
		.name = "ARRAYINDEX",
		.fini = jive_arrayindex_node_fini_, /* override */
		.get_label = _jive_node_get_label, /* inherit */
		.get_attrs = jive_arrayindex_node_get_attrs_, /* override */
		.match_attrs = jive_arrayindex_node_match_attrs_, /* override */
		.create = jive_arrayindex_node_create_, /* override */
		.get_aux_rescls = _jive_node_get_aux_rescls /* inherit */
	},
	
	.flags = jive_binary_operation_none,
	.single_apply_under = NULL,
	.multi_apply_under = NULL,
	.distributive_over = NULL,
	.distributive_under = NULL,
	
	.can_reduce_operand_pair = jive_arrayindex_can_reduce_operand_pair_, /* override */
	.reduce_operand_pair = jive_arrayindex_reduce_operand_pair_ /* override */
};

static void
jive_arrayindex_node_fini_(jive_node * self_)
{
	jive_arrayindex_node * self = (jive_arrayindex_node *) self_;
	jive_type_fini(self->attrs.element_type);
	jive_context_free(self->base.graph->context, self->attrs.element_type);
	_jive_node_fini(&self->base);
}

static const jive_node_attrs *
jive_arrayindex_node_get_attrs_(const jive_node * self_)
{
	const jive_arrayindex_node * self = (const jive_arrayindex_node *) self_;
	return &self->attrs.base;
}

static bool
jive_arrayindex_node_match_attrs_(const jive_node * self_, const jive_node_attrs * attrs_)
{
	const jive_arrayindex_node * self = (const jive_arrayindex_node *) self_;
	const jive_arrayindex_node_attrs * attrs = (const jive_arrayindex_node_attrs *) attrs_;
	
	return jive_type_equals(self->attrs.element_type, attrs->element_type);
}

static jive_node *
jive_arrayindex_node_create_(jive_region * region, const jive_node_attrs * attrs_,
	size_t noperands, jive_output * const operands[])
{
	JIVE_DEBUG_ASSERT(noperands == 2);
	const jive_arrayindex_node_attrs * attrs = (const jive_arrayindex_node_attrs *) attrs_;
	return jive_arrayindex_node_create(region, operands[0], operands[1], attrs->element_type, &attrs->difference_type.base.base);
}

static jive_output *
get_array_base(jive_output * addr, const jive_type * element_type)
{
	jive_arraysubscript_node * node = jive_arraysubscript_node_cast(addr->node);
	if (node && jive_type_equals(element_type, node->attrs.element_type))
		return addr->node->inputs[0]->origin;
	else return addr;
}

static jive_output *
get_array_index(jive_output * addr, jive_output * base, const jive_type * element_type, const jive_bitstring_type * index_type)
{
	jive_output * index = NULL;
	jive_arraysubscript_node * node = jive_arraysubscript_node_cast(addr->node);
	if (node && jive_type_equals(element_type, node->attrs.element_type)) {
		/* FIXME: correct type! */
		index = addr->node->inputs[1]->origin;
	} else {
		char bits[index_type->nbits];
		memset(bits, '0', index_type->nbits);
		index = jive_bitconstant(addr->node->graph, index_type->nbits, bits);
	}
	
	return index;
}

static bool
jive_arrayindex_can_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output * operand1, jive_output * operand2)
{
	const jive_arrayindex_node_attrs * attrs = (const jive_arrayindex_node_attrs *) attrs_;
	jive_output * base1 = get_array_base(operand1, attrs->element_type);
	jive_output * base2 = get_array_base(operand2, attrs->element_type);
	
	return base1 == base2;
}

static bool
jive_arrayindex_reduce_operand_pair_(const jive_node_class * cls, const jive_node_attrs * attrs_, jive_output ** operand1, jive_output ** operand2)
{
	const jive_arrayindex_node_attrs * attrs = (const jive_arrayindex_node_attrs *) attrs_;
	jive_output * base1 = get_array_base(*operand1, attrs->element_type);
	jive_output * base2 = get_array_base(*operand2, attrs->element_type);
	
	if (base1 != base2)
		return false;
	
	jive_output * index1 = get_array_index(*operand1, base1, attrs->element_type, &attrs->difference_type);
	jive_output * index2 = get_array_index(*operand2, base2, attrs->element_type, &attrs->difference_type);
	
	index2 = jive_bitnegate(index2);
	jive_output * operands[2] = {index1, index2};
	*operand1 = jive_bitsum(2, operands);
	
	return true;
}

jive_node *
jive_arrayindex_node_create(jive_region * region,
	jive_output * addr1, jive_output * addr2,
	const jive_type * element_type, const jive_type * difference_type)
{
	jive_context * context = region->graph->context;
	jive_arrayindex_node * node = jive_context_malloc(context, sizeof(*node));
	
	node->base.class_ = &JIVE_ARRAYINDEX_NODE;
	
	JIVE_DECLARE_ADDRESS_TYPE(address_type);
	const jive_type * operand_types[2] = {address_type, address_type};
	jive_output * operands[2] = {addr1, addr2};
	JIVE_DEBUG_ASSERT(difference_type->class_ == &JIVE_BITSTRING_TYPE);
	const jive_type * output_types[1] = {difference_type};
	
	_jive_node_init(&node->base, region,
		2, operand_types, operands,
		1, output_types);
	node->attrs.element_type = jive_type_copy(element_type, context);
	node->attrs.difference_type = *(const jive_bitstring_type *)difference_type;
	
	return &node->base;
}

jive_node *
jive_arrayindex_create(jive_region * region,
	jive_output * addr1, jive_output * addr2,
	const jive_type * element_type, const jive_type * difference_type)
{
	return jive_arrayindex_node_create(region, addr1, addr2, element_type, difference_type);
}

jive_output *
jive_arrayindex(jive_output * addr1, jive_output * addr2,
	const jive_type * element_type, const jive_type * difference_type)
{
	jive_region * region = addr1->node->region;
	if (addr2->node->region->depth > region->depth)
		region = addr2->node->region;
	
	jive_arrayindex_node_attrs attrs;
	attrs.element_type = (jive_type *) element_type;
	attrs.difference_type = *(const jive_bitstring_type *)difference_type;
	
	jive_output * operands[2] = {addr1, addr2};
	return jive_binary_operation_normalized_create(&JIVE_ARRAYINDEX_NODE, region, &attrs.base, 2, operands);
}