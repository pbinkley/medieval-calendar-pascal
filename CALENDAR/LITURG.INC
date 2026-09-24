{new proposed feature: a list of liturgical tags used to designate
sundays.  The tags to be derived from Cheney and put in the data file, in
alphabetical order.   Display like this:

18. O creator spiritus:   Lent 1    Feb. 26

Allow user to scroll through list; if hit ID number (here 18), take to
month chart.  So, need chain of dynamic variables.
}
Const
  LiturStrLen = 40;
Type
  LiturTPtr = ^LiturT;
  LiturT = record
             ID: integer;
             S: string[LiturStrLen];
             Day: integer;
             Prev, Next: LiturTPtr;
           end;
var
  FirstLitur, CurLitur: LiturTPtr;

{this depends on a list of Sundays keyed by integer, to replace the old set
of individual variables.  They would be numbered from Advent 1 on, including
all possible Sundays, so that e.g. 18 would always be Easter (then refer to
by constants e.g. EasterSun = 18).  So have array [1..TopSun] of integer;
the integer value would indicate the day of the year}
