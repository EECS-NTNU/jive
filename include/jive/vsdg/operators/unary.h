#ifndef JIVE_VSDG_OPERATORS_UNARY_H
#define JIVE_VSDG_OPERATORS_UNARY_H

#include <jive/common.h>
#include <jive/vsdg/node.h>

struct jive_binary_operation_class;

typedef struct jive_unary_operation_class jive_unary_operation_class;

typedef struct jive_unary_operation_normal_form_class jive_unary_operation_normal_form_class;
typedef struct jive_unary_operation_normal_form jive_unary_operation_normal_form;

/* node class */

struct jive_unary_operation_class {
	jive_node_class base;
	
	/* algebraic properties */
	const struct jive_binary_operation_class * const * single_apply_over;
	const struct jive_binary_operation_class * const * multi_apply_over;
	
	/* class method */
	bool (*can_reduce_operand)(const jive_node_class * cls, const jive_node_attrs * attrs, jive_output * operand);
	
	/* class method */
	bool (*reduce_operand)(const jive_node_class * cls, const jive_node_attrs * attrs, jive_output ** operand);
};

extern const jive_unary_operation_class JIVE_UNARY_OPERATION_;
#define JIVE_UNARY_OPERATION (JIVE_UNARY_OPERATION_.base)

/* node class inheritable methods */

jive_node_normal_form *
jive_unary_operation_get_default_normal_form_(const jive_node_class * cls, jive_node_normal_form * parent, struct jive_graph * graph);

bool
jive_unary_operation_can_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs, jive_output * operand);

bool
jive_unary_operation_reduce_operand_(const jive_node_class * cls, const jive_node_attrs * attrs, jive_output ** operand);


/* normal form class */

struct jive_unary_operation_normal_form_class {
	jive_node_normal_form_class base;
	void (*set_reducible)(jive_unary_operation_normal_form * self, bool enable);
	jive_output * (*normalized_create)(const jive_unary_operation_normal_form * self, struct jive_region * region, const jive_node_attrs * attrs, jive_output * operand);
};

extern const jive_unary_operation_normal_form_class JIVE_UNARY_OPERATION_NORMAL_FORM_;
#define JIVE_UNARY_OPERATION_NORMAL_FORM (JIVE_UNARY_OPERATION_NORMAL_FORM_.base)

struct jive_unary_operation_normal_form {
	jive_node_normal_form base;
	bool enable_reducible;
};

JIVE_EXPORTED_INLINE jive_unary_operation_normal_form *
jive_unary_operation_normal_form_cast(jive_node_normal_form * self)
{
	const jive_node_normal_form_class * cls = self->class_;
	while (cls) {
		if (cls == &JIVE_UNARY_OPERATION_NORMAL_FORM)
			return (jive_unary_operation_normal_form *)cls;
		cls = cls->parent;
	}
	return 0;
}

JIVE_EXPORTED_INLINE jive_output *
jive_unary_operation_normalized_create(
	const jive_unary_operation_normal_form * self,
	struct jive_region * region,
	const jive_node_attrs * attrs,
	jive_output * operand)
{
	const jive_unary_operation_normal_form_class * cls;
	cls = (const jive_unary_operation_normal_form_class *) self->base.class_;
	
	return cls->normalized_create(self, region, attrs, operand);
}

JIVE_EXPORTED_INLINE bool
jive_unary_operation_can_reduce_operand(const jive_unary_operation_normal_form * self,
	const jive_node_attrs * attrs, jive_output * operand)
{
	return ((const jive_unary_operation_class *)self->base.node_class)->can_reduce_operand(self->base.node_class, attrs, operand);
}

JIVE_EXPORTED_INLINE bool
jive_unary_operation_reduce_operand(const jive_unary_operation_normal_form * self, const jive_node_attrs * attrs, jive_output ** operand)
{
	return ((const jive_unary_operation_class *)self->base.node_class)->reduce_operand(self->base.node_class, attrs, operand);
}

JIVE_EXPORTED_INLINE void
jive_unary_operation_set_reducible(jive_unary_operation_normal_form * self, bool reducible)
{
	const jive_unary_operation_normal_form_class * cls;
	cls = (const jive_unary_operation_normal_form_class *) self->base.class_;
	cls->set_reducible(self, reducible);
}

/* normal form class inheritable methods */

bool
jive_unary_operation_normalize_node_(const jive_node_normal_form * self, jive_node * node);

bool
jive_unary_operation_operands_are_normalized_(const jive_node_normal_form * self, size_t noperands, jive_output * const operands[], const jive_node_attrs * attrs);

jive_output *
jive_unary_operation_normalized_create_(const jive_unary_operation_normal_form * self, struct jive_region * region, const jive_node_attrs * attrs, jive_output * operand);

void
jive_unary_operation_set_reducible_(jive_unary_operation_normal_form * self_, bool enable);

#endif