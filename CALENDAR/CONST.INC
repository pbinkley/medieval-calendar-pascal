const
{Screen codes:}
    HomeCurs=^^;    {Home cursor}
    CLeft=^H;       {Cursor left}
    CRight=^L;      {Cursor right}
    CUp=^K;         {Cursor up}
    CDown=^J;       {Cursor down}
    Tab=^I;         {Tab}
    InsChar=^['Q';  {Insert character}
    InsLine=^['-';  {Insert line}
    DelChar=^['W';  {Delete character}
    DelLine=^['R';  {Delete line}
    DelRight=^['T'; {Delete to end of line}
{File codes:}
    EndFile=26;     {End of file marker}
    CR=13;          {Carriage Return}
    LF=10;          {Line Feed}
    FF=12;          {Form Feed}
    CRPar=141;      {Carriage Return with Parity bit set}
    LFPar=138;      {Line Feed with Parity bit set}
    FFPar=140;      {Form Feed with Parity bit set}
    Esc=27;         {Escape}
    Bell=7;         {Bell}
    VertLine = {^['g'^K^['G'}#179;
    LVertLine = {^['g'^A^['G'}#179;
    RVertLine = {^['g'^D^['G'}#179;

{Memory locations:}
    ScrMem=$F000;   {Beginning of Screen memory}
    TopTPA=$CAFF;   {Top of transient program area}
    TopCCP=$D2FF;   {Top of CCP}


