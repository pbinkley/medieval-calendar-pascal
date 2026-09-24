Type
  DisplayT = (Colour,Mono,Other);
const
  LowVideoByte = 7;
  NormVideoByte = 15;
var
  DisplayMode: DisplayT;
  VideoAttribute: char;
  CursorX,CursorY: byte;
Type
  ScreenT = array[1..25,1..80,1..2] of char;
  ScreenTPtr = ^ScreenT;
Var
  MonoScreen: ScreenT absolute $B000:0000;
  ColourScreen: ScreenT absolute $B800:0000;
  YearScreen, MonthScreen: ScreenTPtr;

Procedure SaveYear;
begin
  if DisplayMode=Colour then
    YearScreen^:=ColourScreen
  else
    YearScreen^:=MonoScreen;
end;

Procedure RestoreYear;
begin
  if DisplayMode=Colour then
    ColourScreen:=YearScreen^
  else
    MonoScreen:=YearScreen^;
end;

Procedure FillMonthScreen;
const
    Month1 = 'Year:       Month:';
    Month2 =
'|Dom.      |Fer. ii   |Fer. iii  |Fer. iv   |Fer. v    |Fer. vi   |Fer. vii  |';
    Month3 =
'|Sunday____|Monday____|Tuesday___|Wednesday_|Thursday__|Friday____|Saturday__|';
    Month4 =
'|          |          |          |          |          |          |          |';
    Month5 =
'|__________|__________|__________|__________|__________|__________|__________|';
  var i,j,k,l: integer;

  Procedure PutLn(S: AnyStr);
  var i: integer;
  begin
    for i:=1 to length(S) do
      MonthScreen^[l,i,1]:=S[i];
    l:=succ(l);
  end;

  begin
    l:=1;
    FillChar(MonthScreen^,SizeOf(ScreenT),' ');
    PutLn(Month1);
    PutLn(Month2);
    PutLn(Month3);
    for i:=1 to 5 do
      for j:=1 to 4 do begin
        if j=4 then
          PutLn(Month5)
        else
          PutLn(Month4);
      end;
    for i:=1 to 25 do
      for j:=1 to 80 do
        MonthScreen^[i,j,2]:=char(NormVideoByte);
  end;

Procedure MakeBright;
begin
  if DisplayMode=Colour then
    TextColor(NormVideoByte)
  else
    NormVideo;
end;

Procedure MakeDim;
begin
  if DisplayMode=Colour then
    TextColor(LowVideoByte)
  else
    LowVideo;
end;

Procedure GetDisplayMode;
var m: byte;
begin
  m:=mem[$0040:$0049];
  if (m=2) or (m=3) then
    DisplayMode:=Colour
  else if m=7 then
    DisplayMode:=Mono
  else DisplayMode:=Other;
  New(YearScreen);
  New(MonthScreen);
  FillMonthScreen;
  MakeBright;
end;

(*
Procedure PutStr(S:AnyStr);
  i,j: byte;
begin
  j:=pred(length(S));
  if DisplayMode=Colour then
    for i:=0 to j do begin
      ColourScreen[CursorY,CursorX+i,1]:=S[succ(i)];
      ColourScreen[CursorY,CursorX+i,2]:=VideoAttribute;
    end
  else if DisplayMode=Mono then
    for i:=0 to j do begin
      MonoScreen[CursorY,CursorX+i,1]:=S[succ(i)];
      MonoScreen[CursorY,CursorX+i,2]:=VideoAttribute;
    end
  else begin
    GotoXY(CursorX,CursorY);
    write(S);
  end;
  CursorX:=CursorX+length(S);
  while CursorX>80 do begin
    CursorY:=succ(CursorY);
    CursorX:=CursorX-80;
  end;
end;

Procedure PutNum(N: integer; F: byte);
var
  S: AnyStr;
begin
  if F>0 then
    Str(N:F,S)
  else
    Str(N,S);
  PutStr(S);
end;


Procedure Writeln;
begin
  CursorX:=1;
  CursorY:=succ(CursorY);
end;

Procedure LowVideo;
begin
  VideoAttribute:=LowVideoByte;
end;

Procedure NormVideo;
begin
  VideoAttribute:=NormVideoByte;
end;

Procedure GotoXY(x,y: byte);
begin
  CursorX:=X;
  CursorY:=Y;
end;

Procedure ClearScreen;
begin
  ClrScr;
  CursorX:=1;
  CursorY:=1;
end;

  *)