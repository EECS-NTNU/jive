/*
 * Copyright 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/types/float/fltconstant.h>
#include <jive/types/float/fltoperation-classes.h>
#include <jive/types/float/flttype.h>

#include <jive/vsdg/control.h>
#include <jive/vsdg/node-private.h>

namespace jive {
namespace flt {

static const type type_instance;

unary_op::~unary_op() noexcept
{
}

const jive::base::type &
unary_op::argument_type(size_t index) const noexcept
{
	return type_instance;
}

const jive::base::type &
unary_op::result_type(size_t index) const noexcept
{
	return type_instance;
}

jive_unop_reduction_path_t
unary_op::can_reduce_operand(
	const jive::output * arg) const noexcept
{
	bool arg_is_constant =
		dynamic_cast<const constant_op *>(&arg->node()->operation());
	
	if (arg_is_constant) {
		return jive_unop_reduction_constant;
	}

	return jive_unop_reduction_none;
}

jive::output *
unary_op::reduce_operand(
	jive_unop_reduction_path_t path,
	jive::output * arg) const
{
	if (path == jive_unop_reduction_constant) {
		jive_graph * graph = arg->node()->graph;
		const constant_op & c =
			static_cast<const constant_op&>(arg->node()->operation());
		value_repr result = reduce_constant(c.value());
		return jive_fltconstant(graph, result);
	}

	return nullptr;
}

binary_op::~binary_op() noexcept
{
}

size_t
binary_op::narguments() const noexcept
{
	return 2;
}

const jive::base::type &
binary_op::argument_type(size_t index) const noexcept
{
	return type_instance;
}

size_t
binary_op::nresults() const noexcept
{
	return 1;
}

const jive::base::type &
binary_op::result_type(size_t index) const noexcept
{
	return type_instance;
}

/* reduction methods */
jive_binop_reduction_path_t
binary_op::can_reduce_operand_pair(
	const jive::output * arg1,
	const jive::output * arg2) const noexcept
{
	bool arg1_is_constant =
		dynamic_cast<const constant_op *>(&arg1->node()->operation());
	bool arg2_is_constant =
		dynamic_cast<const constant_op *>(&arg2->node()->operation());
	
	if (arg1_is_constant && arg2_is_constant) {
		return jive_binop_reduction_constants;
	}

	return jive_binop_reduction_none;
}

jive::output *
binary_op::reduce_operand_pair(
	jive_binop_reduction_path_t path,
	jive::output * arg1,
	jive::output * arg2) const
{
	jive_graph * graph = arg1->node()->graph;

	if (path == jive_binop_reduction_constants) {
		const constant_op & c1 =
			static_cast<const constant_op&>(arg1->node()->operation());
		const constant_op & c2 =
			static_cast<const constant_op&>(arg2->node()->operation());
		value_repr result = reduce_constants(c1.value(), c2.value());
		return jive_fltconstant(graph, result);
	}

	return nullptr;
}

compare_op::~compare_op() noexcept
{
}

size_t
compare_op::narguments() const noexcept
{
	return 2;
}

const jive::base::type &
compare_op::argument_type(size_t index) const noexcept
{
	return type_instance;
}

size_t
compare_op::nresults() const noexcept
{
	return 1;
}

const jive::base::type &
compare_op::result_type(size_t index) const noexcept
{
	static const jive::ctl::type ctl;
	return ctl;
}

jive_binop_reduction_path_t
compare_op::can_reduce_operand_pair(
	const jive::output * arg1,
	const jive::output * arg2) const noexcept
{
	bool arg1_is_constant =
		dynamic_cast<const constant_op *>(&arg1->node()->operation());
	bool arg2_is_constant =
		dynamic_cast<const constant_op *>(&arg2->node()->operation());
	
	if (arg1_is_constant && arg2_is_constant) {
		return jive_binop_reduction_constants;
	}

	return jive_binop_reduction_none;
}

jive::output *
compare_op::reduce_operand_pair(
	jive_binop_reduction_path_t path,
	jive::output * arg1,
	jive::output * arg2) const
{
	jive_graph * graph = arg1->node()->graph;

	if (path == jive_binop_reduction_constants) {
		const constant_op & c1 =
			static_cast<const constant_op&>(arg1->node()->operation());
		const constant_op & c2 =
			static_cast<const constant_op&>(arg2->node()->operation());
		bool result = reduce_constants(c1.value(), c2.value());
		if (result) {
			return jive_control_true(graph);
		} else {
			return jive_control_false(graph);
		}
	}

	return nullptr;
}

}
}

const jive_node_class JIVE_FLTBINARY_NODE = {
	parent : &JIVE_BINARY_OPERATION,
	name : "FLTBINARY",
	fini : jive_node_fini_, /* inherit */
	get_default_normal_form : jive_binary_operation_get_default_normal_form_, /* inherit */
	get_label : nullptr,
	match_attrs : nullptr,
	check_operands : nullptr,
	create : nullptr
};

const jive_node_class JIVE_FLTUNARY_NODE = {
	parent : &JIVE_UNARY_OPERATION,
	name : "FLTUNARY",
	fini : jive_node_fini_, /* inherit */
	get_default_normal_form : jive_unary_operation_get_default_normal_form_, /* inherit */
	get_label : nullptr,
	match_attrs : nullptr,
	check_operands : nullptr,
	create : nullptr
};

const jive_node_class JIVE_FLTCOMPARISON_NODE = {
	parent : &JIVE_BINARY_OPERATION,
	name : "FLTCOMPARISON",
	fini : jive_node_fini_, /* inherit */
	get_default_normal_form : jive_binary_operation_get_default_normal_form_, /* inherit */
	get_label : nullptr,
	match_attrs : nullptr,
	check_operands : nullptr,
	create : nullptr};
