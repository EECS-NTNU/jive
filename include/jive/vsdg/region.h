/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 2014 2015 2016 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_REGION_H
#define JIVE_VSDG_REGION_H

#include <stdbool.h>
#include <stddef.h>

#include <jive/common.h>
#include <jive/util/list.h>
#include <jive/vsdg/basetype.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/section.h>

namespace jive {
	class input;
	class substitution_map;
}

struct jive_cut;

namespace jive {

class node;
class simple_node;
class structural_input;
class structural_output;

class argument final : public oport {
public:
	virtual
	~argument() noexcept;

	argument(
		jive::region * region,
		size_t index,
		jive::structural_input * input,
		const jive::base::type & type);

	argument(
		jive::region * region,
		size_t index,
		jive::structural_input * input,
		jive::gate * gate);

	argument(const argument &) = delete;

	argument(const argument &&) = delete;

	argument &
	operator=(const argument &) = delete;

	argument &
	operator=(const argument &&) = delete;

	virtual const jive::base::type &
	type() const noexcept override;

	virtual jive::region *
	region() const noexcept override;

	virtual jive::node *
	node() const noexcept override;

	inline jive::structural_input *
	input() const noexcept
	{
		return input_;
	}

	struct {
		jive::argument * prev;
		jive::argument * next;
	} input_argument_list;

private:
	jive::region * region_;
	jive::structural_input * input_;
	std::unique_ptr<jive::base::type> type_;
};

class result final : public iport {
public:
	virtual
	~result() noexcept;

	result(
		jive::region * region,
		size_t index,
		jive::oport * origin,
		jive::structural_output * output,
		const jive::base::type & type);

	result(
		jive::region * region,
		size_t index,
		jive::oport * origin,
		jive::structural_output * output,
		jive::gate * gate);

	result(const result &) = delete;

	result(const result &&) = delete;

	result &
	operator=(const result &) = delete;

	result &
	operator=(const result &&) = delete;

	virtual jive::node *
	node() const noexcept override;

	inline jive::structural_output *
	output() const noexcept
	{
		return output_;
	}

	struct {
		jive::result * prev;
		jive::result * next;
	} output_result_list;

private:
	jive::structural_output * output_;
};

class structural_node;

class region {
public:
	~region();

	region(jive::region * parent, jive::graph * graph);

	region(jive::structural_node * node);

	inline jive::graph *
	graph() const noexcept
	{
		return graph_;
	}

	inline jive::structural_node *
	node() const noexcept
	{
		return node_;
	}

	jive::argument *
	add_argument(jive::structural_input * input, const jive::base::type & type);

	jive::argument *
	add_argument(jive::structural_input * input, jive::gate * gate);

	void
	remove_argument(size_t index);

	inline size_t
	narguments() const noexcept
	{
		return arguments_.size();
	}

	inline jive::argument *
	argument(size_t index) const noexcept
	{
		JIVE_DEBUG_ASSERT(index < narguments());
		return arguments_[index];
	}

	jive::result *
	add_result(jive::oport * origin, structural_output * output, const base::type & type);

	jive::result *
	add_result(jive::oport * origin, structural_output * output, jive::gate * gate);

	void
	remove_result(size_t index);

	inline size_t
	nresults() const noexcept
	{
		return results_.size();
	}

	inline jive::result *
	result(size_t index) const noexcept
	{
		JIVE_DEBUG_ASSERT(index < nresults());
		return results_[index];
	}

	jive::simple_node *
	add_simple_node(const jive::operation & op, const std::vector<jive::oport*> & operands);

	jive::structural_node *
	add_structural_node(const jive::operation & op, size_t nsubregions);

	void
	remove_node(jive::node * node);

	/**
		\brief Copy a region with substitutions
		\param target Target region to create nodes in
		\param substitution Operand and gate substitutions
		\param copy_top Copy top node of region
		\param copy_bottom Copy bottom node of region

		Copies all nodes of the specified region and its
		subregions into the target region. Substitutions
		will be performed as specified, and the substitution
		map will be updated as nodes are copied.
	*/
	void
	copy(region * target, substitution_map & smap) const;

	void
	prune(bool recursive);

	typedef jive::detail::intrusive_list<
		jive::node,
		jive::node::region_node_list_accessor
	> region_nodes_list;

	region_nodes_list nodes;

	struct {
		jive::node * first;
		jive::node * last;
	} top_nodes;

	struct {
		jive::node * first;
		jive::node * last;
	} bottom_nodes;

private:
	jive::graph * graph_;
	jive::structural_node * node_;
	std::vector<jive::result*> results_;
	std::vector<jive::argument*> arguments_;
};

} //namespace

#ifdef JIVE_DEBUG
void
jive_region_verify_top_node_list(struct jive::region * region);
#endif

#endif
