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




