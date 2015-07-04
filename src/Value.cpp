#include <boost/lexical_cast.hpp>
#include "Value.hpp"

using namespace std;
using boost::optional;
using boost::lexical_cast;

class Value::Impl abstract {
	ValueType type_;

public:
	explicit Impl(ValueType type) : type_(type) {}
	virtual ~Impl() {}

	ValueType type() const { return type_; }

	virtual bool	equals		(const shared_ptr<Impl>& i) const =0;
	virtual string	stringify	() const =0;

	virtual bool			boolValue	() const { throw runtime_error{ "invalid type." }; }
	virtual int				intValue	() const { throw runtime_error{ "invalid type." }; }
	virtual float			floatValue	() const { throw runtime_error{ "invalid type." }; }
	virtual double			doubleValue	() const { throw runtime_error{ "invalid type." }; }
	virtual StringRef		stringValue	() const { throw runtime_error{ "invalid type." }; }
	virtual ArrayRef		arrayItems	() const { throw runtime_error{ "invalid type." }; }
	virtual const Object&	objectItems	() const { throw runtime_error{ "invalid type." }; }
};


class NullValue final : public Value::Impl {
public:
	explicit NullValue()
		: Impl(ValueType::null) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		return i->type() == ValueType::null;
	}

	std::string	stringify() const override {
		return "null";
	}
};


class BoolValue final : public Value::Impl {
	bool value_;

public:
	explicit BoolValue(bool b)
		: Impl(ValueType::boolean)
		, value_(b) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		return
			i->type()		== ValueType::boolean &&
			i->boolValue()	== value_;
	}

	string stringify() const override {
		return value_ ? "true" : "false";
	}

	bool boolValue() const override {
		return value_;
	}
};


class NumberValue final : public Value::Impl {
	double value_;

public:
	explicit NumberValue(double d)
		: Impl(ValueType::number)
		, value_(d) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		return
			i->type()		== ValueType::number &&
			i->doubleValue()== value_;
	}

	string stringify() const override {
		return lexical_cast<string>(value_);
	}

	double doubleValue() const override {
		return value_;
	}
};


class StringValue final : public Value::Impl {
	string value_;

public:
	explicit StringValue(StringRef s)
		: Impl(ValueType::string)
		, value_(s.to_string()) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		return
			i->type()		== ValueType::number &&
			i->stringValue()== value_;
	}

	string stringify() const override {
		return '"'+value_+'"';
	}

	StringRef stringValue() const override {
		return value_;
	}
};


class ArrayValue final : public Value::Impl {
	Array items_;

public:
	explicit ArrayValue(ArrayRef a)
		: Impl(ValueType::array)
		, items_(a) {}

	explicit ArrayValue(Array&& a)
		: Impl(ValueType::array)
		, items_(std::move(a)) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		if(i->type() != ValueType::array) { return false; }

		auto&& a = i->arrayItems();
		return a.size() == items_.size() && equal(
			a.cbegin(), a.cend(),
			items_.cbegin()
		);
	}

	string stringify() const override {
		string	str		= "[";
		bool	first	= true;

		for(auto&& v : items_) {
			if(!first) {
				str += ",";
			}

			str += v.stringify();
			first = false;
		}

		return str += "]";
	}

	ArrayRef arrayItems() const override {
		return items_;
	}
};


class ObjectValue final : public Value::Impl {
	Object obj_;

public:
	explicit ObjectValue(const Object& o)
		: Impl(ValueType::object)
		, obj_(o) {}

	explicit ObjectValue(Object&& o)
		: Impl(ValueType::object)
		, obj_(std::move(o)) {}

	bool equals(const shared_ptr<Impl>& i) const override {
		if(i->type() != ValueType::object) { return false; }

		auto&& o = i->objectItems();
		return o.size() == obj_.size() && equal(
			o.cbegin(), o.cend(),
			obj_.cbegin(),
			[](const pair<StringRef, Value>& a, const pair<StringRef, Value>& b) {
				return a.first == b.first && a.second == b.second;
			}
		);
	}

