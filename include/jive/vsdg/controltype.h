/*
 * Copyright 2010 2011 2012 Helge Bahmann <hcb@chaoticmind.net>
 * Copyright 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_VSDG_CONTROLTYPE_H
#define JIVE_VSDG_CONTROLTYPE_H

#include <jive/vsdg/statetype.h>

typedef struct jive_control_type jive_control_type;
typedef struct jive_control_output jive_control_output;
typedef struct jive_control_gate jive_control_gate;

extern const jive_type_class JIVE_CONTROL_TYPE;
#define JIVE_DECLARE_CONTROL_TYPE(name) \
	jive_control_type name##_struct; \
	const jive_type * name = &name##_struct

class jive_control_type final : public jive_state_type {
public:
	virtual ~jive_control_type() noexcept;

	jive_control_type() noexcept;
};

class jive_control_input final : public jive_state_input {
public:
	virtual ~jive_control_input() noexcept;

	jive_control_input(struct jive_node * node, size_t index, jive_output * initial_operand);

	virtual const jive_control_type & type() const noexcept { return type_; }

private:
	jive_control_type type_;
};

extern const jive_output_class JIVE_CONTROL_OUTPUT;
class jive_control_output final : public jive_state_output {
public:
	virtual ~jive_control_output() noexcept;

	jive_control_output(bool active, struct jive_node * node, size_t index);

	virtual const jive_control_type & type() const noexcept { return type_; }

	inline void set_active(bool active) noexcept { active_ = active; }

	inline bool active() const noexcept { return active_; }

private:
	bool active_;
	jive_control_type type_;
};

extern const jive_gate_class JIVE_CONTROL_GATE;
class jive_control_gate final : public jive_state_gate {
public:
	virtual ~jive_control_gate() noexcept;

	jive_control_gate(jive_graph * graph, const char name[]);

	virtual const jive_control_type & type() const noexcept { return type_; }

private:
	jive_control_type type_;
};

#endif
