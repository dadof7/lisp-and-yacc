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
