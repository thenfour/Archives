

#include "stdafx.h"
#include "sqlgen.h"
#include <tchar.h>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
void d(const T& s)
{
    std::cout << s.ToString() << std::endl;
}
template<>
void d<SQLGen::Statement>(const SQLGen::Statement& s)
{
    std::cout << s.GetText() << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
    using namespace SQLGen;
    {
        d(Lit("shit").IsNotNull());
    d(  Lit("tblCustomers")                                     );//tblCustomers
    d(  Lit(5)                                                  );//5
    d(  LitStr("Jeff 'The Dude' Lebowsky")                      );//'Jeff ''The Dude'' Lebowsky'
    d(  Null()                                                  );//null
    d(  Sum(Lit("intSales"))                                    );//sum(intSales)
    d(  Count(Lit("CustomerID"))                                );//count(CustomerID)
    d(  Lit("vchrCustomerName") == LitStr("Henry VIII")         );//(vchrCustomerName = 'Henry VIII')
    d(  Sum(Lit("intSales")) + Lit(5) * Sum(Lit("Month"))       );//(sum(intSales) + (5 * sum(Month)))
    d(  (Lit("a") == Lit("b")) && (Lit("c") == Lit("d"))        );//((a = b) and (c = d))
    d(  Lit("Customer").Like(LitStr("Jeff 'The Dude' Lebowsky")));//(Customer like 'Jeff ''The Dude'' Lebowsky')
    d(  Lit("Customer").IsNotNull()                             );//(Customer is not null)
    d(  Lit("Customer").As(Lit("C"))                            );//(Customer as C)
    d(  Not(Lit("Customer") == LitStr("Jim"))                   );//(not Customer = 'Jim')
    d(  Func("dbo.MyFunc")                                      );//dbo.MyFunc()
    d(  Func("dbo.MyFunc", Lit("5") << LitStr("hi"))  );//dbo.MyFunc(5, 'hi')
    d(  Func("Project", Sum(Lit("intSales")))         );//Project(sum(intSales))
    d(
        Case(
            When(Lit("t") == Lit(1), LitStr("t1")) <<
            When(Lit("t") == Lit(2), LitStr("t2")) <<
            Else(LitStr("unk"))
            )// case when (t = 1) then 't1' when (t = 2) then 't2' else 'unk' end
     );

    Clause x = Select(AllFields()) << From(Table("People"));

    d(
        Select(
            Count(Lit("City")).As("C") <<
            Lit("Name")) <<
        From(
            Table("People").As("P") ==
            Table("Cities").On(Lit("P") == Lit("Cities.ID"))
            ) <<
        Where(
            Lit("Name").Like(LitStr("Charles")))
    );

    // alternate syntax for joining tables:
    Table People("People");
    Table Cities("Cities");
    d(
        Select(
            Count(Lit("City")).As("C") <<
            Lit("Name")
            ) <<
        From(
            People ==
            Cities.On(People["city"] == Cities["id"])
            ) <<
        Where(
            Lit("Name").Like(LitStr("Charles"))
            ) <<
        GroupBy(Lit("Name") << Lit("ID")).WithCube()
    );

    d(
        Select(AllFields()) <<
        From(
            Table(Func("dbo.fn")) <<
            Table("People").On(Lit("p") == Lit("id"))
            ) <<
        OrderBy(
            Lit("p").Asc() <<
            Lit("a").Desc()
            )
      );

    d(
        Where(Lit("Age").Between(Lit(15), Lit(25)))
        );

    d(
        Func("fn", Lit(5) << Lit(78))
        );

    /*
        SELECT pub_id, SUM(ytd_sales) AS total
        FROM titles
        GROUP BY pub_id
        HAVING COUNT(*) > 5
    */
    Table t("Titles");
    d(
        Select(t["pub_id"] << Sum(t["ytd_sales"]).As("total")) <<
        From(t) <<
        GroupBy(t["pub_id"]) <<
        Having(Count(AllFields()) > Lit(5))
        );

    /*
        Select * from A where id in (select B.ID from B)
    */
    Table a("A", "A");
    Table b("B", "B");
    d(
        Select(AllFields()) <<
        From(a) <<
        Where(a["id"].In(Select(b["id"]) << From(b)))
        );

    /*
        Select * from fn_A(50)
    */
    Expression MyTable(Func("fn_A", Lit(50)));
    d(
        Select(AllFields()) <<
        From(MyTable)
        );

    // EXEC dynamic sql example
    d(
        Exec(Select(Lit(1)))
        );

    // EXEC dynamic sql example using local variables
    d(
        Exec("@a + @b")
        );

    // EXEC procedure example
    d(
        Exec(Proc("sp_NightlyLoad", GetDate()))
        );

    // OpenQuery example
    Statement remote = Select(AllFields()) << From(Table("SomeRemoteTable")) << Where(Lit("Name").Like(LitStr("O'Malley")));
    d(
        Select(AllFields()) << From(OpenQuery("Gemini8.microsoft.com", remote))
    );
    d(
        Insert(Table("People")) <<
            Value("Name", LitStr("Jeffery LEbowski")) <<
            Value("Age", Lit(16))
    );
    d(
        Update(Table("People")) <<
            Value("Name", LitStr("Jeffery Lebowsky")) <<
            Value("Age", Lit(36)) <<
            Where(Lit("ID") == Lit(100234))
        );
    }
    using namespace SQLGenX;

    /*
        select
            Name,
            case when grouping(Name) = 1 then null else min(NickName) end NickName,
            case when grouping(Name) = 1 then null else min(MarketName) end Market,
            dbo.fn_CalcThingy(s.Fact1) SalesFactCalc
        from
            (Employees e left join
            Markets m on m.ID = e.MarketID) left join
            SalesFacts s on s.EmployeeID = e.ID
        where
            (Name like '%O''Leary')
            and
            (m.ID in (select MarketID from openquery(remoteserver1, 'exec dbo.sp_GetValidMarkets(''Valid Markets ''''95'')')))
        group by Name with rollup
    */
    Table e("Employees", "e");
    Table m("Markets", "m");
    Table sf("SalesFacts", "s");
    d(
        Select(
            l("Name") << 
            Case(When(Grouping(Lit("Name")) == 1, Null()) << Else(Min(Lit("NickName")))) ("NickName") <<
            Case(When(Grouping(Lit("Name")) == 1, Null()) << Else(Min(Lit("MarketName")))) ("Market") <<
            Func("dbo.fn_CalcThingy", sf["Fact1"])
            ) <<
        From(
            (e <<
            m.On(m["ID"] == e["MarketID"])) <<
            sf.On(sf["EmployeeID"] == e["ID"])
            ) <<
        Where(
            (l("Name").Like(s("%O'Leary")))
            &&
            (
            m["ID"].In(Select(l("MarketID")) << From(OpenQuery("remoteserver1", Exec(Proc("dbo.sp_GetValidMarkets", s("Valid Markets '95"))))))
            )
            ) << 
        GroupBy(l("Name")).WithRollup()
    );

    /*
        Insert into People () values ()

        Insert(
            Table("People"),
            Select(AllFields()) << From(Table("OtherPeople"))
            );

        Insert(
            Table("People"),
            Value(Lit("Name"), Str("Name")) <<
            Value(Lit("City"), Lit(5))
            );

        Update(
            Table("People")) <<
        Set(
            Values(Lit("Name"), LitStr("ablasdlkfj")) <<
            Values(Lit("Name"), LitStr("ablasdlkfj")) <<
            Values(Lit("Name"), LitStr("ablasdlkfj"))
           ) <<
        Where(
            Lit("id") == Lit(5)
            );
    */

    return 0;
}







