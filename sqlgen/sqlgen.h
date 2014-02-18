/*
    Goals: make the language similar to SQL, make it predictable and in the same order.
    Pros:
    - Puts a little bit of SQL syntax enforcement right into the C++ language.
      (use sqlgen incorrectly and it won't compile.  If you use just a string
      for your sql generation, the error will not appear until you run the query.
    - Queries are reusable in a logical/functional way (reuse a sub-query many times, or use the same search criteria)

    TODO:
    - Automatic Formatting
    o Compute/Compute By (select)
    o Other EXEC keywords (WITH RECOMPILE)
    o Union (select)
    o Into clause (select)
    - Create table / alter table / function / procedure / view
*/


#pragma once


#include <string>
#include <vector>
#include <sstream>

namespace SQLGen
{

// 1. Basic declarations
// 1.1. Fragment
// any sql code (that can be converted to string with ToString()) inherits this.  The ONLY
// purpose for it is to provide a common interface to ToString().  Don't attempt to do much
// with a Fragment other than use this one function, or else things might start to conflict.
class __declspec(novtable) Fragment
{
public:
    virtual std::string ToString() const = 0;
};

// 1.2. Expression
class Expression : public Fragment
{
public:
    Expression() {}
    Expression(long op)
    {
        std::ostringstream s;
        s << op;
        m_s = s.str();
    }
    Expression(const Expression& s) : m_s(s.m_s) {}
    explicit Expression(const std::string& s) : m_s(s) {}

    //Expression& operator =(const Expression& r)
    //{
    //    m_s = r.m_s;
    //    return *this;
    //}

    void Assign(const std::string& s)
    {
        m_s = s;
    }

    std::string ToString() const { return m_s; }

    // Some SQL operators can't be represented clearly with C++ operators.
    // for example, where Name LIKE '%sanchez%'.
    // where(lit("Name").like());
    //
    // synonym for LIKE is operator *=
    Expression Like(const Expression& r) const
    {
        std::string n("(");
        n.append(m_s);
        n.append(" like ");
        n.append(r.ToString());
        n.push_back(')');
        return Expression(n);
    }

    // you can use an Expression or Select clause in here.
    template<typename T>
    Expression In(const T& r) const
    {
        Expression temp(*this);
        temp.m_s.append(" in (");
        temp.m_s.append(r.ToString());
        temp.m_s.append(")");
        return temp;
    }

    Expression Some(const Expression& r) const
    {
        Expression temp(*this);
        temp.m_s.append(" some ");
        temp.m_s.append(r.ToString());
        return temp;
    }

    Expression IsNull() const
    {
        std::string n("(");
        n.append(m_s);
        n.append(" is null ");
        n.push_back(')');
        return Expression(n);
    }

    Expression IsNotNull() const
    {
        std::string n("(");
        n.append(m_s);
        n.append(" is not null");
        n.push_back(')');
        return Expression(n);
    }

    // synonym for AS is operator ()   Lit("Employee")("e")
    Expression As(const Expression& r) const
    {
        Expression temp(*this);
        temp.m_s.append(" as ");
        temp.m_s.append(r.ToString());
        return temp;
    }

    Expression As(const std::string& r) const
    {
        Expression temp(*this);
        temp.m_s.append(" as ");
        temp.m_s.append(r);
        return temp;
    }
    Expression operator ()(const Expression& s) const
    {
        return As(s);
    }
    Expression operator ()(const std::string& s) const
    {
        return As(s);
    }

    Expression Asc() const
    {
        Expression temp(*this);
        temp.m_s.append(" asc");
        return temp;
    }

    Expression Desc() const
    {
        Expression temp(*this);
        temp.m_s.append(" desc");
        return temp;
    }

