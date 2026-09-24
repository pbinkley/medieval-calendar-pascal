{$V-}
Program Calendar;

const
  Version='3.3';
  VertLine = '|';
  LVertLine = '|';
  RVertLine = '|';
  TopRegT = 10; {number of series of regnal years available; }
                { mustn't be more than will fit on screen}
type
  DateType= string[7];
  Str2 = string[2];
  Str4 = string[4];
  Str7 = string[7];
  Str8 = string[8];
  Str9 = string[9];
  Str10 = string[10];
  Str13 = string[13];
  Str15 = string[15];
  Str30 = string[30];
  Str80 = String[80];
  AnyStr = String[255];
  StartT = (Cor,Elec,Restor,Resum,Start);
  EndT = (Death,Resign,Depos,Ended);
  RegPtr = ^RegRecT;
  RegRecT = record
           Name: str13;
           StartDay, StartYear: integer;
           StartType: StartT;
           EndDay, EndYear: integer;
           EndType: EndT;
           StartDaySpec,AnnivDaySpec,EndDaySpec: boolean;
         end;
  RegT = 1..TopRegT;
  DateCompT = (Earlier,Same,Overlap,Later);
  DayRecT = record
            S: Str10;
            Fixed: boolean;
          end;
  DayT = array[1..366,1..3] of DayRecT;
  FeastDisplayPtr = ^FeastDisplayRecT;
  FeastDisplayRecT = record
                     S: Str10;
                     M,D: byte;
                     Next: FeastDisplayPtr;
                   end;

const
  EasterTable: array[1..19,0..1] of byte =
    ((5,16),(1,5),(6,24),(2,13),(5,2),(3,21),(6,10),(4,29),
    (7,18),(3,7),(1,26),(4,15),(7,4),(5,23),(1,12),(4,1),(2,20),(5,9),(3,28));
  DomLetTable: array[0..27] of byte =
    (1,6,5,4,3,1,7,6,5,3,2,1,7,5,4,3,2,7,6,5,4,2,1,7,6,4,3,2);
  MonthName: array[1..13] of Str4 =
    ('Jan ','Feb ','Mar ','Apr ','May ','June',
    'July','Aug ','Sept','Oct ','Nov ','Dec ','Jan ');
  MonthSt: array[1..13] of integer =
    (1,32,60,91,121,152,182,213,244,274,305,335,366);
  MonNo: array[1..12] of byte =
    (5,5,7,5,7,5,7,5,5,7,5,5); {Nonae for Roman calendar}
  SeptOffset=-63;
  SexOffset=-56;
  QuinOffset=-49;
  PassOffset=-14;
  PalmOffset=-7;
  PentOffset=49;
  TrinOffset=56;
  QuadOffsetRoot=-42;
  PaschaOffsetRoot=7;
  TrinOffsetRoot=63;
  SeptStr='Septua  ';
  SexStr='Sexa    ';
  QuinStr='Quinqua ';
  QuadStr='Quadra  ';
  LentStr='Lent';
  PassStr='Passion ';
  PalmStr='Palm    ';
  EasterStr='Easter  ';
  QuasStr='Quasimod';
  PaschaStr='Easter';
  RogaStr='Rogation';
  AscenStr='Ascen 1';
  PentStr='Whitsun';
  TrinityStr='Trinity ';
  TrinStr='Trin';
  AdventStr='Advent';
  EpiphStr='Epiph';
  ShroveStr='Shrove T.';
  AshStr='Ash Wed.';
  GoodFriStr='Good Fri.';
  AscensionStr='Ascension';
  CorpusCStr='Corpus C.';
  EmberStr='Ember';

const
  LinesPerCol = 16;
  RegLine = 18;
  StartTNames: array[StartT] of Str7 =
    ('Cor.   ','Elec.  ','Restor.','Resum. ','Start  ');
  EndTNames: array[EndT] of Str7 =
    ('Death  ','Resign.','Depos. ','End    ');
  CorpusCStart = 1264;  {first year in which Corpus Christi celebrated}

var
  I,Year,Line,Col: integer;
  DominicalLetter: Integer;
  Offset,Easter,GoldenNumber: Integer;
  DateStr: DateType;
  QuadOffset,PaschaOffset,TrinSunOffset,AdvOffset,EpiphOffset: Integer;
  LeapYear: Boolean;
  LeapDomLet: Integer;
  EpiphOffsetRoot,AdvOffsetRoot: Integer;
  Sept,Sex,Quin,Pass,Pent,Trin,Palm: Integer;
  Quad: array[1..4] of Integer;
  Pascha: array[1..6] of Integer;
  TrinSun: array[1..27] of Integer;
  Advent: array[1..4] of Integer;
  Epiph: array[1..6] of Integer;
  FeastStr: Str8;
  Indiction: byte;
  Rec: Array[1..6] of RegRecT;
  Anniv: array[1..6] of integer;
  NumRecs: byte;
  RegYearNum: array[1..2] of integer;
  MainChoice: char;
  Sunday: array[1..52] of integer;
  SundayCtr: byte;
  Month: byte;
  Day: DayT;
  CurRegX: RegT;
  CurRegY: Integer;
  RegName: array[RegT] of Str30;
  RegArray: array[RegT,1..100] of RegPtr;
  OutsideSeries, YearShown, RegnalShown{, FirstPass}: boolean;
  FeastDisplayRoot, FeastDisplay: FeastDisplayPtr;

{$I cal-scr.inc}

Procedure Hello;
begin
  ClrScr;
  GotoXY(19,10);
  write('Medieval Calendar Aid, Version ',Version);
  GotoXY(19,11);
  write('Copyright (C) Peter Binkley, 1987');
end;

Function CompareDate(a,b: integer): DateCompT;
var ar,br: real;
    cd: DateCompT;

  Procedure DirectComp;
  begin
    if ar<br then cd:=Earlier
    else if ar>br then cd:=Later
    else cd:=Same;
  end;

  Function MComp(M: integer; D: real): DateCompT;
  begin
    M:=M-1000;
    if D<MonthSt[M] then MComp:=Later
    else if D<MonthSt[succ(M)] then MComp:=Overlap
    else MComp:=Earlier;
  end;

begin {CompareDate}
  ar:=a;
  br:=b;
  if ar=366 then ar:=59.5; {Feb. 29}
  if br=366 then br:=59.5;
  if (ar<366) and (br<366) then
    DirectComp
  else begin
    if (ar=1000) or (br=1000) then
      cd:=Overlap
    else begin
      if (ar>365) and (br>365) then begin
        DirectComp;
        if cd=Same then cd:=Overlap;
      end
      else begin
        if ar>1000 then cd:=MComp(a,br)
        else begin
          cd:=MComp(b,ar);
          if cd=Earlier then cd:=Later
          else if cd=Later then cd:=Earlier;
        end;
      end;
    end;
  end;
  CompareDate:=cd;
end;

Procedure DetermineLeapYear;
begin
  LeapYear:=((Year mod 4)=0);
end;

Function FindIndiction(Year: integer): byte;
var b: byte;
begin
  b:=((Year-312) mod 15);
  if b=0 then b:=15;
  FindIndiction:=b;
end;

Function FindEaster(Year: integer): Integer;
begin
  DominicalLetter:=DomLetTable[(Year+9) mod 28];
  if DominicalLetter=0 then DominicalLetter:=7;
  if LeapYear then begin
    LeapDomLet:=DominicalLetter+1;
    if LeapDomLet=8 then LeapDomLet:=1;
  end
  else LeapDomLet:=DominicalLetter;
  GoldenNumber:=(Year+1) mod 19;
  if GoldenNumber=0 then GoldenNumber:=19;
  Offset:=DominicalLetter-EasterTable[GoldenNumber,0];
  if Offset<0 then Offset:=Offset+7;
  FindEaster:=EasterTable[GoldenNumber,1]+Offset+80;
end;

Function DayOfWeek(d: integer): byte;
var I: integer;
begin
  I:=D;
  if I=366 then D:=MonthSt[3]-1;
  D:=D mod 7;
  if D=0 then D:=7;
  D:=D-DominicalLetter+1;
  if LeapYear and (I<MonthSt[3]) then
    D:=D-1;
  if D<1 then D:=D+7;
  DayOfWeek:=D;
end;

Function ConvertDate(x:integer): DateType;
var
  Day,Mon,I: integer;
  Temp: String[2];
begin
 if x<1000 then begin
  if x=366 then begin
    Mon:=2;
    Day:=29;
  end
  else begin
    I:=12;
    Mon:=0;
    while mon=0 do begin
      if x>=MonthSt[I] then begin
        Mon:=I;
        Day:=x-MonthSt[I]+1;
      end;
      I:=I-1;
    end;
  end;
  Str(Day:2,Temp);
  ConvertDate:=MonthName[Mon]+' '+Temp;
 end
 else if x=1000 then
   ConvertDate:='(?)'
 else
   ConvertDate:=MonthName[x-1000];
end;

Procedure DateSundays;

  Function Date(Offset: Integer): Integer;
  var Day: Integer;
  begin
    Day:=Easter+Offset;
    if (Day<MonthSt[3]) and LeapYear then begin
      Day:=Day+1;
      if Day=MonthSt[3] then
        Day:=366; {Feb. 29}
      end;
    Date:=Day;
  end;

  Procedure DateFromRoot(var Root,Day: integer);
  begin
    Day:=Date(Root);
    Root:=Root+7;
  end;

begin
  AdvOffset:=9-DominicalLetter;
  if AdvOffset=8 then AdvOffset:=1;
  AdvOffsetRoot:=359-AdvOffset;
  for I:=4 downto 1 do begin
    Advent[I]:=AdvOffsetRoot;
    AdvOffsetRoot:=AdvOffsetRoot-7;
  end;
  Sept:=Date(SeptOffset);
  Sex:=Date(SexOffset);
  Quin:=Date(QuinOffset);
  Pass:=Date(PassOffset);
  Pent:=Date(PentOffset);
  Trin:=Date(TrinOffset);
  Palm:=Date(PalmOffset);
  EpiphOffset:=LeapDomLet-6;
  if EpiphOffset<=0 then EpiphOffset:=EpiphOffset+7;
  EpiphOffsetRoot:=6+EpiphOffset;
  for I:=1 to 6 do begin
    Epiph[I]:=EpiphOffsetRoot;
    if Epiph[I]>=Sept then
      Epiph[I]:=-1;
    EpiphOffsetRoot:=EpiphOffsetRoot+7;
  end;
  QuadOffset:=QuadOffsetRoot;
  for I:=1 to 4 do
    DateFromRoot(QuadOffset,Quad[I]);
  PaschaOffset:=PaschaOffsetRoot;
  for I:=1 to 6 do
    DateFromRoot(PaschaOffset,Pascha[I]);
  TrinSunOffset:=TrinOffsetRoot;
  for I:=1 to 27 do begin
    DateFromRoot(TrinSunOffset,TrinSun[I]);
    if TrinSun[I]>=Advent[1] then TrinSun[I]:=-1;
  end;
end;

Procedure InsertDayName(D: integer; F: Str10; Fixed,Priority: boolean);
var i,j: integer;
begin
  if Priority then begin
    for i:=3 downto 2 do
      Day[D,i]:=Day[D,pred(i)];
    Day[D,1].S:=F;
    Day[D,1].Fixed:=Fixed;
  end
  else begin
    i:=1;
    j:=4;
    repeat
      if length(Day[D,i].S)=0 then begin
        j:=i;
        i:=4;
      end
      else
        i:=succ(i);
    until i=4;
    if j<4 then begin
      Day[D,j].S:=F;
      Day[D,j].Fixed:=Fixed;
    end;
  end;
end;

Procedure PrintSundays;
var temp: String[2];
    Col,Line: Integer;

  Procedure NextPos;
  begin
    GotoXY(Col,Line);
    Line:=Line+1;
    if Line=succ(LinesPerCol) then begin
      Line:=3;
      Col:=Col+19;
    end;
  end;

  Procedure PutSunday(D: integer; F: Str8);
  var i,j: integer;

    Function MakeStr(n: byte; c: char): Str30;
    var s: Str30;
        i: byte;
    begin
      i:=0;
      s:='';
      repeat
        s:=s+c;
        i:=succ(i);
      until i=n;
      MakeStr:=s;
    end;

  begin {PutSunday}
    DateStr:=ConvertDate(D);
    if not (D=Easter) then
      MakeDim;
    write(F);
    MakeBright;
    write(MakeStr(9-length(F),' ')+DateStr+' '+VertLine+' ');
    Sunday[SundayCtr]:=D;
    SundayCtr:=succ(SundayCtr);
    InsertDayName(D,F,False,True);
  end;

  Procedure ClearMoveables;
  var i,j: integer;
      Bump: boolean;
  begin
    for i:=1 to 366 do begin
      j:=1;
      while (length(Day[i,j].S)>0) and (j<4) do begin
        Bump:=false;
        if not Day[i,j].Fixed then begin
          Day[i,j].S:='';
          Day[i,j].Fixed:=true;
          if j<3 then
            if Day[i,succ(j)].S>'' then begin
              Day[i,j]:=Day[i,succ(j)];
              Day[i,succ(j)].S:='';
              Day[i,succ(j)].Fixed:=true;
              Bump:=true;
            end;
        end;
        if not Bump then
          j:=succ(j);
      end;
    end;
  end;

  Procedure MinorMoveable(S: Str10; D: integer);
  begin
    InsertDayName(D,S,false,false);
  end;

  Function CalcOffset(A,B: integer): integer;
  begin
    if LeapYear then begin
      if A=366 then begin
        if B>0 then
          A:=MonthSt[3]-1
        else
          A:=MonthSt[3];
      end
      else if (A<MonthSt[3]) and (A+B>=MonthSt[3]) then begin
        if A+B=MonthSt[3] then B:=366
        else B:=B-1;
      end
      else if (A>=MonthSt[3]) and (A+B<MonthSt[3]) then begin
        if A+B=MonthSt[3]-1 then B:=366
        else B:=B+1;
      end;
    end;
    if B=366 then
      CalcOffset:=366
    else
      CalcOffSet:=A+B;
  end;

  Procedure CalcEmbers(X: integer);
  var EO: integer;
  begin
    EO:=4-DayOfWeek(X);
    if EO<1 then EO:=EO+7;
    MinorMoveable(EmberStr,X+EO);
    MinorMoveable(EmberStr,X+EO+2);
    MinorMoveable(EmberStr,X+EO+3);
  end;

begin {PrintSundays}
  FillChar(Sunday[1],SizeOf(Sunday),0);
  ClearMoveables;
  Col:=1;
  Line:=3;
  SundayCtr:=1;
  ClrScr;
  write('Sundays of the year ',Year);
  if LeapYear then
    write('   LEAP YEAR');
  write('   Indiction: ',Indiction);
  NextPos;
  for I:=1 to 6 do
    if Epiph[I]>0 then begin
      str(I:2,temp);
      FeastStr:=EpiphStr+temp;
      PutSunday(Epiph[I],FeastStr);
      NextPos;
    end;
  PutSunday(Sept,SeptStr);
  NextPos;
  PutSunday(Sex,SexStr);
  NextPos;
  PutSunday(Quin,QuinStr);
  NextPos;
  for I:=1 to 4 do
    if Quad[I]>0 then begin
      if I=1 then begin
        PutSunday(Quad[i],QuadStr);
        NextPos;
      end
      else begin
        str(I:2,temp);
        FeastStr:=LentStr+temp;
        PutSunday(Quad[i],FeastStr);
        NextPos;
      end;
    end;
  PutSunday(Pass,PassStr);
  NextPos;
  PutSunday(Palm,PalmStr);
  NextPos;
  PutSunday(Easter,EasterStr);
  NextPos;
  for I:=1 to 6 do
    if Pascha[I]>0 then begin
      case I of
        1: begin
            PutSunday(Pascha[i],QuasStr);
            NextPos;
          end;
        2..4: begin
            str(I:2,temp);
            FeastStr:=PaschaStr+temp;
            PutSunday(Pascha[i],FeastStr);
            NextPos;
          end;
        5: begin
            PutSunday(Pascha[i],RogaStr);
            NextPos;
          end;
        6: begin
            PutSunday(Pascha[i],AscenStr);
            NextPos;
          end;
      end;
    end;
  PutSunday(Pent,PentStr);
  NextPos;
  PutSunday(Trin,TrinityStr);
  NextPos;
  for I:=1 to 27 do
    if TrinSun[I]>0 then begin
      str(I:2,temp);
      FeastStr:=TrinStr+' '+temp;
      PutSunday(TrinSun[i],FeastStr);
      NextPos;
    end;
  for I:=1 to 4 do
    if Advent[I]>0 then begin
      str(I:2,temp);
      FeastStr:=AdventStr+temp;
      PutSunday(Advent[i],FeastStr);
      NextPos;
    end;
  MinorMoveable(ShroveStr,CalcOffset(Quin,2));
  MinorMoveable(AshStr,CalcOffset(Quin,3));
  MinorMoveable(GoodFriStr,CalcOffset(Easter,-2));
  MinorMoveable(AscensionStr,CalcOffset(Pascha[5],4));
  if Year>=CorpusCStart then
    MinorMoveable(CorpusCStr,CalcOffset(Trin,4));
{ modification made Feb. 1 1988
  MinorMoveable(EmberStr,CalcOffset(Quad[1],3));
  MinorMoveable(EmberStr,CalcOffset(Quad[1],5));
  MinorMoveable(EmberStr,CalcOffset(Quad[1],6));
  MinorMoveable(EmberStr,CalcOffset(Pent,3));
  MinorMoveable(EmberStr,CalcOffset(Pent,5));
  MinorMoveable(EmberStr,CalcOffset(Pent,6));
  }
  CalcEmbers(Quad[1]);
  CalcEmbers(Pent);
  { end }
  CalcEmbers(257); {Exaltatio crucis}
  CalcEmbers(347); {St. Lucy}
end;

Procedure KillMenu;
begin
  GotoXY(1,23);
  DelLine;
  DelLine;
end;

{$I regnal.inc}
{$I month.inc}

Procedure ChangeRegDefaults;
begin
  SaveYear;
  I:=0;
  repeat
    ClrScr;
    writeln('Regnal Series:');
    writeln;
    for i:=1 to TopRegT do begin
      write(i:2,': ');
      if RegName[i]>'' then
        writeln(RegName[i])
      else begin
        MakeDim;
        writeln('  - not in use -');
        MakeBright;
      end;
    end;
    writeln;
    write('Your choice: ');
    readln(I);
  until (I>0) and (I<=TopRegT) and (RegArray[I,1]<>nil);
  CurRegX:=I;
  RestoreYear;
  YearShown:=true;
end;

Function YearNoGood: boolean;
begin
  if (Year>=500) and (Year<=1582) then begin
    YearNoGood:=false;
  end
  else begin
    Write(^G);
    YearNoGood:=true;
  end;
end;

Procedure EnterYear;
var HoldYear: integer;
{    c: char;}
begin
  SaveYear;
  HoldYear:=Year;
  Year:=0;
  repeat
    write('Enter Year:   ');
    readln(Year);
    Writeln;
    if Year>0 then
      if YearNoGood then Year:=-1;
  until Year>-1;
  if Year=0 then begin
{    writeln('Year = 0');
    read(kbd,c); }
    Year:=HoldYear;
    RestoreYear;
    YearShown:=true;
    RegnalShown:=true;
  end;
end;

{$I pickreg.inc}

Procedure ShowYear;
begin
  DetermineLeapYear;
  Easter:=FindEaster(Year);
  Indiction:=FindIndiction(Year);
  DateSundays;
  PrintSundays;
  RegnalShown:=false;
end;

{$I Feast.inc}

Procedure HandleYear;
var YearChoice: char;
begin
  YearShown:=false;
  RegnalShown:=true;
  repeat
    if (not YearShown) and {(not FirstPass)} (Year>0) then {begin
      ClrScr;
      writeln('YearShown: ',YearShown);
      writeln('RegnalShown: ',RegnalShown);
      writeln('Year: ',Year);
      read(kbd,YearChoice);}
      ShowYear;
{    end;}
    YearShown:=false;
    if (not RegnalShown) and {(not FirstPass)} (Year>0) then
      FindRegnalYear;
    RegnalShown:=false;
    GotoXY(1,23);
    MakeDim;
    if {FirstPass} Year=0 then
      Write('Menu: (E)nter Year, (R)egnal Year, (C)hange regnal series, (Q)uit: ')
    else begin
      Writeln('Menu: (E)nter Year, (N)ext, (P)revious, (C)hange series,');
      ClrEol;
      write('(R)egnal Year, (M)onth, (F)east days, (Q)uit: ');
    end;
    ClrEol;
    MakeBright;
    repeat
      Read(Kbd,YearChoice);
      YearChoice:=UpCase(YearChoice);
    until (YearChoice in ['C','E','Q','R']) or
      ((YearChoice in ['N','P','M','S','F']) and {(not FirstPass)} (Year>0));
    Write(YearChoice);
    case YearChoice of
      'E': begin
             KillMenu;
             GotoXY(1,23);
             EnterYear;
           end;
      'N': begin
             Year:=succ(Year);
             if YearNoGood then
               Year:=pred(Year);
           end;
      'P': begin
            Year:=pred(Year);
            if YearNoGood then
              Year:=succ(Year);
          end;
      'C': ChangeRegDefaults;
      'M': HandleMonth;
      'R': PickReg;
      'F': ShowFeasts;
    end;
{    if YearChoice<>'C' then
      FirstPass:=false;}
  until YearChoice='Q';
end;

{$I readdata.inc}

begin {Main Program Block}
  GetDisplayMode;
  Hello;
  ReadDataFile;
  CurRegX:=1;
  Year:=0;
{  FirstPass:=true;}
  HandleYear;
  ClrScr;
end.

