Procedure FindRegnalYear;

var
  HoldStr: string[2];
  Done, Suppressed,Found: boolean;
  i, StartDNum, EndDNum, AnnivDNum: integer;
  StartTStr, EndTStr: string[7];
  CurLine, RegCol: byte;
  RegYearNumStr: string[6];


Procedure FillRec;
var i: integer;

  Function HandleSuppression(var R: RegRecT; var a: integer): boolean;
  begin
    with RegArray[CurRegX,CurRegY]^ do begin
      if (Year>StartYear) and (Year<EndYear) then
        HandleSuppression:=true
      else begin
        if Year=StartYear then begin
          R.EndDay:=StartDay;
          R.EndDaySpec:=StartDaySpec;
          R.EndType:=EndType;
          if CompareDate(a,StartDay)=Later then
            a:=0;
        end
        else if Year=EndYear then begin
          R.StartDay:=EndDay;
          R.StartDaySpec:=EndDaySpec;
          R.StartType:=StartType;
          if CompareDate(a,StartDay)=Earlier then
            a:=0;
        end;
        HandleSuppression:=false;
      end;
    end;
  end;

begin {FillRec}
  CurRegY:=1;
  NumRecs:=1;
  Done:=false;
  OutsideSeries:=false;
  Found:=false;
  while not Done do begin
    if RegArray[CurRegX,CurRegY]^.StartYear>Year then begin
      Done:=True;
      if CurRegY=1 then
        OutsideSeries:=true;
    end
    else if (RegArray[CurRegX,CurRegY]^.EndYear>=Year) then begin
      Suppressed:=false;
      Rec[NumRecs]:=RegArray[CurRegX,CurRegY]^;
      Found:=true;
      with Rec[NumRecs] do begin
        if StartDay=-1 then
          StartDay:=Pascha[5]+4; {Ascension}
        if StartYear=Year then
          Anniv[NumRecs]:=0
        else begin
          Anniv[NumRecs]:=StartDay;
          StartDay:=0;
        end;
        if Year<EndYear then
          EndDay:=0
        else
          if CompareDate(Anniv[NumRecs],EndDay)=Later then
            Anniv[NumRecs]:=0;
      end;
      CurRegY:=succ(CurRegY);
      if (RegArray[CurRegX,CurRegY]=nil) and (not Found) then begin
        Done:=true;
        OutsideSeries:=true;
      end
      else begin
        if RegArray[CurRegX,CurRegY]^.Name='*' then begin
          Suppressed:=HandleSuppression(Rec[NumRecs],Anniv[NumRecs]);
          CurRegY:=succ(CurRegY);
        end;
        if not Suppressed then
          NumRecs:=succ(NumRecs);
      end;
    end
    else begin
      CurRegY:=succ(CurRegY);
      if (RegArray[CurRegX,CurRegY]=nil) and (not Found) then begin
        Done:=true;
        OutsideSeries:=true;
      end;
    end;
  end;
  NumRecs:=pred(NumRecs);
end;

Procedure GetDates(var R: integer);
begin
  with Rec[R] do begin
    StartDNum:=StartDay;
    AnnivDNum:=Anniv[r];
    StartTStr:=StartTNames[StartType];
    EndDNum:=EndDay;
    EndTStr:=EndTNames[EndType];
    RegYearNum[1]:=Year-StartYear;
    RegYearNum[2]:=succ(RegYearNum[1]);
    if AnnivDNum=0 then
      if EndDay>0 then begin
        if RegYearNum[1]>0 then
          RegYearNum[2]:=0;
      end
      else
        RegYearNum[1]:=0;
  end;
end;

Procedure ShowRegYear(var R: RegRecT);
var i: integer;

  Procedure NextCol;
  begin
    RegCol:=RegCol+20;
    CurLine:=RegLine;
    GotoXY(RegCol,CurLine);
  end;

  Procedure NextLine;
  begin
    CurLine:=succ(CurLine);
    GotoXY(RegCol,CurLine);
  end;

  Function FindMonth(M: integer): Str4;
  var i: byte;
  begin
    i:=1;
    while MonthSt[i]<>M do
      i:=succ(i);
    FindMonth:=MonthName[i];
  end;

begin
  with R do begin
    NextCol;
{    if TooEarly then
      write('(Too early)')
    else if TooLate then
      write('(Too late)')
    else begin}
{    if (Name<>'Pre-Conquest') and (Name<>'Post-Medieval') then begin}
      RegYearNumStr:='';
      for i:=1 to 2 do
        if RegYearNum[i]>0 then begin
          str(RegYearNum[i],HoldStr);
          RegYearNumStr:=RegYearNumStr+HoldStr;
          if (i=1) and (RegYearNum[2]>0) then
            RegYearNumStr:=RegYearNumStr+'-';
        end;
      Write(RegYearNumStr,' ',Name);
      ClrEol;
      NextLine;
      if StartDNum>0 then begin
        if StartDaySpec then
          DateStr:=ConvertDate(StartDNum)
        else
          DateStr:=FindMonth(StartDNum);
        Write(StartTStr,' ',DateStr);
        ClrEol;
        NextLine;
      end;
      if AnnivDNum>0 then begin
        if AnnivDaySpec then
          DateStr:=ConvertDate(AnnivDNum)
        else
          DateStr:=FindMonth(AnnivDNum);
        Write('Anniv.  ',DateStr);
        ClrEol;
        NextLine;
      end;
      if EndDNum>0 then begin
        if EndDaySpec then
          DateStr:=ConvertDate(EndDNum)
        else
          DateStr:=FindMonth(EndDNum);
        Write(EndTStr,' ',DateStr);
        ClrEol;
      end;
{    end;}
{    else
      Write(Name);}
  end;
end;

begin {FindRegnalYear}
  GotoXY(1,pred(RegLine));
  MakeDim;
  write('  ',RegName[CurRegX],':');
  ClrEol;
  MakeBright;
  GotoXY(1,RegLine);
{  if Year<1066 then Write('Pre-Conquest')
  else if Year>1500 then Write('Post-Medieval')}
{  else begin}
    FillRec;
    RegCol:=-19;
    gotoXY(1,RegLine);
    if OutsideSeries then begin
      write('(Outside series)');
      ClrEol;
    end
    else if NumRecs=0 then begin
      write('Interregnum');
      ClrEol;
    end
    else for i:=1 to NumRecs do begin
      GetDates(i);
      ShowRegYear(Rec[i]);
    end;
{  end;}
end;