    Expression Between(const Expression& lwr, const Expression& upr) const
    {
        Expression temp(*this);
        temp.m_s.append(" between ");
        temp.m_s.append(lwr.ToString());
        temp.m_s.append(" and ");
        temp.m_s.append(upr.ToString());
        return temp;
    }

protected:
    std::string m_s;
};

inline Expression operator <<(const Expression& lhs, const Expression& rhs)
{
    return Expression(lhs.ToString() + ", " + rhs.ToString());
}

// 1.3. Clause ************************************************************
// This is separate from the Expression class because operator << will treat them differently.
// an Expression is separated by commas, a Clause is separated by just a space.
class Clause : public Fragment
{
public:
    Clause() {}
    Clause(const std::string& s) : m_s(s) {}
    std::string ToString() const { return m_s; }
protected:
    std::string m_s;
};

inline Clause operator <<(const Clause& l, const Clause& w)
{
    return Clause(l.ToString() + " " + w.ToString());
}

// 2. Special Functions

// 2.1.
// Helper functions for SQL functions.  __Func("left", Expression("str"), Expression("1")) will return "left(str, 1)"
inline Expression __Func(const std::string& FunctionName, const Expression& l)
{
    return Expression(FunctionName + "(" + l.ToString() + ")");
}
inline Expression __Func(const std::string& FunctionName, const Expression& l, const Expression& r)
{
    return Expression(FunctionName + "(" + l.ToString() + ", " + r.ToString() + ")");
}
inline Expression __Func(const std::string& FunctionName, const Expression& a, const Expression& b, const Expression& c)
{
    return Expression(FunctionName + "(" + a.ToString() + ", " + b.ToString() + ", " + c.ToString() + ")");
}
inline Expression __Func(const std::string& FunctionName, const Expression& a, const Expression& b, const Expression& c, const Expression& d)
{
    return Expression(FunctionName + "(" + a.ToString() + ", " + b.ToString() + ", " + c.ToString() + ", " + d.ToString() + ")");
}

// 2.2. Lit()
// lit is "literal".  Use it anywhere you need to specify an identifier or literal.
// no translation is performed when constructing the SQL string.
// use it similar to a function:
// 
// select(...) << where(lit("intAge") > lit("18"));
//
// This is like the only inline function that cannot accept expressions for arguments.
// in other words this is legal:   lit(1)
// this is not:                    lit(lit("blah") == lit(1))
template<typename T>
inline Expression Lit(const T& op)
{
    std::ostringstream s;
    s << op;
    return Expression(s.str());
}

// 2.3. LitStr()
// literal string.  This will do the SQL conversion... this will put
// single quotes around it and replace ' with ''.
// use it similar to a function:
// 
// select(...) << where(lit("intName") == LitStr("Henry 'The Snake' GoldBerg"));
// that will get translated to intName = 'Henry ''The Snake'' GoldBerg'
inline Expression LitStr(const std::string& src)
{
    // do a basic find/replace
    std::ostringstream os;
    std::string SearchStr("'");
    std::string Replacement("''");
    std::string::size_type searchLength = SearchStr.length();
    std::string::size_type currentPos = 0;
    std::string::size_type findPos;

    os << "'";

    while(std::string::npos != (findPos = src.find(SearchStr, currentPos)))
    {
        os << src.substr(currentPos, findPos - currentPos);
        os << Replacement;
        currentPos = findPos + searchLength;
    }

    os << src.substr(currentPos) << "'";

    return Expression(os.str());
}

// 2.4. Constant and other misc functions
// where(lit("Name").not(null()));
inline Expression Null() { return Expression("null"); }
inline Expression Default() { return Expression("default"); }
// GroupBy(All());
inline Expression All() { return Expression("all"); }
/*
    Table t1("titles", "t1");
    Table t2("titles", "t2");
    Having(Max(Lit("advance")) >= All(
                Select(
                        Lit(2)*Avg(t2["Advance"])
                    ) <<
                From(Lit("Titles")) <<
                Where(t1["type"] == t2["type"])
            )
        );
*/
inline Expression All(const Expression& e) { return __Func("all", e); }
// Select(AllFields()) << From(...
inline Expression AllFields() { return Expression("*"); }
inline Expression Exists(const Expression& op) { return __Func("exists", op); }
inline Expression Distinct(const Expression& op) { return Expression("Distinct " + op.ToString()); }
inline Expression Not(const Expression& l) { return __Func("not", l); }
// Cast(Lit("Age").As(Lit("String")))
inline Expression Cast(const Expression& l) { return __Func("cast", l); }
inline Expression Grouping(const Expression& l) { return __Func("grouping", l); }


// 3. SQL Functions.
// these are all 1:1 with the SQL equivelants.

// 3.1. AGGREGATE FUNCTIONS ******************************************************
inline Expression Sum(const Expression& op) { return __Func("sum", op); }
inline Expression Count(const Expression& op) { return __Func("count", op); }
inline Expression Avg(const Expression& op) { return __Func("avg", op); }
inline Expression BinaryChecksum(const Expression& op) { return __Func("binary_checksum", op); }
inline Expression Checksum(const Expression& op) { return __Func("checksum", op); }
inline Expression ChecksumAgg(const Expression& op) { return __Func("checksum_agg", op); }
inline Expression CountBig(const Expression& op) { return __Func("count_big", op); }
inline Expression Stdev(const Expression& op) { return __Func("stdev", op); }
inline Expression StdevP(const Expression& op) { return __Func("stdevp", op); }
inline Expression Var(const Expression& op) { return __Func("var", op); }
inline Expression VarP(const Expression& op) {  return __Func("varp", op); }
inline Expression Min(const Expression& op) { return __Func("min", op); }
inline Expression Max(const Expression& op) { return __Func("max", op); }

// 3.2. DATE FUNCTIONS ******************************************************
inline Expression GetDate(){ return Expression("getdate()"); }
inline Expression GetUTCDate() { return Expression("getutcdate()"); }
// Where(Lit("ActDate") > DateAdd("year", Lit(1), GetDate()));
inline Expression DateAdd(const std::string& dp, const Expression& n, const Expression& d) { return __Func("dateadd", Expression(dp), n, d); }
inline Expression DateDiff(const std::string& dp, const Expression& s, const Expression& e) { return __Func("datediff", Expression(dp), s, e); }
inline Expression DateName(const std::string& dp, const Expression& d) { return __Func("datename", Expression(dp), d); }
inline Expression DatePart(const std::string& dp, const Expression& d) { return __Func("datepart", Expression(dp), d); }
inline Expression Month(const Expression& date) { return __Func("month", date); }
inline Expression Day(const Expression& date) { return __Func("day", date); }
inline Expression Year(const Expression& date) { return __Func("year", date); }

// 3.3. MATH FUNCTIONS ******************************************************
inline Expression Abs(const Expression& v) { return __Func("abs", v); }
inline Expression Degrees(const Expression& v) { return __Func("degrees", v); }
inline Expression Rand(const Expression& v) { return __Func("rand", v); }
inline Expression Rand() { return Expression("rand()"); }
inline Expression ACos(const Expression& v) { return __Func("acos", v); }
inline Expression Exp(const Expression& v) { return __Func("exp", v); }
inline Expression Round(const Expression& v, const Expression& length) { return __Func("round", v, length); }
inline Expression Round(const Expression& v, const Expression& length, const Expression& fn) { return __Func("abs", v, length, fn); }
inline Expression ASin(const Expression& v) { return __Func("asin", v); }
inline Expression Floor(const Expression& v) { return __Func("floor", v); }
inline Expression Sign(const Expression& v) { return __Func("sign", v); }
inline Expression ATan(const Expression& v) { return __Func("atan", v); }
inline Expression Log(const Expression& v) { return __Func("log", v); }
inline Expression Sin(const Expression& v) { return __Func("sin", v); }
inline Expression Atn2(const Expression& l, const Expression& r) { return __Func("atn2", l, r); }
inline Expression Log10(const Expression& v) { return __Func("log10", v); }
inline Expression Square(const Expression& v) { return __Func("square", v); }
inline Expression Ceiling(const Expression& v) { return __Func("ceiling", v); }
inline Expression Pi() { return Expression("pi()"); }
inline Expression Sqrt(const Expression& v) { return __Func("sqrt", v); }
inline Expression Cos(const Expression& v) { return __Func("cos", v); }
inline Expression Power(const Expression& v, const Expression& p) { return __Func("power", v, p); }
inline Expression Tan(const Expression& v) { return __Func("tan", v); }
inline Expression Cot(const Expression& v) { return __Func("cot", v); }
inline Expression Radians(const Expression& v) { return __Func("radians", v); }

// 3.4. STRING FUNCTIONS ******************************************************
inline Expression ASCII(const Expression& v) { return __Func("ascii", v); }
inline Expression NChar(const Expression& v) { return __Func("nchar", v); }
inline Expression SoundEx(const Expression& v) { return __Func("soundex", v); }
inline Expression Char(const Expression& v) { return __Func("char", v); }
inline Expression PatIndex(const Expression& v, const Expression& r) { return __Func("patindex", v, r); }
inline Expression Space(const Expression& v) { return __Func("ascii", v); }
inline Expression CharIndex(const Expression& a, const Expression& b, const Expression& start) { return __Func("charindex", a, b, start); }
inline Expression CharIndex(const Expression& a, const Expression& b) { return __Func("charindex", a, b); }
inline Expression Replace(const Expression& a, const Expression& b, const Expression& c) { return __Func("replace", a, b, c); }
inline Expression Str(const Expression& a, const Expression& b, const Expression& c) { return __Func("str", a, b, c); }
inline Expression Str(const Expression& v, const Expression& b) { return __Func("str", v, b); }
inline Expression Str(const Expression& v) { return __Func("str", v); }
inline Expression Difference(const Expression& a, const Expression& b) { return __Func("difference", a, b); }
inline Expression QuoteName(const Expression& v, const Expression& b) { return __Func("quotename", v, b); }
inline Expression QuoteName(const Expression& v) { return __Func("quotename", v); }
inline Expression Stuff(const Expression& a, const Expression& b, const Expression& c, const Expression& d) { return __Func("stuff", a, b, c, d); }
inline Expression Left(const Expression& v, const Expression& l) { return __Func("left", v, l); }
inline Expression Replicate(const Expression& v, const Expression l) { return __Func("replicate", v, l); }
inline Expression SubString(const Expression& v, const Expression s, const Expression l) { return __Func("substring", v, s, l); }
inline Expression Len(const Expression& v) { return __Func("len", v); }
inline Expression Reverse(const Expression& v) { return __Func("reverse", v); }
inline Expression Unicode(const Expression& v) { return __Func("unicode", v); }
inline Expression Lower(const Expression& v) { return __Func("lower", v); }
inline Expression Right(const Expression& v, const Expression& l) { return __Func("right", v, l); }
inline Expression Upper(const Expression& v) { return __Func("upper", v); }
inline Expression LTrim(const Expression& v) { return __Func("ltrim", v); }
inline Expression RTrim(const Expression& v) { return __Func("rtrim", v); }


// 4. Operators

// 4.1. this is a helper function that "joins" two operands with Tstr
inline Expression __BinaryOperator(const std::string& s, const Expression& l, const Expression& r)
{
    return Expression("(" + l.ToString() + " " + s + " " + r.ToString() + ")");
};
// 4.2. Comparisons
inline Expression operator ==(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("=", lhs, rhs); }
inline Expression operator <(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("<", lhs, rhs); }
inline Expression operator >(const Expression& lhs, const Expression& rhs) { return __BinaryOperator(">", lhs, rhs); }
inline Expression operator <=(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("<=", lhs, rhs); }
inline Expression operator >=(const Expression& lhs, const Expression& rhs) { return __BinaryOperator(">=", lhs, rhs); }
inline Expression operator !=(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("<>", lhs, rhs); }

// 4.3. Arithmetic
inline Expression operator *(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("*", lhs, rhs); }
inline Expression operator /(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("/", lhs, rhs); }
inline Expression operator +(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("+", lhs, rhs); }
inline Expression operator -(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("-", lhs, rhs); }
inline Expression operator %(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("%", lhs, rhs); }

// 4.4. Bitwise
inline Expression operator &(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("&", lhs, rhs); }
inline Expression operator |(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("|", lhs, rhs); }
inline Expression operator ^(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("^", lhs, rhs); }

// 4.5. Logical
inline Expression operator &&(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("and", lhs, rhs); }
inline Expression operator ||(const Expression& lhs, const Expression& rhs) { return __BinaryOperator("or", lhs, rhs); }


// 5. User defined functions
// 5.1. Function that takes arguments.
inline Expression Func(const std::string& name, const Expression& a)
{
    return Expression(name + "(" + a.ToString() + ")");
}

// 5.2. Function that doesn't take any arguments.
inline Expression Func(const std::string& name)
{
    return Expression(name + "()");
}

// 6. User Stored Procedures
// same thing, for stored procedures.  this is a class so it can be treated as a special type
// since stored procedures aren't really expressions or complete statements or anything.
// Exec(Proc("sp_blah", Args() << Lit(5) << Lit("hi")));
class Proc : public Fragment
{
public:
    // Statement s = Exec(Proc("sp_GetDateFormat", Args() << GetDate()));
    Proc(const std::string& name, const Expression& a)
    {
        m_s = name + "(" + a.ToString() + ")";
    }
    // Statement s = Exec(Proc("sp_GetDateFormat");
    Proc(const std::string& name)
    {
        m_s = name + "()";
    }
    std::string ToString() const
    {
        return m_s;
    }
private:
    std::string m_s;
};


// 7. Case When Then Else End //////////////////////////////////////////////////////////////////////////////////
//Case(                                                   Case
//    When(Lit("type") = Lit(1), Str("Insert")) <<            When (type = 1) Then "Insert"
//    When(Lit("type") = Lit(2), Str("Update")) <<            When (type = 2) Then "Update"
//    Else(Str("Unknown"))                                    Else 0
//    )                                                       End

// 7.1. when (criteria) then (result)
// When([expression], [expression]) << When(...) << When(...)
class When : public Fragment
{
public:
    When(const Expression& crit, const Expression& result)
    {
        m_s = "when " + crit.ToString() + " then " + result.ToString();
    }
    When(const When& r)
    {
        m_s = r.m_s;
    }
    std::string ToString() const { return m_s; }
private:
    std::string m_s;
};

// 7.2. This can be used almost identically to When().
// When([expression], [expression]) << When(...) << When(...) << Else([expression])
class Else : public Fragment
{
public:
    Else(const Expression& result)
    {
        m_s = "else " + result.ToString();
    }
    Else(const Else& r)
    {
        m_s = r.m_s;
    }
    std::string ToString() const { return m_s; }
private:
    std::string m_s;
};

// 7.3 Similar to the Args() class but this is for the CASE-WHEN-ELSE thing.
// Represents a chain of sections, and an optional else section.
class CaseSections : public Fragment
{
public:
    CaseSections() { }
    CaseSections(const CaseSections& r)
    {
        m_s = r.m_s;
    }

