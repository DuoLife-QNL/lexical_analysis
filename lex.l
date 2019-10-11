%{
    #include<global.h>
%}
line    \n
blank   [ \t]
blanks  {blank}+
letter  [A-Za-z]
digit   [0-9]
id      {letter|_}({letter}|{digit}|_)*

%%

{blanks}    {}
