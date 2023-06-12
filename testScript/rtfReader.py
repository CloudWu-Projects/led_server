import time






import rtfcre

rtf=r"""
{\rtf1\fbidis\ansi\ansicpg936\deff0\nouicompat\rtldoc{\fonttbl{\f0\fswiss\fprq2\fcharset134 \'cb\'ce\'cc\'e5;}{\f1\fswiss\fcharset0 Calibri;}}
{\colortbl ;\red255\green0\blue0;}
{\*\generator Riched20 10.0.22621}\viewkind4\uc1 
\pard\ltrpar\cf1\f0\fs24\lang2052\'b9\'cc\'b6\'a8\'ba\'ba\'d7\'d6\f1\lang1033 2\f0\lang2052\par
}
�
""".lstrip()

dic = rtfcre.loads(rtf)

print(dic)