    CaseSections(const When& r)
    {
        m_s = r.ToString();
    }

    void Add(const std::string& w)
    {
        if(m_s.empty())
        {
            m_s = w;
        }
        else
        {
            m_s.append(" ");
            m_s.append(w);
        }
    }
    void Add(const When& w) { Add(w.ToString()); }
    void Add(const Else& w) { Add(w.ToString()); }
    std::string ToString() const
    {
        return m_s;
    }
private:
    std::string m_s;
};

// 7.4. Operators to add WHEN phrases to the thingy.
// this will be used when there are more than 2 When() Expressions in a row
inline CaseSections& operator <<(CaseSections& lhs, const When& rhs)
{
    lhs.Add(rhs);
    return lhs;
}
// this will be used for the first 2 When() Expressions
inline CaseSections operator <<(const When& lhs, const When& rhs)
{
    CaseSections r(lhs);
    r.Add(rhs);
    return r;
}
// This is used if there just 1 When() and 1 Else()
inline CaseSections operator <<(const When& lhs, const Else& rhs)
{
    CaseSections r(lhs);
    r.Add(rhs);
    return r;
}
// This is used when Else() is at the end of >1 When() Expressions
inline CaseSections& operator <<(CaseSections& lhs, const Else& rhs)
{
    lhs.Add(rhs);
    return lhs;
}

// 7.5. Puts everything together.
// Case(
//    When(Lit("@a")==Lit(1), LitStr("Choice #1")) <<
//    When(Lit("@a")==Lit(2), LitStr("Choice #2")) <<
//      );
inline Expression Case(const CaseSections& sections)
{
    return Expression("case " + sections.ToString() + " end");
}

// 8. Misc Clauses.  These are the major "clauses" in a sql statement, all inheriting from Clause.

// 8.1. Where.
// Select(...) << From(...) << Where(Lit("Name") == LitStr("John"));
class Where : public Clause
{
public:
    explicit Where(const Expression& r)
    {
        m_s = "where " + r.ToString();
    }
};

// 8.2. Having Clause.
// Select(...) << From(...) << GroupBy(...) << Having(Lit("Name") == LitStr("John"));
class Having : public Clause
{
public:
    explicit Having(const Expression& r)
    {
        m_s = "having " + r.ToString();
    }
};

// 8.3. From Clause
// 8.3.1. Table class.  For use in the FROM clause.
// Tables are yet another type of code Fragment which are "joined" in yet another way
// for instance, Expressions are joined (by the << operator) wtih a comma, Clauses are
// joined with a space - and tables are joined a little more complexly (is that a word?)
// consider Table P("People").As("P"); and Table C("Cities").As("C").
// We want to left outer join these suckas. Here are the ways to do it:
// in SQL:           People P left join Cities C on P.City = C.ID
// Using operator A: 
//                   Table P("People").As("P");
//                   Table C("Cities").As("C");
//                   P << C.On(P["City"] == C["ID"]);
// Using operator B:
//                   Table("People").As("P") << Table("Cities").As("C").On(Lit("P.City") == Lit("C.ID"));
// Using functions:
//                   Table P("People").As("P");
//                   Table C("Cities").As("C");
//                   P.LeftJoin(C.On(P["City"] == C["ID"]));
// Other join operators:
// P << C         : Left Join
// P == C         : Inner Join
// C >> P         : Right Join
class Table : public Fragment
{
public:
    explicit Table(const std::string& s) : m_s(s), m_Alias(s) { }
    explicit Table(const std::string& s, const std::string& sAlias)
    {
        m_s = s + " as " + sAlias;
        m_Alias = sAlias;
    }
    explicit Table(const Expression& s) : m_s(s.ToString()) { }
    Table(const Table& s) : m_s(s.m_s), m_Alias(s.m_Alias) { }

