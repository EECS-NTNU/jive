/*
 * Copyright 2010 2011 2012 2013 2014 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2011 2012 2013 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_GRAPH_H
#define JIVE_VSDG_GRAPH_H

#include <stdbool.h>
#include <stdlib.h>

#include <typeindex>

#include <jive/common.h>
#include <jive/context.h>
#include <jive/vsdg/node-normal-form.h>
#include <jive/vsdg/node.h>
#include <jive/vsdg/notifiers.h>
#include <jive/vsdg/tracker.h>

/* graph tail node */

namespace jive {

class gate;

class graph_tail_operation final : public operation {
public:
	virtual
	~graph_tail_operation() noexcept;

	virtual bool
	operator==(const operation & other) const noexcept override;

	virtual size_t
	narguments() const noexcept override;

	virtual const jive::base::type &
	argument_type(size_t index) const noexcept override;

	virtual size_t
	nresults() const noexcept override;

	virtual const jive::base::type &
	result_type(size_t index) const noexcept override;

	virtual jive_node *
	create_node(
		jive_region * region,
		size_t narguments,
		jive::output * const arguments[]) const override;

	virtual std::string
	debug_string() const override;

	virtual std::unique_ptr<jive::operation>
	copy() const override;
};

}

typedef jive::operation_node<jive::graph_tail_operation> jive_graph_tail_node;

/* graph */

typedef struct jive_graph jive_graph;
typedef struct jive_tracker_depth_state jive_tracker_depth_state;
typedef struct jive_tracker_nodestate_list jive_tracker_nodestate_list;
typedef struct jive_tracker_slot_reservation jive_tracker_slot_reservation;

struct jive_node;
struct jive_node_class;
struct jive_region;
struct jive_resource;

struct jive_graph {
	jive_context * context;
	
	struct {
		struct jive_node * first;
		struct jive_node * last;
	} bottom;
	
	struct jive_region * root_region;
	
	struct {
		jive::gate * first;
		jive::gate * last;
	} gates;
	
	struct {
		struct jive_variable * first;
		struct jive_variable * last;
	} variables;
	struct {
		struct jive_variable * first;
		struct jive_variable * last;
	} unused_variables;
	
	bool resources_fully_assigned;
	bool normalized;
	size_t floating_region_count;
	
	std::vector<jive_tracker_slot_reservation> tracker_slots;
	
	jive::node_normal_form_hash new_node_normal_forms;
	
	jive_region_notifier_slot on_region_create;
	jive_region_notifier_slot on_region_destroy;
	jive_region_ssavar_notifier_slot on_region_add_used_ssavar;
	jive_region_ssavar_notifier_slot on_region_remove_used_ssavar;
	
	jive_node_notifier_slot on_node_create;
	jive_node_notifier_slot on_node_destroy;
	jive_node_depth_notifier_slot on_node_depth_change;
	
	jive_input_notifier_slot on_input_create;
	jive_input_change_notifier_slot on_input_change;
	jive_input_notifier_slot on_input_destroy;
	
	jive_output_notifier_slot on_output_create;
	jive_output_notifier_slot on_output_destroy;
	
	jive_variable_notifier_slot on_variable_create;
	jive_variable_notifier_slot on_variable_destroy;
	jive_variable_gate_notifier_slot on_variable_assign_gate;
	jive_variable_gate_notifier_slot on_variable_unassign_gate;
	jive_variable_resource_class_notifier_slot on_variable_resource_class_change;
	jive_variable_resource_name_notifier_slot on_variable_resource_name_change;
	
	jive_gate_notifier_slot on_gate_interference_add;
	jive_gate_notifier_slot on_gate_interference_remove;
	
	jive_ssavar_notifier_slot on_ssavar_create;
	jive_ssavar_notifier_slot on_ssavar_destroy;
	jive_ssavar_input_notifier_slot on_ssavar_assign_input;
	jive_ssavar_input_notifier_slot on_ssavar_unassign_input;
	jive_ssavar_output_notifier_slot on_ssavar_assign_output;
	jive_ssavar_output_notifier_slot on_ssavar_unassign_output;
	jive_ssavar_divert_notifier_slot on_ssavar_divert_origin;
	jive_ssavar_variable_notifier_slot on_ssavar_variable_change;
};

JIVE_EXPORTED_INLINE struct jive_region *
jive_graph_get_root_region(const jive_graph * self)
{
	return self->root_region;
}

JIVE_EXPORTED_INLINE void
jive_graph_export(struct jive_graph * self, jive::output * operand)
{
	jive_node_add_input(self->root_region->bottom, &operand->type(), operand);
}

jive_graph *
jive_graph_create(jive_context * context);

void
jive_graph_destroy(jive_graph * self);

jive_graph *
jive_graph_copy(jive_graph * self, jive_context * context);

void
jive_graph_prune(jive_graph * self);

bool
jive_graph_has_active_traversers(const jive_graph * self);

void
jive_graph_push_outward(jive_graph * self);

void
jive_graph_pull_inward(jive_graph * self);

jive::node_normal_form *
jive_graph_get_nodeclass_form(
	jive_graph * self,
	const std::type_info & type);

void
jive_graph_mark_denormalized(jive_graph * self);

void
jive_graph_normalize(jive_graph * self);

#endif