	string stringify() const override {
		string	str		= "{";
		bool	first	= true;

		for(auto&& x : obj_) {
			if(!first) {
				str += ",";
			}

			str += '"';
			str += x.first;
			str += '"';
			str += ':';

			str += x.second.stringify();
			first = false;
		}

		return str += "}";
	}
};


Value::Value()					: impl_(make_shared<NullValue>	())							{}
Value::Value(nullptr_t)			: impl_(make_shared<NullValue>	())							{}
Value::Value(bool b)			: impl_(make_shared<BoolValue>	(b))						{}
Value::Value(int i)				: impl_(make_shared<NumberValue>(static_cast<double>(i)))	{}
Value::Value(float f)			: impl_(make_shared<NumberValue>(static_cast<double>(f)))	{}
Value::Value(double d)			: impl_(make_shared<NumberValue>(d))						{}
Value::Value(StringRef s)		: impl_(make_shared<StringValue>(s))						{}
Value::Value(ArrayRef a)		: impl_(make_shared<ArrayValue>	(a))						{}
Value::Value(const Object& o)	: impl_(make_shared<ObjectValue>(o))						{}
Value::Value(Array&& a)			: impl_(make_shared<ArrayValue>	(std::move(a)))				{}
Value::Value(Object&& o)		: impl_(make_shared<ObjectValue>(std::move(o)))				{}

bool		Value::equals	(const Value& v) const	{ return impl_->equals(v.impl_); }
ValueType	Value::type		() const				{ return impl_->type(); }
std::string	Value::stringify() const				{ return impl_->stringify(); }

bool Value::isNull	() const { return type() == ValueType::null; }
bool Value::isBool	() const { return type() == ValueType::boolean; }
bool Value::isNumber() const { return type() == ValueType::number; }
bool Value::isString() const { return type() == ValueType::string; }
bool Value::isArray	() const { return type() == ValueType::array; }
bool Value::isObject() const { return type() == ValueType::object; }

bool			Value::boolValue	() const { return impl_->boolValue	(); }
int				Value::intValue		() const { return impl_->intValue	(); }
float			Value::floatValue	() const { return impl_->floatValue	(); }
double			Value::doubleValue	() const { return impl_->doubleValue(); }
StringRef		Value::stringValue	() const { return impl_->stringValue(); }
ArrayRef		Value::arrayItems	() const { return impl_->arrayItems	(); }
const Object&	Value::objectItems	() const { return impl_->objectItems(); }

bool		Value::boolValue	(bool defaultValue)			const { return isBool	() ? impl_->boolValue	() : defaultValue; }
int			Value::intValue		(int defaultValue)			const { return isNumber	() ? impl_->intValue	() : defaultValue; }
float		Value::floatValue	(float defaultValue)		const { return isNumber	() ? impl_->floatValue	() : defaultValue; }
double		Value::doubleValue	(double defaultValue)		const { return isNumber	() ? impl_->doubleValue	() : defaultValue; }
StringRef	Value::stringValue	(StringRef defaultValue)	const { return isString	() ? impl_->stringValue	() : defaultValue; }

boost::optional<bool>		Value::boolValueOpt		() const {  if(isBool	()) { return impl_->boolValue	(); } return boost::none; } 
boost::optional<int>		Value::intValueOpt		() const {  if(isNumber	()) { return impl_->intValue	(); } return boost::none; } 
boost::optional<float>		Value::floatValueOpt	() const {  if(isNumber	()) { return impl_->floatValue	(); } return boost::none; } 
boost::optional<double>		Value::doubleValueOpt	() const {  if(isNumber	()) { return impl_->doubleValue	(); } return boost::none; } 
boost::optional<StringRef>	Value::stringValueOpt	() const {  if(isString	()) { return impl_->stringValue	(); } return boost::none; } 

bool Value::operator==(const Value& v) const { return equals(v); }
bool Value::operator!=(const Value& v) const { return !equals(v); }

const Value& Value::operator[](size_t pos)				const { return arrayItems ().at(pos);	}
const Value& Value::operator[](const std::string& name)	const { return objectItems().at(name);	}