    Expression operator [](const std::string& col)
    {
        if(m_Alias.empty()) return Expression(col);
        return Expression(m_Alias + "." + col);
    }
    // synonym for AS is operator ()   Table("Employee")("e")
    Table As(const Expression& s) const
    {
        Table r(*this);
        r.m_s.append(" as ");
        r.m_s.append(s.ToString());
        return r;
    }
    Table As(const std::string& s) const
    {
        Table r(*this);
        r.m_s.append(" as ");
        r.m_s.append(s);
        return r;
    }
    Table operator ()(const Expression& s) const
    {
        return As(s);
    }
    Table operator ()(const std::string& s) const
    {
        return As(s);
    }
    // operator <<
    Table LeftJoin(const Table& t) const
    {
        return Table("(" + m_s + " left join " + t.ToString() + ")");
    }
    // operator >>
    Table RightJoin(const Table& t) const
    {
        return Table("(" + m_s + " right join " + t.ToString() + ")");
    }
    // operator ==
    Table InnerJoin(const Table& t) const
    {
        return Table("(" + m_s + " inner join " + t.ToString() + ")");
    }
    Table FullOuterJoin(const Table& t) const
    {
        return Table("(" + m_s + " full outer join " + t.ToString() + ")");
    }
    Table CrossJoin(const Table& t) const
    {
        return Table("(" + m_s + " cross join " + t.ToString() + ")");
    }

