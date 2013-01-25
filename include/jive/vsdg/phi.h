/*
 * Copyright 2012 Nico Reißmann <nico.reissmann@gmail.com>
 * Copyright 2012 Helge Bahmann <hcb@chaoticmind.net>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_PHI_H
#define JIVE_VSDG_PHI_H

#include <jive/vsdg/node.h>
#include <jive/vsdg/anchor.h>

/* phi node normal form */

typedef struct jive_anchor_node_normal_form jive_phi_node_normal_form;
typedef struct jive_phi_node_normal_form_class jive_phi_node_normal_form_class;

extern const jive_phi_node_normal_form_class JIVE_PHI_NODE_NORMAL_FORM_;
#define JIVE_PHI_NODE_NORMAL_FORM (JIVE_PHI_NODE_NORMAL_FORM_.base.base)

struct jive_phi_node_normal_form_class {
	jive_anchor_node_normal_form_class base;
	void (*normalized_create)(const jive_phi_node_normal_form * self,
		jive_region * phi_region, jive_output * results[]);
};

JIVE_EXPORTED_INLINE jive_phi_node_normal_form *
jive_phi_node_normal_form_cast(jive_node_normal_form * self)
{
	if (jive_node_normal_form_isinstance(self, &JIVE_PHI_NODE_NORMAL_FORM))
		return (jive_phi_node_normal_form *) self;
	else
		return NULL;
}

/* phi node */

extern const jive_node_class JIVE_PHI_NODE;
extern const jive_node_class JIVE_PHI_ENTER_NODE;
extern const jive_node_class JIVE_PHI_LEAVE_NODE;

typedef struct jive_phi jive_phi;
typedef struct jive_phi_fixvar jive_phi_fixvar;

struct jive_phi_build_state;

/**
	\brief Represent a phi construct under construction
*/
struct jive_phi {
	struct jive_region * region;
	struct jive_phi_build_state * internal_state;
};

/**
	\brief Represent information about a phi fixpoint value
*/
struct jive_phi_fixvar {
	struct jive_output * value;
	struct jive_gate * gate;
};

/**
	\brief Begin constructing a phi region
*/
jive_phi
jive_phi_begin(struct jive_graph * graph);

/**
	\brief Add a fixpoint variable of given type
*/
jive_phi_fixvar
jive_phi_fixvar_enter(jive_phi self, const struct jive_type * type);

/**
	\brief Set fixpoint value of variable
*/
void
jive_phi_fixvar_leave(jive_phi self, jive_gate * var,
	struct jive_output * post_value);

/**
	\brief End constructing a phi region
*/
struct jive_node *
jive_phi_end(jive_phi self,
	     size_t npost_values, jive_phi_fixvar * fix_values);

#endif