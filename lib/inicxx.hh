#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

#define INI_GLOBAL ""

#ifdef INI_DONT_USE_EXCEPTIONS
#	define INI_OK    0
#	define INI_ERROR -1
#endif

#define __INI_TYPE_NONE    0
#define __INI_TYPE_SECTION 1
#define __INI_TYPE_KEY     2
#define __INI_TYPE_VALUE   3

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;

typedef size_t   word;
typedef intmax_t sword;

namespace INI {
	typedef std::map<std::string, std::string> Section;
	typedef std::map<std::string, Section> Sections;

#ifndef INI_DONT_USE_EXCEPTIONS
	class Exception {
		public:
			Exception(const std::string& p_Message): 
				m_Message(p_Message) 
			{};
			
			~Exception() {};

			const std::string& What() const {
				return m_Message;
			};
			
		private:
			std::string m_Message;
	};
	
	class ParserException: public Exception {
		public:
			ParserException(const std::string& p_Message, word p_Line): 
				Exception(p_Message),
				m_Line(p_Line)
			{};
			
			~ParserException() {};
			
			word Line() const {
				return m_Line;
			};
		
		private:
			word m_Line;
	};
#endif

	class Structure {
		public:
#ifdef INI_DONT_USE_EXCEPTIONS
			Structure():
				m_Line(0),
				m_ErrorMessage("")
			{};
#else
			Structure() {};
#endif

			~Structure() {};
			
#ifdef INI_DONT_USE_EXCEPTIONS
			Structure(const Sections& p_Sects):
				m_Line(0),
				m_ErrorMessage("")
#else
			Structure(const Sections& p_Sects)
#endif
			{
				m_Sects = p_Sects;
			};
			
			Structure& operator=(const Structure& p_Structure) {
				m_Sects = p_Structure.m_Sects;
				
				return *this;
			};

			Section& operator[](const std::string& p_Idx) {
				return m_Sects[p_Idx];
			};
			
			Section& At(const std::string& p_Idx) {
				return m_Sects.at(p_Idx);
			};
			
			std::string& At(const std::string& p_IdxA, const std::string& p_IdxB) {
				return m_Sects.at(p_IdxA).at(p_IdxB);
			};

			bool Contains(const std::string& p_Idx) const {
				return m_Sects.count(p_Idx);
			};
			
			bool Contains(const std::string& p_IdxA, const std::string& p_IdxB) const {
				if (m_Sects.count(p_IdxA))
					if (m_Sects.at(p_IdxA).count(p_IdxB))
						return true;

				return false;
			};

			void Clear() {
				m_Sects.clear();
			};
			
			void Clear(const std::string& p_Idx) {
				m_Sects[p_Idx].clear();
			};

			double AsNumber(const std::string& p_IdxA, const std::string& p_IdxB) {
				return std::stod(m_Sects[p_IdxA][p_IdxB]);
			};
			
			sword AsInteger(const std::string& p_IdxA, const std::string& p_IdxB) {
				return std::stoi(m_Sects[p_IdxA][p_IdxB]);
			};
			
