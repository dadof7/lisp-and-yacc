## Lisp front-end for a yacc generated parser

I wanted a lisp front-end for a yacc generated parser so that I can
do semantic analysis using lists.
Another lisp again? Sorry, yes it is.

This lisp is a simple straight forward implementation based on the work
of Yuumi Yoshida at ey-office.com.
I got a permission to use the code listed below.

```
https://atmarkit.itmedia.co.jp/ait/articles/0810/31/news148.html
```

By assigning values to `$$`'s, we can get results as lists. Once we got AST as a list,
we can massage it in whatever way we like.

From REPL, we can call the parser by evaluating `(parse "input.txt")` .
However syntax and lexical analysis stuff are hard coded into `yacc` and `lex` files.
Every time you change your grammar, you have to recompile the whole entire things by `make`.
That is OK for my purpose, but it may bother other people.

### Example

Included `yacc.y` describes SQL `create table` statement.
Suppose we have the following code in `k.sql`.

```
create table tmEmp (                                            -- master table for employees
        idno                    decimal(15,0) not null      
                                default autoincrement,
        code                    varchar(5) not null,            
        nm                      varchar(40) not null,           
        zip                     varchar(10),                    
        addr                    varchar(80),                    
        phone                   varchar(20),                    
        cellphone               varchar(20),                    
        del                     char(1) not null,               -- delete flg
        primary key(idno)
)
```

Evaluating `(parse "k.sql")` yields

```
((tmEmp
  (idno (:type decimal 15 . 0) (:not-null . t) (:autoincrement . t))
  (code (:type varchar . 5) (:not-null . t))
  (nm (:type varchar . 40) (:not-null . t))
  (zip (:type varchar . 10))
  (addr (:type varchar . 80))
  (phone (:type varchar . 20))
  (cellphone (:type varchar . 20))
  (del (:type char . 1) (:not-null . t))
  (:primary-key . idno)
  ))
```

as a list. (the actual result is NOT pretty-printed)

For instance, you can use this list to generate some input-form.
Once we get into the lisp world, you can try many thins using REPL.
That may be fun.


### Language features

Integers, reals, and strings can be used as atomic values.

Functions and variables share the same name space. It's a deep binding and dynamic
scoping lisp.

\` works in macro definitions as well as `,` and `,@`.

Tail-calls are executed by jumps.

|form|implementation|explanation|
|----|-----|-----|
  | `>` | C | numberic comparison
  | `>=` | C 
  | `<` | C
  | `<=` | C
  | `%` | C | arithmetic binary operator
  | `mul2` | C
  | `sub2` | C
  | `add2` | C
  | `div2` | C
  | `if` | C 
  | `not` | C
  | `setq` | C
  | `quote` | C | ' can be used instead
  | `_bquote` | C |
  | `dump` | C
  | `gc` | C | force garbage collection
  | `cons` | C
  | `env` | C
  | `car` | C
  | `cdr` | C
  | `cdr!` | C
  | `=` | C
  | `eq` | C
  | `atom` | C
  | `intp` | C
  | `nump` | C
  | `symbolp` | C
  | `symbolp:` | C
  | `listp` | C
  | `consp` | C
  | `null` | C
  | `endp` | C
  | `and` | C
  | `or` | C
  | `progn` | C
  | `let` | C
  | `let*` | C
  | `parse` | C | invoke your parser
  | `intern` | C
  | `cond` | C
  | `concatenate` | C | string concatination
  | `cons-counter` | C
  | `pr` | C | print things
  | `prln` | C | `pr` with new-line
  | `len` | C | string length
  | `assoc` | C 
  | `2str` | C | making a string from integer or real
  | `gensym` | C | generate a new symbol 
  | `macroexpand` | C | expand macro just once
  | `funcall` | C
  | `apply` | C
  | `eval` | C
  | `range` | C | returns a list of numbers between given two integers
  | `foreach` | C | `(foreach i <list> expr1 expr2 ...)` 
  | `foreach-t` | C | same syntax as `foreach`
| defun | lisp
| defmacro | lisp
| unless | lisp
| fac | lisp
| append | lisp
| reverse | lisp
| length | lisp
| nth | lisp
| + | lisp | arithmetic operator taking any number of perands
| - | lisp
| / | lisp
| * | lisp
| equal | lisp
| cat | lisp | string concatination for any number of strings

--------------
## `yacc`で生成したパーサの意味付けを`Lisp`で

`yacc`で作ったパーサでパース後、意味付けをするのに`lisp`のリストが使えれば便利だと思って作りました.
"anoter lisp, again?"と思われた方、すみません.

インタープリタは、吉田裕美さんの「ちょっと変わったLisp入門」をベースにしています.
本人のご許可を頂きました. 

```
https://atmarkit.itmedia.co.jp/ait/articles/0810/31/news148.html
```

`$$`に`cons`セルを代入すれば、結果をリストとして受け取ることができます.
**AST**がリストになれば、`lisp`でいろいろ触わることができます.

`REPL`から`(parse "input.txt")`としてパーサーを呼び出します.
ただし、スキャナや文法はそれぞれ`lex.l`と`yacc.y`にハードコードされているので、
文法を変更した場合は、`make`して全体を作り直す必要があります.

### 例

今は`yacc.y`にSQLの`create table`文の文法が定義してあります. 

`k.sql`の内容が以下だとして、
```
create table tmEmp (
        idno                    decimal(15,0) not null          -- ID
                                default autoincrement,
        code                    varchar(5) not null,            -- コード
        nm                      varchar(40) not null,           -- 名前
        zip                     varchar(10),                    -- 郵便番号
        addr                    varchar(80),                    -- 住所
        phone                   varchar(20),                    -- 電話番号
        cellphone               varchar(20),                    -- 携帯番号
        del                     char(1) not null,               -- 削除FLG
        primary key(idno)
)
```

`lisp`のREPLで`(parse "k.sql")`を評価すると、

```
((tmEmp
  (idno (:type decimal 15 . 0) (:not-null . t) (:autoincrement . t))
  (code (:type varchar . 5) (:not-null . t))
  (nm (:type varchar . 40) (:not-null . t))
  (zip (:type varchar . 10))
  (addr (:type varchar . 80))
  (phone (:type varchar . 20))
  (cellphone (:type varchar . 20))
  (del (:type char . 1) (:not-null . t))
  (:primary-key . idno)
  ))
```

が返ります. (※実際には、ppされないベタなフォームとして印字されます)

例えば、これを適当に走査して入力画面を生成したりするのに使えるでしょう.
一旦`lisp`の世界に入ってしまえば、REPLからいろいろなことが試せて面白そうです.




