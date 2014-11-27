HAI 1.3
	I HAS A parent ITZ A BUKKIT
	parent HAS A foo ITZ 1
	parent HAS A bar ITZ 2.34
	parent HAS A baz ITZ FAIL
	parent HAS A qux ITZ "HAI"

	VISIBLE parent 'Z foo
	VISIBLE parent 'Z bar
	VISIBLE SUM OF 0 AN parent 'Z baz
	VISIBLE parent 'Z qux

	O HAI IM child IM LIEK parent
		VISIBLE foo
		VISIBLE bar
		VISIBLE SUM OF 0 AN baz
		VISIBLE qux

		I HAS A foo ITZ 9
		I HAS A bar ITZ 8.76
		I HAS A baz ITZ WIN
		I HAS A qux ITZ "BAI"

		VISIBLE foo
		VISIBLE bar
		VISIBLE SUM OF 0 AN baz
		VISIBLE qux

		VISIBLE parent 'Z foo
		VISIBLE parent 'Z bar
		VISIBLE SUM OF 0 AN parent 'Z baz
		VISIBLE parent 'Z qux
	KTHX

	VISIBLE child 'Z foo
	VISIBLE child 'Z bar
	VISIBLE SUM OF 0 AN child 'Z baz
	VISIBLE child 'Z qux

	VISIBLE parent 'Z foo
	VISIBLE parent 'Z bar
	VISIBLE SUM OF 0 AN parent 'Z baz
	VISIBLE parent 'Z qux

KTHXBYE
