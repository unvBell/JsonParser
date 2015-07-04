#include "Json.hpp"

class Parser final {
	using String = std::string;

	String		src_;
	String		error_;
	const char*	ch_;

	explicit Parser(StringRef src)
		: src_(src.to_string())
		, ch_(src_.c_str()) {}


	void whitespace() {
		while(*ch_ == ' ' || *ch_ == '\n' || *ch_ == '\r' || *ch_ == '\t')
			ch_++;
	}


	bool parseNumber(double& n) {
		const char* p = ch_;

		//	符号
		if(*ch_ == '-')
			ch_++;

		if(!isdigit(*ch_))
			return false;

		//	整数部
		if(*ch_ == '0') {
			ch_++;
			if(isdigit(*ch_))
				return false;
		}

		while(isdigit(*ch_))
			ch_++;

		//	小数部
		if(*ch_ == '.') {
			ch_++;
			if(!isdigit(*ch_))
				return false;

			while(isdigit(*ch_))
				ch_++;
		}

		//	指数部
		if(*ch_ == 'e' || *ch_ == 'E') {
			ch_++;
			if(*ch_ == '+' || *ch_ == '-')
				ch_++;

			if(!isdigit(*ch_))
				return false;

			while(isdigit(*ch_))
				ch_++;
		}

		n = atof(p);
		return true;
	}


	bool parseString(String& s) {
		if(*ch_ != '"')
			return false;

		ch_++;
		while(*ch_ != '"') {
			if(*ch_ == '\0')
				return false;

			if(*ch_ == '\\') {
				//	エスケープ
				ch_++;

				switch(*ch_) {
				case '\\': s += '\\'; break;
				case  '"': s += '\"'; break;
				case  '/': s +=  '/'; break;
				case  'n': s += '\n'; break;
				case  'b': s += '\b'; break;
				case  'f': s += '\f'; break;
				case  'r': s += '\r'; break;
				case  't': s += '\t'; break;
				case  'u':
					//	TODO Unicodeシーケンス
					break;
				default  : return false;
				}
			}
			else {
				s += *ch_;
			}

			ch_++;
		}

		ch_++;
		return true;
	}


	bool parseValue(Value& v) {
		whitespace();

		//	string
		if(*ch_ == '"') {
			String s;
			if(!parseString(s))
				return false;
			v = { s };
			return true;
		}

		//	number
		if(isdigit(*ch_)) {
			double n;
			if(!parseNumber(n))
				return false;
			v = n;
			return true;
		}

		//	null
		if(*ch_ == 'n') {
			if(strncmp(ch_, "null", 4))
				return false;
			v = nullptr;
			ch_ += 4;
			return true;
		}

		//	true
		if(*ch_ == 't') {
			if(strncmp(ch_, "true", 4))
				return false;
			v = true;
			ch_ += 4;
			return true;
		}

		//	false
		if(*ch_ == 'f') {
			if(strncmp(ch_, "false", 5))
				return false;
			v = false;
			ch_ += 5;
			return true;
		}

		//	array
		if(*ch_ == '[') {
			ch_++;
			whitespace();

			Array a;

			while(*ch_ != ']') {
				Value w;
				if(!parseValue(w))
					return false;

				a.push_back(w);
				ch_++;
				whitespace();

				if(*ch_ == ']')
					break;

				if(*ch_ != ',')
					return false;

				ch_++;
				whitespace();
			}

			v = a;
			ch_++;
			return true;
		}

		//	object
		if(*ch_ == '{') {
			Object o;

			ch_++;
			whitespace();

			while(*ch_ != '}') {
				std::string name;
				if(!parseString(name))
					return false;

				whitespace();
				if(*ch_ != ':')
					return false;

				ch_++;
				whitespace();

				Value v;
				if(!parseValue(v))
					return false;

				o.emplace(std::move(name), v);
				whitespace();

				if(*ch_ == '}')
					break;

				if(*ch_ != ',')
					return false;

				ch_++;
				whitespace();
			}

			v = o;
			ch_++;
			return true;
		}

		return false;
	}

public:
	static Value parse(StringRef src, String& error) {
		Parser parser(src);
		Value  value;

		if(parser.parseValue(value)) {
			error = parser.error_;
		}

		return value;
	}
};


Value parse(StringRef src) {
	std::string dummy;
	return parse(src, dummy);
}


Value parse(StringRef src, std::string& error) {
	return Parser::parse(src, error);
}
