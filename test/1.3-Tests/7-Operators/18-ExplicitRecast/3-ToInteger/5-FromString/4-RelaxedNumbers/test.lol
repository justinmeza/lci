HAI 1.3
	I HAS A var1 ITZ " 123" BTW leading whitespace is ignored
	I HAS A var2 ITZ "123x" BTW parsing stops at the first non-digit
	I HAS A var3 ITZ "0377" BTW octal prefix
	I HAS A var4 ITZ "0xFF" BTW hexadecimal prefix
	var1 IS NOW A NUMBR
	var2 IS NOW A NUMBR
	var3 IS NOW A NUMBR
	var4 IS NOW A NUMBR
	VISIBLE var1
	VISIBLE var2
	VISIBLE var3
	VISIBLE var4
KTHXBYE