			std::string AsString(const std::string& p_IdxA, const std::string& p_IdxB) {
				return m_Sects[p_IdxA][p_IdxB];
			};

#ifdef INI_DONT_USE_EXCEPTIONS
			i8 AsBoolean(const std::string& p_IdxA, const std::string& p_IdxB) {
#else
			bool AsBoolean(const std::string& p_IdxA, const std::string& p_IdxB) {
#endif
				const std::string& Val = m_Sects[p_IdxA][p_IdxB];

				if (Val == "true")
					return true;
				else if (Val == "false")
					return false;

#ifdef INI_DONT_USE_EXCEPTIONS
				m_ErrorMessage = "Attempt to use ASBoolean() on a non-boolean value";
				
				return INI_ERROR;
#else
				throw Exception("Attempt to use AsBoolean() on a non-boolean value");
#endif
			};

#ifdef INI_DONT_USE_EXCEPTIONS
			const std::string& GetErrorMessage() const {
				return m_ErrorMessage;
			};

			word GetErrorLine() const {
				return m_Line;
			};
#endif

			std::string Stringify() const {
				std::string Stringified = "";

				for (const std::pair<const std::string&, const Section&>& Sect : m_Sects) {
					if (Sect.first[0] == ' ' and Sect.first.back() == ' ')
						Stringified += "[\"" + INIEscape(Sect.first) + "\"]\n";
					else
						Stringified += "[" + INIEscape(Sect.first) + "]\n";
					
					for (const std::pair<const std::string&, const std::string&> &Val : Sect.second) {
						if (Val.first[0] == ' ' and Val.first.back() == ' ')
							Stringified += "\"" + INIEscape(Val.first) + "\"";
						else
							Stringified += INIEscape(Val.first);

						Stringified += "=";

						if (Val.second[0] == ' ' and Val.second.back() == ' ')
							Stringified += "\"" + INIEscape(Val.second) + "\"\n";
						else
							Stringified += INIEscape(Val.second) + "\n";
					};
				};
				
				return Stringified;
			};

#ifdef INI_DONT_USE_EXCEPTIONS
			ui8 Parse(const std::string& p_Text) {
#else
			void Parse(const std::string& p_Text) {
#endif
				Clear();
				
				std::string Line = "";
				std::string CurrentSect = INI_GLOBAL;
				word CurrentLine = 0;

				for (const char &ch : p_Text + "\n") {
					switch (ch) {
						case '\n': {
							++ CurrentLine;
							Trim(Line);
							
							switch (Line[0]) {
								case ';': case '#': break;

								case '[': {
									word Pos = Line.find_first_of("]");

									if (Pos == std::string::npos) {									
#ifdef INI_DONT_USE_EXCEPTIONS
										ErrorMessage = "Sector name closing expected";
										
										return INI_ERROR;
#else
										throw ParserException("Sector name closing expected", CurrentLine);	
#endif
									};
									
									CurrentSect = Line.substr(1, Pos - 1);
									Trim(CurrentSect);
									CheckApostrophes(CurrentSect);
									
									break;	
								};
								
								default: {
									if (Line == "")
										break;

									word Pos = 0;

									do {
										Pos = Line.find_first_of("=", Pos + 1);
										
										if (Pos == std::string::npos) {									
#ifdef INI_DONT_USE_EXCEPTIONS
											ErrorMessage = "Expected assignment";
											
											return INI_ERROR;
#else
											throw ParserException("Expected assignment", CurrentLine);	
#endif
										};
									} while (Line[Pos - 1] == '\\');
									
									std::string Key = Line.substr(0, Pos);
									std::string Value = "";
									Line = Line.substr(Pos + 1);

									if (INIUnescape(std::string(Key), Key)) {
#ifdef INI_DONT_USE_EXCEPTIONS
										ErrorMessage = "Expected assignment";
										
										return INI_ERROR;
#else
										throw ParserException("Expected assignment", CurrentLine);	
#endif
									};
									
									INIUnescape(Line, Value);

									Trim(Key); Trim(Value);
									CheckApostrophes(Key); CheckApostrophes(Value);
									
									if (Key == "") {
#ifdef INI_DONT_USE_EXCEPTIONS
										ErrorMessage = "Key name expected";
										
										return INI_ERROR;
#else
										throw ParserException("Key name expected", CurrentLine);	
#endif
									};

									m_Sects[CurrentSect][Key] = Value;
			
									break;
								};
							};

							Line = "";

							break;
						};
						
						default: Line += ch; break;
					};
				};

#ifdef INI_DONT_USE_EXCEPTIONS
				return INI_OK;
#endif
			};

		private:
			void Trim(std::string &p_Str) { 
				word Start = p_Str.find_first_not_of(" \t");
				
				if (Start == std::string::npos) {
					p_Str = "";

					return;
				};
				
				word End = p_Str.find_last_not_of(" \t");

				p_Str = p_Str.substr(Start, End + 1 - Start);
			};

			void CheckApostrophes(std::string &p_Str) {
				if (p_Str[0] == '"' and p_Str.back() == '"')
					p_Str = p_Str.substr(1, p_Str.length() - 2);
			};
			
			std::string INIEscape(const std::string &p_Str) const {
				std::string Escaped = "";

				for (word i = 0; i < p_Str.length(); ++ i)
					switch (p_Str[i]) {
						case '\n': Escaped += "\\n"; break;
						case '\r': Escaped += "\\r"; break;
						case '\t': Escaped += "\\t"; break;
						case '\f': Escaped += "\\f"; break;
						case '\b': Escaped += "\\b"; break;
						case '\a': Escaped += "\\a"; break;
						case '\0': Escaped += "\\0"; break;
						case '"': Escaped += "\\\""; break;

						default: Escaped += p_Str[i]; break;
					};

				return Escaped;
			};

			bool INIUnescape(const std::string &p_Line, std::string &p_Str) {
				bool Escape = false;
				p_Str = "";
				
				for (const char &ch : p_Line) {
					if (Escape) {
						Escape = false;
						
						switch (ch) {
							case 'n':  p_Str += '\n'; break;
							case 'r':  p_Str += '\r'; break;
							case 't':  p_Str += '\t'; break;
							case 'f':  p_Str += '\f'; break;
							case 'b':  p_Str += '\b'; break;
							case 'a':  p_Str += '\a'; break;
							case '0':  p_Str += '\0'; break;
							case '"':  p_Str += '\"'; break;
							case '\\': p_Str += '\\'; break;
							case ';':  p_Str += ';';  break;
						};
					};

					switch (ch) {
						case ';': case '#': return true;
						case '\\': Escape = true; break;
						default: p_Str += ch; break;
					};
				};

				return false;
			};

			Sections m_Sects;

#ifdef INI_DONT_USE_EXCEPTIONS
			std::string m_ErrorMessage;
			word m_Line;
#endif
	};
};
