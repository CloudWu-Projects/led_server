from pyth.plugins.rtf15.reader import Rtf15Reader
from pyth.plugins.plaintext.writer import PlaintextWriter

rtf_string = r'{\rtf1\fbidis\ansi\ansicpg936\deff0\nouicompat\rtldoc{\fonttbl{\f0\fswiss\fprq2\fcharset134 \'cb\'ce\'cc\'e5;}}{\colortbl ;\red255\green0\blue0;}{*\generator Riched20 10.0.22621}\viewkind4\uc1\pard\ltrpar\cf1\f0\fs24\lang2052\'d7\'d6\'c4\'bb1\par}'

# parse the RTF-formatted text content
doc = Rtf15Reader.read(rtf_string)

# extract the plain text content
plain_text = PlaintextWriter.write(doc).getvalue()

# extract the color information
color_info = doc.content[0].color

print(f"Plain text: {plain_text}")
print(f"Color information: {color_info}")