    // synonym for ON is operator % Table("Employees")("e") << Table("sales")("s") % (Lit("e.ID") == Lit(""))
    // because I am using operator <<, >>, and == for joins, make sure the ON operator is
    // of HIGHER precedence so you aren't forced to use parenthesis.
    // also make sure that it's LOWER precedence than operator (), which is used for AS()
    Table On(const Expression& e) const
    {
        Table r(*this);
        r.m_s.append(" on ");
        r.m_s.append(e.ToString());
        return r;
    }

    std::string ToString() const { return m_s; }
private:
    std::string m_s;
    std::string m_Alias;
};

// 8.3.2 Join operators.
inline Table operator <<(const Table& lhs, const Table& rhs)
{
    return lhs.LeftJoin(rhs);
}

inline Table operator >>(const Table& lhs, const Table& rhs)
{
    return lhs.RightJoin(rhs);
}

inline Table operator ==(const Table& lhs, const Table& rhs)
{
    return lhs.InnerJoin(rhs);
}

// 8.3.3 From clause - can take either a table definition or an expression (in the case where you're
// selecting from a local table or user defined function or something)
class From : public Clause
{
public:
    explicit From(const Table& r)
    {
        m_s = "from " + r.ToString();
    }
    explicit From(const Expression& r)
    {
        m_s = "from " + r.ToString();
    }
};

// 8.4 GROUP BY clause.  Use the standard expression joiner (operator <<) to group by multiple
// fields.
// Select(Lit("Name")) << GroupBy(Lit("Name") << Lit("City")).WithRollup()
class GroupBy : public Clause
{
public:
    explicit GroupBy(const Expression& r)
    {
        m_s = "group by " + r.ToString();
    }
    GroupBy(const GroupBy& r)
    {
        m_s = r.m_s;
    }
    GroupBy WithRollup()
    {
        GroupBy r(*this);
        r.m_s.append(" with rollup");
        return r;
    }
    GroupBy WithCube()
    {
        GroupBy r(*this);
        r.m_s.append(" with cube");
        return r;
    }
};

// 8.5. ORDER BY clause.
// Select(..) << From(..) << OrderBy(Lit("Name").Asc() << Lit("City") << Lit("Address").Desc());
class OrderBy : public Clause
{
public:
    explicit OrderBy(const Expression& r)
    {
        m_s = "order by " + r.ToString();
    }
};

// 8.5. SELECT clause.
// This isn't the whole STATEMENT, just teh clause. The Select clause is just the columns to
// select out of the returned table.  Select(Lit("Name") << Lit(1)) evaluates to Select Name, 1
class Select : public Clause
{
public:
    explicit Select(const Expression& r)
    {
        m_s = "select " + r.ToString();
    }
};


// 8.6. Values
// 8.6.1. Value Class.
// Now unlike ALL other clauses up till here we just store a "dumb"
// string and modify it as we go.  Because of the lame syntax of the INSERT INTO
// statement, we'll have to actually store the specifics of the thing
// and construct the string later.
// Insert(Table("Customers")) << Value(col, expr) << Value(col, expr)
// The Values class will keep a list of these.
class Value
{
public:
    // Value("EmployeeName", LitStr("Jack O'Brian"))
    Value(const std::string& col, const Expression& val)
    {
        m_col = col;
        m_val = val.ToString();
    }
    std::string GetColumn() const{ return m_col; }
    std::string GetValue() const{ return m_val; }
private:
    std::string m_col;
    std::string m_val;
};

// 8.6.2. ValueList class
// a non-visible list of Values.  This object gets created by Values operator << (Value&, Value&);
// and Values operator << (Values&, Value&);
// this is used by both the UPDATE and INSERT clauses
class ValueList
{
public:
    ValueList() {}
    explicit ValueList(const Value& v)
    {
        m_list.push_back(v);
    }
    ValueList(const ValueList& v)
    {
        m_list = v.m_list;
    }
    void Add(const Value& v)
    {
        m_list.push_back(v);
    }
    // INSERT mytable [ (a,b,c) Values (a,b,c) ]
    std::string ToInsertString() const
    {
        std::string names_("(");
        std::string values_(") Values (");
        std::vector<Value>::const_iterator it;
        for(it = m_list.begin(); it != m_list.end(); it ++)
        {
            if(it != m_list.begin())
            {
                names_.append(", ");
                values_.append(", ");
            }
            names_.append(it->GetColumn());
            values_.append(it->GetValue());
        }
        return names_ + values_ + ")";
    }
    // UPDATE mytable set [a=a,b=b,c=c] where
    std::string ToUpdateString() const
    {
        std::string r;
        std::vector<Value>::const_iterator it;
        for(it = m_list.begin(); it != m_list.end(); it ++)
        {
            if(it != m_list.begin())
            {
                r.append(", ");
            }
            r.append(it->GetColumn());
            r.append("=");
            r.append(it->GetValue());
        }
        return r;
    }
private:
    std::vector<Value> m_list;
};

// 8.6.3 Append operator for values list
// this will be called for the first 2 values
// Insert("mytable") << Value(...) << Value(...);
inline ValueList operator <<(const Value& l, const Value& r)
{
    ValueList ret(l);
    ret.Add(r);
    return ret;
}

// 8.6.4 Append operator for values list
// this will be called for the 3rd+ value
// Insert(myt) << Value("blah","blah") << Value("v2","v2") << Value("v3","v3"))
inline ValueList& operator <<(ValueList& l, const Value& r)
{
    l.Add(r);
    return l;
}

// 8.7. INSERT clause
// 8.7.1. class Insert
// Insert(Table("people"), Value("name", LitStr("Carl")) << Value("Age", Lit(5)));
/*
    I have decided to offer 2 different ways to do this.
    a: Insert(table, value << value << value);
    b: Insert(table) << value << value << value;
*/
class Insert : public Clause
{
public:
    explicit Insert(const Table& t, const ValueList& r) :
        m_v(r)
    {
        m_s = "insert " + t.ToString() + " ";
    }
    explicit Insert(const Table& t)
    {
        m_s = "insert " + t.ToString() + " ";
    }
    std::string ToString() const
    {
        return m_s + m_v.ToInsertString();
    }
    void Add(const Value& v)
    {
        m_v.Add(v);
    }
private:
    std::string m_s;
    ValueList m_v;
};

// 8.7.2 This appends values to the insert statement.
inline Insert& operator <<(Insert& l, const Value& r)
{
    l.Add(r);
    return l;
}

// 8.8. Update clause
// 8.8.1. Update class
// Update(Table(...)) << Value(...,...) << Value(..,..) << Where(...);
class Update : public Clause
{
public:
    explicit Update(const Table& t) :
        m_bFirstValue(true)
    {
        m_s = "update " + t.ToString();
    }
    void Add(const Value& v)
    {
        if(m_bFirstValue)
        {
            m_s.append(" set ");
            m_bFirstValue = false;
        }
        else
        {
            m_s.append(", ");
        }
        m_s.append(v.GetColumn());
        m_s.append(" = ");
        m_s.append(v.GetValue());
    }
    std::string ToString() const
    {
        return m_s;
    }
private:
    bool m_bFirstValue;
    std::string m_s;
};

// 8.8.2 This appends values to the update statement.
inline Update& operator <<(Update& l, const Value& r)
{
    l.Add(r);
    return l;
}


// 9. Statement
// "Statement" is a catch-all class that can be used to pass sql statements around,
// or store them.  Really nothing in the sql namespace should use this, since everything
// here operates on sub-divisions of a statement (Clause, Expression, etc)
// There is no operator << for statements.
class Statement
{
public:
    Statement() {}

