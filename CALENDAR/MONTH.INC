Procedure ShowMonth;

  const
    Filler = 'XXXXXXXXXX';
    MonthFullName: array[1..13] of Str9 =
      ('January','February','March','April','May ','June',
      'July','August','September','October','November','December','January');

  var
    i,j,k: integer;
    MRow,MCol,MRowOffSet: byte;
    FirstDay: byte;
    MonLen: byte;
    RomCalStr: Str7;
    S: str10;

  Procedure ShowChart;
  var i,j,k: integer;
  begin
    if DisplayMode= Colour then
      ColourScreen:=MonthScreen^
    else
      MonoScreen:=MonthScreen^;
  end;

  Procedure NextPos;
  begin
    if MRow=0 then begin
      MRow:=4;
      MCol:=2;
    end
    else begin
      MCol:=MCol+11;
      if MCol>68 then begin
        MCol:=2;
        MRow:=MRow+4;
      end;
      if MRow>20 then
        MRow:=4;
    end;
    MRowOffSet:=0;
    GotoXY(MCol,MRow);
  end;

  Procedure NextLine;
  begin
    MRowOffSet:=succ(MRowOffSet);
    GotoXY(MCol,MRow+MRowOffSet);
  end;

  Function DayInMonth(D: integer): boolean;
  begin
    if (CompareDate(D,Month+1000)=Overlap) then
      DayInMonth:=true
    else
      DayInMonth:=false;
  end;

  Procedure FindFirstDay;
  begin
    i:=1;
    while not DayInMonth(Sunday[i]) do
      i:=succ(i);
    i:=succ(Sunday[i]-MonthSt[Month]);
    while i>7 do
      i:=i-7;
    FirstDay:=8-i;
    if FirstDay=7 then FirstDay:=0;
  end;

  Procedure FindMonLen;
  begin
    MonLen:=MonthSt[succ(Month)]-MonthSt[Month];
    if LeapYear then
      if Month=2 then
        MonLen:=succ(MonLen);
  end;

  Function RomCal(D: integer): Str7;
  var NStr,DStr,MStr: Str2;

    Function MAbbr(M: byte): Str2;
    begin
      if M in [1,4,10] then
        MAbbr:=Copy(MonthName[M],1,2)
      else
        MAbbr:=MonthName[M][1]+MonthName[M][3];
    end;

  begin {RomCal}
    If D>MonNo[Month]+8 then begin
      if (LeapYear) and (Month=2) and (D<25) then begin
        if D=24 then
          NStr:='B6'
        else
          str((30-D):2,NStr);
      end
      else
        Str((MonLen+2-D):2,NStr);
      DStr:='Kl';
      MStr:=MAbbr(succ(Month));
    end
    else if D>MonNo[Month] then begin
      Str(succ(MonNo[Month]+8-D):2,NStr);
      DStr:='Id';
      MStr:=MAbbr(Month);
    end
    else if D>1 then begin
      Str(succ(MonNo[Month]-D):2,NStr);
      DStr:='No';
      MStr:=MAbbr(Month);
    end
    else begin
      NStr:='  ';
      DStr:='Kl';
      MStr:=MAbbr(Month);
    end;
    if NStr=' 1' then NStr:='  '
    else if NStr=' 2' then NStr:='Pr';
    RomCal:=NStr+DStr+' '+MStr;
  end;

begin {ShowMonth}
  ShowChart;
  GotoXY(7,1);
  Write(Year);
  GotoXY(20,1);
  Write(MonthFullName[Month]);
  MRow:=0;
  FindFirstDay;
  i:=0;
  while i<FirstDay do begin
    NextPos;
    i:=succ(i);
  end;
  FindMonLen;
  k:=MonthSt[Month];
  for i:=1 to MonLen do begin
    NextPos;
    RomCalStr:=RomCal(i);
    Write(i:2,' ');
    MakeDim;
    write(RomCalStr);
    MakeBright;
    j:=1;
    while j<4 do begin
      if length(Day[k,j].S)>0 then begin
        NextLine;
        write(Day[k,j].S);
        j:=succ(j);
      end
      else
        j:=4;
    end;
    k:=succ(k);
    if LeapYear and (i=pred(MonLen)) and (Month=2) then
      k:=366;
  end;
  for i:=1 to 35-MonLen do begin
    NextPos;
    write(Filler);
    for j:=1 to 3 do begin
      NextLine;
      write(Filler);
    end;
  end;
end; {ShowMonth}

Procedure HandleMonth;
var MonthChoice: char;
    j: integer;
begin
  KillMenu;
  SaveYear;
  MonthChoice:='E';
  repeat
  case MonthChoice of
    'E': begin
           repeat
             GotoXY(1,24);
             DelLine;
             Write('Enter month (1-12): ');
             Read(Month);
           until Month in [1..12];
           ShowMonth;
         end;
    'N': begin
           Month:=succ(Month);
           if Month<=12 then ShowMonth
           else begin
             Write(^G);
             Month:=pred(Month);
           end;
         end;
    'P': begin
           Month:=pred(Month);
           if Month>=1 then ShowMonth
           else begin
             Write(^G);
             Month:=succ(Month);
           end;
         end;
    end;
    GotoXY(1,24);
    DelLine;
    MakeDim;
    Write('Menu: (E)nter Month, (N)ext, (P)revious, (R)eturn: ');
    MakeBright;
    repeat
      Read(Kbd,MonthChoice);
      MonthChoice:=UpCase(MonthChoice);
    until MonthChoice in ['E','N','P','S','R'];
    Write(MonthChoice);
  until MonthChoice='R';
  RestoreYear;
  YearShown:=true;
  RegnalShown:=true;
end;
