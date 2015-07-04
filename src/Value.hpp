#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>

enum class ValueType {
	null,
	boolean,
	number,
	string,
	array,
	object,
};

class Value;

using StringRef	= boost::string_ref;
using Array		= std::vector<Value>;
using ArrayRef	= const Array&;
using Object	= std::unordered_map<std::string, Value>;


class Value final {
	friend class NullValue;
	friend class BoolValue;
	friend class NumberValue;
	friend class StringValue;
	friend class ArrayValue;
	friend class ObjectValue;

	class Impl;
	std::shared_ptr<Impl> impl_;

public:
	Value();
	Value(std::nullptr_t);
	Value(bool b);
	Value(int i);
	Value(float f);
	Value(double d);
	Value(StringRef s);
	Value(ArrayRef a);
	Value(const Object& o);
	Value(Array&& a);
	Value(Object&& o);

	bool		equals		(const Value& v) const;
	ValueType	type		() const;
	std::string	stringify	() const;

	bool isNull		() const;
	bool isBool 	() const;
	bool isNumber	() const;
	bool isString	() const;
	bool isArray	() const;
	bool isObject	() const;

	bool			boolValue	() const;
	int				intValue	() const;
	float			floatValue	() const;
	double			doubleValue	() const;
	StringRef		stringValue	() const;
	ArrayRef		arrayItems	() const;
	const Object&	objectItems	() const;

	bool		boolValue	(bool defaultValue) const;
	int			intValue	(int defaultValue) const;
	float		floatValue	(float defaultValue) const;
	double		doubleValue	(double defaultValue) const;
	StringRef	stringValue	(StringRef defaultValue) const;

	boost::optional<bool>		boolValueOpt	() const;
	boost::optional<int>		intValueOpt		() const;
	boost::optional<float>		floatValueOpt	() const;
	boost::optional<double>		doubleValueOpt	() const;
	boost::optional<StringRef>	stringValueOpt	() const;

	bool operator==(const Value& v) const;
	bool operator!=(const Value& v) const;

	const Value& operator[](size_t pos) const;
	const Value& operator[](const std::string& name) const;
};