    // (see below for the "revised story" to spare you from reading this whole thing)

    // now this is a little interesting.  There are two sides of this story.
    // 1) I need to be able to convert all SQL stuff to a statement with NOT 'explicit',
    //    so client apps can use Expression and statement interchangeably for converting
    //    to string
    // 2) We can't be too liberal with non-explicit conversions, or devs will get confused
    //    about usage.  For instance:
    //    Exec(Lit("@a") + Lit("@b"));  <-- this looks perfectly legit.. this should
    //    evaluate to EXEC (@a+@b), which is wonderful.  Except what REALLY happens is that
    //    Lit()+Lit() expression gets implicitly converted to a Statement, and it's treated
    //    like a dynamic sql statement - EXEC ('(@a+@b)').  That's not very intuitive, and in
    //    order for Exec() to be able to make the distinction I would have to make a new set
    //    of Lit()-type functions.
    //
    //    Here's another example where implicit conversion screws things up:
    //    Exec("@a + @b").  This is ambiguous because it could be either Exec(Statement) or Exec(String)
    // But all in all I will choose to allow implicit conversions as that's probably the most
    // common usage, and the exceptions are few, far between, and are easy to spot and fix.
    /*
        ** OK here's the revised story.  In order to solve the problem where Expressions and other
        sql code was being too liberally implicitly converted, I made all stuff wtih "ToString()" 
        inherit a tiny base class that has virtual ToString().  This makes it explicit that Statement
        is being constructed from another sql code fragment.  It can also be EXPLICITLY constructed
        from a string.
    */
    Statement(const Fragment& r)
    {
        Assign(r);
    }
    explicit Statement(const std::string& r)
    {
        Assign(r);
    }

