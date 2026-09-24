Procedure ShowFeasts;
const
  TopLine = 3;
  BottomLine = 22;
var
  x,y: byte;
  c: char;
begin
  SaveYear;
  FeastDisplay:=FeastDisplayRoot;
  repeat
    ClrScr;
    write('Fixed feasts:');
    x:=1;
    y:=TopLine;
    while (FeastDisplay<>nil) and (X<5) do begin
      GotoXY((X-1)*20+1,Y);
      with FeastDisplay^ do begin
        write(S);
        GotoXY((X-1)*20+12,Y);
        write(MonthName[M],' ',D:2,' ');
        MakeDim;
        write('|');
        MakeBright;
      end;
      FeastDisplay:=FeastDisplay^.Next;
      y:=succ(y);
      if y>BottomLine then begin
        y:=TopLine;
        x:=succ(x);
      end;
    end;
    GotoXY(1,25);
    write('Press any key to ');
    if FeastDisplay<>nil then
      write('continue or R to ');
    write('return.');
    read(kbd,c);
  until (FeastDisplay=nil) or (UpCase(C)='R');
  RestoreYear;
  YearShown:=true;
  RegnalShown:=true;
end;
