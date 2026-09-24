Type
  DisplayT = (Colour,Mono,Other);
const
  LowVideoByte = #7;
  NormVideoByte = #15;
var
  DisplayMode: DisplayT;
  VideoAttribute: char;
  CursorX,CursorY: byte;

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

Procedure GetDisplayMode;
var m: byte;
begin
  m:=mem[$0040:$0049];
  if (m=2) or (m=3) then
    DisplayMode:=Colour
  else if m=7 then
    DisplayMode:=Mono
  else DisplayMode:=Other;
  NormVideo;
  ClrScr;
  CursorX:=1;
  CursorY:=1;
end;

Procedure PutStr(S:AnyStr);
Type
  ScreenT = array[1..25,1..80,1..2] of char;
Var
  MonoScreen: ScreenT absolute $B000:0000;
  ColourScreen: ScreenT absolute $B800:0000;
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
