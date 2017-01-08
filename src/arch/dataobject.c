/*
 * Copyright 2010 2011 2012 2013 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2014 2015 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#include <jive/arch/dataobject.h>

#include <jive/arch/addresstype.h>
#include <jive/arch/memlayout.h>
#include <jive/types/bitstring/constant.h>
#include <jive/types/bitstring/type.h>
#include <jive/types/float/flttype.h>
#include <jive/types/record/rcdgroup.h>
#include <jive/types/record/rcdtype.h>
#include <jive/types/union/unntype.h>
#include <jive/types/union/unnunify.h>
#include <jive/vsdg/anchortype.h>
#include <jive/vsdg/graph.h>
#include <jive/vsdg/seqtype.h>
#include <jive/vsdg/simple_node.h>
#include <jive/vsdg/statetype.h>

namespace jive {

dataobj_head_op::~dataobj_head_op() noexcept
{
}

size_t
dataobj_head_op::narguments() const noexcept
{
	return types_.size();
}

const base::type &
dataobj_head_op::argument_type(size_t index) const noexcept
{
	return *types_[index];
}

size_t
dataobj_head_op::nresults() const noexcept
{
	return 1;
}

const base::type &
dataobj_head_op::result_type(size_t index) const noexcept
{
	return seq::seqtype;
}

std::string
dataobj_head_op::debug_string() const
{
	return "DATA_HEAD";
}

std::unique_ptr<jive::operation>
dataobj_head_op::copy() const
{
	return std::unique_ptr<jive::operation>(new dataobj_head_op(*this));
}

dataobj_tail_op::~dataobj_tail_op() noexcept
{
}

size_t
dataobj_tail_op::narguments() const noexcept
{
	return 1;
}

const base::type &
dataobj_tail_op::argument_type(size_t index) const noexcept
{
	return seq::seqtype;
}

std::string
dataobj_tail_op::debug_string() const
{
	return "DATA_TAIL";
}

std::unique_ptr<jive::operation>
dataobj_tail_op::copy() const
{
	return std::unique_ptr<jive::operation>(new dataobj_tail_op(*this));
}

dataobj_op::~dataobj_op() noexcept
{
}

size_t
dataobj_op::nresults() const noexcept
{
	return 1;
}

const base::type &
dataobj_op::result_type(size_t index) const noexcept
{
	return jive::addr::type::instance();
}
std::string
dataobj_op::debug_string() const
{
	return "DATA";
}

std::unique_ptr<jive::operation>
dataobj_op::copy() const
{
	return std::unique_ptr<jive::operation>(new dataobj_op(*this));
}

}


static inline bool
is_powerof2(size_t v)
{
	return (v & (v-1)) == 0;
}

static void
squeeze_data_items(std::vector<jive::oport*> & items)
{
	size_t k = 0;
	for (size_t n = 0; n < items.size(); n++) {
		if (items[n])
			items[k++] = items[n];
	}
	items.resize(k);
}

static std::vector<jive::oport*>
flatten_data_items(
	jive::oport * data,
	jive::memlayout_mapper * layout_mapper)
{
	auto tmp = dynamic_cast<jive::output*>(data);

	std::vector<jive::oport*> items;
	const jive::base::type * type_ = &data->type();
	if (dynamic_cast<const jive::bits::type*>(type_)) {
		const jive::bits::type * type = static_cast<const jive::bits::type*>(type_);
		
		if (type->nbits() < 8 || !is_powerof2(type->nbits())) {
			throw jive::compiler_error(
				"Type mismatch: primitive data items must be power-of-two bytes in size");
		}

		items.resize(type->nbits() / 8, nullptr);
		items[0] = data;
	} else if (dynamic_cast<const jive::addr::type*>(type_)) {
		const jive::dataitem_memlayout & layout = layout_mapper->map_address();
		items.resize(layout.size(), nullptr);
		items[0] = data;
	} else if (dynamic_cast<const jive::flt::type*>(type_)) {
		items.resize(4, nullptr);
		items[0] = data;
	} else if (dynamic_cast<const jive::rcd::type*>(type_)) {
		const jive::rcd::type * type = static_cast<const jive::rcd::type*>(type_);
		std::shared_ptr<const jive::rcd::declaration> decl = type->declaration();
		const jive::record_memlayout & layout = layout_mapper->map_record(decl);

		if (!dynamic_cast<const jive::rcd::group_op *>(&tmp->node()->operation())) {
			throw jive::compiler_error("Type mismatch: can only serialize simple record compounds");
		}

		jive_graph * graph = data->region()->graph();
		
		auto zero_pad = jive_bitconstant(graph->root(), 8, "00000000");
		items.resize(layout.size(), zero_pad);

		for (size_t k = 0; k < decl->nelements(); k++) {
			auto sub_items = flatten_data_items(tmp->node()->input(k)->origin(), layout_mapper);
			
			if (sub_items.size() + layout.element(k).offset() > items.size()) {
				throw jive::compiler_error("Invalid record layout: element exceeds record");
			}
			
			for (size_t n = 0; n < sub_items.size(); n++) {
				if (items[n + layout.element(k).offset()] != zero_pad) {
					throw jive::compiler_error("Invalid record layout: members overlap");
				}
				items[n + layout.element(k).offset()] = sub_items[n];
			}
		}
	} else if (dynamic_cast<const jive::unn::type*>(type_)) {
		const jive::unn::type * type = static_cast<const jive::unn::type*>(type_);
		const jive::union_memlayout & layout = layout_mapper->map_union(type->declaration());
		
		if (!dynamic_cast<const jive::unn::unify_op *>(&tmp->node()->operation())) {
			throw jive::compiler_error("Type mismatch: can only serialize simple union compounds");
		}
		
		jive_graph * graph = data->region()->graph();
		
		auto zero_pad = jive_bitconstant(graph->root(), 8, "00000000");
		items.resize(layout.size(), zero_pad);
		
		auto sub_items = flatten_data_items(tmp->node()->input(0)->origin(), layout_mapper);
		
		if (sub_items.size() > items.size()) {
			throw jive::compiler_error("Invalid union layout: element exceeds union");
		}
		
		std::copy(sub_items.begin(), sub_items.end(), items.begin());
	} else {
		throw jive::compiler_error("Type mismatch: can only serialize record and primitive data items");
	}

	return items;
}

static jive::oport *
jive_dataobj_internal(
	jive::oport * data,
	jive::memlayout_mapper * layout_mapper,
	jive::region * parent,
	jive::region * region)
{
	std::vector<jive::oport*> data_items = flatten_data_items(data, layout_mapper);
	squeeze_data_items(data_items);
	std::vector<std::unique_ptr<const jive::base::type>> types;
	std::vector<jive::oport*> arguments;
	for (size_t n = 0; n < data_items.size(); ++n) {
		types.emplace_back(data_items[n]->type().copy());
		arguments.push_back(data_items[n]);
	}

	jive::node * head;
	head = jive_opnode_create(jive::dataobj_head_op(std::move(types)), region, arguments);
	jive::output * tmp = dynamic_cast<jive::output*>(head->output(0));
	jive::node * tail = jive_opnode_create(jive::dataobj_tail_op(), region, {tmp});
	tmp = dynamic_cast<jive::output*>(tail->output(0));
	jive::node * obj = jive_opnode_create(jive::dataobj_op(), parent, {tmp});

	return obj->output(0);
}

jive::oport *
jive_dataobj(jive::oport * data, jive::memlayout_mapper * layout_mapper)
{
	jive::region * parent = data->region()->graph()->root();
	jive::region * region = new jive::region(parent, parent->graph());

	return jive_dataobj_internal(data, layout_mapper, parent, region);
}

jive::oport *
jive_rodataobj(jive::oport * data, jive::memlayout_mapper * layout_mapper)
{
	jive::region * parent = data->region()->graph()->root();
	jive::region * region = new jive::region(parent, parent->graph());

	return jive_dataobj_internal(data, layout_mapper, parent, region);
}

jive::oport *
jive_bssobj(jive::oport * data, jive::memlayout_mapper * layout_mapper)
{
	jive::region * parent = data->region()->graph()->root();
	jive::region * region = new jive::region(parent, parent->graph());

	return jive_dataobj_internal(data, layout_mapper, parent, region);
}
