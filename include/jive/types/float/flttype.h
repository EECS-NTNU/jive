/*
 * Copyright 2012 2013 2014 Nico Reißmann <nico.reissmann@gmail.com>
 * See COPYING for terms of redistribution.
 */

#ifndef JIVE_TYPES_FLOAT_FLTTYPE_H
#define JIVE_TYPES_FLOAT_FLTTYPE_H

#include <jive/vsdg/valuetype.h>

/* float type */

class jive_float_type final : public jive_value_type {
public:
	virtual ~jive_float_type() noexcept;

	inline constexpr jive_float_type() noexcept : jive_value_type() {};

	virtual void label(jive_buffer & buffer) const override;

	virtual bool operator==(const jive_type & other) const noexcept override;

	virtual jive_float_type * copy() const override;

	virtual jive_input * create_input(jive_node * node, size_t index,
		jive_output * origin) const override;

	virtual jive_output * create_output(jive_node * node, size_t index) const override;

	virtual jive_gate * create_gate(jive_graph * graph, const char * name) const override;
};

/* float input */

class jive_float_input final : public jive_value_input {
public:
	virtual ~jive_float_input() noexcept;

	jive_float_input(struct jive_node * node, size_t index, jive_output * origin);

	virtual const jive_float_type & type() const noexcept { return type_; }

private:
	jive_float_type type_;
};

/* float output */

class jive_float_output final : public jive_value_output {
public:
	virtual ~jive_float_output() noexcept;

	jive_float_output(struct jive_node * node, size_t index);

	virtual const jive_float_type & type() const noexcept { return type_; }

private:
	jive_float_type type_;
};

/* float gate */

class jive_float_gate final : public jive_value_gate {
public:
	virtual ~jive_float_gate() noexcept;

	jive_float_gate(jive_graph * graph, const char name[]);

	virtual const jive_float_type & type() const noexcept { return type_; }

private:
	jive_float_type type_;
};

#endif
