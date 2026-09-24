Procedure ReadDataFile;
var
  DataF: text;
  DataFN: String[30];
  S: Str80;
  Problem: boolean;

  Procedure Caps(var S:AnyStr);
  var I: byte;
  begin
    for I:=1 to length(S) do
      S[I]:=UpCase(S[I]);
  end;

Procedure ReadRegnalYears;
const
  StartTable: array[StartT] of Str7 =('COR','ELEC','RESTOR',
                                      'RESUM','START');
  EndTable: array[EndT] of Str7 = ('DEATH','RESIGN','DEPOS','ENDED');
var
  RegL,RegLHold,S: str80;
  Reg: RegRecT;
  p: byte;
  CurStart: StartT;
  CurEnd: EndT;
  Found: boolean;
Label RegErr,RegOK;

  Procedure Trim(var S: AnyStr);
  begin
    if length(S)>0 then begin
      while (S[length(S)]=' ') or (S[length(S)]=#9) do
        S[0]:=pred(S[0]);
      while (S[1]=' ') or (S[1]=#9) do
        delete(S,1,1);
    end;
  end;

  Procedure Bite(var A,B: AnyStr; c: char);
  begin
    p:=pos(c,A);
    if p>0 then begin
      B:=copy(A,1,p);
      Delete(A,1,p);
      Trim(A);
    end;
  end;

  Function ConvertDate(var S: AnyStr;
                       var D,Y: integer;
                       var Spec: boolean): boolean;
  Var B: Str4;
      I,M: integer;
  label DateErr;
  begin
    Bite(S,B,'/');
    if p=0 then goto DateErr;
    B[0]:=pred(B[0]);
    val(B,D,I);
    if I<>0 then goto DateErr;
    if D<0 then goto DateErr;
    Spec:=(D>0);
    Bite(S,B,'/');
    if p=0 then goto DateErr;
    B[0]:=pred(B[0]);
    val(B,M,I);
    if I<>0 then goto DateErr;
    if (M<1) or (M>12) then goto DateErr;
    if not Spec then
      D:=MonthSt[M]
    else
      if (D=29) and (M=2) then
        D:=366
      else begin
        if D>MonthSt[succ(M)]-MonthSt[M] then goto DateErr;
        D:=MonthSt[M]+D-1;
      end;
    val(S,Y,I);
    if I=0 then begin
      ConvertDate:=true;
    end
    else
      DateErr: ConvertDate:=false;
  end;

begin
  Write('Reading Regnal years...');

  FillChar(RegArray,SizeOf(RegArray),char(0));
  FillChar(RegName,SizeOf(RegName),char(0));


    CurRegX:=0;
    readln(DataF,RegL);
    while (not eof(DataF)) and (RegL[1]<>'\') do begin
{      write(RegL);}
      Trim(RegL);
      if length(RegL)>0 then begin
        RegLHold:=RegL;
        if RegL[1]='#' then begin
          CurRegX:=succ(CurRegX);
          Delete(RegL,1,1);
          Trim(RegL);
          RegName[CurRegX]:=RegL;
          CurRegY:=1;
        end
        else begin
          Bite(RegL,Reg.Name,':');
          if p=0 then goto RegErr;
          Reg.Name[0]:=pred(Reg.Name[0]);  {delete ':'}
          Bite(RegL,S,' ');
          if p=0 then goto RegErr;
          Trim(S);
          Caps(S);
          Found:=false;
          CurStart:=Cor;
          while not Found and (CurStart<>Start) do begin
            if S=StartTable[CurStart] then begin
              Reg.StartType:=CurStart;
              Found:=true;
            end;
            CurStart:=succ(CurStart);
          end;
          if not Found then
            Reg.StartType:=Start;
          Bite(RegL,S,' ');
          if p=0 then goto RegErr;
          if Copy(S,1,9)='Ascension' then begin
            Reg.StartDay:=-1;
            Reg.StartDaySpec:=true;
            Delete(S,1,10);
            Trim(S);
            Val(S,Reg.StartYear,I);
            if I<>0 then goto RegErr;
          end
          else
            with Reg do
              if not ConvertDate(S,StartDay,StartYear,StartDaySpec) then
                goto RegErr;
          Reg.AnnivDaySpec:=Reg.StartDaySpec;
          Bite(RegL,S,' ');
          if p=0 then goto RegErr;
          Trim(S);
          Caps(S);
          Found:=false;
          CurEnd:=Death;
          while not Found and (CurEnd<>Ended) do begin
            if S=EndTable[CurEnd] then begin
              Reg.EndType:=CurEnd;
              Found:=true;
            end;
            CurEnd:=succ(CurEnd);
          end;
          if not Found then
            Reg.EndType:=Ended;
          with Reg do
            if not ConvertDate(RegL,EndDay,EndYear,EndDaySpec) then
              goto RegErr;
          RegL:='';
          New(RegArray[CurRegX,CurRegY]);
          RegArray[CurRegX,CurRegY]^:=Reg;
          CurRegY:=succ(CurRegY);
{          with Reg do begin
            writeln;
            write(Name,': ',StartDay,'/',StartYear,' - ',EndDay,'/',EndYear);
            write(' ',StartDaySpec,' ',EndDaySpec);
          end;   }
          Goto RegOK;
          RegErr:
            RegL:='';
            if not Problem then begin
              writeln;
              writeln('Problems with Regnal Years in Data file:');
            end;
            Problem:=true;
            writeln(RegLHold);
          RegOK:
        end;
      end;
      readln(DataF,RegL);
    end;
    if Problem then begin
      writeln('Press any key to continue.');
      read(kbd,RegL[1]);
    end
    else
      writeln(' Done.');
end;

Procedure ReadFixedFeasts;
var
    FF,HoldFF: Str30;
    Name: Str10;
    M,D,I: integer;
    S: Str2;
    OK, FirstFeast: boolean;
begin
  write('Reading fixed feasts...');
  New(FeastDisplayRoot);
  FirstFeast:=true;
  FeastDisplay:=FeastDisplayRoot;
  Problem:=false;
  FillChar(Day,SizeOf(Day),Char(0));
    readln(DataF,FF);
    while (not eof(DataF)) and (FF[1]<>'\') do begin
      OK:=true;
      HoldFF:=FF;
      while FF[1]=' ' do
        Delete(FF,1,1);
      while FF[length(FF)]=' ' do
        FF[0]:=pred(FF[0]);
      M:=pos('=',FF);
      if M>0 then begin
        Name:=Copy(FF,1,pred(M));
        Delete(FF,1,M);
        M:=pos('/',FF);
        if M>0 then begin
          S:=Copy(FF,1,pred(M));
          Delete(FF,1,M);
          Val(S,D,I);
          if (I=0) and (D>0) and (D<32) then begin
            Val(FF,M,I);
            if (I<>0) or (M<1) or (M>12) then
              OK:=false;
          end
          else OK:=false;
        end
        else OK:=false;
      end
      else OK:=false;
      if OK then begin
        if not FirstFeast then begin
          New(FeastDisplay^.Next);
          FeastDisplay:=FeastDisplay^.Next;
        end;
        FirstFeast:=false;
        FeastDisplay^.S:=Name;
        FeastDisplay^.M:=M;
        FeastDisplay^.D:=D;
        FeastDisplay^.Next:=nil;
        D:=MonthSt[M]+D-1;
        InsertDayName(D,Name,true,false);
      end
      else begin
        if not Problem then begin
          writeln;
          writeln('Problems with Fixed Feasts in Data file:');
        end;
        Problem:=true;
        writeln(HoldFF);
      end;
      readln(DataF,FF);
    end;
  if not Problem then writeln(' Done.');
end;

begin  {ReadDataFile}
  Problem:=false;
  GotoXY(1,15);
  if ParamCount=0 then
    DataFN:='Calendar.dat'
  else
    DataFN:=ParamStr(1);
  Assign(DataF,DataFN);
  {$I-} reset(DataF) {I+};
  if IOResult=0 then begin
    writeln('Data file found - Loading...');
    repeat
      repeat
        readln(DataF,S);
      until (S[1]='\') or (Eof(DataF));
      if not eof(DataF) then begin
        Delete(S,1,1);
        S:=Copy(S,1,5);
        Caps(S);
        if S='REGNA' then
          ReadRegnalYears
        else if S='FEAST' then
          ReadFixedFeasts
        else writeln('Error: I don''t understand ',S);
      end;
    until Eof(DataF);
  end
  else begin
    write('File ',DataFN,' not found:');
    writeln(' no regnal years or feasts are available.');
  end;
  Close(DataF);
end;
