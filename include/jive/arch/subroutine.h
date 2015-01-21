/*
 * Copyright 2010 2011 2012 2013 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_ARCH_SUBROUTINE_H
#define JIVE_ARCH_SUBROUTINE_H

#include <string>
#include <vector>

#include <jive/arch/subroutine/nodes.h>
#include <jive/arch/subroutine/signature.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/node.h>

struct jive_subroutine;

namespace jive {
class gate;
class output;

class subroutine_hl_builder_interface {
public:
	virtual
	~subroutine_hl_builder_interface() noexcept;

	virtual output *
	value_parameter(
		jive_subroutine & subroutine,
		size_t index) = 0;

	virtual void
	value_return(
		jive_subroutine & subroutine,
		size_t index,
		output * value) = 0;
	
	virtual output *
	finalize(
		jive_subroutine & subroutine) = 0;
};

class subroutine_builder_state {
public:
	struct gated_port {
		gated_port() : gate(0), output(0) {}
		jive::gate * gate;
		jive::output * output;
	};

	inline
	subroutine_builder_state(const subroutine_machine_signature & sig) noexcept
		: arguments(sig.arguments.size())
		, passthroughs(sig.passthroughs.size())
		, results(sig.results.size())
	{
	}

	std::vector<gated_port> arguments;
	std::vector<gated_port> passthroughs;
	std::vector<gated_port> results;
};

}

struct jive_instructionset;
struct jive_region;

struct jive_subroutine_abi_class;

struct jive_subroutine {
	jive_region * region;
	
	std::unique_ptr<jive::subroutine_builder_state> builder_state;
	std::unique_ptr<jive::subroutine_hl_builder_interface> hl_builder;
	jive::subroutine_machine_signature signature;
};

/**
	\brief Begin constructing a subroutine region
*/
jive_subroutine
jive_subroutine_begin(
	jive_graph * graph,
	jive::subroutine_machine_signature sig,
	std::unique_ptr<jive::subroutine_hl_builder_interface> hl_builder);

/**
	\brief End constructing a subroutine region
*/
jive_node *
jive_subroutine_end(jive_subroutine & self);

/**
	\brief Get argument value
*/
jive::output *
jive_subroutine_simple_get_argument(
	jive_subroutine & self,
	size_t index);

void
jive_subroutine_simple_set_result(
	jive_subroutine & self,
	size_t index,
	jive::output * value);

jive::output *
jive_subroutine_simple_get_global_state(const jive_subroutine & self);

void
jive_subroutine_simple_set_global_state(jive_subroutine & self, jive::output * state);

/* FIXME: these are quite C-specific, so really do not belong here */
enum jive_argument_type {
	jive_argument_void = 0,
	jive_argument_pointer = 1,
	jive_argument_int = 2,
	jive_argument_long = 3,
	jive_argument_float = 4
};

struct jive_value_split_factory {
	jive::output * (*split)(const jive_value_split_factory * self, jive::output * value);
};

struct jive_subroutine_late_transforms {
	void (*value_split)(
		const jive_subroutine_late_transforms * self,
		jive::output * value_in,
		jive::input * value_out,
		const jive_value_split_factory * enter_split,
		const jive_value_split_factory * leave_split);
};

struct jive_subroutine_stackframe_info {
	/* lower bound of frame (relative to initial position of stack pointer) */
	ssize_t lower_bound;
	/* upper bound of frame (relative to initial position of stack pointer) */
	ssize_t upper_bound;
	/* offset of frame pointer to initial position of stack pointer */
	ssize_t frame_pointer_offset;
	/* offset of stack pointer to initial position of stack pointer */
	ssize_t stack_pointer_offset;
	/* size of argument area for calls */
	size_t call_area_size;
};

struct jive_subroutine_abi_class {
	void (*prepare_stackframe)(
		const jive::subroutine_op & op,
		jive_region * region,
		jive_subroutine_stackframe_info * frame,
		const jive_subroutine_late_transforms * xfrm);
	jive::input *(*add_fp_dependency)(
		const jive::subroutine_op & op,
		jive_region * region,
		jive_node * node);
	jive::input *(*add_sp_dependency)(
		const jive::subroutine_op & op,
		jive_region * region,
		jive_node * node);
	const jive_instructionset * instructionset;
};

void
jive_subroutine_node_prepare_stackframe(
	jive_node * self,
	const jive::subroutine_op & op,
	jive_subroutine_stackframe_info * frame,
	const jive_subroutine_late_transforms * xfrm);

jive::input *
jive_subroutine_node_add_fp_dependency(
	const jive_node * self,
	const jive::subroutine_op & op,
	jive_node * node);

jive::input *
jive_subroutine_node_add_sp_dependency(
	const jive_node * self,
	const jive::subroutine_op & op,
	jive_node * node);

jive_node *
jive_region_get_subroutine_node(const jive_region * region);

const jive_instructionset *
jive_region_get_instructionset(const jive_region * region);

jive::output *
jive_subroutine_node_get_sp(const jive_node * self);

jive::output *
jive_subroutine_node_get_fp(const jive_node * self);

#endif