    std::string GetText() const
    {
        return m_s;
    }

    Statement& operator =(const Fragment& r)
    {
        return Assign(r);
    }

    Statement& Assign(const Fragment& r)
    {
        m_s = r.ToString();
        return *this;
    }

    Statement& Assign(const std::string& r)
    {
        m_s.assign(r);
        return *this;
    }

private:
    std::string m_s;
};

// 10. OpenQuery
// OK so I lied - this is like the only "internal" thing that uses Statement
// because it deals with entire sql statements for external servers.
// OpenQuery cannot accept expressions or variables as arguments.
//
// example:
// Statement remote = Select(AllFields()) << From(Table("SomeRemoteTable")) << Where(Lit("Name").Like(LitStr("O'Malley")));
// Statement local = Select(AllFields()) << From(OpenQuery("Gemini8.microsoft.com", remote));
//
// That evaluates to:
// 
// select * from openquery(Gemini8.microsoft.com, 'select * from SomeRemoteTable where Name like ''O''''Malley''')
//
// Note how the quotes had to be converted twice.
inline Table OpenQuery(const std::string& server, const Statement& s)
{
    return Table(std::string("openquery(") + server + ", " + LitStr(s.GetText()).ToString() + ")");
}


// 11. EXEC
// 11.1. EXEC with dynamic sql.
// In SQL:     EXEC ('Select * from mytable')
// In SQLGen:  Exec(Select(AllFields()) << From(Table("mytable")));
inline Statement Exec(const Statement& s)
{
    return Statement(std::string("exec (") + LitStr(s.GetText()).ToString() + ")");
}
// 11.2 EXEC with dynamic sql, using local variables
// In SQL:     EXEC (@a+@b+@c)
// In SQLGen:  Exec("@a+@b+@c");
// dynamic sql here too, for use with local string variables.
// I can't use an Expression here because EXEC doesn't really take an expression, just a 
// string literal, with the option of putting variables in there.
// in other words, EXEC (@a+(@b+@c)) is illegal, where EXEC (@a+@b+@c) is legal.
// lame sql technicality I guess.
// Exec("@a + @b");
inline Statement Exec(const std::string& s)
{
    return Statement(Expression(std::string("exec (") + s + ")"));
}
// 11.3 EXEC with stored procedure.
// In SQL:     EXEC sp_blah(5, Left(@a, 7))
// In SQLGen:  Exec(Proc("sp_blah", Lit(5) << Left(Lit("@a"), Lit(7))));
inline Statement Exec(const Proc& s)
{
    return Statement(Expression(std::string("exec ") + s.ToString()));
}


}


namespace SQLGenX
{
// Just helper stuff

// synonym for Lit()
template<typename T>
inline SQLGen::Expression l(const T& op)
{
    return SQLGen::Lit(op);
}

// synonym for LitStr()
inline SQLGen::Expression s(const std::string& src)
{
    return SQLGen::LitStr(src);
}

// synonym for LitStr()
inline SQLGen::Expression str(const std::string& src)
{
    return SQLGen::LitStr(src);
}

typedef SQLGen::Table t;// synonym for Table()
typedef SQLGen::Value v;// synonym for Value()
inline SQLGen::Expression Star() { return SQLGen::AllFields(); }// synonym for AllFields()


// SQL:
// Update People set Name='Jeff Lebowsky', Age=16 Where ID=125

// SQLGen:
// Update(Table("People")) << Value("Name", LitStr("Jeff Lebowsky")) << Value("Age", Lit(16)) << Where(Lit("ID") == Lit(125));

// SQLGenX:
// Update(t("People")) << v("Name", s("Jeff Lebowsky")) << v("Age", l(16)) << Where(l("ID") == l(125));

};
