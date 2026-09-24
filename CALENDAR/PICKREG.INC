Procedure PickReg;
const
  PgUp = 'U';
  PgDn = 'D';
  CR = #13;
  BS = #8;
  Prompt = 'Use (U)p, (D)own; enter number, hit "Enter". Your choice: ';
var
  Line: byte;
  Ch,ReignLength: integer;
  C: char;
  NumStr: Str4;
  HoldYear: integer;
  Abort: boolean;

  Procedure FillScreen;
  var Hold: integer;
  begin
    ClrScr;
    MakeDim;
    write(Prompt);
    MakeBright;
    Hold:=CurRegY;
    Line:=2;
    repeat
      if RegArray[CurRegX,CurRegY]<>nil then begin
        with RegArray[CurRegX,CurRegY]^ do begin
          if Name<>'*' then begin
            gotoXY(1,Line);
            write(CurRegY:3,': ');
            write(Name);
            gotoXY(20,Line);
            write('(',StartYear);
            if EndYear<>StartYear then
              write('-',EndYear);
            write(')');
            Line:=succ(Line);
          end;
        end;
        CurRegY:=succ(CurRegY);
      end
      else
        Line:=24;
    until Line=24;
    CurRegY:=Hold;
  end;

  Procedure ScrollUp;
  begin
    if CurRegY>1 then begin
      CurRegY:=CurRegY-22;
      if CurRegY<1 then
        CurRegY:=1;
      FillScreen;
    end
    else
      write(^G);
  end;

  Procedure ScrollDown;
  begin
    if CurRegY<100 then begin
      CurRegY:=CurRegY+22;
      if CurRegY>100 then
        CurRegY:=100;
      while RegArray[CurRegX,succ(CurRegY)]=nil do
        CurRegY:=pred(CurRegY);
      FillScreen;
    end
    else
      write(^G);
  end;

begin
  SaveYear;
  CurRegY:=1;
  HoldYear:=Year;
  Abort:=false;
  Ch:=0;
  FillScreen;
  repeat
    NumStr:='';
    GotoXY(length(Prompt)+1,1);
    ClrEol;
    C:=#0;
    repeat
      read(kbd,C);
      C:=upcase(C);
    until ((C>='0') and (C<='9')) or (C=PgUp) or (C=PgDn) or (C=CR);
    case C of
      CR: Abort:=true;
      PgUp: ScrollUp;
      PgDn: ScrollDown;
      '0'..'9': begin
        NumStr:=C;
        write(NumStr);
        repeat
          read(kbd,C);
          C:=upcase(C);
          Case C of
            CR: begin
              val(NumStr,Ch,I);
              if Ch=0 then
                Abort:=true;
            end;
            BS: begin
              NumStr[0]:=pred(NumStr[0]);
              GotoXY(length(Prompt)+1,1);
              ClrEol;
              write(NumStr);
            end;
            PgUp: begin
              ScrollUp;
              NumStr:='';
            end;
            PgDn: begin
              ScrollDown;
              NumStr:='';
            end;
            '0'..'9': begin
              write(C);
              Insert(C,NumStr,255);
            end;
          end;
        until (C=CR) or (length(NumStr)=0);
      end;
    end;
  until Abort or ((C=CR) and (Ch>0) and (Ch<=100) and 
        (RegArray[CurRegX,Ch]<>nil) and (RegArray[CurRegX,Ch]^.Name<>'*'));
 if not Abort then begin
    CurRegY:=Ch;
    ClrScr;
    with RegArray[CurRegX,CurRegY]^ do begin
      ReignLength:=EndYear-StartYear;
      if CompareDate(StartDay,EndDay)=earlier then
        ReignLength:=succ(ReignLength);
      writeln('Reign of ',Name,': ',ReignLength,' years');
      writeln;
      Ch:=Year;
      repeat
        write('Enter year: ');
        readln(Year);
      until (Year<=ReignLength);
      if Year>0 then begin
        Year:=Year+StartYear-1;
        if YearNoGood then
          Abort:=true;
      end
      else Abort:=true;
    end;
  end;
  if Abort then begin
    Year:=HoldYear;
    RestoreYear;
    YearShown:=true;
    RegnalShown:=true;
  end;
end;

